#include "sc2opt/tuner/Champion.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace sc2opt::tuner {
namespace {

bool PolicyValid(const ChampionPolicy& policy) noexcept
{
    return policy.minimum_samples > 0 &&
           std::isfinite(policy.minimum_absolute_gain_ns) &&
           policy.minimum_absolute_gain_ns >= 0.0 &&
           std::isfinite(policy.minimum_relative_gain) &&
           policy.minimum_relative_gain >= 0.0 &&
           (!policy.require_confidence_separation ||
            (std::isfinite(policy.z_score) && policy.z_score >= 0.0));
}

bool DuplicateIds(std::span<const CandidateStats> candidates) noexcept
{
    for (std::size_t i = 0; i < candidates.size(); ++i)
    {
        for (std::size_t j = 0; j < i; ++j)
        {
            if (candidates[i].id == candidates[j].id)
                return true;
        }
    }
    return false;
}

bool Eligible(const CandidateStats& candidate, const ChampionPolicy& policy) noexcept
{
    return candidate.correctness_passed &&
           candidate.total_cost_ns.samples >= policy.minimum_samples &&
           std::isfinite(candidate.total_cost_ns.mean) &&
           candidate.total_cost_ns.mean >= 0.0 &&
           std::isfinite(candidate.total_cost_ns.m2) &&
           candidate.total_cost_ns.m2 >= 0.0;
}

const CandidateStats* Find(std::span<const CandidateStats> candidates, CandidateId id) noexcept
{
    for (const CandidateStats& candidate : candidates)
    {
        if (candidate.id == id)
            return &candidate;
    }
    return nullptr;
}

double LowerBound(const CandidateStats& candidate, double z_score) noexcept
{
    const double se = candidate.total_cost_ns.StandardError();
    return std::isfinite(se) ? candidate.total_cost_ns.mean - z_score * se
                             : -std::numeric_limits<double>::infinity();
}

double UpperBound(const CandidateStats& candidate, double z_score) noexcept
{
    const double se = candidate.total_cost_ns.StandardError();
    return std::isfinite(se) ? candidate.total_cost_ns.mean + z_score * se
                             : std::numeric_limits<double>::infinity();
}

}  // namespace

void RunningStats::Reset() noexcept
{
    samples = 0;
    mean = 0.0;
    m2 = 0.0;
}

bool RunningStats::Add(double value) noexcept
{
    if (!std::isfinite(value) || value < 0.0 ||
        samples == std::numeric_limits<std::uint64_t>::max())
    {
        return false;
    }

    const std::uint64_t new_samples = samples + 1;
    const double delta = value - mean;
    const double new_mean = mean + delta / static_cast<double>(new_samples);
    const double delta2 = value - new_mean;
    const double new_m2 = m2 + delta * delta2;

    if (!std::isfinite(new_mean) || !std::isfinite(new_m2))
        return false;

    samples = new_samples;
    mean = new_mean;
    m2 = std::max(0.0, new_m2);
    return true;
}

double RunningStats::SampleVariance() const noexcept
{
    if (samples < 2 || !std::isfinite(m2))
        return 0.0;
    return std::max(0.0, m2 / static_cast<double>(samples - 1));
}

double RunningStats::StandardError() const noexcept
{
    if (samples < 2)
        return std::numeric_limits<double>::infinity();
    return std::sqrt(SampleVariance() / static_cast<double>(samples));
}

void ResetToBaseline(ChampionState& state, TuningMode mode) noexcept
{
    state.champion = kBaselineCandidate;
    state.mode = mode;
}

void ResetEvidence(std::span<CandidateStats> candidates) noexcept
{
    for (CandidateStats& candidate : candidates)
    {
        candidate.total_cost_ns.Reset();
        candidate.correctness_passed = false;
    }
}

void ApplyChampionDecision(ChampionState& state, const ChampionDecision& decision) noexcept
{
    state.champion = state.mode == TuningMode::BaselineOnly ? kBaselineCandidate : decision.champion;
}

ChampionDecision EvaluateChampion(std::span<const CandidateStats> candidates,
                                  ChampionState state,
                                  ChampionPolicy policy) noexcept
{
    if (!PolicyValid(policy) || DuplicateIds(candidates))
        return {kBaselineCandidate, kBaselineCandidate, 0.0, 0.0,
                ChampionReason::EvidenceInvalid};

    if (state.mode == TuningMode::BaselineOnly)
        return {kBaselineCandidate, kBaselineCandidate, 0.0, 0.0, ChampionReason::BaselineMode};

    const CandidateStats* baseline = Find(candidates, kBaselineCandidate);
    if (baseline == nullptr || !Eligible(*baseline, policy))
        return {};

    const CandidateStats* current = Find(candidates, state.champion);
    ChampionReason current_reason = ChampionReason::NoEligibleChallenger;
    if (current == nullptr || !Eligible(*current, policy))
    {
        current = baseline;
        current_reason = ChampionReason::CurrentChampionInvalid;
    }

    const CandidateStats* challenger = nullptr;
    bool saw_eligible_challenger = false;
    for (const CandidateStats& candidate : candidates)
    {
        if (&candidate == current || !Eligible(candidate, policy))
            continue;

        saw_eligible_challenger = true;
        if (challenger == nullptr || candidate.total_cost_ns.mean < challenger->total_cost_ns.mean)
            challenger = &candidate;
    }

    if (!saw_eligible_challenger)
        return {current->id, kBaselineCandidate, 0.0, 0.0, current_reason};

    if (challenger == nullptr || challenger->total_cost_ns.mean >= current->total_cost_ns.mean)
        return {current->id, challenger != nullptr ? challenger->id : kBaselineCandidate,
                0.0, 0.0, ChampionReason::NoBetterChallenger};

    const double gain_ns = current->total_cost_ns.mean - challenger->total_cost_ns.mean;
    const double relative_gain = current->total_cost_ns.mean > 0.0
                                     ? gain_ns / current->total_cost_ns.mean
                                     : 0.0;

    if (!(gain_ns > 0.0) || gain_ns < policy.minimum_absolute_gain_ns ||
        relative_gain < policy.minimum_relative_gain)
    {
        return {current->id, challenger->id, gain_ns, 0.0,
                ChampionReason::ChallengerInsufficientGain};
    }

    double conservative_gain_ns = gain_ns;
    if (policy.require_confidence_separation)
    {
        conservative_gain_ns =
            LowerBound(*current, policy.z_score) - UpperBound(*challenger, policy.z_score);
        if (!(conservative_gain_ns > 0.0))
        {
            return {current->id, challenger->id, gain_ns, conservative_gain_ns,
                    ChampionReason::ChallengerInsufficientConfidence};
        }
    }

    return {challenger->id, challenger->id, gain_ns, conservative_gain_ns,
            ChampionReason::ChallengerWon};
}

}  // namespace sc2opt::tuner

#include "sc2opt/tuner/NetBenefit.hpp"

#include <cmath>

namespace sc2opt::tuner {
namespace {

bool PolicyValid(const NetBenefitPolicy& policy) noexcept
{
    return policy.minimum_samples > 0 &&
           std::isfinite(policy.minimum_absolute_gain_ns) &&
           policy.minimum_absolute_gain_ns >= 0.0 &&
           std::isfinite(policy.minimum_relative_gain) &&
           policy.minimum_relative_gain >= 0.0;
}

bool Eligible(const CandidateEvidence& candidate, const NetBenefitPolicy& policy) noexcept
{
    return candidate.correctness_passed && candidate.samples >= policy.minimum_samples &&
           std::isfinite(candidate.mean_total_cost_ns) && candidate.mean_total_cost_ns >= 0.0;
}

}  // namespace

NetBenefitDecision ChooseNetBenefitChampion(std::span<const CandidateEvidence> evidence,
                                            NetBenefitPolicy policy) noexcept
{
    if (!PolicyValid(policy))
        return {kBaselineCandidate, 0.0, 0.0, DecisionReason::PolicyInvalid};

    const CandidateEvidence* baseline = nullptr;
    for (const CandidateEvidence& candidate : evidence)
    {
        if (candidate.id == kBaselineCandidate)
        {
            baseline = &candidate;
            break;
        }
    }

    if (baseline == nullptr || !Eligible(*baseline, policy))
        return {};

    const CandidateEvidence* best = baseline;
    bool saw_eligible_challenger = false;

    for (const CandidateEvidence& candidate : evidence)
    {
        if (candidate.id == kBaselineCandidate || !Eligible(candidate, policy))
            continue;

        saw_eligible_challenger = true;
        if (candidate.mean_total_cost_ns < best->mean_total_cost_ns)
            best = &candidate;
    }

    if (!saw_eligible_challenger)
        return {kBaselineCandidate, 0.0, 0.0, DecisionReason::NoEligibleChallenger};

    if (best == baseline)
        return {kBaselineCandidate, 0.0, 0.0, DecisionReason::NoPositiveNetBenefit};

    const double gain_ns = baseline->mean_total_cost_ns - best->mean_total_cost_ns;
    const double relative_gain = baseline->mean_total_cost_ns > 0.0
                                     ? gain_ns / baseline->mean_total_cost_ns
                                     : 0.0;

    const bool positive = gain_ns > 0.0;
    const bool clears_absolute = gain_ns >= policy.minimum_absolute_gain_ns;
    const bool clears_relative = relative_gain >= policy.minimum_relative_gain;

    if (!positive || !clears_absolute || !clears_relative)
        return {kBaselineCandidate, gain_ns, relative_gain, DecisionReason::NoPositiveNetBenefit};

    return {best->id, gain_ns, relative_gain, DecisionReason::ChallengerWon};
}

}  // namespace sc2opt::tuner

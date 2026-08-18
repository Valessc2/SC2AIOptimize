#pragma once

#include "sc2opt/tuner/NetBenefit.hpp"

#include <cstdint>
#include <span>

namespace sc2opt::tuner {

struct RunningStats {
    std::uint64_t samples = 0;
    double mean = 0.0;
    double m2 = 0.0;

    void Reset() noexcept;
    [[nodiscard]] bool Add(double value) noexcept;
    [[nodiscard]] double SampleVariance() const noexcept;
    [[nodiscard]] double StandardError() const noexcept;
};

struct CandidateStats {
    CandidateId id = kBaselineCandidate;
    RunningStats total_cost_ns{};
    bool correctness_passed = false;
};

enum class TuningMode : std::uint8_t {
    Adaptive,
    BaselineOnly
};

struct ChampionState {
    CandidateId champion = kBaselineCandidate;
    TuningMode mode = TuningMode::Adaptive;
};

struct ChampionPolicy {
    std::uint64_t minimum_samples = 5;
    double minimum_absolute_gain_ns = 0.0;
    double minimum_relative_gain = 0.0;
    double z_score = 1.96;
    bool require_confidence_separation = true;
};

enum class ChampionReason : std::uint8_t {
    EvidenceInvalid,
    BaselineInvalid,
    BaselineMode,
    CurrentChampionInvalid,
    NoEligibleChallenger,
    NoBetterChallenger,
    ChallengerInsufficientGain,
    ChallengerInsufficientConfidence,
    ChallengerWon
};

struct ChampionDecision {
    CandidateId champion = kBaselineCandidate;
    CandidateId challenger = kBaselineCandidate;
    double estimated_gain_ns = 0.0;
    double conservative_gain_ns = 0.0;
    ChampionReason reason = ChampionReason::BaselineInvalid;
};

void ResetToBaseline(ChampionState& state, TuningMode mode = TuningMode::Adaptive) noexcept;
void ResetEvidence(std::span<CandidateStats> candidates) noexcept;
void ApplyChampionDecision(ChampionState& state, const ChampionDecision& decision) noexcept;

[[nodiscard]] ChampionDecision EvaluateChampion(
    std::span<const CandidateStats> candidates,
    ChampionState state,
    ChampionPolicy policy = {}) noexcept;

}  // namespace sc2opt::tuner

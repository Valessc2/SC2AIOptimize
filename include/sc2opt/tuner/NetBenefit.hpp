#pragma once

#include <cstdint>
#include <span>

namespace sc2opt::tuner {

using CandidateId = std::uint32_t;
inline constexpr CandidateId kBaselineCandidate = 0;

struct CandidateEvidence {
    CandidateId id = kBaselineCandidate;
    std::uint64_t samples = 0;
    double mean_total_cost_ns = 0.0;
    bool correctness_passed = false;
};

struct NetBenefitPolicy {
    std::uint64_t minimum_samples = 1;
    double minimum_absolute_gain_ns = 0.0;
    double minimum_relative_gain = 0.0;
};

enum class DecisionReason : std::uint8_t {
    BaselineInvalid,
    BaselineOnly,
    NoEligibleChallenger,
    NoPositiveNetBenefit,
    ChallengerWon
};

struct NetBenefitDecision {
    CandidateId champion = kBaselineCandidate;
    double estimated_gain_ns = 0.0;
    double estimated_relative_gain = 0.0;
    DecisionReason reason = DecisionReason::BaselineInvalid;
};

// Selects from already measured/correctness-qualified evidence. This function does not
// benchmark and does not allocate. Total cost must include adapter/dispatch/bookkeeping cost.
[[nodiscard]] NetBenefitDecision ChooseNetBenefitChampion(
    std::span<const CandidateEvidence> evidence,
    NetBenefitPolicy policy = {}) noexcept;

}  // namespace sc2opt::tuner

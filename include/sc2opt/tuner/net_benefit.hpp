#pragma once

#include <cstdint>

namespace sc2opt::tuner {

enum class CandidateChoice : std::uint8_t {
    Baseline = 0,
    Optimized = 1,
};

struct NetBenefitPolicy {
    // Candidate must beat baseline by both thresholds. Defaults deliberately resist noise.
    double minimum_fractional_gain = 0.02;
    std::uint64_t minimum_absolute_gain_ns = 0;
};

struct NetBenefitSample {
    std::uint64_t baseline_ns = 0;
    std::uint64_t optimized_ns = 0;
    std::uint64_t integration_overhead_ns = 0;
    bool correctness_equivalent = false;
    bool measurement_valid = false;
};

struct NetBenefitDecision {
    CandidateChoice choice = CandidateChoice::Baseline;
    std::uint64_t optimized_total_ns = 0;
    std::uint64_t net_saving_ns = 0;
    double fractional_gain = 0.0;
};

// Fail closed. Baseline wins on invalid evidence, failed equivalence, overflow, ties, losses or
// insufficient margin. This function is control-plane logic and is not intended for hot loops.
[[nodiscard]] NetBenefitDecision decide_net_benefit(
    const NetBenefitSample& sample,
    const NetBenefitPolicy& policy = {} ) noexcept;

} // namespace sc2opt::tuner

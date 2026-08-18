#pragma once

#include "sc2opt/proof/Manifest.hpp"

#include <cstdint>

namespace sc2opt::proof {

struct PerformanceRegressionPolicy {
    double maximum_p95_relative_increase = 0.05;
    double maximum_p99_relative_increase = 0.10;
    std::uint64_t maximum_p95_absolute_increase_ns = 0;
    std::uint64_t maximum_p99_absolute_increase_ns = 0;
};

enum class RegressionIssue : std::uint8_t {
    None,
    InvalidPolicy,
    IdentityMismatch,
    InvalidEvidence,
    P95Regression,
    P99Regression
};

struct RegressionDecision {
    RegressionIssue issue = RegressionIssue::None;
    double p95_relative_increase = 0.0;
    double p99_relative_increase = 0.0;

    [[nodiscard]] constexpr bool passed() const noexcept { return issue == RegressionIssue::None; }
};

[[nodiscard]] RegressionDecision EvaluatePerformanceRegression(
    const BenchmarkRecord& reference,
    const BenchmarkRecord& candidate,
    PerformanceRegressionPolicy policy = {}) noexcept;

}  // namespace sc2opt::proof

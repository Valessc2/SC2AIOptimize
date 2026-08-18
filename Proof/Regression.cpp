#include "sc2opt/proof/Regression.hpp"

#include <cmath>
#include <limits>

namespace sc2opt::proof {
namespace {

bool PolicyValid(const PerformanceRegressionPolicy& policy) noexcept
{
    return std::isfinite(policy.maximum_p95_relative_increase) &&
           policy.maximum_p95_relative_increase >= 0.0 &&
           std::isfinite(policy.maximum_p99_relative_increase) &&
           policy.maximum_p99_relative_increase >= 0.0;
}

bool EvidenceValid(const BenchmarkRecord& record) noexcept
{
    return record.samples > 0 && record.correctness_passed &&
           std::isfinite(record.mean_total_cost_ns) && record.mean_total_cost_ns >= 0.0 &&
           record.p50_ns <= record.p95_ns && record.p95_ns <= record.p99_ns;
}

double RelativeIncrease(std::uint64_t reference, std::uint64_t candidate) noexcept
{
    if (candidate <= reference)
        return 0.0;
    if (reference == 0)
        return std::numeric_limits<double>::infinity();
    return static_cast<double>(candidate - reference) / static_cast<double>(reference);
}

bool Exceeds(std::uint64_t reference,
             std::uint64_t candidate,
             double relative_limit,
             std::uint64_t absolute_limit) noexcept
{
    if (candidate <= reference)
        return false;
    const std::uint64_t increase = candidate - reference;
    if (absolute_limit != 0 && increase > absolute_limit)
        return true;
    return RelativeIncrease(reference, candidate) > relative_limit;
}

}  // namespace

RegressionDecision EvaluatePerformanceRegression(const BenchmarkRecord& reference,
                                                  const BenchmarkRecord& candidate,
                                                  PerformanceRegressionPolicy policy) noexcept
{
    if (!PolicyValid(policy))
        return {RegressionIssue::InvalidPolicy, 0.0, 0.0};
    if (reference.candidate != candidate.candidate ||
        reference.workload_size != candidate.workload_size)
    {
        return {RegressionIssue::IdentityMismatch, 0.0, 0.0};
    }
    if (!EvidenceValid(reference) || !EvidenceValid(candidate))
        return {RegressionIssue::InvalidEvidence, 0.0, 0.0};

    const double p95_relative = RelativeIncrease(reference.p95_ns, candidate.p95_ns);
    const double p99_relative = RelativeIncrease(reference.p99_ns, candidate.p99_ns);

    if (Exceeds(reference.p95_ns, candidate.p95_ns, policy.maximum_p95_relative_increase,
                policy.maximum_p95_absolute_increase_ns))
    {
        return {RegressionIssue::P95Regression, p95_relative, p99_relative};
    }
    if (Exceeds(reference.p99_ns, candidate.p99_ns, policy.maximum_p99_relative_increase,
                policy.maximum_p99_absolute_increase_ns))
    {
        return {RegressionIssue::P99Regression, p95_relative, p99_relative};
    }

    return {RegressionIssue::None, p95_relative, p99_relative};
}

}  // namespace sc2opt::proof

#include "sc2opt/tuner/net_benefit.hpp"

#include <limits>

namespace sc2opt::tuner {

NetBenefitDecision decide_net_benefit(const NetBenefitSample& sample,
                                      const NetBenefitPolicy& policy) noexcept
{
    NetBenefitDecision decision{};

    if (!sample.measurement_valid || !sample.correctness_equivalent || sample.baseline_ns == 0)
        return decision;

    if (sample.optimized_ns > std::numeric_limits<std::uint64_t>::max() - sample.integration_overhead_ns)
        return decision;

    decision.optimized_total_ns = sample.optimized_ns + sample.integration_overhead_ns;
    if (decision.optimized_total_ns >= sample.baseline_ns)
        return decision;

    decision.net_saving_ns = sample.baseline_ns - decision.optimized_total_ns;
    decision.fractional_gain = static_cast<double>(decision.net_saving_ns) /
                               static_cast<double>(sample.baseline_ns);

    if (decision.net_saving_ns < policy.minimum_absolute_gain_ns)
        return decision;
    if (decision.fractional_gain < policy.minimum_fractional_gain)
        return decision;

    decision.choice = CandidateChoice::Optimized;
    return decision;
}

} // namespace sc2opt::tuner

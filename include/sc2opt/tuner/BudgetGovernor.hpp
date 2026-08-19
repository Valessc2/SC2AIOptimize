#pragma once

#include "sc2opt/tuner/Defaults.hpp"

#include <cstdint>
#include <limits>

namespace sc2opt::tuner {

enum class BudgetPressure : std::uint8_t { Normal, Soft, Hard };
enum class WorkClass : std::uint8_t { Critical, Important, Optional };

struct BudgetPolicy {
    // Disabled by default: SC2AIOptimize never limits consumer work unless authority is
    // explicitly delegated by the consumer.
    bool enabled = false;
    std::uint64_t resume_ns = 0;
    std::uint64_t soft_ns = 0;
    std::uint64_t hard_ns = 0;
};

struct BudgetState {
    BudgetPressure pressure = BudgetPressure::Normal;
    std::uint64_t previous_duration_ns = 0;
    bool policy_valid = true;
};

[[nodiscard]] constexpr bool IsValid(const BudgetPolicy& policy) noexcept
{
    return !policy.enabled ||
           (policy.resume_ns < policy.soft_ns && policy.soft_ns < policy.hard_ns);
}

[[nodiscard]] constexpr std::uint64_t PercentageOf(std::uint64_t value,
                                                   std::uint32_t percent) noexcept
{
    const std::uint64_t quotient = value / 100u;
    const std::uint64_t remainder = value % 100u;
    return quotient * percent + (remainder * percent) / 100u;
}

// Converts a consumer-owned full compute envelope into the default hysteresis profile.
// Passing zero leaves the governor disabled. The helper does not impose a budget by itself.
[[nodiscard]] constexpr BudgetPolicy BudgetPolicyFromEnvelope(
    std::uint64_t envelope_ns) noexcept
{
    if (envelope_ns == 0)
        return {};

    return {true,
            PercentageOf(envelope_ns, defaults::kBudgetResumePercent),
            PercentageOf(envelope_ns, defaults::kBudgetSoftPercent),
            PercentageOf(envelope_ns, defaults::kBudgetHardPercent)};
}

// Uses the previous completed duration; it never times work itself. Disabled policy always
// returns Normal pressure. Invalid enabled policy fails hard so only Critical work is allowed.
[[nodiscard]] BudgetState UpdateBudgetState(BudgetState previous,
                                            std::uint64_t completed_duration_ns,
                                            BudgetPolicy policy = {}) noexcept;

[[nodiscard]] constexpr bool AllowsWork(BudgetState state, WorkClass work) noexcept
{
    if (!state.policy_valid)
        return work == WorkClass::Critical;
    if (state.pressure == BudgetPressure::Hard)
        return work == WorkClass::Critical;
    if (state.pressure == BudgetPressure::Soft)
        return work != WorkClass::Optional;
    return true;
}

}  // namespace sc2opt::tuner

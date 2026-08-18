#pragma once

#include <cstdint>

namespace sc2opt::tuner {

enum class BudgetPressure : std::uint8_t { Normal, Soft, Hard };
enum class WorkClass : std::uint8_t { Critical, Important, Optional };

struct BudgetPolicy {
    std::uint64_t resume_ns = 22'000'000;
    std::uint64_t soft_ns = 28'000'000;
    std::uint64_t hard_ns = 38'000'000;
};

struct BudgetState {
    BudgetPressure pressure = BudgetPressure::Normal;
    std::uint64_t previous_duration_ns = 0;
    bool policy_valid = true;
};

[[nodiscard]] constexpr bool IsValid(const BudgetPolicy& policy) noexcept
{
    return policy.resume_ns < policy.soft_ns && policy.soft_ns < policy.hard_ns;
}

// Uses the previous completed duration; it never times work itself. Invalid policies fail hard
// so only consumer-declared Critical work is allowed.
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

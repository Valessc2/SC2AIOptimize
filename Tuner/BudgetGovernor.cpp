#include "sc2opt/tuner/BudgetGovernor.hpp"

namespace sc2opt::tuner {

BudgetState UpdateBudgetState(BudgetState previous,
                              std::uint64_t completed_duration_ns,
                              BudgetPolicy policy) noexcept
{
    BudgetState next = previous;
    next.previous_duration_ns = completed_duration_ns;
    next.policy_valid = IsValid(policy);

    if (!next.policy_valid)
    {
        next.pressure = BudgetPressure::Hard;
        return next;
    }

    if (completed_duration_ns >= policy.hard_ns)
        next.pressure = BudgetPressure::Hard;
    else if (completed_duration_ns >= policy.soft_ns)
        next.pressure = BudgetPressure::Soft;
    else if (completed_duration_ns <= policy.resume_ns)
        next.pressure = BudgetPressure::Normal;

    return next;
}

}  // namespace sc2opt::tuner

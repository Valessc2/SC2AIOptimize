#include "sc2opt/kernel/compute/CombatEvaluation.hpp"

#include <cmath>
#include <limits>

namespace sc2opt::kernel::compute {

CombatGroupAggregate AggregateCombatants(std::span<const Combatant> combatants) noexcept
{
    CombatGroupAggregate aggregate;
    for (const Combatant& combatant : combatants)
    {
        if (!std::isfinite(combatant.effective_health) ||
            !std::isfinite(combatant.effective_dps) ||
            combatant.effective_health < 0.0f || combatant.effective_dps < 0.0f)
        {
            return {CombatStatus::InvalidInput, 0.0, 0.0, 0};
        }

        if (combatant.effective_health <= 0.0f)
            continue;

        aggregate.effective_health += combatant.effective_health;
        aggregate.effective_dps += combatant.effective_dps;
        ++aggregate.active_units;
    }

    if (!std::isfinite(aggregate.effective_health) || !std::isfinite(aggregate.effective_dps))
        return {CombatStatus::InvalidInput, 0.0, 0.0, 0};

    return aggregate;
}

ContinuousExchangeEstimate EstimateContinuousExchange(std::span<const Combatant> side_a,
                                                       std::span<const Combatant> side_b) noexcept
{
    const CombatGroupAggregate a = AggregateCombatants(side_a);
    const CombatGroupAggregate b = AggregateCombatants(side_b);
    if (!a || !b)
        return {CombatStatus::InvalidInput, ExchangeOutcome::Stalemate};

    const double infinity = std::numeric_limits<double>::infinity();
    const double seconds_to_clear_b = b.effective_health <= 0.0
                                          ? 0.0
                                          : (a.effective_dps > 0.0
                                                 ? b.effective_health / a.effective_dps
                                                 : infinity);
    const double seconds_to_clear_a = a.effective_health <= 0.0
                                          ? 0.0
                                          : (b.effective_dps > 0.0
                                                 ? a.effective_health / b.effective_dps
                                                 : infinity);

    ExchangeOutcome outcome = ExchangeOutcome::Stalemate;
    if (seconds_to_clear_a == infinity && seconds_to_clear_b == infinity)
    {
        outcome = ExchangeOutcome::Stalemate;
    }
    else if (seconds_to_clear_a == seconds_to_clear_b)
    {
        outcome = ExchangeOutcome::MutualDestruction;
    }
    else if (seconds_to_clear_b < seconds_to_clear_a)
    {
        outcome = ExchangeOutcome::SideAPrevails;
    }
    else
    {
        outcome = ExchangeOutcome::SideBPrevails;
    }

    return {CombatStatus::Ok, outcome, seconds_to_clear_a, seconds_to_clear_b};
}

}  // namespace sc2opt::kernel::compute

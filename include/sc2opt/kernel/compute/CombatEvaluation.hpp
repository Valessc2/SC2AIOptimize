#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>

namespace sc2opt::kernel::compute {

struct Combatant {
    // Inputs are deliberately effective values. Consumers own SC2-specific armor, upgrades,
    // targetability, splash, cooldown and policy semantics.
    float effective_health = 0.0f;
    float effective_dps = 0.0f;
};

enum class CombatStatus : std::uint8_t { Ok = 0, InvalidInput };

enum class ExchangeOutcome : std::uint8_t {
    SideAPrevails = 0,
    SideBPrevails,
    MutualDestruction,
    Stalemate
};

struct CombatGroupAggregate {
    CombatStatus status = CombatStatus::Ok;
    double effective_health = 0.0;
    double effective_dps = 0.0;
    std::size_t active_units = 0;

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return status == CombatStatus::Ok;
    }
};

struct ContinuousExchangeEstimate {
    CombatStatus status = CombatStatus::Ok;
    ExchangeOutcome outcome = ExchangeOutcome::Stalemate;
    double seconds_to_clear_a = std::numeric_limits<double>::infinity();
    double seconds_to_clear_b = std::numeric_limits<double>::infinity();

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return status == CombatStatus::Ok;
    }
};

[[nodiscard]] CombatGroupAggregate AggregateCombatants(
    std::span<const Combatant> combatants) noexcept;

// Minimal continuous-DPS exchange estimate. This is a generic compute primitive, not an SC2
// simulator or strategy decision. Consumers decide whether this approximation is useful.
[[nodiscard]] ContinuousExchangeEstimate EstimateContinuousExchange(
    std::span<const Combatant> side_a,
    std::span<const Combatant> side_b) noexcept;

}  // namespace sc2opt::kernel::compute

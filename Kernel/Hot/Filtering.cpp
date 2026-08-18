#include "sc2opt/kernel/hot/Filtering.hpp"

namespace sc2opt::kernel::hot {
namespace {

[[nodiscard]] bool Matches(const model::UnitView& unit, UnitFilter filter) noexcept
{
    if ((unit.flags & filter.required_flags) != filter.required_flags)
        return false;
    if ((unit.flags & filter.excluded_flags) != 0u)
        return false;
    if (filter.consumer_type_id != kAnyConsumerType &&
        unit.consumer_type_id != filter.consumer_type_id)
    {
        return false;
    }
    return unit.health >= filter.minimum_health;
}

}  // namespace

FilterResult FilterUnitIndices(std::span<const model::UnitView> units,
                               UnitFilter filter,
                               std::span<std::uint32_t> output) noexcept
{
    std::size_t required = 0;
    std::size_t written = 0;

    for (std::size_t i = 0; i < units.size(); ++i)
    {
        if (!Matches(units[i], filter))
            continue;

        if (written < output.size())
            output[written] = static_cast<std::uint32_t>(i);
        ++written;
        ++required;
    }

    if (required > output.size())
        return {FilterStatus::OutputTooSmall, 0, required};
    return {FilterStatus::Ok, required, required};
}

}  // namespace sc2opt::kernel::hot

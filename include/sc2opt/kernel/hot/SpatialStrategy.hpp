#pragma once

#include <cstddef>
#include <limits>

namespace sc2opt::kernel::hot {

enum class SpatialQueryStrategy {
    BruteForce = 0,
    GridInputOrder,
    GridUnordered
};

struct SpatialCrossoverProfile {
    // `max()` means "not certified": baseline remains champion for that variant.
    std::size_t grid_input_order_min_points = std::numeric_limits<std::size_t>::max();
    std::size_t grid_unordered_min_points = std::numeric_limits<std::size_t>::max();
};

[[nodiscard]] constexpr SpatialQueryStrategy ChooseSpatialQueryStrategy(
    std::size_t point_count,
    bool require_input_order,
    bool grid_ready,
    SpatialCrossoverProfile profile) noexcept
{
    if (!grid_ready)
        return SpatialQueryStrategy::BruteForce;

    if (require_input_order)
    {
        return point_count >= profile.grid_input_order_min_points
                   ? SpatialQueryStrategy::GridInputOrder
                   : SpatialQueryStrategy::BruteForce;
    }

    return point_count >= profile.grid_unordered_min_points
               ? SpatialQueryStrategy::GridUnordered
               : SpatialQueryStrategy::BruteForce;
}

}  // namespace sc2opt::kernel::hot

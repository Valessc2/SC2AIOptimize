#include "sc2opt/kernel/compute/Influence.hpp"

#include <algorithm>
#include <cmath>

namespace sc2opt::kernel::compute {
namespace {

[[nodiscard]] bool ValidSpec(const RadialInfluenceSpec& spec) noexcept
{
    return std::isfinite(spec.center.x) && std::isfinite(spec.center.y) &&
           std::isfinite(spec.inner_radius) && std::isfinite(spec.outer_radius) &&
           std::isfinite(spec.inner_value) && std::isfinite(spec.outer_value) &&
           spec.inner_radius >= 0.0f && spec.outer_radius >= spec.inner_radius;
}

[[nodiscard]] bool EvaluateCell(float x,
                                float y,
                                const RadialInfluenceSpec& spec,
                                float& addition) noexcept
{
    const float dx = x - spec.center.x;
    const float dy = y - spec.center.y;
    const float distance_squared = dx * dx + dy * dy;
    const float outer_squared = spec.outer_radius * spec.outer_radius;

    const bool inside_outer = spec.outer_boundary == hot::BoundaryMode::Inclusive
                                  ? distance_squared <= outer_squared
                                  : distance_squared < outer_squared;
    if (!inside_outer)
        return false;

    const float inner_squared = spec.inner_radius * spec.inner_radius;
    if (distance_squared <= inner_squared || spec.outer_radius == spec.inner_radius)
    {
        addition = spec.inner_value;
        return true;
    }

    const float distance = std::sqrt(distance_squared);
    const float t = (distance - spec.inner_radius) / (spec.outer_radius - spec.inner_radius);
    addition = spec.inner_value + (spec.outer_value - spec.inner_value) * t;
    return true;
}

}  // namespace

InfluenceResult ApplyRadialInfluenceReference(model::FloatGridView grid,
                                              RadialInfluenceSpec spec) noexcept
{
    if (!model::IsValid(grid))
        return {InfluenceStatus::InvalidGrid, 0};
    if (!ValidSpec(spec))
        return {InfluenceStatus::InvalidSpec, 0};

    std::size_t touched = 0;
    for (std::uint32_t y = 0; y < grid.height; ++y)
    {
        const std::size_t row = static_cast<std::size_t>(y) * grid.stride;
        for (std::uint32_t x = 0; x < grid.width; ++x)
        {
            float addition = 0.0f;
            if (!EvaluateCell(static_cast<float>(x), static_cast<float>(y), spec, addition))
                continue;
            grid.values[row + x] += addition;
            ++touched;
        }
    }
    return {InfluenceStatus::Ok, touched};
}

InfluenceResult ApplyRadialInfluenceBounded(model::FloatGridView grid,
                                            RadialInfluenceSpec spec) noexcept
{
    if (!model::IsValid(grid))
        return {InfluenceStatus::InvalidGrid, 0};
    if (!ValidSpec(spec))
        return {InfluenceStatus::InvalidSpec, 0};

    const double min_x_raw = std::floor(static_cast<double>(spec.center.x) - spec.outer_radius);
    const double max_x_raw = std::ceil(static_cast<double>(spec.center.x) + spec.outer_radius);
    const double min_y_raw = std::floor(static_cast<double>(spec.center.y) - spec.outer_radius);
    const double max_y_raw = std::ceil(static_cast<double>(spec.center.y) + spec.outer_radius);

    const double grid_max_x = static_cast<double>(grid.width - 1);
    const double grid_max_y = static_cast<double>(grid.height - 1);
    if (max_x_raw < 0.0 || max_y_raw < 0.0 || min_x_raw > grid_max_x || min_y_raw > grid_max_y)
        return {InfluenceStatus::Ok, 0};

    const std::uint32_t min_x = static_cast<std::uint32_t>(std::clamp(min_x_raw, 0.0, grid_max_x));
    const std::uint32_t max_x = static_cast<std::uint32_t>(std::clamp(max_x_raw, 0.0, grid_max_x));
    const std::uint32_t min_y = static_cast<std::uint32_t>(std::clamp(min_y_raw, 0.0, grid_max_y));
    const std::uint32_t max_y = static_cast<std::uint32_t>(std::clamp(max_y_raw, 0.0, grid_max_y));

    std::size_t touched = 0;
    for (std::uint32_t y = min_y; y <= max_y; ++y)
    {
        const std::size_t row = static_cast<std::size_t>(y) * grid.stride;
        for (std::uint32_t x = min_x; x <= max_x; ++x)
        {
            float addition = 0.0f;
            if (!EvaluateCell(static_cast<float>(x), static_cast<float>(y), spec, addition))
                continue;
            grid.values[row + x] += addition;
            ++touched;
        }
    }
    return {InfluenceStatus::Ok, touched};
}

}  // namespace sc2opt::kernel::compute

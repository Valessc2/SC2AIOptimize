#pragma once

#include "sc2opt/kernel/hot/Geometry.hpp"

#include <cmath>

namespace sc2opt::kernel::hot {

struct Circle2 {
    Vec2 center;
    float radius = 0.0f;
};

// SC2-style center distance with both collision radii included. Negative inputs fail closed.
[[nodiscard]] constexpr bool WithinEdgeRange(Circle2 source,
                                             Circle2 target,
                                             float range,
                                             BoundaryMode boundary = BoundaryMode::Inclusive) noexcept
{
    if (source.radius < 0.0f || target.radius < 0.0f || range < 0.0f)
        return false;
    const float effective = source.radius + target.radius + range;
    const float distance_squared = DistanceSquared(source.center, target.center);
    const float effective_squared = effective * effective;
    return boundary == BoundaryMode::Inclusive ? distance_squared <= effective_squared
                                               : distance_squared < effective_squared;
}

// `facing_unit` is a pre-normalized direction owned/cached by the caller. `minimum_dot`
// represents cos(max angular error) and must be in [-1, 1]. Coincident points are treated as
// facing because no turn is required. This avoids atan2 and avoids normalizing the facing vector
// inside a hot query.
[[nodiscard]] inline bool WithinFacingCone(Vec2 origin,
                                           Vec2 facing_unit,
                                           Vec2 target,
                                           float minimum_dot,
                                           BoundaryMode boundary = BoundaryMode::Inclusive) noexcept
{
    if (minimum_dot < -1.0f || minimum_dot > 1.0f)
        return false;

    const float dx = target.x - origin.x;
    const float dy = target.y - origin.y;
    const float distance_squared = dx * dx + dy * dy;
    if (distance_squared == 0.0f)
        return true;

    const float inverse_distance = 1.0f / std::sqrt(distance_squared);
    const float dot = facing_unit.x * (dx * inverse_distance) +
                      facing_unit.y * (dy * inverse_distance);
    return boundary == BoundaryMode::Inclusive ? dot >= minimum_dot : dot > minimum_dot;
}

}  // namespace sc2opt::kernel::hot

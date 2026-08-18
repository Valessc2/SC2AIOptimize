#pragma once

#include <cmath>
#include <cstdint>

namespace sc2opt::kernel::hot {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

enum class BoundaryMode : std::uint8_t {
    Strict = 0,
    Inclusive
};

[[nodiscard]] constexpr float DistanceSquared(Vec2 a, Vec2 b) noexcept
{
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

[[nodiscard]] constexpr bool WithinRadius(Vec2 a,
                                          Vec2 b,
                                          float radius,
                                          BoundaryMode boundary = BoundaryMode::Inclusive) noexcept
{
    if (radius < 0.0f)
        return false;
    const float distance_squared = DistanceSquared(a, b);
    const float radius_squared = radius * radius;
    return boundary == BoundaryMode::Inclusive ? distance_squared <= radius_squared
                                               : distance_squared < radius_squared;
}

[[nodiscard]] inline float Distance(Vec2 a, Vec2 b) noexcept
{
    return std::sqrt(DistanceSquared(a, b));
}

}  // namespace sc2opt::kernel::hot

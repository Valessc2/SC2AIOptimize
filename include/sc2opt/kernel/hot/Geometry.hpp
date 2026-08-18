#pragma once

#include <cmath>

namespace sc2opt::kernel::hot {

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;
};

[[nodiscard]] constexpr float DistanceSquared(Vec2 a, Vec2 b) noexcept
{
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

[[nodiscard]] constexpr bool WithinRadius(Vec2 a, Vec2 b, float radius) noexcept
{
    if (radius < 0.0f)
        return false;
    return DistanceSquared(a, b) <= radius * radius;
}

[[nodiscard]] inline float Distance(Vec2 a, Vec2 b) noexcept
{
    return std::sqrt(DistanceSquared(a, b));
}

}  // namespace sc2opt::kernel::hot

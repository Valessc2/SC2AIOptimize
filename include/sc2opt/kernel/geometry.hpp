#pragma once

namespace sc2opt::kernel {

struct Point2 {
    float x = 0.0f;
    float y = 0.0f;
};

[[nodiscard]] constexpr float distance_squared(Point2 a, Point2 b) noexcept
{
    const float dx = a.x - b.x;
    const float dy = a.y - b.y;
    return dx * dx + dy * dy;
}

[[nodiscard]] constexpr bool within_radius(Point2 a, Point2 b, float radius) noexcept
{
    return radius >= 0.0f && distance_squared(a, b) <= radius * radius;
}

} // namespace sc2opt::kernel

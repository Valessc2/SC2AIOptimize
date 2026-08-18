#include "sc2opt/kernel/hot/BatchGeometry.hpp"

namespace sc2opt::kernel::hot {
namespace {

[[nodiscard]] constexpr std::size_t PointCount(std::span<const float> xy) noexcept
{
    return xy.size() / 2;
}

[[nodiscard]] BatchResult Validate(std::span<const float> xy, std::size_t output_size) noexcept
{
    if ((xy.size() & 1u) != 0u)
        return {BatchStatus::InvalidCoordinateCount, 0};

    const std::size_t points = PointCount(xy);
    if (output_size < points)
        return {BatchStatus::OutputTooSmall, 0};

    return {BatchStatus::Ok, points};
}

}  // namespace

BatchResult DistanceSquaredFromPoint(std::span<const float> xy,
                                     Vec2 anchor,
                                     std::span<float> output) noexcept
{
    const BatchResult validation = Validate(xy, output.size());
    if (!validation)
        return validation;

    for (std::size_t i = 0; i < validation.written; ++i)
    {
        const float dx = xy[i * 2] - anchor.x;
        const float dy = xy[i * 2 + 1] - anchor.y;
        output[i] = dx * dx + dy * dy;
    }

    return validation;
}

BatchResult WithinRadiusMaskFromPoint(std::span<const float> xy,
                                      Vec2 anchor,
                                      float radius,
                                      std::span<std::uint8_t> output) noexcept
{
    const BatchResult validation = Validate(xy, output.size());
    if (!validation)
        return validation;
    if (radius < 0.0f)
        return {BatchStatus::InvalidRadius, 0};

    const float radius_squared = radius * radius;
    for (std::size_t i = 0; i < validation.written; ++i)
    {
        const float dx = xy[i * 2] - anchor.x;
        const float dy = xy[i * 2 + 1] - anchor.y;
        output[i] = static_cast<std::uint8_t>(dx * dx + dy * dy <= radius_squared);
    }

    return validation;
}

}  // namespace sc2opt::kernel::hot

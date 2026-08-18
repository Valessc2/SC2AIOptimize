#pragma once

#include "sc2opt/kernel/hot/Geometry.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

namespace sc2opt::kernel::hot {

enum class BatchStatus : std::uint8_t {
    Ok = 0,
    InvalidCoordinateCount,
    OutputTooSmall,
    InvalidRadius
};

struct BatchResult {
    BatchStatus status = BatchStatus::Ok;
    std::size_t written = 0;

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return status == BatchStatus::Ok;
    }
};

// xy is [x0, y0, x1, y1, ...]. Output must have at least xy.size()/2 entries.
// No allocation, logging or exceptions are performed by these kernels.
[[nodiscard]] BatchResult DistanceSquaredFromPoint(std::span<const float> xy,
                                                   Vec2 anchor,
                                                   std::span<float> output) noexcept;

// Writes 1 when a point is within the inclusive radius, otherwise 0.
[[nodiscard]] BatchResult WithinRadiusMaskFromPoint(std::span<const float> xy,
                                                    Vec2 anchor,
                                                    float radius,
                                                    std::span<std::uint8_t> output) noexcept;

}  // namespace sc2opt::kernel::hot

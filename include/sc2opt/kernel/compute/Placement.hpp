#pragma once

#include "sc2opt/model/GridView.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

namespace sc2opt::kernel::compute {

struct FootprintMaskView {
    std::span<const std::uint8_t> values;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t stride = 0;
};

enum class PlacementStatus : std::uint8_t {
    Ok = 0,
    InvalidGrid,
    InvalidFootprint,
    OutputTooSmall
};

struct PlacementFilterResult {
    PlacementStatus status = PlacementStatus::Ok;
    std::size_t written = 0;
    std::size_t required = 0;

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return status == PlacementStatus::Ok;
    }
};

// Candidate anchor is the footprint's top-left cell. Non-zero footprint mask cells require a
// non-zero buildable grid cell. Consumers translate their own anchor/footprint convention here.
[[nodiscard]] bool CanPlaceMask(model::ByteGridView buildable,
                                FootprintMaskView footprint,
                                model::GridPoint anchor) noexcept;

[[nodiscard]] PlacementFilterResult FilterValidPlacements(
    model::ByteGridView buildable,
    FootprintMaskView footprint,
    std::span<const model::GridPoint> candidates,
    std::span<std::uint32_t> output) noexcept;

}  // namespace sc2opt::kernel::compute

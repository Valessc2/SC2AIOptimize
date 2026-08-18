#include "sc2opt/kernel/compute/Placement.hpp"

namespace sc2opt::kernel::compute {
namespace {

[[nodiscard]] bool ValidFootprint(FootprintMaskView footprint) noexcept
{
    return model::GridShapeFits(footprint.values.size(),
                                footprint.width,
                                footprint.height,
                                footprint.stride);
}

}  // namespace

bool CanPlaceMask(model::ByteGridView buildable,
                  FootprintMaskView footprint,
                  model::GridPoint anchor) noexcept
{
    if (!model::IsValid(buildable) || !ValidFootprint(footprint))
        return false;
    if (footprint.width > buildable.width || footprint.height > buildable.height)
        return false;
    if (anchor.x > buildable.width - footprint.width ||
        anchor.y > buildable.height - footprint.height)
    {
        return false;
    }

    for (std::uint32_t y = 0; y < footprint.height; ++y)
    {
        const std::size_t footprint_row = static_cast<std::size_t>(y) * footprint.stride;
        const std::size_t grid_row = static_cast<std::size_t>(anchor.y + y) * buildable.stride;
        for (std::uint32_t x = 0; x < footprint.width; ++x)
        {
            if (footprint.values[footprint_row + x] == 0)
                continue;
            if (buildable.values[grid_row + anchor.x + x] == 0)
                return false;
        }
    }

    return true;
}

PlacementFilterResult FilterValidPlacements(model::ByteGridView buildable,
                                            FootprintMaskView footprint,
                                            std::span<const model::GridPoint> candidates,
                                            std::span<std::uint32_t> output) noexcept
{
    if (!model::IsValid(buildable))
        return {PlacementStatus::InvalidGrid, 0, 0};
    if (!ValidFootprint(footprint))
        return {PlacementStatus::InvalidFootprint, 0, 0};

    std::size_t required = 0;
    for (std::size_t i = 0; i < candidates.size(); ++i)
    {
        if (!CanPlaceMask(buildable, footprint, candidates[i]))
            continue;
        if (required < output.size())
            output[required] = static_cast<std::uint32_t>(i);
        ++required;
    }

    if (required > output.size())
        return {PlacementStatus::OutputTooSmall, 0, required};
    return {PlacementStatus::Ok, required, required};
}

}  // namespace sc2opt::kernel::compute

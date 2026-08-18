#include "sc2opt/kernel/hot/SpatialGrid.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

namespace sc2opt::kernel::hot {
namespace {

using CellKey = std::uint64_t;

[[nodiscard]] CellKey MakeCellKey(std::int32_t x, std::int32_t y) noexcept
{
    return (static_cast<CellKey>(static_cast<std::uint32_t>(x)) << 32) |
           static_cast<std::uint32_t>(y);
}

[[nodiscard]] bool CellCoord(float value, float cell_size, std::int32_t& output) noexcept
{
    const double coordinate = std::floor(static_cast<double>(value) / cell_size);
    if (coordinate < static_cast<double>(std::numeric_limits<std::int32_t>::min()) ||
        coordinate > static_cast<double>(std::numeric_limits<std::int32_t>::max()))
    {
        return false;
    }
    output = static_cast<std::int32_t>(coordinate);
    return true;
}

[[nodiscard]] SpatialStatus ValidateCoordinates(std::span<const float> xy) noexcept
{
    if ((xy.size() & 1u) != 0u)
        return SpatialStatus::InvalidCoordinateCount;
    if (xy.size() / 2 > std::numeric_limits<std::uint32_t>::max())
        return SpatialStatus::TooManyPoints;
    for (float value : xy)
    {
        if (!std::isfinite(value))
            return SpatialStatus::NonFiniteCoordinate;
    }
    return SpatialStatus::Ok;
}

[[nodiscard]] bool ValidQuery(Vec2 center, float radius) noexcept
{
    return std::isfinite(center.x) && std::isfinite(center.y) && std::isfinite(radius) &&
           radius >= 0.0f;
}

[[nodiscard]] bool Inside(float distance_squared,
                          float radius_squared,
                          BoundaryMode boundary) noexcept
{
    return boundary == BoundaryMode::Inclusive ? distance_squared <= radius_squared
                                               : distance_squared < radius_squared;
}

}  // namespace

SpatialQueryResult RadiusQueryBruteForce(std::span<const float> xy,
                                         Vec2 center,
                                         float radius,
                                         std::span<std::uint32_t> output,
                                         BoundaryMode boundary) noexcept
{
    const SpatialStatus coordinates = ValidateCoordinates(xy);
    if (coordinates != SpatialStatus::Ok)
        return {coordinates, 0, 0};
    if (!ValidQuery(center, radius))
        return {SpatialStatus::InvalidQuery, 0, 0};

    const float radius_squared = radius * radius;
    std::size_t required = 0;
    const std::size_t points = xy.size() / 2;
    for (std::size_t i = 0; i < points; ++i)
    {
        const float dx = xy[i * 2] - center.x;
        const float dy = xy[i * 2 + 1] - center.y;
        if (!Inside(dx * dx + dy * dy, radius_squared, boundary))
            continue;

        if (required < output.size())
            output[required] = static_cast<std::uint32_t>(i);
        ++required;
    }

    if (required > output.size())
        return {SpatialStatus::OutputTooSmall, 0, required};
    return {SpatialStatus::Ok, required, required};
}

SpatialStatus SpatialGrid2D::SetCellSize(float cell_size) noexcept
{
    if (!std::isfinite(cell_size) || cell_size <= 0.0f)
    {
        ready_ = false;
        point_count_ = 0;
        active_cell_count_ = 0;
        return SpatialStatus::InvalidCellSize;
    }

    if (cell_size != cell_size_)
    {
        cell_size_ = cell_size;
        cells_.clear();
        touched_cells_.clear();
        point_count_ = 0;
        active_cell_count_ = 0;
        ready_ = false;
    }
    return SpatialStatus::Ok;
}

SpatialStatus SpatialGrid2D::Rebuild(std::span<const float> xy) noexcept
{
    const SpatialStatus coordinates = ValidateCoordinates(xy);
    if (coordinates != SpatialStatus::Ok)
    {
        ready_ = false;
        point_count_ = 0;
        active_cell_count_ = 0;
        return coordinates;
    }
    if (!std::isfinite(cell_size_) || cell_size_ <= 0.0f)
    {
        ready_ = false;
        point_count_ = 0;
        active_cell_count_ = 0;
        return SpatialStatus::InvalidCellSize;
    }

    const std::size_t points = xy.size() / 2;
    for (std::size_t i = 0; i < points; ++i)
    {
        std::int32_t x = 0;
        std::int32_t y = 0;
        if (!CellCoord(xy[i * 2], cell_size_, x) || !CellCoord(xy[i * 2 + 1], cell_size_, y))
        {
            ready_ = false;
            point_count_ = 0;
            active_cell_count_ = 0;
            return SpatialStatus::NonFiniteCoordinate;
        }
    }

    for (CellKey key : touched_cells_)
    {
        const auto found = cells_.find(key);
        if (found != cells_.end())
            found->second.clear();
    }
    touched_cells_.clear();

    const float current_capacity =
        static_cast<float>(cells_.bucket_count()) * cells_.max_load_factor();
    if (static_cast<float>(points) > current_capacity)
        cells_.reserve(points);
    if (touched_cells_.capacity() < points)
        touched_cells_.reserve(points);

    for (std::size_t i = 0; i < points; ++i)
    {
        std::int32_t cell_x = 0;
        std::int32_t cell_y = 0;
        static_cast<void>(CellCoord(xy[i * 2], cell_size_, cell_x));
        static_cast<void>(CellCoord(xy[i * 2 + 1], cell_size_, cell_y));
        const CellKey key = MakeCellKey(cell_x, cell_y);

        auto found = cells_.find(key);
        if (found == cells_.end())
            found = cells_.emplace(key, std::vector<Entry>{}).first;
        if (found->second.empty())
            touched_cells_.push_back(key);
        found->second.push_back(
            {xy[i * 2], xy[i * 2 + 1], static_cast<std::uint32_t>(i)});
    }

    point_count_ = points;
    active_cell_count_ = touched_cells_.size();
    ready_ = true;
    return SpatialStatus::Ok;
}

SpatialQueryResult SpatialGrid2D::Query(Vec2 center,
                                        float radius,
                                        std::span<std::uint32_t> output,
                                        SpatialOrder order,
                                        BoundaryMode boundary) const noexcept
{
    if (!ready_)
        return {SpatialStatus::NotReady, 0, 0};
    if (!ValidQuery(center, radius))
        return {SpatialStatus::InvalidQuery, 0, 0};

    std::int32_t min_x = 0;
    std::int32_t max_x = 0;
    std::int32_t min_y = 0;
    std::int32_t max_y = 0;
    if (!CellCoord(center.x - radius, cell_size_, min_x) ||
        !CellCoord(center.x + radius, cell_size_, max_x) ||
        !CellCoord(center.y - radius, cell_size_, min_y) ||
        !CellCoord(center.y + radius, cell_size_, max_y))
    {
        return {SpatialStatus::InvalidQuery, 0, 0};
    }

    const float radius_squared = radius * radius;
    std::size_t required = 0;

    for (std::int64_t x = min_x; x <= max_x; ++x)
    {
        for (std::int64_t y = min_y; y <= max_y; ++y)
        {
            const auto found = cells_.find(
                MakeCellKey(static_cast<std::int32_t>(x), static_cast<std::int32_t>(y)));
            if (found == cells_.end())
                continue;

            for (const Entry& entry : found->second)
            {
                const float dx = entry.x - center.x;
                const float dy = entry.y - center.y;
                if (!Inside(dx * dx + dy * dy, radius_squared, boundary))
                    continue;

                if (required < output.size())
                    output[required] = entry.index;
                ++required;
            }
        }
    }

    if (required > output.size())
        return {SpatialStatus::OutputTooSmall, 0, required};

    if (order == SpatialOrder::InputOrder)
        std::sort(output.begin(), output.begin() + static_cast<std::ptrdiff_t>(required));

    return {SpatialStatus::Ok, required, required};
}

}  // namespace sc2opt::kernel::hot

#include "sc2opt/kernel/spatial_index.hpp"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace sc2opt::kernel {

SpatialIndex::SpatialIndex(float cell_size) : cell_size_(cell_size)
{
    if (!(cell_size_ > 0.0f))
        throw std::invalid_argument("SpatialIndex cell_size must be positive");
}

void SpatialIndex::reserve(std::size_t entity_count)
{
    cells_.reserve(std::max<std::size_t>(32, entity_count));
    touched_cells_.reserve(entity_count);
}

int SpatialIndex::cell_coord(float value) const noexcept
{
    return static_cast<int>(std::floor(value / cell_size_));
}

SpatialIndex::CellKey SpatialIndex::cell_key(int x, int y) noexcept
{
    const auto ux = static_cast<std::uint32_t>(x);
    const auto uy = static_cast<std::uint32_t>(y);
    return (static_cast<CellKey>(ux) << 32u) | static_cast<CellKey>(uy);
}

void SpatialIndex::clear_touched_cells() noexcept
{
    for (const CellKey key : touched_cells_)
    {
        const auto it = cells_.find(key);
        if (it != cells_.end())
            it->second.clear();
    }
    touched_cells_.clear();
}

void SpatialIndex::rebuild(std::span<const EntityView> entities)
{
    clear_touched_cells();
    entities_ = entities;
    reserve(entities.size());

    for (std::size_t i = 0; i < entities.size(); ++i)
    {
        const auto& entity = entities[i];
        const CellKey key = cell_key(cell_coord(entity.position.x), cell_coord(entity.position.y));
        auto [it, inserted] = cells_.try_emplace(key);
        (void)inserted;
        if (it->second.empty())
            touched_cells_.push_back(key);
        it->second.push_back(static_cast<std::uint32_t>(i));
    }
}

void SpatialIndex::query_radius(Point2 position, float radius, std::vector<std::uint32_t>& out) const
{
    out.clear();
    if (radius < 0.0f)
        return;

    const int min_x = cell_coord(position.x - radius);
    const int max_x = cell_coord(position.x + radius);
    const int min_y = cell_coord(position.y - radius);
    const int max_y = cell_coord(position.y + radius);
    const float radius_sq = radius * radius;

    for (int x = min_x; x <= max_x; ++x)
    {
        for (int y = min_y; y <= max_y; ++y)
        {
            const auto it = cells_.find(cell_key(x, y));
            if (it == cells_.end())
                continue;

            for (const std::uint32_t index : it->second)
            {
                const auto offset = static_cast<std::size_t>(index);
                if (offset >= entities_.size())
                    continue;
                if (distance_squared(position, entities_[offset].position) <= radius_sq)
                    out.push_back(index);
            }
        }
    }
}

} // namespace sc2opt::kernel

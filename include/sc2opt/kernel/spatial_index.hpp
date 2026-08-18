#pragma once

#include "sc2opt/kernel/geometry.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <unordered_map>
#include <vector>

namespace sc2opt::kernel {

struct EntityView {
    std::uint64_t stable_id = 0;
    Point2 position{};
    float radius = 0.0f;
};

// Framework-neutral Z2-style candidate reducer. The source span is non-owning and must remain
// valid until the next rebuild/destruction. Output is caller-owned to avoid hidden allocations.
class SpatialIndex final {
public:
    explicit SpatialIndex(float cell_size = 4.0f);

    void reserve(std::size_t entity_count);
    void rebuild(std::span<const EntityView> entities);
    void query_radius(Point2 position, float radius, std::vector<std::uint32_t>& out) const;

    [[nodiscard]] std::size_t size() const noexcept { return entities_.size(); }
    [[nodiscard]] float cell_size() const noexcept { return cell_size_; }

private:
    using CellKey = std::uint64_t;

    [[nodiscard]] int cell_coord(float value) const noexcept;
    [[nodiscard]] static CellKey cell_key(int x, int y) noexcept;
    void clear_touched_cells() noexcept;

    float cell_size_;
    std::span<const EntityView> entities_{};
    std::unordered_map<CellKey, std::vector<std::uint32_t>> cells_;
    std::vector<CellKey> touched_cells_;
};

} // namespace sc2opt::kernel

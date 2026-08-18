#pragma once

#include "sc2opt/kernel/hot/Geometry.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <unordered_map>
#include <vector>

namespace sc2opt::kernel::hot {

enum class SpatialStatus : std::uint8_t {
    Ok = 0,
    NotReady,
    InvalidCellSize,
    InvalidCoordinateCount,
    TooManyPoints,
    NonFiniteCoordinate,
    InvalidQuery,
    OutputTooSmall
};

enum class SpatialOrder : std::uint8_t {
    Unordered = 0,
    InputOrder
};

struct SpatialQueryResult {
    SpatialStatus status = SpatialStatus::Ok;
    std::size_t written = 0;
    std::size_t required = 0;

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return status == SpatialStatus::Ok;
    }
};

// Exact O(n) reference/fallback query over [x0,y0,x1,y1,...]. Results are input-index ordered.
[[nodiscard]] SpatialQueryResult RadiusQueryBruteForce(
    std::span<const float> xy,
    Vec2 center,
    float radius,
    std::span<std::uint32_t> output,
    BoundaryMode boundary = BoundaryMode::Inclusive) noexcept;

class SpatialGrid2D {
public:
    SpatialGrid2D() = default;

    [[nodiscard]] SpatialStatus SetCellSize(float cell_size) noexcept;
    [[nodiscard]] SpatialStatus Rebuild(std::span<const float> xy) noexcept;

    [[nodiscard]] SpatialQueryResult Query(
        Vec2 center,
        float radius,
        std::span<std::uint32_t> output,
        SpatialOrder order = SpatialOrder::InputOrder,
        BoundaryMode boundary = BoundaryMode::Inclusive) const noexcept;

    [[nodiscard]] float CellSize() const noexcept { return cell_size_; }
    [[nodiscard]] std::size_t PointCount() const noexcept { return point_count_; }
    [[nodiscard]] std::size_t ActiveCellCount() const noexcept { return active_cell_count_; }
    [[nodiscard]] bool Ready() const noexcept { return ready_; }

private:
    struct Entry {
        float x = 0.0f;
        float y = 0.0f;
        std::uint32_t index = 0;
    };

    using CellKey = std::uint64_t;

    float cell_size_ = 4.0f;
    std::unordered_map<CellKey, std::vector<Entry>> cells_;
    std::vector<CellKey> touched_cells_;
    std::size_t point_count_ = 0;
    std::size_t active_cell_count_ = 0;
    bool ready_ = false;
};

}  // namespace sc2opt::kernel::hot

#pragma once

#include "sc2opt/kernel/hot/Geometry.hpp"
#include "sc2opt/model/GridView.hpp"

#include <cstddef>
#include <cstdint>

namespace sc2opt::kernel::compute {

enum class InfluenceStatus : std::uint8_t { Ok = 0, InvalidGrid, InvalidSpec };

struct RadialInfluenceSpec {
    hot::Vec2 center;
    float inner_radius = 0.0f;
    float outer_radius = 0.0f;
    float inner_value = 0.0f;
    float outer_value = 0.0f;
    hot::BoundaryMode outer_boundary = hot::BoundaryMode::Inclusive;
};

struct InfluenceResult {
    InfluenceStatus status = InfluenceStatus::Ok;
    std::size_t touched = 0;

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return status == InfluenceStatus::Ok;
    }
};

// Exact reference scans every logical grid cell.
[[nodiscard]] InfluenceResult ApplyRadialInfluenceReference(
    model::FloatGridView grid,
    RadialInfluenceSpec spec) noexcept;

// Candidate scans only the clamped bounding square, rejects by squared distance, and calls sqrt
// only in the falloff ring. Semantics must remain equivalent to the reference implementation.
[[nodiscard]] InfluenceResult ApplyRadialInfluenceBounded(
    model::FloatGridView grid,
    RadialInfluenceSpec spec) noexcept;

}  // namespace sc2opt::kernel::compute

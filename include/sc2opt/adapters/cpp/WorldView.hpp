#pragma once

#include <cstdint>
#include <span>
#include <type_traits>

namespace sc2opt::adapters::cpp {

enum UnitFlag : std::uint32_t {
    UnitAlive = 1u << 0,
    UnitFlying = 1u << 1,
    UnitStructure = 1u << 2,
    UnitVisible = 1u << 3
};

struct UnitView {
    std::uint64_t tag = 0;
    std::uint32_t consumer_type_id = 0;
    std::uint32_t flags = 0;
    float x = 0.0f;
    float y = 0.0f;
    float health = 0.0f;
    float shields = 0.0f;
    float energy = 0.0f;
    float radius = 0.0f;
};

struct WorldView {
    std::uint32_t frame = 0;
    std::span<const UnitView> self;
    std::span<const UnitView> enemy;
    std::span<const UnitView> neutral;
};

static_assert(std::is_trivially_copyable_v<UnitView>);
static_assert(std::is_standard_layout_v<UnitView>);

}  // namespace sc2opt::adapters::cpp

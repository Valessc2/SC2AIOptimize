#pragma once

#include <cstdint>
#include <string_view>

namespace sc2opt::registry {

enum class Race : std::uint8_t {
    Neutral = 0,
    Terran = 1,
    Protoss = 2,
    Zerg = 3,
};

enum class Movement : std::uint8_t {
    Ground = 0,
    Flying = 1,
    Other = 2,
};

enum Attribute : std::uint32_t {
    Light = 1u << 0,
    Armored = 1u << 1,
    Biological = 1u << 2,
    Mechanical = 1u << 3,
    Robotic = 1u << 4,
    Psionic = 1u << 5,
    Massive = 1u << 6,
    Structure = 1u << 7,
    Hover = 1u << 8,
    Heroic = 1u << 9,
    Summoned = 1u << 10,
};

using UnitIndex = std::uint32_t;
inline constexpr UnitIndex kInvalidUnitIndex = UINT32_MAX;

struct RegistryMetadata {
    std::string_view sc2_build;
    std::string_view data_version;
    std::uint32_t schema_version = 1;
};

// Foundation schema derived from the proven MMEvo Z1 static-data shape. It intentionally
// remains framework-neutral and extensible; weapon, ability and footprint detail will be
// promoted as separate records rather than bloating every UnitStatic row.
struct UnitStatic {
    std::string_view catalog_name;
    Race race = Race::Neutral;
    Movement movement = Movement::Other;
    std::uint32_t attributes = 0;
    float life_max = 0.0f;
    float shields_max = 0.0f;
    float energy_max = 0.0f;
    float life_armor = 0.0f;
    float shield_armor = 0.0f;
    float speed = 0.0f;
    float radius = 0.0f;
    float sight = 0.0f;
    float food = 0.0f;
    std::int32_t minerals = 0;
    std::int32_t vespene = 0;
    std::uint16_t weapon_count = 0;
    std::uint16_t ability_count = 0;
    bool is_multiplayer = false;
    bool has_footprint = false;
};

} // namespace sc2opt::registry

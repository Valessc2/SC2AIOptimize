#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace sc2opt::registry {

enum class Race : std::uint8_t { Neutral = 0, Terran = 1, Protoss = 2, Zerg = 3 };
enum class Mover : std::uint8_t { Ground = 0, Fly = 1, Other = 2 };

enum AttributeMask : std::uint32_t {
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
    Summoned = 1u << 10
};

struct UnitStatic {
    const char* catalog_name;
    Race race;
    Mover mover;
    bool is_multiplayer;
    std::uint32_t attributes;
    float life_max;
    float shields_max;
    float energy_max;
    float life_armor;
    float shield_armor;
    float speed_raw_catalog;
    float radius;
    float sight;
    float food_raw_catalog;
    std::int32_t minerals;
    std::int32_t vespene;
    std::uint16_t weapon_count;
    std::uint16_t ability_count;
    bool has_footprint;
};

struct BuildIdentity {
    std::string_view sc2_build;
    std::string_view data_version;
};

struct RegistryView {
    BuildIdentity identity;
    std::span<const UnitStatic> units;
};

inline constexpr std::string_view kBase75689Build = "75689";
inline constexpr std::string_view kBase75689DataVersion = "B89B5D6FA7CBF6452E721311BFBC6CB2";
inline constexpr std::size_t kBase75689ExpectedUnitCount = 245;

[[nodiscard]] RegistryView Base75689() noexcept;

// Name lookup is intentionally a simple, allocation-free startup/adapter operation.
// Hot consumers should build a dense consumer-ID -> UnitStatic pointer map once, then use that.
[[nodiscard]] const UnitStatic* FindByCatalogName(RegistryView registry,
                                                  std::string_view catalog_name) noexcept;

}  // namespace sc2opt::registry

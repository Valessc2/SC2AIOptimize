#include "sc2opt/registry/SC2Registry.hpp"

namespace sc2opt::registry {
namespace {

const UnitStatic kUnits[] = {
#include "data/75689/units_a.inc"
#include "data/75689/units_b.inc"
};

static_assert(sizeof(kUnits) / sizeof(kUnits[0]) == kBase75689ExpectedUnitCount,
              "Base75689 registry count changed: regenerate and re-certify provenance");

}  // namespace

RegistryView Base75689() noexcept
{
    return {{kBase75689Build, kBase75689DataVersion}, kUnits};
}

const UnitStatic* FindByCatalogName(RegistryView registry, std::string_view catalog_name) noexcept
{
    for (const UnitStatic& unit : registry.units)
    {
        if (catalog_name == unit.catalog_name)
            return &unit;
    }
    return nullptr;
}

}  // namespace sc2opt::registry

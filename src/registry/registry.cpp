#include "sc2opt/registry/registry.hpp"

namespace sc2opt::registry {

const UnitStatic* RegistryView::unit(UnitIndex index) const noexcept
{
    const auto offset = static_cast<std::size_t>(index);
    return offset < units_.size() ? &units_[offset] : nullptr;
}

UnitIndex RegistryView::find_by_catalog_name(std::string_view name) const noexcept
{
    for (std::size_t i = 0; i < units_.size(); ++i)
    {
        if (units_[i].catalog_name == name)
            return static_cast<UnitIndex>(i);
    }
    return kInvalidUnitIndex;
}

} // namespace sc2opt::registry

#pragma once

#include "sc2opt/registry/types.hpp"

#include <span>
#include <string_view>

namespace sc2opt::registry {

// Non-owning immutable view. Generated registry storage owns the data; hot consumers pay no
// allocation cost. Catalog-name lookup is a startup/integration helper, not a hot-path API.
class RegistryView final {
public:
    constexpr RegistryView(RegistryMetadata metadata, std::span<const UnitStatic> units) noexcept
        : metadata_(metadata), units_(units) {}

    [[nodiscard]] constexpr const RegistryMetadata& metadata() const noexcept { return metadata_; }
    [[nodiscard]] constexpr std::span<const UnitStatic> units() const noexcept { return units_; }
    [[nodiscard]] const UnitStatic* unit(UnitIndex index) const noexcept;
    [[nodiscard]] UnitIndex find_by_catalog_name(std::string_view name) const noexcept;

private:
    RegistryMetadata metadata_;
    std::span<const UnitStatic> units_;
};

} // namespace sc2opt::registry

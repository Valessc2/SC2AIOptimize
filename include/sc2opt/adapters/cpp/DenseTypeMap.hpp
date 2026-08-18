#pragma once

#include "sc2opt/registry/SC2Registry.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace sc2opt::adapters::cpp {

struct TypeNameBinding {
    std::uint32_t consumer_type_id = 0;
    std::string_view catalog_name;
};

enum class TypeMapStatus : std::uint8_t {
    Ok = 0,
    OutputTooSmall,
    DuplicateConsumerTypeId,
    EmptyCatalogName
};

struct TypeMapReport {
    TypeMapStatus status = TypeMapStatus::Ok;
    std::size_t required_size = 0;
    std::size_t mapped = 0;
    std::size_t unmatched = 0;

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return status == TypeMapStatus::Ok;
    }
};

// Startup/adapter helper. Unknown catalog names are not contradictions: the build-pinned
// registry may intentionally contain only a subset. Duplicate consumer IDs are ambiguous and
// fail closed. `output` is always cleared before a successful mapping pass.
[[nodiscard]] TypeMapReport BuildDenseTypeMap(
    registry::RegistryView source,
    std::span<const TypeNameBinding> bindings,
    std::span<const registry::UnitStatic*> output) noexcept;

}  // namespace sc2opt::adapters::cpp

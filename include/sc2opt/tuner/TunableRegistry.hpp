#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace sc2opt::tuner {

enum class TunableKind : std::uint8_t { Continuous, Integer, Boolean, Choice };

enum TunableFlag : std::uint32_t {
    TunableOnlineSafe = 1u << 0,
    TunableRequiresReset = 1u << 1,
    TunableStrategic = 1u << 2
};

struct TunableSpec {
    std::uint32_t id = 0;
    std::string_view name;
    TunableKind kind = TunableKind::Continuous;
    double baseline = 0.0;
    double minimum = 0.0;
    double maximum = 0.0;
    double step = 0.0;
    std::uint32_t flags = 0;
};

[[nodiscard]] constexpr bool IsValid(const TunableSpec& spec) noexcept
{
    if (spec.name.empty() || spec.minimum > spec.maximum)
        return false;
    if (spec.baseline < spec.minimum || spec.baseline > spec.maximum)
        return false;
    if (spec.kind == TunableKind::Boolean)
        return spec.minimum <= 0.0 && spec.maximum >= 1.0 &&
               (spec.baseline == 0.0 || spec.baseline == 1.0);
    return spec.step > 0.0;
}

enum class TunableRegistryIssue : std::uint8_t {
    None,
    InvalidSpec,
    DuplicateId,
    DuplicateName
};

struct TunableRegistryValidation {
    TunableRegistryIssue issue = TunableRegistryIssue::None;
    std::size_t index = 0;
    std::size_t conflicting_index = 0;

    [[nodiscard]] constexpr bool ok() const noexcept { return issue == TunableRegistryIssue::None; }
};

[[nodiscard]] TunableRegistryValidation ValidateTunableRegistry(
    std::span<const TunableSpec> specs) noexcept;

[[nodiscard]] const TunableSpec* FindTunableById(std::span<const TunableSpec> specs,
                                                std::uint32_t id) noexcept;

[[nodiscard]] const TunableSpec* FindTunableByName(std::span<const TunableSpec> specs,
                                                  std::string_view name) noexcept;

}  // namespace sc2opt::tuner

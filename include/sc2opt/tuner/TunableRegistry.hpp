#pragma once

#include <cmath>
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

[[nodiscard]] inline bool IsIntegralValue(double value) noexcept
{
    return std::isfinite(value) && std::floor(value) == value;
}

[[nodiscard]] inline bool IsValid(const TunableSpec& spec) noexcept
{
    if (spec.name.empty() || !std::isfinite(spec.baseline) || !std::isfinite(spec.minimum) ||
        !std::isfinite(spec.maximum) || !std::isfinite(spec.step) || spec.minimum > spec.maximum)
    {
        return false;
    }

    if (spec.baseline < spec.minimum || spec.baseline > spec.maximum)
        return false;

    switch (spec.kind)
    {
    case TunableKind::Continuous:
        return spec.step > 0.0;
    case TunableKind::Integer:
        return IsIntegralValue(spec.baseline) && IsIntegralValue(spec.minimum) &&
               IsIntegralValue(spec.maximum) && IsIntegralValue(spec.step) && spec.step >= 1.0;
    case TunableKind::Boolean:
        return spec.minimum == 0.0 && spec.maximum == 1.0 &&
               (spec.baseline == 0.0 || spec.baseline == 1.0) &&
               (spec.step == 0.0 || spec.step == 1.0);
    case TunableKind::Choice:
        return IsIntegralValue(spec.baseline) && IsIntegralValue(spec.minimum) &&
               IsIntegralValue(spec.maximum) && spec.minimum >= 0.0 && spec.step == 1.0;
    }
    return false;
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

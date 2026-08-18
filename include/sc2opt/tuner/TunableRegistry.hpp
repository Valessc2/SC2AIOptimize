#pragma once

#include <cstdint>
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
    return spec.kind == TunableKind::Boolean || spec.step >= 0.0;
}

}  // namespace sc2opt::tuner

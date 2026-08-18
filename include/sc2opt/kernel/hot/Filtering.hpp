#pragma once

#include "sc2opt/model/WorldView.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>

namespace sc2opt::kernel::hot {

inline constexpr std::uint32_t kAnyConsumerType = std::numeric_limits<std::uint32_t>::max();

struct UnitFilter {
    std::uint32_t required_flags = 0;
    std::uint32_t excluded_flags = 0;
    std::uint32_t consumer_type_id = kAnyConsumerType;
    float minimum_health = 0.0f;
};

enum class FilterStatus : std::uint8_t { Ok = 0, OutputTooSmall };

struct FilterResult {
    FilterStatus status = FilterStatus::Ok;
    std::size_t written = 0;
    std::size_t required = 0;

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return status == FilterStatus::Ok;
    }
};

[[nodiscard]] FilterResult FilterUnitIndices(std::span<const model::UnitView> units,
                                             UnitFilter filter,
                                             std::span<std::uint32_t> output) noexcept;

}  // namespace sc2opt::kernel::hot

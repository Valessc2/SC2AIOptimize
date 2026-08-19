#define SC2OPT_C_BUILD
#include "sc2opt/adapters/c/sc2opt_c.h"

#include "sc2opt/Version.hpp"
#include "sc2opt/kernel/hot/BatchGeometry.hpp"
#include "sc2opt/registry/SC2Registry.hpp"

#include <span>

namespace {

sc2opt_c_status ToCStatus(sc2opt::kernel::hot::BatchStatus status) noexcept
{
    using sc2opt::kernel::hot::BatchStatus;
    switch (status)
    {
    case BatchStatus::Ok:
        return SC2OPT_C_OK;
    case BatchStatus::OutputTooSmall:
        return SC2OPT_C_OUTPUT_TOO_SMALL;
    case BatchStatus::InvalidCoordinateCount:
    case BatchStatus::InvalidRadius:
        return SC2OPT_C_INVALID_ARGUMENT;
    }
    return SC2OPT_C_INTERNAL_ERROR;
}

bool ValidPointers(const float* xy, size_t xy_count, const void* output, size_t output_count,
                   const size_t* written) noexcept
{
    return written != nullptr && (xy_count == 0 || xy != nullptr) &&
           (output_count == 0 || output != nullptr);
}

}  // namespace

extern "C" {

uint32_t sc2opt_c_api_version_major(void)
{
    return static_cast<uint32_t>(sc2opt::kApiContractMajor);
}

uint32_t sc2opt_c_api_version_minor(void)
{
    return static_cast<uint32_t>(sc2opt::kApiContractMinor);
}

size_t sc2opt_c_registry_unit_count(void)
{
    return sc2opt::registry::Base75689().units.size();
}

sc2opt_c_status sc2opt_c_distance_squared_into(const float* xy,
                                                size_t xy_count,
                                                float anchor_x,
                                                float anchor_y,
                                                float* output,
                                                size_t output_count,
                                                size_t* written)
{
    if (!ValidPointers(xy, xy_count, output, output_count, written))
        return SC2OPT_C_INVALID_ARGUMENT;

    *written = 0;
    const auto result = sc2opt::kernel::hot::DistanceSquaredFromPoint(
        std::span<const float>(xy, xy_count), {anchor_x, anchor_y},
        std::span<float>(output, output_count));
    if (result)
        *written = result.written;
    return ToCStatus(result.status);
}

sc2opt_c_status sc2opt_c_within_radius_mask_into(const float* xy,
                                                  size_t xy_count,
                                                  float anchor_x,
                                                  float anchor_y,
                                                  float radius,
                                                  uint8_t* output,
                                                  size_t output_count,
                                                  size_t* written)
{
    if (!ValidPointers(xy, xy_count, output, output_count, written))
        return SC2OPT_C_INVALID_ARGUMENT;

    *written = 0;
    const auto result = sc2opt::kernel::hot::WithinRadiusMaskFromPoint(
        std::span<const float>(xy, xy_count), {anchor_x, anchor_y}, radius,
        std::span<std::uint8_t>(output, output_count));
    if (result)
        *written = result.written;
    return ToCStatus(result.status);
}

}  // extern "C"

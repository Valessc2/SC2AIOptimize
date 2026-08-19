#define SC2OPT_C_BUILD
#include "sc2opt/adapters/c/sc2opt_c.h"

#include "sc2opt/Version.hpp"
#include "sc2opt/adapters/IntegrationContract.hpp"
#include "sc2opt/kernel/hot/BatchGeometry.hpp"
#include "sc2opt/registry/SC2Registry.hpp"

#include <span>
#include <string_view>

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

sc2opt_c_integration_status ToCIntegrationStatus(sc2opt::adapters::IntegrationStatus status) noexcept
{
    using sc2opt::adapters::IntegrationStatus;
    switch (status)
    {
    case IntegrationStatus::Ready:
        return SC2OPT_C_INTEGRATION_READY;
    case IntegrationStatus::ApiMajorMismatch:
        return SC2OPT_C_INTEGRATION_API_MAJOR_MISMATCH;
    case IntegrationStatus::ApiMinorTooOld:
        return SC2OPT_C_INTEGRATION_API_MINOR_TOO_OLD;
    case IntegrationStatus::UnitViewAbiMismatch:
        return SC2OPT_C_INTEGRATION_UNIT_VIEW_ABI_MISMATCH;
    case IntegrationStatus::RegistryBuildMismatch:
        return SC2OPT_C_INTEGRATION_REGISTRY_BUILD_MISMATCH;
    case IntegrationStatus::RegistryDataVersionMismatch:
        return SC2OPT_C_INTEGRATION_REGISTRY_DATA_VERSION_MISMATCH;
    case IntegrationStatus::MissingCapability:
        return SC2OPT_C_INTEGRATION_MISSING_CAPABILITY;
    }
    return SC2OPT_C_INTEGRATION_MISSING_CAPABILITY;
}

sc2opt::adapters::IntegrationStatus FromCIntegrationStatus(
    sc2opt_c_integration_status status) noexcept
{
    using sc2opt::adapters::IntegrationStatus;
    switch (status)
    {
    case SC2OPT_C_INTEGRATION_READY:
        return IntegrationStatus::Ready;
    case SC2OPT_C_INTEGRATION_API_MAJOR_MISMATCH:
        return IntegrationStatus::ApiMajorMismatch;
    case SC2OPT_C_INTEGRATION_API_MINOR_TOO_OLD:
        return IntegrationStatus::ApiMinorTooOld;
    case SC2OPT_C_INTEGRATION_UNIT_VIEW_ABI_MISMATCH:
        return IntegrationStatus::UnitViewAbiMismatch;
    case SC2OPT_C_INTEGRATION_REGISTRY_BUILD_MISMATCH:
        return IntegrationStatus::RegistryBuildMismatch;
    case SC2OPT_C_INTEGRATION_REGISTRY_DATA_VERSION_MISMATCH:
        return IntegrationStatus::RegistryDataVersionMismatch;
    case SC2OPT_C_INTEGRATION_MISSING_CAPABILITY:
        return IntegrationStatus::MissingCapability;
    }
    return IntegrationStatus::MissingCapability;
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

uint32_t sc2opt_c_unit_view_abi_version(void)
{
    return sc2opt::model::kUnitViewAbiVersion;
}

const char* sc2opt_c_package_version(void)
{
    return SC2OPT_VERSION_STRING;
}

const char* sc2opt_c_registry_build(void)
{
    return sc2opt::registry::kBase75689Build.data();
}

const char* sc2opt_c_registry_data_version(void)
{
    return sc2opt::registry::kBase75689DataVersion.data();
}

size_t sc2opt_c_registry_unit_count(void)
{
    return sc2opt::registry::Base75689().units.size();
}

uint64_t sc2opt_c_integration_capabilities(void)
{
    return sc2opt::adapters::kSharedIntegrationCapabilities;
}

sc2opt_c_integration_status sc2opt_c_check_integration(
    uint32_t expected_api_major,
    uint32_t minimum_api_minor,
    uint32_t expected_unit_view_abi,
    const char* expected_sc2_build,
    const char* expected_data_version,
    uint64_t required_capabilities,
    uint64_t* missing_capabilities)
{
    sc2opt::adapters::IntegrationRequirements requirements{};
    requirements.api_major = static_cast<std::uint16_t>(expected_api_major);
    requirements.minimum_api_minor = static_cast<std::uint16_t>(minimum_api_minor);
    requirements.unit_view_abi = expected_unit_view_abi;
    requirements.sc2_build = expected_sc2_build == nullptr ? std::string_view{} : expected_sc2_build;
    requirements.data_version =
        expected_data_version == nullptr ? std::string_view{} : expected_data_version;
    requirements.required_capabilities = required_capabilities;

    const auto report = sc2opt::adapters::CheckIntegrationCompatibility(requirements);
    if (missing_capabilities != nullptr)
        *missing_capabilities = report.missing_capabilities;
    return ToCIntegrationStatus(report.status);
}

const char* sc2opt_c_integration_status_name(sc2opt_c_integration_status status)
{
    return sc2opt::adapters::IntegrationStatusName(FromCIntegrationStatus(status)).data();
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

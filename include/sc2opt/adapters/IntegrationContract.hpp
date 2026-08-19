#pragma once

#include "sc2opt/Version.hpp"
#include "sc2opt/model/WorldView.hpp"
#include "sc2opt/registry/SC2Registry.hpp"

#include <cstdint>
#include <string_view>

namespace sc2opt::adapters {

enum IntegrationCapability : std::uint64_t {
    IntegrationCpuBaseline = 1ull << 0,
    IntegrationSafeDisable = 1ull << 1,
    IntegrationRegistryIdentity = 1ull << 2,
    IntegrationBatchGeometry = 1ull << 3
};

inline constexpr std::uint64_t kSharedIntegrationCapabilities =
    IntegrationCpuBaseline | IntegrationSafeDisable | IntegrationRegistryIdentity |
    IntegrationBatchGeometry;

enum class IntegrationStatus : std::uint8_t {
    Ready = 0,
    ApiMajorMismatch = 1,
    ApiMinorTooOld = 2,
    UnitViewAbiMismatch = 3,
    RegistryBuildMismatch = 4,
    RegistryDataVersionMismatch = 5,
    MissingCapability = 6
};

struct IntegrationContract {
    std::uint16_t api_major = 0;
    std::uint16_t api_minor = 0;
    std::uint32_t unit_view_abi = 0;
    std::string_view package_version;
    std::string_view sc2_build;
    std::string_view data_version;
    std::uint64_t capabilities = 0;
};

struct IntegrationRequirements {
    std::uint16_t api_major = kApiContractMajor;
    std::uint16_t minimum_api_minor = kApiContractMinor;
    std::uint32_t unit_view_abi = model::kUnitViewAbiVersion;
    std::string_view sc2_build = registry::kBase75689Build;
    std::string_view data_version = registry::kBase75689DataVersion;
    std::uint64_t required_capabilities = kSharedIntegrationCapabilities;
};

struct IntegrationReport {
    IntegrationStatus status = IntegrationStatus::Ready;
    IntegrationContract contract{};
    std::uint64_t missing_capabilities = 0;

    [[nodiscard]] constexpr bool ready() const noexcept
    {
        return status == IntegrationStatus::Ready;
    }
};

[[nodiscard]] inline IntegrationContract CurrentIntegrationContract() noexcept
{
    const auto registry_view = registry::Base75689();
    return {
        kApiContractMajor,
        kApiContractMinor,
        model::kUnitViewAbiVersion,
        kPackageVersion,
        registry_view.identity.sc2_build,
        registry_view.identity.data_version,
        kSharedIntegrationCapabilities,
    };
}

[[nodiscard]] inline IntegrationReport CheckIntegrationCompatibility(
    const IntegrationRequirements& requirements) noexcept
{
    const IntegrationContract contract = CurrentIntegrationContract();
    IntegrationReport report{IntegrationStatus::Ready, contract, 0};

    if (requirements.api_major != contract.api_major)
        report.status = IntegrationStatus::ApiMajorMismatch;
    else if (requirements.minimum_api_minor > contract.api_minor)
        report.status = IntegrationStatus::ApiMinorTooOld;
    else if (requirements.unit_view_abi != contract.unit_view_abi)
        report.status = IntegrationStatus::UnitViewAbiMismatch;
    else if (!requirements.sc2_build.empty() && requirements.sc2_build != contract.sc2_build)
        report.status = IntegrationStatus::RegistryBuildMismatch;
    else if (!requirements.data_version.empty() && requirements.data_version != contract.data_version)
        report.status = IntegrationStatus::RegistryDataVersionMismatch;
    else
    {
        report.missing_capabilities = requirements.required_capabilities & ~contract.capabilities;
        if (report.missing_capabilities != 0)
            report.status = IntegrationStatus::MissingCapability;
    }

    return report;
}

[[nodiscard]] inline constexpr std::string_view IntegrationStatusName(
    IntegrationStatus status) noexcept
{
    switch (status)
    {
    case IntegrationStatus::Ready:
        return "ready";
    case IntegrationStatus::ApiMajorMismatch:
        return "api_major_mismatch";
    case IntegrationStatus::ApiMinorTooOld:
        return "api_minor_too_old";
    case IntegrationStatus::UnitViewAbiMismatch:
        return "unit_view_abi_mismatch";
    case IntegrationStatus::RegistryBuildMismatch:
        return "registry_build_mismatch";
    case IntegrationStatus::RegistryDataVersionMismatch:
        return "registry_data_version_mismatch";
    case IntegrationStatus::MissingCapability:
        return "missing_capability";
    }
    return "unknown";
}

}  // namespace sc2opt::adapters

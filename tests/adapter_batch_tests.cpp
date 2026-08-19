#include "sc2opt/adapters/IntegrationContract.hpp"
#include "sc2opt/adapters/cpp/DenseTypeMap.hpp"
#include "sc2opt/kernel/hot/BatchGeometry.hpp"
#include "sc2opt/registry/SC2Registry.hpp"

#include <array>
#include <cstdint>
#include <iostream>

namespace {

int failures = 0;

void Check(bool condition, const char* message)
{
    if (!condition)
    {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}

void TestBatchGeometry()
{
    using namespace sc2opt::kernel::hot;

    const std::array<float, 6> xy{0.0f, 0.0f, 3.0f, 4.0f, 6.0f, 8.0f};
    std::array<float, 3> distances{};
    const auto distance = DistanceSquaredFromPoint(xy, {0.0f, 0.0f}, distances);
    Check(distance.status == BatchStatus::Ok && distance.written == 3, "distance batch status");
    Check(distances[0] == 0.0f && distances[1] == 25.0f && distances[2] == 100.0f,
          "distance batch values");

    std::array<std::uint8_t, 3> mask{};
    const auto radius = WithinRadiusMaskFromPoint(xy, {0.0f, 0.0f}, 5.0f, mask);
    Check(radius.status == BatchStatus::Ok, "radius batch status");
    Check(mask[0] == 1 && mask[1] == 1 && mask[2] == 0, "inclusive radius mask");

    const std::array<float, 3> odd_xy{1.0f, 2.0f, 3.0f};
    Check(DistanceSquaredFromPoint(odd_xy, {0.0f, 0.0f}, distances).status ==
              BatchStatus::InvalidCoordinateCount,
          "odd coordinate count fails closed");

    std::array<float, 2> too_small{};
    Check(DistanceSquaredFromPoint(xy, {0.0f, 0.0f}, too_small).status ==
              BatchStatus::OutputTooSmall,
          "small output fails closed");
    Check(WithinRadiusMaskFromPoint(xy, {0.0f, 0.0f}, -1.0f, mask).status ==
              BatchStatus::InvalidRadius,
          "negative radius fails closed");
}

void TestDenseTypeMap()
{
    using namespace sc2opt;

    const std::array<adapters::cpp::TypeNameBinding, 3> bindings{{
        {2, "Marine"},
        {5, "Zergling"},
        {8, "NotInPinnedSubset"},
    }};
    std::array<const registry::UnitStatic*, 9> output{};
    const auto report = adapters::cpp::BuildDenseTypeMap(registry::Base75689(), bindings, output);
    Check(report.status == adapters::cpp::TypeMapStatus::Ok, "type map status");
    Check(report.required_size == 9 && report.mapped == 2 && report.unmatched == 1,
          "type map counts");
    Check(output[2] != nullptr && output[5] != nullptr && output[8] == nullptr,
          "type map values");

    std::array<const registry::UnitStatic*, 2> tiny{};
    Check(adapters::cpp::BuildDenseTypeMap(registry::Base75689(), bindings, tiny).status ==
              adapters::cpp::TypeMapStatus::OutputTooSmall,
          "small type map output fails closed");

    const std::array<adapters::cpp::TypeNameBinding, 2> duplicate{{
        {3, "Marine"},
        {3, "Zergling"},
    }};
    Check(adapters::cpp::BuildDenseTypeMap(registry::Base75689(), duplicate, output).status ==
              adapters::cpp::TypeMapStatus::DuplicateConsumerTypeId,
          "duplicate consumer id fails closed");
}

void TestIntegrationContract()
{
    using namespace sc2opt::adapters;

    const auto contract = CurrentIntegrationContract();
    Check(contract.api_major == sc2opt::kApiContractMajor &&
              contract.api_minor == sc2opt::kApiContractMinor,
          "integration contract API identity");
    Check(contract.unit_view_abi == sc2opt::model::kUnitViewAbiVersion,
          "integration contract UnitView ABI identity");
    Check(contract.sc2_build == sc2opt::registry::kBase75689Build &&
              contract.data_version == sc2opt::registry::kBase75689DataVersion,
          "integration contract registry identity");
    Check((contract.capabilities & kSharedIntegrationCapabilities) ==
              kSharedIntegrationCapabilities,
          "integration contract shared capability set");

    Check(CheckIntegrationCompatibility(IntegrationRequirements{}).ready(),
          "default integration requirements are compatible");

    IntegrationRequirements requirements{};
    requirements.api_major += 1u;
    Check(CheckIntegrationCompatibility(requirements).status == IntegrationStatus::ApiMajorMismatch,
          "integration rejects API major mismatch");

    requirements = {};
    requirements.minimum_api_minor = contract.api_minor + 1u;
    Check(CheckIntegrationCompatibility(requirements).status == IntegrationStatus::ApiMinorTooOld,
          "integration rejects too-new API minor requirement");

    requirements = {};
    requirements.unit_view_abi += 1u;
    Check(CheckIntegrationCompatibility(requirements).status == IntegrationStatus::UnitViewAbiMismatch,
          "integration rejects UnitView ABI mismatch");

    requirements = {};
    requirements.sc2_build = "99999";
    Check(CheckIntegrationCompatibility(requirements).status == IntegrationStatus::RegistryBuildMismatch,
          "integration rejects registry build mismatch");

    requirements = {};
    requirements.data_version = "NOT_THE_PINNED_DATA_VERSION";
    Check(CheckIntegrationCompatibility(requirements).status ==
              IntegrationStatus::RegistryDataVersionMismatch,
          "integration rejects registry data-version mismatch");

    requirements = {};
    requirements.required_capabilities |= (1ull << 63);
    const auto missing = CheckIntegrationCompatibility(requirements);
    Check(missing.status == IntegrationStatus::MissingCapability &&
              missing.missing_capabilities == (1ull << 63),
          "integration reports missing capability bits");
}

}  // namespace

int main()
{
    TestBatchGeometry();
    TestDenseTypeMap();
    TestIntegrationContract();

    if (failures == 0)
    {
        std::cout << "SC2AIOptimize adapter/batch tests passed\n";
        return 0;
    }

    std::cerr << failures << " adapter/batch test(s) failed\n";
    return 1;
}

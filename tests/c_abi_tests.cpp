#include "sc2opt/adapters/c/sc2opt_c.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>

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

}  // namespace

int main()
{
    Check(sc2opt_c_api_version_major() == 1u, "C ABI major contract");
    Check(sc2opt_c_api_version_minor() >= 1u, "C ABI additive integration minor contract");
    Check(sc2opt_c_unit_view_abi_version() == 1u, "C ABI UnitView ABI contract");
    Check(std::string(sc2opt_c_package_version()) == "0.1.0-dev", "C ABI package version");
    Check(std::string(sc2opt_c_registry_build()) == "75689", "C ABI registry build");
    Check(std::string(sc2opt_c_registry_data_version()) ==
              "B89B5D6FA7CBF6452E721311BFBC6CB2",
          "C ABI registry data version");
    Check(sc2opt_c_registry_unit_count() > 0u, "C ABI registry access");

    const std::uint64_t required_capabilities =
        static_cast<std::uint64_t>(SC2OPT_C_CAP_CPU_BASELINE) |
        static_cast<std::uint64_t>(SC2OPT_C_CAP_COMPATIBILITY_GATE) |
        static_cast<std::uint64_t>(SC2OPT_C_CAP_REGISTRY_IDENTITY) |
        static_cast<std::uint64_t>(SC2OPT_C_CAP_BATCH_GEOMETRY);
    Check((sc2opt_c_integration_capabilities() & required_capabilities) == required_capabilities,
          "C ABI shared capability set");

    std::uint64_t missing_capabilities = ~std::uint64_t{0};
    auto integration_status = sc2opt_c_check_integration(
        1u, 1u, 1u, "75689", "B89B5D6FA7CBF6452E721311BFBC6CB2",
        required_capabilities, &missing_capabilities);
    Check(integration_status == SC2OPT_C_INTEGRATION_READY && missing_capabilities == 0u,
          "C ABI integration handshake ready");
    Check(std::string(sc2opt_c_integration_status_name(integration_status)) == "ready",
          "C ABI integration status name");

    integration_status = sc2opt_c_check_integration(
        2u, 1u, 1u, "75689", "B89B5D6FA7CBF6452E721311BFBC6CB2",
        required_capabilities, &missing_capabilities);
    Check(integration_status == SC2OPT_C_INTEGRATION_API_MAJOR_MISMATCH,
          "C ABI rejects API major mismatch");

    integration_status = sc2opt_c_check_integration(
        1u, 1u, 1u, nullptr, nullptr, required_capabilities, &missing_capabilities);
    Check(integration_status == SC2OPT_C_INTEGRATION_READY,
          "C ABI permits unconstrained registry identity");

    const std::uint64_t unavailable_capability = std::uint64_t{1} << 63;
    integration_status = sc2opt_c_check_integration(
        1u, 1u, 1u, "75689", "B89B5D6FA7CBF6452E721311BFBC6CB2",
        required_capabilities | unavailable_capability, &missing_capabilities);
    Check(integration_status == SC2OPT_C_INTEGRATION_MISSING_CAPABILITY &&
              missing_capabilities == unavailable_capability,
          "C ABI reports missing capability bits");

    const std::array<float, 6> xy{0.0f, 0.0f, 3.0f, 4.0f, 10.0f, 0.0f};
    std::array<float, 3> distances{};
    std::size_t written = 0;
    Check(sc2opt_c_distance_squared_into(xy.data(), xy.size(), 0.0f, 0.0f,
                                         distances.data(), distances.size(), &written) ==
              SC2OPT_C_OK,
          "C ABI distance call");
    Check(written == 3u && distances[0] == 0.0f && distances[1] == 25.0f &&
              distances[2] == 100.0f,
          "C ABI distance values");

    std::array<std::uint8_t, 3> mask{};
    Check(sc2opt_c_within_radius_mask_into(xy.data(), xy.size(), 0.0f, 0.0f, 5.0f,
                                           mask.data(), mask.size(), &written) == SC2OPT_C_OK,
          "C ABI radius call");
    Check(written == 3u && mask[0] == 1u && mask[1] == 1u && mask[2] == 0u,
          "C ABI radius values");

    Check(sc2opt_c_distance_squared_into(xy.data(), xy.size() - 1u, 0.0f, 0.0f,
                                         distances.data(), distances.size(), &written) ==
              SC2OPT_C_INVALID_ARGUMENT,
          "C ABI rejects odd coordinate count");
    Check(sc2opt_c_distance_squared_into(xy.data(), xy.size(), 0.0f, 0.0f,
                                         distances.data(), 1u, &written) ==
              SC2OPT_C_OUTPUT_TOO_SMALL,
          "C ABI reports output capacity");
    Check(sc2opt_c_distance_squared_into(xy.data(), xy.size(), 0.0f, 0.0f,
                                         distances.data(), distances.size(), nullptr) ==
              SC2OPT_C_INVALID_ARGUMENT,
          "C ABI requires written pointer");

    if (failures == 0)
    {
        std::cout << "SC2AIOptimize C ABI tests passed\n";
        return 0;
    }

    std::cerr << failures << " C ABI test(s) failed\n";
    return 1;
}

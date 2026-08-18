#include "sc2opt/adapters/c/sc2opt_c.h"

#include <array>
#include <cstddef>
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

}  // namespace

int main()
{
    Check(sc2opt_c_api_version_major() == 1u, "C ABI major contract");
    Check(sc2opt_c_registry_unit_count() > 0u, "C ABI registry access");

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
#include "sc2opt/adapters/c/sc2opt_c.h"

#include <cstdint>

int main()
{
    const std::uint64_t required_capabilities =
        static_cast<std::uint64_t>(SC2OPT_C_CAP_CPU_BASELINE) |
        static_cast<std::uint64_t>(SC2OPT_C_CAP_COMPATIBILITY_GATE) |
        static_cast<std::uint64_t>(SC2OPT_C_CAP_REGISTRY_IDENTITY) |
        static_cast<std::uint64_t>(SC2OPT_C_CAP_BATCH_GEOMETRY);

    std::uint64_t missing_capabilities = 0;
    const sc2opt_c_integration_status status = sc2opt_c_check_integration(
        1u,
        1u,
        1u,
        "75689",
        "B89B5D6FA7CBF6452E721311BFBC6CB2",
        required_capabilities,
        &missing_capabilities);

    if (status != SC2OPT_C_INTEGRATION_READY || missing_capabilities != 0u)
        return 1;

    return sc2opt_c_api_version_major() == 1u && sc2opt_c_api_version_minor() >= 1u ? 0 : 2;
}

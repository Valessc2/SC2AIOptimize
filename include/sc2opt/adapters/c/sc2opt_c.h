#pragma once

#include <stddef.h>
#include <stdint.h>

#if defined(_WIN32)
#  if defined(SC2OPT_C_BUILD)
#    define SC2OPT_C_API __declspec(dllexport)
#  else
#    define SC2OPT_C_API __declspec(dllimport)
#  endif
#else
#  define SC2OPT_C_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum sc2opt_c_status {
    SC2OPT_C_OK = 0,
    SC2OPT_C_INVALID_ARGUMENT = 1,
    SC2OPT_C_OUTPUT_TOO_SMALL = 2,
    SC2OPT_C_INTERNAL_ERROR = 3
} sc2opt_c_status;

typedef enum sc2opt_c_integration_status {
    SC2OPT_C_INTEGRATION_READY = 0,
    SC2OPT_C_INTEGRATION_API_MAJOR_MISMATCH = 1,
    SC2OPT_C_INTEGRATION_API_MINOR_TOO_OLD = 2,
    SC2OPT_C_INTEGRATION_UNIT_VIEW_ABI_MISMATCH = 3,
    SC2OPT_C_INTEGRATION_REGISTRY_BUILD_MISMATCH = 4,
    SC2OPT_C_INTEGRATION_REGISTRY_DATA_VERSION_MISMATCH = 5,
    SC2OPT_C_INTEGRATION_MISSING_CAPABILITY = 6
} sc2opt_c_integration_status;

typedef enum sc2opt_c_integration_capability {
    SC2OPT_C_CAP_CPU_BASELINE = 1u << 0,
    SC2OPT_C_CAP_SAFE_DISABLE = 1u << 1,
    SC2OPT_C_CAP_REGISTRY_IDENTITY = 1u << 2,
    SC2OPT_C_CAP_BATCH_GEOMETRY = 1u << 3
} sc2opt_c_integration_capability;

SC2OPT_C_API uint32_t sc2opt_c_api_version_major(void);
SC2OPT_C_API uint32_t sc2opt_c_api_version_minor(void);
SC2OPT_C_API uint32_t sc2opt_c_unit_view_abi_version(void);
SC2OPT_C_API const char* sc2opt_c_package_version(void);
SC2OPT_C_API const char* sc2opt_c_registry_build(void);
SC2OPT_C_API const char* sc2opt_c_registry_data_version(void);
SC2OPT_C_API size_t sc2opt_c_registry_unit_count(void);
SC2OPT_C_API uint64_t sc2opt_c_integration_capabilities(void);

// Startup compatibility gate shared by native consumers. Empty/null build or data-version
// strings mean "consumer does not constrain this field". `missing_capabilities` is optional.
// A non-ready result means the consumer must keep its normal baseline/OFF path active.
SC2OPT_C_API sc2opt_c_integration_status sc2opt_c_check_integration(
    uint32_t expected_api_major,
    uint32_t minimum_api_minor,
    uint32_t expected_unit_view_abi,
    const char* expected_sc2_build,
    const char* expected_data_version,
    uint64_t required_capabilities,
    uint64_t* missing_capabilities);

SC2OPT_C_API const char* sc2opt_c_integration_status_name(
    sc2opt_c_integration_status status);

// `xy_count` is the number of floats, not points, and must be even. `output_count` is the
// number of float slots. On success, `written` is the point count. No allocation occurs.
SC2OPT_C_API sc2opt_c_status sc2opt_c_distance_squared_into(
    const float* xy,
    size_t xy_count,
    float anchor_x,
    float anchor_y,
    float* output,
    size_t output_count,
    size_t* written);

// Writes one byte per point: 1 when inside/on radius, 0 otherwise. No allocation occurs.
SC2OPT_C_API sc2opt_c_status sc2opt_c_within_radius_mask_into(
    const float* xy,
    size_t xy_count,
    float anchor_x,
    float anchor_y,
    float radius,
    uint8_t* output,
    size_t output_count,
    size_t* written);

#ifdef __cplusplus
}
#endif

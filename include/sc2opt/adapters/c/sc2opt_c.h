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

SC2OPT_C_API uint32_t sc2opt_c_api_version_major(void);
SC2OPT_C_API uint32_t sc2opt_c_api_version_minor(void);
SC2OPT_C_API size_t sc2opt_c_registry_unit_count(void);

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

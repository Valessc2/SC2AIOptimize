#include "sc2opt/adapters/c/sc2opt_c.h"

#include <node_api.h>

#include <cstddef>
#include <cstdint>

namespace {

napi_value StatusValue(napi_env env, sc2opt_c_status status)
{
    napi_value value = nullptr;
    static_cast<void>(napi_create_int32(env, static_cast<std::int32_t>(status), &value));
    return value;
}

bool Float32Array(napi_env env, napi_value value, float*& data, std::size_t& length)
{
    napi_typedarray_type type{};
    napi_value array_buffer = nullptr;
    std::size_t byte_offset = 0;
    void* raw = nullptr;
    if (napi_get_typedarray_info(env, value, &type, &length, &raw, &array_buffer, &byte_offset) !=
            napi_ok ||
        type != napi_float32_array)
    {
        return false;
    }
    data = static_cast<float*>(raw);
    return true;
}

bool Uint8Array(napi_env env, napi_value value, std::uint8_t*& data, std::size_t& length)
{
    napi_typedarray_type type{};
    napi_value array_buffer = nullptr;
    std::size_t byte_offset = 0;
    void* raw = nullptr;
    if (napi_get_typedarray_info(env, value, &type, &length, &raw, &array_buffer, &byte_offset) !=
            napi_ok ||
        type != napi_uint8_array)
    {
        return false;
    }
    data = static_cast<std::uint8_t*>(raw);
    return true;
}

bool Number(napi_env env, napi_value value, double& output)
{
    return napi_get_value_double(env, value, &output) == napi_ok;
}

napi_value ApiVersion(napi_env env, napi_callback_info)
{
    napi_value result = nullptr;
    static_cast<void>(napi_create_array_with_length(env, 2, &result));
    napi_value major = nullptr;
    napi_value minor = nullptr;
    static_cast<void>(napi_create_uint32(env, sc2opt_c_api_version_major(), &major));
    static_cast<void>(napi_create_uint32(env, sc2opt_c_api_version_minor(), &minor));
    static_cast<void>(napi_set_element(env, result, 0, major));
    static_cast<void>(napi_set_element(env, result, 1, minor));
    return result;
}

napi_value RegistryUnitCount(napi_env env, napi_callback_info)
{
    napi_value result = nullptr;
    static_cast<void>(napi_create_double(
        env, static_cast<double>(sc2opt_c_registry_unit_count()), &result));
    return result;
}

napi_value DistanceSquaredInto(napi_env env, napi_callback_info info)
{
    std::size_t argc = 4;
    napi_value argv[4]{};
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok || argc != 4)
        return StatusValue(env, SC2OPT_C_INVALID_ARGUMENT);

    float* xy = nullptr;
    float* output = nullptr;
    std::size_t xy_count = 0;
    std::size_t output_count = 0;
    double anchor_x = 0.0;
    double anchor_y = 0.0;
    if (!Float32Array(env, argv[0], xy, xy_count) ||
        !Float32Array(env, argv[1], output, output_count) ||
        !Number(env, argv[2], anchor_x) || !Number(env, argv[3], anchor_y))
    {
        return StatusValue(env, SC2OPT_C_INVALID_ARGUMENT);
    }

    std::size_t written = 0;
    return StatusValue(env, sc2opt_c_distance_squared_into(
        xy, xy_count, static_cast<float>(anchor_x), static_cast<float>(anchor_y), output,
        output_count, &written));
}

napi_value WithinRadiusMaskInto(napi_env env, napi_callback_info info)
{
    std::size_t argc = 5;
    napi_value argv[5]{};
    if (napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr) != napi_ok || argc != 5)
        return StatusValue(env, SC2OPT_C_INVALID_ARGUMENT);

    float* xy = nullptr;
    std::uint8_t* output = nullptr;
    std::size_t xy_count = 0;
    std::size_t output_count = 0;
    double anchor_x = 0.0;
    double anchor_y = 0.0;
    double radius = 0.0;
    if (!Float32Array(env, argv[0], xy, xy_count) ||
        !Uint8Array(env, argv[1], output, output_count) ||
        !Number(env, argv[2], anchor_x) || !Number(env, argv[3], anchor_y) ||
        !Number(env, argv[4], radius))
    {
        return StatusValue(env, SC2OPT_C_INVALID_ARGUMENT);
    }

    std::size_t written = 0;
    return StatusValue(env, sc2opt_c_within_radius_mask_into(
        xy, xy_count, static_cast<float>(anchor_x), static_cast<float>(anchor_y),
        static_cast<float>(radius), output, output_count, &written));
}

void Export(napi_env env, napi_value exports, const char* name, napi_callback callback)
{
    napi_value function = nullptr;
    if (napi_create_function(env, name, NAPI_AUTO_LENGTH, callback, nullptr, &function) == napi_ok)
        static_cast<void>(napi_set_named_property(env, exports, name, function));
}

}  // namespace

NAPI_MODULE_INIT()
{
    Export(env, exports, "apiVersion", ApiVersion);
    Export(env, exports, "registryUnitCount", RegistryUnitCount);
    Export(env, exports, "distanceSquaredInto", DistanceSquaredInto);
    Export(env, exports, "withinRadiusMaskInto", WithinRadiusMaskInto);
    return exports;
}
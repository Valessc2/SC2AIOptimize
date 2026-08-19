#include "sc2opt/adapters/c/sc2opt_c.h"

#include <jni.h>

#include <cstddef>
#include <cstdint>
#include <limits>

namespace {

bool ValidCount(jint count) noexcept
{
    return count >= 0;
}

}  // namespace

extern "C" {

JNIEXPORT jint JNICALL Java_sc2opt_SC2AIOptimize_apiVersionMajor(JNIEnv*, jclass)
{
    return static_cast<jint>(sc2opt_c_api_version_major());
}

JNIEXPORT jint JNICALL Java_sc2opt_SC2AIOptimize_apiVersionMinor(JNIEnv*, jclass)
{
    return static_cast<jint>(sc2opt_c_api_version_minor());
}

JNIEXPORT jlong JNICALL Java_sc2opt_SC2AIOptimize_registryUnitCount(JNIEnv*, jclass)
{
    const std::size_t count = sc2opt_c_registry_unit_count();
    if (count > static_cast<std::size_t>(std::numeric_limits<jlong>::max()))
        return static_cast<jlong>(-1);
    return static_cast<jlong>(count);
}

JNIEXPORT jint JNICALL Java_sc2opt_SC2AIOptimize_distanceSquaredInto(
    JNIEnv* env,
    jclass,
    jobject xy_buffer,
    jint xy_count,
    jfloat anchor_x,
    jfloat anchor_y,
    jobject output_buffer,
    jint output_count)
{
    if (!ValidCount(xy_count) || !ValidCount(output_count))
        return static_cast<jint>(SC2OPT_C_INVALID_ARGUMENT);

    const auto* xy = static_cast<const float*>(env->GetDirectBufferAddress(xy_buffer));
    auto* output = static_cast<float*>(env->GetDirectBufferAddress(output_buffer));
    if ((xy_count != 0 && xy == nullptr) || (output_count != 0 && output == nullptr))
        return static_cast<jint>(SC2OPT_C_INVALID_ARGUMENT);

    std::size_t written = 0;
    return static_cast<jint>(sc2opt_c_distance_squared_into(
        xy, static_cast<std::size_t>(xy_count), anchor_x, anchor_y, output,
        static_cast<std::size_t>(output_count), &written));
}

JNIEXPORT jint JNICALL Java_sc2opt_SC2AIOptimize_withinRadiusMaskInto(
    JNIEnv* env,
    jclass,
    jobject xy_buffer,
    jint xy_count,
    jfloat anchor_x,
    jfloat anchor_y,
    jfloat radius,
    jobject output_buffer,
    jint output_count)
{
    if (!ValidCount(xy_count) || !ValidCount(output_count))
        return static_cast<jint>(SC2OPT_C_INVALID_ARGUMENT);

    const auto* xy = static_cast<const float*>(env->GetDirectBufferAddress(xy_buffer));
    auto* output = static_cast<std::uint8_t*>(env->GetDirectBufferAddress(output_buffer));
    if ((xy_count != 0 && xy == nullptr) || (output_count != 0 && output == nullptr))
        return static_cast<jint>(SC2OPT_C_INVALID_ARGUMENT);

    std::size_t written = 0;
    return static_cast<jint>(sc2opt_c_within_radius_mask_into(
        xy, static_cast<std::size_t>(xy_count), anchor_x, anchor_y, radius, output,
        static_cast<std::size_t>(output_count), &written));
}

}  // extern "C"
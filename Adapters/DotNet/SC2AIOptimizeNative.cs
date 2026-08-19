using System;
using System.Runtime.InteropServices;

namespace SC2AIOptimize;

public static class Native
{
    private const string Library = "sc2opt_c";

    [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
    public static extern uint sc2opt_c_api_version_major();

    [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
    public static extern uint sc2opt_c_api_version_minor();

    [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
    public static extern UIntPtr sc2opt_c_registry_unit_count();

    [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
    public static extern unsafe int sc2opt_c_distance_squared_into(
        float* xy, UIntPtr xyCount, float anchorX, float anchorY,
        float* output, UIntPtr outputCount, UIntPtr* written);

    [DllImport(Library, CallingConvention = CallingConvention.Cdecl)]
    public static extern unsafe int sc2opt_c_within_radius_mask_into(
        float* xy, UIntPtr xyCount, float anchorX, float anchorY, float radius,
        byte* output, UIntPtr outputCount, UIntPtr* written);
}

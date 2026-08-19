package sc2opt;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/** Thin Java 17 edge over the stable SC2AIOptimize C ABI. */
public final class SC2AIOptimize {
    static {
        System.loadLibrary("sc2opt_jni");
    }

    private SC2AIOptimize() {}

    public static native int apiVersionMajor();
    public static native int apiVersionMinor();
    public static native long registryUnitCount();

    public static native int distanceSquaredInto(
        ByteBuffer xyFloat32,
        int xyCount,
        float anchorX,
        float anchorY,
        ByteBuffer outputFloat32,
        int outputCount);

    public static native int withinRadiusMaskInto(
        ByteBuffer xyFloat32,
        int xyCount,
        float anchorX,
        float anchorY,
        float radius,
        ByteBuffer outputBytes,
        int outputCount);

    public static ByteBuffer directNativeBuffer(int bytes) {
        return ByteBuffer.allocateDirect(bytes).order(ByteOrder.nativeOrder());
    }
}
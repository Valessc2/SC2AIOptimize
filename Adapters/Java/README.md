# Java 17 edge

`SC2AIOptimize.java` + `sc2opt_jni.cpp` are a thin JNI edge over the optional `sc2opt_c` shared library.

The Java hot-call surface requires **direct** `ByteBuffer` instances so JNI can hand native memory straight to the C ABI instead of constructing per-unit Java objects. Buffers containing floats must use native byte order.

Build with:

```text
-DSC2OPT_BUILD_C_ABI=ON
-DSC2OPT_BUILD_JAVA_JNI=ON
```

This edge contains no StarCraft framework policy. A Java bot owns translation from its own framework state into packed buffers.
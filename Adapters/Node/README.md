# Node.js edge

`sc2opt_napi.cpp` is a thin N-API module over the optional `sc2opt_c` shared library. It accepts `Float32Array` / `Uint8Array` buffers and calls the same caller-owned-buffer kernels as the other language edges.

Build with:

```text
-DSC2OPT_BUILD_C_ABI=ON
-DSC2OPT_BUILD_NODE_NAPI=ON
-DSC2OPT_NODE_INCLUDE_DIR=/path/containing/node_api.h
```

The edge is optional and is never linked by the core runtime or direct C++ consumers.
# Generic Python adapter

The optional `_sc2opt` extension binds the same framework-independent Kernel/Registry contracts used by C++ consumers. It has no `python-sc2` or NumPy dependency.

The first boundary accepts standard Python buffer-protocol objects and writes batch results into caller-owned buffers. This makes `array.array`, `memoryview`, NumPy (when a consumer already uses it), and other compatible arrays viable without forcing a data-library dependency on SC2AIOptimize.

`pybind11` is optional and build-time only. It is not a dependency of the C++ core, and the binding implementation remains challengeable under the project's net-benefit rule.

# Generic Python adapter

The optional `_sc2opt` extension binds the same framework-independent Kernel/Registry contracts used by C++ consumers. It has no `python-sc2` or NumPy dependency.

The Python edge exposes `integration_contract()` and `check_integration()` from the same C++ compatibility authority used by the native/C ABI edges. Python does not own a duplicate version, registry or capability policy.

A consumer should perform the compatibility check before enabling SC2AIOptimize-backed work. A non-ready result means keep the consumer's baseline/OFF path. Module availability and packaging are separate deployment concerns; the check cannot protect against a module that failed to load.

The first boundary accepts standard Python buffer-protocol objects and writes batch results into caller-owned buffers. This makes `array.array`, `memoryview`, NumPy (when a consumer already uses it), and other compatible arrays viable without forcing a data-library dependency on SC2AIOptimize.

`pybind11` is optional and build-time only. It is not a dependency of the C++ core, and the binding implementation remains challengeable under the project's net-benefit rule.

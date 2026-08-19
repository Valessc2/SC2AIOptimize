# Generic adapter contract

SC2AIOptimize owns no bot-specific adapter. Consumers translate at their own boundary into neutral SC2AIOptimize views/buffers.

## Shared integration contract

`sc2opt/adapters/IntegrationContract.hpp` is the single bot-neutral compatibility authority used before an optimisation edge is admitted. It exposes and validates:

- SC2AIOptimize API major/minimum-minor compatibility;
- `UnitView` ABI identity;
- pinned SC2 registry build and data-version identity;
- generic capability bits.

The same contract is exposed through the stable C ABI and the optional Python module. Those language edges must not invent their own compatibility rules.

A non-ready handshake is not a gameplay failure: the consumer keeps its normal baseline/OFF implementation. The handshake begins only after the library/module is available; packaging or optional dynamic-loading policy remains the consumer's responsibility.

This is the shared connector model:

```text
consumer-owned adapter/packing
          |
          v
SC2AIOptimize shared integration contract
          |
          +--> C++20 native surface
          +--> stable C ABI
          +--> Python edge
          |
          v
registry / kernel / tuner capabilities
```

Bot policy and framework types never enter this contract.

## C++

`UnitView` / `WorldView` are framework-free, standard-layout data views. `BuildDenseTypeMap` maps a consumer's numeric type IDs to `SC2Registry` records once during startup using catalog names. Unknown names remain unaccelerated/null rather than becoming contradictions; duplicate consumer IDs fail closed.

The startup mapper intentionally uses allocation-free O(n^2) duplicate validation and linear registry lookup. Startup cost is not a hot-path problem. A faster mapper may be added only if measurement shows startup mapping matters.

The native public surface is C++20. Consumers on an older C++ language standard must use the stable C ABI rather than importing C++20 headers. The CI C++14 consumer proves that compatibility edge without changing the consumer's language standard.

## C ABI

The optional `sc2opt_c` shared library exposes the integration handshake and generic batch operations through a C-compatible header. It is the intended boundary for native consumers that cannot or should not adopt the C++20 API directly.

The C ABI is still an optional runtime component. A consumer that wants missing-library fallback must arrange optional loading or otherwise prove that the library is always packaged; the handshake cannot run before the library is loaded.

## Python

The optional `_sc2opt` module is built with pybind11, but pybind11 is a **build-time-only optional dependency**. The core library does not require Python or pybind11.

The Python edge exposes `integration_contract()` and `check_integration()` from the same C++ compatibility authority. It does not duplicate version/registry/capability policy in Python.

The first Python boundary uses the standard buffer protocol rather than NumPy-specific APIs. Batch inputs/outputs therefore work with objects such as `array.array`, `memoryview`, and compatible third-party arrays. Kernel outputs are written into caller-owned buffers to avoid mandatory per-call result allocation.

The adapter intentionally does not depend on `python-sc2`. MetaSwarm or any other Python bot performs its own framework-to-buffer translation.

## Boundary performance

Adapter technology is itself subject to the net-benefit law. The first implementation is not declared permanently optimal. Future candidates may include different packing layouts, buffer strategies, GIL-release thresholds, C-ABI loading strategies or another binding generator. They must beat the existing champion on total end-to-end cost before replacing it.

## Batch rule

Prefer one useful native batch over thousands of tiny language crossings. The benchmark must include packing/conversion + call + kernel + result handling, not merely the inner C++ loop.

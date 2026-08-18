# Generic adapter contract

SC2AIOptimize owns no bot-specific adapter. Consumers translate at their own boundary into neutral SC2AIOptimize views/buffers.

## C++

`UnitView` / `WorldView` are framework-free, standard-layout data views. `BuildDenseTypeMap` maps a consumer's numeric type IDs to `SC2Registry` records once during startup using catalog names. Unknown names remain unaccelerated/null rather than becoming contradictions; duplicate consumer IDs fail closed.

The startup mapper intentionally uses allocation-free O(n^2) duplicate validation and linear registry lookup. Startup cost is not a hot-path problem. A faster mapper may be added only if measurement shows startup mapping matters.

## Python

The optional `_sc2opt` module is built with pybind11, but pybind11 is a **build-time-only optional dependency**. The core library does not require Python or pybind11.

The first Python boundary uses the standard buffer protocol rather than NumPy-specific APIs. Batch inputs/outputs therefore work with objects such as `array.array`, `memoryview`, and compatible third-party arrays. Kernel outputs are written into caller-owned buffers to avoid mandatory per-call result allocation.

The adapter intentionally does not depend on `python-sc2`. MetaSwarm or any other Python bot performs its own framework-to-buffer translation.

## Boundary performance

Adapter technology is itself subject to the net-benefit law. The first implementation is not declared permanently optimal. Future candidates may include different packing layouts, buffer strategies, GIL-release thresholds, a C ABI, or another binding generator. They must beat the existing champion on total end-to-end cost before replacing it.

## Batch rule

Prefer one useful native batch over thousands of tiny language crossings. The benchmark must include packing/conversion + call + kernel + result handling, not merely the inner C++ loop.

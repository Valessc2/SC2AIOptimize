# Stable C ABI adapter

The optional `sc2opt_c` shared library is the stable C-compatible edge for consumers that should not import the C++20 public API directly.

It exposes the shared integration handshake from `sc2opt/adapters/IntegrationContract.hpp` plus generic allocation-free batch operations. The C layer does not own a second compatibility policy.

A native consumer should perform the integration handshake during startup before enabling SC2AIOptimize-backed work. A non-ready result means keep the consumer's normal baseline/OFF path.

The handshake validates API major/minimum minor, `UnitView` ABI identity, optional pinned registry build/data version and required generic capability bits.

## Older C++ consumers

C++14 consumers can include only `sc2opt/adapters/c/sc2opt_c.h` and link/load `sc2opt_c`; they do not need to compile SC2AIOptimize's C++20 headers. CI contains an explicit C++14 consumer for this boundary.

This does **not** by itself make a missing shared library optional. Consumers that require missing-library fallback must use an appropriate optional-loading strategy or prove the library is always present in their package/runtime.

No SC2API, python-sc2, MetaSwarm, MMEvo or gameplay-policy types belong in this ABI.

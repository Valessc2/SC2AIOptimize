# API stability

Public headers under `include/sc2opt/` are the intended consumer surface. Internal benchmark/test/generated machinery must not leak into the runtime API.

## V1 consumer contract

The current V1 API contract generation is `1.1`, exposed by `sc2opt/Version.hpp`. Its status is `v1-candidate` until the external S32 certification gates close.

API `1.1` is an additive extension of `1.0`. It adds the bot-neutral shared integration compatibility contract and does not invalidate existing `1.0` consumers.

Compatibility rules once V1 certification is promoted:

- patch-level implementation changes must preserve source/semantic compatibility for the same API contract;
- additive compatible API changes increment the API minor version;
- breaking public contract/ABI changes require an API major increment and re-certification;
- neutral ABI records with their own version constants (for example `UnitView`) must bump those versions when layout changes;
- certification/output schemas are versioned independently and consumers must validate both API and schema identity.

## Shared integration handshake

`sc2opt/adapters/IntegrationContract.hpp` is the single C++ authority for startup compatibility checks. It reports package/API generation, `UnitView` ABI identity, pinned registry identity and generic capability bits. The stable C ABI and optional Python module expose the same authority rather than reimplementing compatibility rules.

A consumer requiring a newer minor version, incompatible ABI/registry identity or unavailable capability receives a non-ready result and should retain its baseline/OFF path. The handshake assumes the library/module has been loaded successfully; optional loading, package presence and missing-library fallback remain consumer/deployment responsibilities.

CMake package exports use the `sc2opt::` namespace. `sc2opt::sc2opt` is the runtime umbrella; `sc2opt::proof` is deliberately separate and is never linked transitively by the runtime umbrella.

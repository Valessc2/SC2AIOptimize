# API stability

Public headers under `include/sc2opt/` are the intended consumer surface. Internal benchmark/test/generated machinery must not leak into the runtime API.

## V1 consumer contract

The V1 API contract generation is `1.0`, exposed by `sc2opt/Version.hpp`. Its status is `v1-candidate` until the external S32 certification gates close.

Compatibility rules once V1 certification is promoted:

- patch-level implementation changes must preserve source/semantic compatibility for the same API contract;
- additive compatible API changes increment the API minor version;
- breaking public contract/ABI changes require an API major increment and re-certification;
- neutral ABI records with their own version constants (for example `UnitView`) must bump those versions when layout changes;
- certification/output schemas are versioned independently and consumers must validate both API and schema identity.

CMake package exports use the `sc2opt::` namespace. `sc2opt::sc2opt` is the runtime umbrella; `sc2opt::proof` is deliberately separate and is never linked transitively by the runtime umbrella.

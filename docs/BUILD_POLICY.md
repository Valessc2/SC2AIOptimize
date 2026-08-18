# Build/toolchain policy

Release optimisation features are explicit capability/evidence gates, never silent defaults.

## Supported hardening switches

- `SC2OPT_ENABLE_ASAN` — AddressSanitizer where the compiler supports it.
- `SC2OPT_ENABLE_UBSAN` — UndefinedBehaviorSanitizer on GNU/Clang.
- `SC2OPT_ENABLE_LTO` — CMake IPO/LTO; configuration fails if unsupported.
- `SC2OPT_ENABLE_AVX2` — explicit AVX2 hardware contract; default OFF.
- `SC2OPT_PGO_GENERATE` / `SC2OPT_PGO_USE` — mutually exclusive PGO modes. MSVC and GNU are supported; GNU use mode requires an explicit profile directory.
- `SC2OPT_ENABLE_CLANG_TIDY` — clang-analyzer-backed static analysis; configuration fails if clang-tidy is unavailable.

ASAN/UBSAN, LTO, AVX2 and PGO flags are applied to project-owned targets only. No ISA-specific mode is enabled merely because the build host happens to support it.

PGO infrastructure is not trained evidence. A profile may only be used for release certification when its representative corpus, source revision and hardware/toolchain provenance are recorded.

Performance regression checks compare matching candidate/workload records and fail closed on malformed or mismatched evidence. CI timing is not used as a universal performance baseline.

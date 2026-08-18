# Engineering standard

This repository follows the ValesCodex pre-code rule: inspect the actual repository, branch, HEAD and relevant baseline before changing code. Never implement against an assumed state.

## Change discipline

- Keep each change scoped and provenance-readable.
- Distinguish source/design review, successful compile/test, and runtime/performance certification. Never report one as another.
- Fail closed on data/version contradictions and unsupported optimisation/toolchain modes.
- Preserve a known-correct baseline until a replacement is equivalence-proven.
- Generated or benchmark-derived outputs are evidence, not silent source authority.

## Runtime discipline

- Standard C++20 first; third-party runtime dependencies require demonstrated value.
- Core code is independent of SC2API, python-sc2 and specific bots.
- Hot kernels avoid dynamic allocation, logging and needless abstraction/dispatch.
- Deterministic single-threaded execution is the default.
- SIMD, parallelism, PGO, caching and alternate algorithms are candidates, not assumptions.
- Measure total cost including data conversion, adapter crossings, dispatch, cache maintenance and tuner bookkeeping.
- Baseline/OFF is a first-class champion and must remain selectable where an optimisation loses.

## Correctness gates

Optimised candidates require the strongest applicable combination of:

- unit tests;
- reference/equivalence tests;
- property/invariant tests;
- fuzz/extreme-input tests;
- regression tests for prior failures;
- adapter equivalence tests;
- deterministic/tie-order checks when ordering can change behaviour.

Correctness failure disqualifies a candidate. A speedup cannot waive the correctness gate.

## Performance gates

Performance claims require reproducible workloads and must include sample count plus distribution/tail metrics where meaningful. Compare baseline and candidate on the same workload and environment. Record compiler, flags, OS, CPU/hardware profile, source revision, SC2 data/build identity and benchmark version.

Tiny differences are treated as noise until confidence is sufficient. An optimisation that does not clear the configured net-benefit threshold stays off.

## CI and release

CI proves builds/tests/static contracts; it does not invent runtime performance certification. BotOps may consume SC2AIOptimize manifests and enforce integrated release policy, but BotOps does not own SC2AIOptimize runtime code.

A release/certification statement must say exactly which layers were proven: source/design, configure/build, tests, representative benchmark, bot integration and ladder/runtime evidence.

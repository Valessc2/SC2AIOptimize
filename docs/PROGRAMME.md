# V1 Programme — 32 slices

V1 is deliberately proof-led. A slice is complete only when its declared correctness, build and evidence gates are satisfied; implementation alone is not certification.

## Foundation — S01-S04

- **S01 Repo/build foundation** — C++20, CMake, presets, warnings, CI, zero mandatory runtime dependencies.
- **S02 Neutral contracts** — versioned POD/span-based types, error/result semantics, ownership/lifetime rules.
- **S03 SC2Registry Base75689** — import Z1 static facts with provenance, identity and integrity tests.
- **S04 Engineering gates** — correctness/performance separation, fail-closed rules, benchmark provenance schema.

## Generic interfaces — S05-S08

- **S05 Generic C++ adapter** — framework-free packed world/unit views.
- **S06 Generic Python adapter** — optional binding over the same neutral contracts; no python-sc2 dependency.
- **S07 Batch transport** — amortised Python/native boundary and result buffers.
- **S08 Third-consumer proof** — synthetic C++ and Python consumers prove no MS/MME coupling.

## Kernel/Hot — S09-S14

- **S09 Geometry/distance** — baseline + native batch candidates.
- **S10 Range/facing** — exact boundary/equivalence tests.
- **S11 Filtering/lookup** — packed filters and dense lookups.
- **S12 Spatial index** — graduate MMEvo Z2 pattern with baseline fallback.
- **S13 Grid/local queries** — reusable grid primitives and workload crossovers.
- **S14 Hot variant certification** — layout/vectorisation variants, crossover outputs, no speculative ISA assumptions.

## Kernel/Compute — S15-S19

- **S15 Target/scoring batches** — graduate Z3 pattern while preserving ordering semantics.
- **S16 Influence/threat** — graduate Z4 kernel and prove boundaries.
- **S17 Pathing** — generic A*/grid jobs, informed by Z8 equivalence lessons.
- **S18 Placement** — generic placement/bounds jobs where profiling justifies them.
- **S19 Combat evaluation** — larger batch/simulation boundary using Registry extensions actually required by evidence.

## Tuner — S20-S24

- **S20 Net-benefit gate** — baseline is candidate zero; OFF is a valid champion.
- **S21 TunableRegistry** — explicit consumer-owned legal knobs and safety metadata.
- **S22 Champion/challenger** — repeated evidence, confidence and deterministic reset/baseline mode.
- **S23 Context/crossover** — workload/hardware contexts and different champions by size/state.
- **S24 Persistence/provenance** — versioned champion evidence with source/build/hardware identity.

## Runtime optimisation — S25-S27

- **S25 Profiling/telemetry** — p50/p95/p99, counts and total-cost instrumentation with negligible disabled cost.
- **S26 Compute-budget governor** — generic latency pressure/hysteresis pattern from Z7; consumer decides optional work.
- **S27 Auto-calibration** — kernel/batch/cache/crossover calibration; tuner overhead is itself benchmarked.

## Proof/output — S28-S30

- **S28 Deep tests** — unit, equivalence, property, fuzz, regression and adapter equivalence.
- **S29 Benchmark/workload profiles** — synthetic + representative workload corpora and reproducibility metadata.
- **S30 Consumable outputs** — kernel/tuning/hardware/benchmark/certification manifests for bots and BotOps.

## Hardening — S31-S32

- **S31 Toolchain hardening** — sanitizers, static analysis, LTO/PGO support, safe SIMD capability gates, perf regression checks.
- **S32 V1 certification** — stable API/schema contract, generic C++/Python consumer proof, BotOps integration contract and release evidence.

## Programme invariant

No slice may make the runtime permanently more expensive merely because an optimisation exists. Optional optimisation candidates must remain rejectable/disableable when measured net benefit is non-positive.

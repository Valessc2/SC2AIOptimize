# V1 roadmap

Target: 32 bounded slices. Each slice must close with correctness evidence and, where performance is claimed, net-benefit evidence.

1-4 Foundation: build/contracts, neutral registry types, ownership/dependency law, proof harness.
5-8 Generic consumption: C++ boundary, Python packed boundary, ID mapping/validation, adapter equivalence.
9-14 Kernel Hot: geometry/range, filtering/batching, spatial, grids/lookups, data layout/SIMD candidates, crossover proof.
15-19 Kernel Compute: threat/influence, pathing, placement, combat evaluation, larger batched workloads.
20-24 Tuner: tunable registry, champion/challenger, confidence/statistics, context/exploration, persistence/reset/provenance.
25-27 Runtime optimisation: profiler telemetry, compute-budget governor, automatic kernel/crossover/batch/cache selection.
28-30 Proof/output: equivalence/property/fuzz/regression, hardware/workload benchmark profiles, consumable outputs/reports.
31-32 Hardening: sanitizers/static analysis/performance regression gates, stable API/release contract for BotOps.

The roadmap is allowed to shrink if a slice cannot demonstrate value. Architecture is not a quota.

# MMEvo Z1-Z12 -> SC2AIOptimize mapping

Source programme: MicroMachineEvo `mmevo-v0.9.2z`. The goal is to extract reusable engineering, not import MME-specific gameplay semantics.

| Z | MMEvo result | SC2AIOptimize treatment |
|---|---|---|
| Z1 | Build75689 static-data fast path + fail-closed live bridge | **Promote** into SC2Registry schema/data/provenance and generic validation contract |
| Z2 | Reusable frame spatial index | **Promote** into Kernel; preserve completeness/fallback and allocation-reuse lessons |
| Z3 | Target-selection fast path/cache | **Generalise** selection/reduction kernels; MME target semantics stay outside |
| Z4 | Influence-map kernel | **Promote** into Kernel after reference/equivalence/benchmark slice |
| Z5 | Compact MME F runtime state | **Pattern only**; consumer state is not shared infrastructure |
| Z6 | Selective Cyclone ability batching | **Pattern only** for adapter/query planning; unit-policy predicate remains consumer-owned |
| Z7 | Frame-time governor | **Generalise** into Tuner/runtime budget control with consumer-declared optional work |
| Z8 | A* heap/hash/ownership improvement | **Promote** into Kernel pathing after deterministic equivalence tests |
| Z9 | LTO/PGO infrastructure | **Promote** as build/profile tooling; trained profiles remain hardware/workload specific |
| Z10 | safe optional AVX2/vectorization | **Promote pattern**; feature variants compete with scalar baseline and may lose |
| Z11 | rejected speculative parallelism | **Keep as law**: no worker pool without measured net benefit |
| Z12 | hidden copy/correctness cleanup | **Engineering lesson**, not a reusable module |

## Z1 data import

The existing MMEvo Z1 table is a 245-record Base75689 multiplayer/static seed and carries unit core stats/counts/footprint presence. It is not sufficient to freeze the final SC2Registry schema: deeper combat/pathing consumers will eventually require weapon, ability and footprint detail. Those additions should be demand-driven, compact and build-pinned.

The current foundation defines the neutral schema first. The generated Base75689 payload and generic live-validation adapter are separate slices so framework types cannot leak into Registry.

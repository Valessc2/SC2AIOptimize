# Compute kernel contract

`Kernel/Compute` owns larger framework-independent jobs that are worth crossing the native boundary for. It does not own bot strategy semantics.

## S15 — score selection

`SelectBestScore` implements the MMEvo Z3 lesson generically: when only the best candidate is required, do a linear best-of reduction instead of building an ordered container. Scores and eligibility are supplied by the consumer. Non-finite scores are rejected, and equal-score tie behaviour is explicit (`First` or `Last`).

## S16 — radial influence

Two implementations share one exact contract:

- `ApplyRadialInfluenceReference` scans the full logical grid.
- `ApplyRadialInfluenceBounded` scans only the clamped radial bounding square, rejects with squared distance, and calls `sqrt` only in the linear falloff ring.

The reference remains available for equivalence and net-benefit measurement. Grid stride/padding is respected. Boundary semantics are explicit.

## S17 — pathing

`AStarWorkspace` owns reusable buffers and supports two exact modes:

- `PathHeuristic::None` — Dijkstra/reference mode.
- `PathHeuristic::Manhattan` — A* challenger mode.

The V1 traversal contract is deliberately narrow: 4-neighbour movement, finite passable costs >= 1.0, +infinity blocked, and movement pays the destination cell cost. This makes the unit-scale Manhattan heuristic admissible. Tie behaviour is deterministic by `(f, g, cell index)`. Workspace capacity is retained between searches.

No consumer should assume A* is always the faster champion; benchmark the complete search on representative maps/workloads.

## S18 — placement

Placement consumes a byte buildability grid plus an arbitrary byte footprint mask. Non-zero mask cells require non-zero buildable cells. The consumer translates its own anchor convention; SC2AIOptimize does not assume SC2API footprint types or building-centre semantics.

Candidate filtering preserves candidate order and writes indices into caller-owned output.

## S19 — combat evaluation

The first combat primitive intentionally consumes **effective health and effective DPS supplied by the consumer**. `EstimateContinuousExchange` is a minimal continuous-DPS estimate, not an SC2 simulator and not a strategic decision.

This avoids baking incomplete weapon/armor/upgrade assumptions into V1 before `SC2Registry` has a fully proven combat-data extension. More detailed combat kernels must arrive with the corresponding data provenance and equivalence tests.

## Admission rule

Compute code is not automatically a runtime win just because it is native. For each consumer/workload, compare the original/baseline path against the full native path including packing, dispatch, cache/workspace maintenance and result handling. Baseline/OFF remains valid.

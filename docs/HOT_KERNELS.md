# Hot kernel contract

`Kernel/Hot` contains small operations with a known-correct baseline and explicit semantics. Presence in this directory does **not** mean every consumer should always use the accelerated candidate.

## Geometry

Distance/radius operations expose explicit `BoundaryMode::Strict` and `BoundaryMode::Inclusive` semantics. This exists because MMEvo Z2 demonstrated that changing `< radius` to `<= radius` during candidate reduction is a real behavioural regression.

Facing accepts a pre-normalized facing vector and precomputed dot threshold so hot calls do not pay repeated angle/trigonometry work that the consumer can cache.

## Filtering

Filtering writes matching indices into caller-owned storage. No result container allocation is required. If capacity is insufficient, the call reports the required size and the caller must ignore partial output.

## Spatial query

`RadiusQueryBruteForce` is the exact O(n) baseline. `SpatialGrid2D` is the reusable grid candidate inspired by MMEvo Z2:

- cell/bucket storage is retained across rebuilds;
- only active/touched bucket vectors are cleared;
- exact radius filtering happens after bucket candidate reduction;
- each input point occurs in exactly one bucket;
- stable `InputOrder` mode sorts results back to source order;
- `Unordered` mode exists only for consumers whose semantics permit it;
- query output is caller-owned;
- invalid state/query fails closed.

The grid is **off by default** at the strategy layer. `SpatialCrossoverProfile` initializes both thresholds to `size_t::max()`, which means the brute-force baseline remains champion until target-workload calibration supplies a certified crossover.

## Benchmark discipline

`sc2opt_spatial_query_bench` is a synthetic calibration tool. It includes grid rebuild cost in the grid workload and verifies stable-result equivalence before timing. CI compiles it but does not run it as a performance gate and does not promote thresholds from runner timing.

Real promotion requires the target hardware and representative workload/profile evidence.

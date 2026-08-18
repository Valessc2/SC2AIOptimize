# Development status

SC2AIOptimize is pre-V1. Implementation presence is not runtime/performance certification.

## Merged foundations

- **S01-S04 — foundation:** merged. Base75689 SC2Registry provenance is exact; Linux/Windows Release CI is green.
- **S05-S08 — generic interfaces:** merged. Synthetic unrelated C++ consumer and optional Python buffer-protocol adapter are proven on Linux/Windows.
- **S09-S14 — Kernel/Hot:** merged. Geometry, filtering and spatial baseline/candidate contracts are correctness-tested; spatial optimisation remains **certified OFF by default** until target-workload crossover evidence exists.
- **S15-S19 — Kernel/Compute:** merged. Deterministic score reduction, reference/bounded influence, Dijkstra/A* pathing, footprint placement and minimal caller-supplied combat evaluation are present with correctness contracts.

## Current work

- **S20-S24 — Tuner:** net-benefit baseline/OFF selection, validated TunableRegistry, repeated champion/challenger evidence, baseline-only/reset mode, context/crossover selection and versioned provenance persistence.

## Not yet claimed

No target-hardware crossover, bot integration, ladder-runtime speedup, behavioural equivalence or V1 release certification is claimed yet. Those require the later runtime optimisation, benchmark/profile/output and hardening slices plus representative consumer evidence.

# Development status

SC2AIOptimize is pre-V1. Implementation presence is not runtime/performance certification.

## Merged foundations

- **S01-S04 — foundation:** merged. Base75689 SC2Registry provenance is exact; Linux/Windows Release CI is green.
- **S05-S08 — generic interfaces:** merged. Synthetic unrelated C++ consumer and optional Python buffer-protocol adapter are proven on Linux/Windows.
- **S09-S14 — Kernel/Hot:** merged. Geometry, filtering and spatial baseline/candidate contracts are correctness-tested; spatial optimisation remains **certified OFF by default** until target-workload crossover evidence exists.
- **S15-S19 — Kernel/Compute:** merged. Deterministic score reduction, reference/bounded influence, Dijkstra/A* pathing, footprint placement and minimal caller-supplied combat evaluation are present with correctness contracts.
- **S20-S24 — Tuner:** merged. Net-benefit baseline/OFF selection, validated TunableRegistry, repeated champion/challenger evidence, baseline-only/reset mode, context/crossover selection and versioned provenance persistence are present.
- **S25-S27 — runtime optimisation:** merged. Fixed-storage telemetry, compile-time-disabled timing, generic latency hysteresis governor, crossover calibration and an explicit control-overhead cheapness gate are present.

## Current work

- **S28-S30 — proof/output:** deterministic deep contract/property tests, reproducible workload profiles, p50/p95/p99 benchmark output and a proof-only versioned certification-manifest contract for BotOps/consumers.

## Not yet claimed

No target-hardware crossover, bot integration, ladder-runtime speedup, behavioural equivalence or V1 release certification is claimed yet. S31 toolchain hardening and S32 integrated certification remain outstanding, as do representative consumer evidence and target-hardware measurements.

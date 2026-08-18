# SC2AIOptimize

High-performance kernel and self-tuning engine for StarCraft II AI bots, with generic C++ and Python adapters.

SC2AIOptimize exists to make expensive SC2-bot computation cheaper **only when measurement proves that the optimisation is a net win**. The unoptimised baseline is always a valid candidate and may remain champion.

## Runtime pillars

- **SC2Registry** — build-pinned, read-only SC2 facts behind a framework-neutral contract.
- **Kernel** — framework-neutral compute kernels, from tiny hot operations to larger batched jobs.
- **Tuner** — lightweight champion/challenger selection, confidence and runtime-budget decisions. `Baseline` is a first-class candidate.

Adapters are deliberately generic. SC2AIOptimize does not depend on MetaSwarm, MicroMachineEvo, python-sc2 or Blizzard's SC2 API. Consumers translate their own objects into the neutral contracts.

## Core law

```text
reference/baseline
      vs
candidate optimisation
      |
correctness/equivalence
      |
representative benchmark
      |
include adapter + bookkeeping cost
      |
positive net benefit?
  yes       no
   |         |
enable    baseline wins
```

An optimisation that is slower, insufficiently proven, workload-inappropriate, or no longer beneficial is rejected or disabled.

## Current status

**Foundation / pre-V1.** The repository establishes the neutral registry, kernel and tuner contracts plus the first net-benefit gate. It is not yet a performance-certified implementation.

See `docs/ARCHITECTURE.md`, `docs/VC_CONTRACT.md`, and `docs/MMEVO_Z1_Z12_MAPPING.md`.

# SC2AIOptimize

High-performance kernel and self-tuning engine for StarCraft II AI, with generic multi-language adapters.

SC2AIOptimize exists to make expensive SC2 AI computation cheaper **only when measurement proves the optimisation is worth using**. The baseline implementation is always a valid candidate; an optimisation that does not produce positive measured net benefit is rejected or disabled.

## Status

The **32-slice V1 software programme is 32/32 implemented**. The public API contract is `1.0` with status **`v1-candidate`**: component/build/toolchain proof is in place, while representative target-hardware, real bot/framework integration, behavioural, ladder/runtime and BotOps release evidence remain separate certification gates.

Production seed policies and multi-language edge adapters are maintained as post-programme V1 polish. They are defaults and optional integrations, not bot-performance limits.

See `docs/STATUS.md` and `docs/V1_CERTIFICATION.md` for the exact proof boundary.

## Runtime architecture

The runtime is deliberately small:

- **SC2Registry** — immutable, build-pinned SC2 facts. Seeded from MicroMachineEvo v0.9.2Z / Base75689 data, with provenance retained.
- **Kernel** — framework-independent compute. `Hot` contains tiny proven hot-path primitives; larger compute belongs in Kernel only when profiling justifies the native boundary.
- **Tuner** — cheap selection and tuning. Baseline/champion/challenger, confidence and context live here; expensive discovery is not paid on every hot call.
- **Adapters** — direct generic C++, optional Python buffer binding, optional stable C ABI, and thin Rust/Go/.NET/Java/Node edges. Consumers adapt to SC2AIOptimize; SC2AIOptimize does not depend on a bot framework.

The core has no dependency on SC2API, python-sc2, MetaSwarm, MicroMachineEvo, BotOps or an ML runtime.

## Production seed policy

The V1 engineering priors are intentionally conservative and consumer-overridable:

- operational evidence: **20 samples**
- certification evidence seed: **50 samples**
- default promotion margin: **2% and 1 microsecond** per declared representative workload
- confidence separation: **z = 1.96**
- tuner/control overhead ceiling: **1% and 50 microseconds**
- workload seeds: **8, 16, 32, 64, 128, 256, 512, 1024**
- batch seeds: **16, 32, 64, 128, 256, 512, 1024**
- spatial-cell seeds: **2, 3, 4, 6, 8, 12**

These values are starting priors, not universal truths or hard ceilings. A consumer may tighten, relax or replace them when representative evidence justifies it.

The compute-budget governor is **OFF by default**. A consumer may explicitly delegate a full compute envelope; the helper then seeds resume/soft/hard hysteresis at **55% / 70% / 95%**. SC2AIOptimize never invents or silently imposes a bot compute budget.

## Prime rule

> Correctness first. Measure total cost. Keep an optimisation only when its net benefit is positive.

Total cost includes adapter, dispatch and bookkeeping overhead. `OFF` / baseline is a first-class champion.

See `docs/ARCHITECTURE.md`, `docs/PROGRAMME.md`, `docs/TUNER.md`, `Adapters/AIArena/README.md` and `docs/MMEVO_Z_MIGRATION.md`.
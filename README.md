# SC2AIOptimize

High-performance kernel and self-tuning engine for StarCraft II AI bots, with generic C++ and Python adapters.

SC2AIOptimize exists to make expensive SC2 AI computation cheaper **only when measurement proves the optimisation is worth using**. The baseline implementation is always a valid candidate; an optimisation that does not produce positive measured net benefit is rejected or disabled.

## Runtime architecture

The runtime is deliberately small:

- **SC2Registry** — immutable, build-pinned SC2 facts. Seeded from MicroMachineEvo v0.9.2Z / Base75689 data, with provenance retained.
- **Kernel** — framework-independent compute. `Hot` contains tiny proven hot-path primitives; larger compute belongs in Kernel only when profiling justifies the native boundary.
- **Tuner** — cheap selection and tuning. Baseline/champion/challenger, confidence and context live here; expensive discovery is not paid on every hot call.
- **Adapters** — generic C++ and Python integration surfaces. Consumers adapt to SC2AIOptimize; SC2AIOptimize does not depend on a bot framework.

The core has no dependency on SC2API, python-sc2, MetaSwarm, MicroMachineEvo, BotOps or an ML runtime.

## Prime rule

> Correctness first. Measure total cost. Keep an optimisation only when its net benefit is positive.

Total cost includes adapter, dispatch and bookkeeping overhead. `OFF` / baseline is a first-class champion.

See `docs/ARCHITECTURE.md`, `docs/PROGRAMME.md` and `docs/MMEVO_Z_MIGRATION.md`.

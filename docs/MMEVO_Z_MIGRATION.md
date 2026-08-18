# MMEvo v0.9.2Z -> SC2AIOptimize migration map

Source branch: `Valessc2/MMEvo:mmevo-v0.9.2z`  
Source commit inspected: `d704c4420818fc0a3af319e2ea8e18518f7e0b7a`

MMEvo Z1-Z12 is an evidence-rich donor programme, not a file-copy mandate. SC2AIOptimize keeps generic mechanisms and proven lessons while leaving MME-specific gameplay semantics in MMEvo.

| Z | MMEvo result | SC2AIOptimize disposition |
|---|---|---|
| Z1 | Base75689 static-data FastPath | **Promote now** to `SC2Registry` with build/data identity and provenance. SC2API bridge remains consumer-side/generic-adapter work. |
| Z2 | frame spatial index | **Graduate** to Kernel/Hot spatial candidate with baseline fallback, stable identity and completeness proof. |
| Z3 | target-selection FastPath | **Graduate pattern** to generic scoring/selection kernels; preserve caller-defined ordering/tie semantics. |
| Z4 | influence-map kernel | **Graduate** as Kernel/Compute after reference/equivalence benchmark harness exists. |
| Z5 | compact MME per-unit runtime state | **Do not copy policy/state.** Retain stable-ID/lifetime/reset lessons for generic buffer/cache design. |
| Z6 | selective Cyclone ability queries | **Do not copy Cyclone policy.** Retain producer/consumer single-authority and “do not request unused data” rule for adapters. |
| Z7 | frame-time governor | **Graduate pattern** to generic budget/hysteresis machinery; consumers declare what work is optional. |
| Z8 | A* heap/best-cost/ownership | **Graduate** to Kernel pathing with explicit tie/equivalence contract and ownership tests. |
| Z9 | LTO/PGO infrastructure | **Graduate** as optional toolchain profiles; no fake profile data and no unsupported silent fallback. |
| Z10 | safe SIMD/AVX2 | **Graduate policy.** Vectorizer-friendly baseline first; ISA-specific candidate only under explicit hardware capability/evidence. |
| Z11 | parallel compute closed | **Keep the conclusion.** Deterministic single-thread default; persistent parallelism only if later profiling proves positive net benefit. |
| Z12 | hidden copy/correctness cleanup | **Keep the lesson.** Performance work must audit copies/ownership/correctness before adding machinery. |

## Z1 data imported

The initial Base75689 registry imports the same 245-record multiplayer/static subset used by MMEvo Z1. The data contains catalog name, race/mover, attributes, life/shields/energy, armor, raw speed, radius, sight, raw food, costs, weapon/ability counts and footprint presence.

This is sufficient as the first static registry, not as a claim that every future Kernel has all facts it will ever need. Weapon details, ability details, footprint geometry and other facts are added only when a proven consumer requires them, with versioned provenance.

## Critical non-port

`MMEPerformanceZ` itself is not copied into the shared library because it directly owns `CCBot`, `sc2::Unit`, Cyclone policy, MME candidate views and MME frame state. Those are exactly the couplings SC2AIOptimize exists to avoid.

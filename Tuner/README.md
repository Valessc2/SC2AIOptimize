# Tuner

The Tuner is generic control-plane machinery. Consumers register legal knobs and candidate implementations; SC2AIOptimize owns evidence aggregation and selection, not gameplay meaning.

Rules:

- `Baseline` / `OFF` is candidate `0` and remains a valid champion.
- Measurement inputs are total cost: kernel + adapter + dispatch + bookkeeping.
- Invalid/duplicate evidence and invalid policies fail closed.
- Default champion promotion requires repeated samples and conservative confidence separation.
- `BaselineOnly` mode and deterministic reset are explicit.
- Context/crossover selection uses compact numeric IDs and can choose different champions by workload/hardware/state.
- Runtime telemetry uses fixed storage; compile-time-disabled timers perform no clock call or mutation.
- The budget governor operates on the previous completed duration and only sheds work the consumer declared non-critical.
- Auto-calibration delegates winner selection to the single net-benefit authority and may produce baseline crossover bands.
- Tuner/control overhead must itself pass a measured cheapness gate.
- Persistence is versioned and provenance-bound; stale evidence is not silently reused.
- Tuning/search is not performed inside hot kernel loops.

See `docs/TUNER.md` and `docs/RUNTIME_OPTIMIZATION.md`.

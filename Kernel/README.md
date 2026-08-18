# Kernel

Framework-independent compute lives here.

`Hot` contains small baseline/candidate operations with explicit semantics and caller-owned output where practical. An accelerated implementation is not automatically selected merely because it exists; strategy/crossover profiles default to baseline until measurement certifies otherwise.

Larger native jobs belong in Compute only when profiling shows the larger native boundary produces positive measured net benefit.

See `docs/HOT_KERNELS.md`.

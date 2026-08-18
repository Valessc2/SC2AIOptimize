# Bench

Benchmarks are proof infrastructure, not runtime dependencies.

Every optional optimisation must be compared against a reference/baseline with representative workloads and include integration overhead in the decision. Required outputs eventually include p50/p95/p99, workload size/context, hardware/compiler identity, correctness status and crossover ranges.

A benchmark may conclude that the baseline is champion.

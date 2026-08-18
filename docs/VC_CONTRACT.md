# VC contract for SC2AIOptimize

This file records the ValesCodex rules currently established for this repository. It deliberately does not invent unavailable/unstated VC clauses.

## Authority and ownership

1. One authority per concern. Duplicate producer/consumer truth is a defect.
2. Core contracts are framework-neutral. Consumer semantics remain in the consumer.
3. Python/C++ adapters translate data; they do not own gameplay policy.
4. Generated/profiling outputs are evidence, not silent source truth.

## Correctness and failure

1. Correctness/equivalence precedes speed claims.
2. Unknown, stale, contradictory or insufficient optimisation evidence fails closed to baseline/reference behavior.
3. Lifecycle/reset behavior is explicit; state must not leak across games/tests.
4. Deterministic single-threaded behavior is preferred until profiling proves parallelism worth its complexity and cost.

## Performance

1. Profile first; optimise proven cost, not intuition.
2. Total cost includes adapter crossing, packing, bookkeeping and selection overhead.
3. Baseline is a first-class candidate and may be champion for any context.
4. An optional optimisation must be rejectable/disableable when measured net benefit is non-positive.
5. Hot paths avoid allocations, logging, virtual dispatch and tuning/search work unless measurement proves otherwise.
6. Prefer coarse/batched native calls and packed/contiguous data.
7. Hardware-specific features are explicit capability decisions, never blind assumptions.

## Dependencies

1. C++20/STL is the runtime baseline.
2. No python-sc2, SC2API, MetaSwarm, MMEvo or BotOps dependency in the core.
3. Third-party dependencies must justify runtime/build cost and stay isolated.
4. Python bindings are optional; a binding library must not become a dependency of C++ consumers.

## Proof and hygiene

1. Unit, equivalence, regression/property and representative benchmark evidence are separate concerns.
2. Performance failure is not correctness failure, and vice versa.
3. Compiler warnings are treated as defects in project-owned code.
4. Source, generated data, benchmarks and outputs carry provenance/version identity.
5. Implementation completion is never represented as runtime/performance certification.

When the authoritative ValesCodex source becomes accessible, this contract must be diffed against it rather than silently assuming equivalence.

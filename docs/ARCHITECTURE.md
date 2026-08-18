# Architecture

## Runtime pillars

SC2AIOptimize has three runtime pillars and no bot/framework ownership:

1. **SC2Registry** — immutable, build-pinned SC2 facts.
2. **Kernel** — framework-independent compute.
3. **Tuner** — cheap evidence-based selection/configuration.

`Adapters/Cpp` and `Adapters/Python` are generic integration surfaces. MetaSwarm, MicroMachineEvo and future bots adapt their own state into SC2AIOptimize contracts. The core never includes SC2API or python-sc2 types.

## Dependency direction

```text
consumer bot/framework
        |
        v
 generic adapter contract
        |
        +----> SC2Registry
        |
        +----> Kernel/Compute ----> Kernel/Hot
        |
        +----> Tuner
```

Kernel/Hot must not depend on Kernel/Compute. Registry must not depend on adapters or bots. Tuner may select registered Kernel variants but must not own bot policy semantics.

## Net-benefit law

The baseline implementation is candidate `0` and is always a legitimate champion. An optional optimisation is admitted only after correctness/equivalence proof and representative measurement of **total** cost, including adapter, dispatch and bookkeeping overhead.

An optimisation that is slower, too noisy, incorrect, or below the configured benefit threshold is disabled. `OFF` is a successful tuning result.

Runtime selection should be resolved outside inner loops wherever practical. Hot-path code should see a preselected function/strategy or a trivial context branch, not a live search process.

## Cheap-by-construction rules

- no mandatory third-party runtime dependencies;
- no ML framework in the tuner;
- no logging, heap allocation, RTTI-driven dispatch or framework object traversal in Hot kernels;
- packed/POD-like input surfaces where useful;
- cache and allocation ownership explicit;
- deterministic single-threaded behaviour by default;
- SIMD, parallelism, PGO and hardware-specific variants are opt-in candidates that must prove net benefit;
- generated benchmark/tuning/report machinery stays outside the ladder runtime dependency graph.

## Correctness and performance are separate gates

A performance win never excuses semantic drift. Candidate lifecycle:

```text
reference/baseline
    -> correctness contract
    -> equivalence/property/fuzz tests
    -> representative benchmark
    -> net-benefit decision
    -> champion/crossover output
    -> consumer
```

A performance failure means baseline remains active. A correctness failure disqualifies the candidate.

## SC2Registry

The first registry is Base75689, promoted from the MMEvo v0.9.2Z Z1 static-data work. It is a 245-record multiplayer/static subset with build and data-version identity. It is a seed, not a frozen universal schema: weapons, abilities, footprints and other proven-required facts may be added by versioned extensions.

Catalog-name lookup is intentionally simple and allocation-free. Consumers needing hot access should build a dense consumer-ID mapping once during startup/validation.

## Tuner ownership

Consumers own parameter meaning, legal ranges and safety. `TunableRegistry` describes legal knobs; the Tuner owns generic evidence, champion/challenger selection, confidence/context machinery and persistence. It must never introspect arbitrary bot memory.

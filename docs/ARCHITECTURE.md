# Architecture

## Runtime pillars

SC2AIOptimize has three runtime pillars and no bot/framework ownership:

1. **SC2Registry** — immutable, build-pinned SC2 facts.
2. **Kernel** — framework-independent compute.
3. **Tuner** — cheap evidence-based selection/configuration.

`Adapters/Cpp`, the stable C ABI and `Adapters/Python` are generic integration surfaces. MetaSwarm, MicroMachineEvo and future bots adapt their own state into SC2AIOptimize contracts. The core never includes SC2API or python-sc2 types.

## Shared integration connector

`sc2opt/adapters/IntegrationContract.hpp` is the one compatibility authority in front of those language edges. It validates API generation, neutral view ABI, registry identity and generic capability availability. C/C++14 and Python consumers reach the same authority through their respective edges rather than maintaining bot-specific compatibility logic.

The connector does not own framework traversal, bot state, packing policy, gameplay decisions or consumer lifecycle. A failed compatibility gate means the optimisation path is not admitted; the consumer's baseline remains authoritative.

## Dependency direction

```text
consumer bot/framework
        |
        v
consumer-owned translation / packing
        |
        v
shared integration compatibility contract
        |
        +----> C++20 / C ABI / Python language edge
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

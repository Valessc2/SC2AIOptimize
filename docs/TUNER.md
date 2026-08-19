# Tuner design

## Purpose

The Tuner answers a narrow question: **which already-correct implementation/configuration should be used for this declared context?** It does not own bot semantics and it does not run an ML framework.

## Production seed policy

`Defaults.hpp` is the single authority for V1 engineering priors:

- operational minimum evidence: 20 samples
- certification evidence seed: 50 samples
- minimum default promotion margin: 2% and 1,000 ns
- champion confidence seed: z = 1.96
- maximum tuner/control overhead: 1% and 50,000 ns
- workload sizes: 8/16/32/64/128/256/512/1024
- batch sizes: 16/32/64/128/256/512/1024
- spatial cell sizes: 2/3/4/6/8/12

These are **priors, not limits**. Consumers may supply stricter or looser policies when representative evidence warrants it. A tiny repeated hotspot may legitimately lower the absolute/relative promotion margins; SC2AIOptimize does not impose a universal performance ceiling.

## S20 — net-benefit gate

Candidate `0` is the baseline/OFF path. `ChooseNetBenefitChampion` consumes already-measured total-cost evidence and refuses to promote incorrect, under-sampled or non-beneficial candidates. Defaults require 20 samples and the V1 seed promotion margin, but callers own the policy.

## S21 — TunableRegistry

Consumers own tunable meaning, storage and safety. `TunableSpec` carries only the legal numeric contract and flags such as online-safe, reset-required and strategic. Registry validation is startup/control-plane work: invalid specs, non-finite values, malformed Integer/Choice contracts, duplicate IDs and duplicate names fail closed.

## S22 — champion/challenger

`RunningStats` uses Welford accumulation and stores only sample count, mean and M2. No sample history is retained. `EvaluateChampion` is allocation-free and compares the current champion against the best eligible challenger.

Default promotion requires at least 20 samples, the V1 seed gain margin, and conservative interval separation using z = 1.96. This is deliberately a lightweight normal-approximation gate, not a claim of full statistical inference. Certification/benchmark profiles may require more evidence; the seed certification count is 50 samples.

`BaselineOnly` is an explicit mode. `ResetToBaseline` and `ResetEvidence` make test/game lifecycle behavior deterministic.

## S23 — context and crossovers

`ContextKey` is three cheap numeric dimensions: workload, hardware and state flags. Unknown contexts fail to the supplied fallback, normally baseline.

Ordered `CrossoverBand` records implement the useful case where baseline wins for small N and another kernel wins only after a measured crossover. Invalid/non-increasing bands fail closed.

The intended steady state is:

```text
context / N -> persisted champion or crossover band -> execute
```

not repeated benchmarking on every call.

## S24 — persistence and provenance

Champion evidence persists in a strict versioned standard-library text record. Records carry source revision, SC2 build/data version, compiler profile, hardware profile and workload profile. Missing identity, unsupported schema, invalid numeric values or malformed data is rejected.

The persistence path may allocate because it is control-plane/I/O work; it is not part of the hot runtime path.

## Cheapness invariant

The Tuner must never become the thing being optimised. Candidate measurement and discovery are infrequent/offline or explicitly scheduled. Once a champion is known, consumers should pay only a tiny context/ID lookup plus the chosen implementation.
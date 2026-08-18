# Architecture

## Runtime dependency shape

```text
consumer policy (outside repo)
       |
generic C++ / Python boundary
       |
+------+-------+---------+
| SC2Registry  | Kernel  | Tuner |
+--------------+---------+-------+
```

The three runtime pillars are deliberately small. Bench, tests, generators, profilers and reports are development/proof tooling and do not belong in the ladder hot path.

## SC2Registry

Immutable, build-pinned facts. The registry owns neutral schemas and generated data, not framework IDs. Adapters map external IDs at startup. The Z1 Base75689 table from MMEvo is the seed model, not a frozen final schema: weapon, ability and footprint detail should be separate compact records when real consumers require them.

Registry hot access is numeric/O(1). Name matching and live validation are startup-only operations.

## Kernel

Pure/shared compute. `Hot` admission is earned by profiling; larger `Compute` jobs exist when a coarse native boundary beats many small crossings. Kernels consume neutral data only and return scores/results, never bot policy decisions.

A kernel may have several implementations plus a baseline. The baseline is always eligible to win.

## Tuner

Lightweight control-plane machinery. It owns measurement aggregation, champion/challenger selection, confidence, context/crossover selection and persistence. It does not understand what a bot parameter *means* and it does not inspect arbitrary bot memory.

Normal runtime should pay only the cost of an already-resolved choice wherever possible. Exploration/calibration is infrequent or offline.

## Dependency law

- Registry must not depend on Kernel, Tuner or consumer frameworks.
- Hot kernels must not depend upward on larger Compute/Tuner/adapters.
- Kernel may read Registry through narrow neutral views when required.
- Tuner may choose among Kernel/baseline options but must not become part of a kernel inner loop.
- Adapters translate only; no gameplay policy.
- Consumers adapt to SC2AIOptimize, not vice versa.

## Proof chain

```text
implementation
 -> correctness/reference proof
 -> equivalence/property proof
 -> benchmark
 -> include integration overhead
 -> net-benefit gate
 -> champion/crossover output
 -> consumer
```

No stage may promote an optimisation by assumption.

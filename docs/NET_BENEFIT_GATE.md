# Net-benefit gate

`Baseline` is not a fallback of shame; it is a first-class implementation candidate.

An optional optimisation is eligible only when all of the following are true:

1. Its output is correctness/equivalence-valid for the tested contract.
2. The measurement itself is valid and representative of the declared context.
3. Total candidate cost includes integration overhead such as packing, boundary crossing, bookkeeping and selection.
4. The measured improvement clears the configured absolute/fractional admission margin.
5. The evidence identity matches the relevant hardware/compiler/workload/schema context.

Otherwise baseline wins.

The V0.1 `decide_net_benefit` primitive intentionally fails closed and does not pretend to provide statistical confidence. Repeated samples, variance/tail handling, confidence and persisted context-specific champions belong to later Tuner slices.

Runtime target after a champion is established:

```text
context bucket -> already-resolved champion -> execute
```

The expensive question of *which implementation wins?* should be asked infrequently, offline, at calibration time, or in tightly controlled exploration—not in every hot loop.

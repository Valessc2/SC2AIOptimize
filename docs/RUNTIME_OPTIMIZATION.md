# Runtime optimisation — S25-S27

## S25 — profiling / telemetry

`FixedLatencyHistogram` is a 65-bucket log2 histogram with fixed storage, allocation-free observation and saturating counters. It reports sample count, total/min/max and conservative p50/p95/p99 bucket upper bounds. The coarse quantiles are intentional: live telemetry should be cheap; exact distribution analysis belongs in benchmark/proof tooling.

`ScopedLatencyTimer<false>` is a compile-time-disabled empty type: no clock read and no histogram mutation. Consumers can therefore compile telemetry out of genuinely hot paths rather than paying a runtime feature switch on every call.

## S26 — compute-budget governor

The governor generalises the MMEvo Z7 latency-hysteresis pattern without importing MME policy. **The governor is disabled by default.** When disabled it always reports Normal pressure and permits all consumer work.

A consumer may explicitly delegate a full compute envelope using `BudgetPolicyFromEnvelope`. The default helper converts that consumer-owned value into:

- resume: 55% of envelope
- soft: 70% of envelope
- hard: 95% of envelope

For example, a consumer that explicitly supplies a 40 ms envelope receives 22/28/38 ms thresholds. SC2AIOptimize never invents the 40 ms envelope itself.

When enabled:

- Normal: all declared work is allowed.
- Soft: consumer-declared Optional work is shed.
- Hard: only consumer-declared Critical work is allowed.
- Between resume and soft thresholds, previous pressure is retained to prevent oscillation.
- Invalid enabled threshold ordering fails closed to Hard.

The governor never decides what gameplay work means and never times work itself; it consumes the previous completed duration. A consumer that does not delegate shedding authority is completely unaffected.

## S27 — auto-calibration

`BuildCrossoverCalibration` accepts caller-owned workload points and already-measured candidate evidence. It validates grouping, requires a baseline at every point, delegates candidate selection to `ChooseNetBenefitChampion`, and compresses adjacent equal champions into tiny `CrossoverBand` outputs.

This preserves one authority for net-benefit semantics. Calibration does not duplicate the selection rule and may produce baseline/OFF for any workload range.

`CheckControlOverhead` makes the optimiser measure itself. The V1 seed ceiling is both 1% of chosen work and 50 microseconds absolute. Consumers may choose stricter or looser values; these are admission priors, not performance caps.

## Seed candidate sets

The initial geometric/discrete search sets are:

- workload N: 8/16/32/64/128/256/512/1024
- batch size: 16/32/64/128/256/512/1024
- spatial cell size: 2/3/4/6/8/12

Calibration is expected to reject, narrow or replace these per hardware/workload context.

## Runtime target

```text
fixed/compiled telemetry when justified
        -> previous completed duration (only if consumer enabled a governor)
        -> tiny hysteresis/context state
        -> already-resolved champion ID
        -> chosen implementation
```

Discovery and benchmarking remain control-plane/offline operations. The hot path should execute an already-resolved choice, not search for one.
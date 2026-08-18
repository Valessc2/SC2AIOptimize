# Runtime optimisation — S25-S27

## S25 — profiling / telemetry

`FixedLatencyHistogram` is a 65-bucket log2 histogram with fixed storage, allocation-free observation and saturating counters. It reports sample count, total/min/max and conservative p50/p95/p99 bucket upper bounds. The coarse quantiles are intentional: live telemetry should be cheap; exact distribution analysis belongs in benchmark/proof tooling.

`ScopedLatencyTimer<false>` is a compile-time-disabled empty type: no clock read and no histogram mutation. Consumers can therefore compile telemetry out of genuinely hot paths rather than paying a runtime feature switch on every call.

## S26 — compute-budget governor

The governor generalises the MMEvo Z7 latency-hysteresis pattern without importing MME policy. The default thresholds retain the proven seed values (22 ms resume, 28 ms soft, 38 ms hard), but consumers own the budget policy and classify their own work as `Critical`, `Important` or `Optional`.

- Normal: all declared work is allowed.
- Soft: Optional work is shed.
- Hard: only Critical work is allowed.
- Between resume and soft thresholds, previous pressure is retained to prevent oscillation.
- Invalid threshold ordering fails closed to Hard.

The governor never decides what gameplay work *means* and never times work itself; it consumes the previous completed duration.

## S27 — auto-calibration

`BuildCrossoverCalibration` accepts caller-owned workload points and already-measured candidate evidence. It validates grouping, requires a baseline at every point, delegates candidate selection to `ChooseNetBenefitChampion`, and compresses adjacent equal champions into tiny `CrossoverBand` outputs.

This preserves one authority for net-benefit semantics. Calibration does not duplicate the selection rule and may produce baseline/OFF for any workload range.

`CheckControlOverhead` makes the optimiser measure itself: a selector/calibration/control path can be rejected when measured overhead exceeds an absolute and/or fractional budget. The default fractional ceiling is 1% of chosen work; target projects may choose stricter values.

## Runtime target

```text
fixed/compiled telemetry when justified
        -> previous completed duration
        -> tiny hysteresis state
        -> context/crossover champion ID
        -> chosen implementation
```

Discovery and benchmarking remain control-plane/offline operations. The hot path should execute an already-resolved choice, not search for one.

## Evidence status

S25-S27 implementation and component tests do not certify a target-hardware speedup. Representative benchmark profiles, target-hardware crossover values and consumer integration evidence remain S28-S32 work.

# Proof and outputs — S28-S30

## S28 deep tests

The deterministic deep-contract suite adds property/fuzz-style coverage without a fuzzing runtime dependency:

- geometry symmetry and boundary invariants across thousands of generated cases;
- filtering equivalence against an independent reference predicate;
- spatial-grid exact InputOrder equivalence against brute force across generated worlds, cell sizes, radii and strict/inclusive boundaries;
- unordered spatial membership equivalence;
- non-finite input fail-closed regression coverage.

The generator seed is fixed so failures are reproducible in CI. Dedicated sanitizer/fuzzer toolchain integration remains S31.

## S29 benchmark/workload profiles

Workloads have explicit identity, SC2 build/data version, family, version, kind and corpus identity. Synthetic and representative evidence are different types of proof.

The spatial microbenchmark now records p50/p95/p99 from 31 samples and declares its deterministic profile/seed in output. CI timing remains synthetic calibration data and is explicitly not certification.

`Profiles/representative.template.json` is only a contract/template. It cannot become evidence until a real consumer corpus replaces all placeholders and receives a stable corpus identity/hash.

## S30 consumable outputs

`sc2opt_proof` is a separate proof-only library and is **not linked by `sc2opt::sc2opt`**. It validates hardware/workload identity and writes a versioned certification JSON manifest with benchmark records and proof-layer status.

The writer enforces monotonic proof layers:

```text
source/design -> build/tests -> representative benchmark -> consumer integration -> ladder/runtime
```

Representative benchmark proof additionally requires a representative workload and non-empty correctness-passing benchmark evidence. This prevents a green CI run or synthetic benchmark from being serialized as a stronger certification than it really is.

The JSON schema is published under `schemas/` for BotOps and other consumers.

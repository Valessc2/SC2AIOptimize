# V1 implementation and certification status

## Implementation

The 32-slice V1 software programme is **32/32 implemented**:

- S01-S04 foundation;
- S05-S08 generic C++/Python consumption;
- S09-S19 Kernel Hot/Compute;
- S20-S27 Tuner/runtime optimisation;
- S28-S30 proof/output;
- S31 toolchain hardening;
- S32 stable API/schema + BotOps integration contract.

The current public API contract is `1.1` with status `v1-candidate`. API `1.1` is an additive post-programme extension of the original S32 `1.0` contract, adding the shared bot-neutral integration compatibility handshake without invalidating `1.0` consumers. Package/source version remains pre-1.0 until the external certification gates close.

This is **not the same statement as V1 runtime certification**.

## Proven component layers

Repository CI has proven the source/build/component layer across Ubuntu and Windows core builds/tests, generic Python adapters, ASAN+UBSAN, clang-tidy, install/export consumption with LTO on both operating systems, and an explicit AVX2 compile contract.

The shared integration connector additionally has synthetic native/C ABI/Python compatibility tests and a C++14 consumer proof. Those component results do not claim target-hardware performance or bot/ladder behaviour.

## Certification layers still outstanding

The following remain external evidence gates before the API status can be promoted from `v1-candidate` to a certified V1 release:

1. representative target-hardware benchmark corpus with recorded p50/p95/p99 and net-benefit/crossover decisions;
2. generic real consumer integration evidence (at minimum one C++/native and one Python bot/framework integration beyond synthetic consumers);
3. target bot behavioural/equivalence evidence where replacing existing logic;
4. ladder/runtime evidence for the intended release lane;
5. BotOps acceptance of the resulting manifest/package under its own release policy.

No source code change should flip those proof bits without the corresponding external evidence artifact.

# V1 implementation and certification status

## Implementation

The 32-slice V1 software programme is implemented when S31-S32 land:

- S01-S04 foundation;
- S05-S08 generic C++/Python consumption;
- S09-S19 Kernel Hot/Compute;
- S20-S27 Tuner/runtime optimisation;
- S28-S30 proof/output;
- S31 toolchain hardening;
- S32 stable API/schema + BotOps integration contract.

This is **not the same statement as V1 runtime certification**.

## Certification layers

Current repository/CI work can prove source/design and build/tests. The following remain external evidence gates before the API status can be promoted from `v1-candidate` to a certified V1 release:

1. representative target-hardware benchmark corpus with recorded p50/p95/p99 and net-benefit/crossover decisions;
2. generic real consumer integration evidence (at minimum one C++ and one Python bot/framework integration beyond synthetic consumers);
3. target bot behavioural/equivalence evidence where replacing existing logic;
4. ladder/runtime evidence for the intended release lane;
5. BotOps acceptance of the resulting manifest/package under its own release policy.

No source code change should flip those proof bits without the corresponding external evidence artifact.

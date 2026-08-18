# BotOps integration contract

SC2AIOptimize exposes evidence; BotOps owns integration/release policy.

The machine-readable handshake is `integrations/botops/contract-v1.json` plus `schemas/sc2aiopt-certification-v1.schema.json`.

BotOps should validate:

- API contract major/minor;
- source revision;
- SC2 build/data identity;
- hardware/workload identity and corpus provenance;
- benchmark correctness status and tail metrics;
- each proof layer independently.

SC2AIOptimize does **not** dictate which proof layer BotOps requires for a given release lane. For example, a component CI lane may reasonably accept source/build proof while an AI Arena ladder promotion can require representative benchmark, consumer integration and ladder/runtime evidence.

BotOps must not infer a later proof layer from an earlier one. A green SC2AIOptimize CI run is not a representative benchmark, and a representative benchmark is not a live bot integration result.

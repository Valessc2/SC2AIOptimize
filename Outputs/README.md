# Outputs

Generated benchmark, tuning, hardware/workload and certification manifests live here when explicitly produced for a run/release. They are not source authority and are git-ignored by default unless a reviewed fixture/baseline is intentionally committed.

Every consumable output must carry enough provenance to answer: what source/build produced this, on what hardware/workload, with what correctness status and confidence?

S30 provides a standard-library JSON writer plus `schemas/sc2aiopt-certification-v1.schema.json`. The writer fails closed when proof layers skip required gates, a synthetic workload attempts to claim representative proof, benchmark records are malformed, or identity/provenance is missing.

Proof layers are ordered:

```text
source/design -> build/tests -> representative benchmark -> consumer integration -> ladder/runtime
```

A later layer cannot be asserted without all earlier layers. This keeps CI/build success from being misrepresented as runtime certification.

# Profiles

Hardware and workload profiles make benchmark evidence contextual rather than universal.

- `spatial.synthetic.v1.json` is a deterministic synthetic calibration workload. It is useful for equivalence and rough crossover discovery but **cannot** claim representative benchmark proof.
- `representative.template.json` defines the minimum identity shape for a consumer-supplied replay/game/workload corpus. The template itself is not evidence and must never be treated as certified data.

Representative profiles should identify the exact corpus (preferably a content hash), SC2 build/data version, selection rule and workload family. Hardware identity belongs in generated evidence/manifests rather than being hard-coded as source truth.

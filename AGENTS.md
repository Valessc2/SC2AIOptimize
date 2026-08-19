# SC2AIOptimize agent rules

All coding work in this repository is governed by the canonical **ValesCodex** standard.

**Required ValesCodex contract revision:** `VC-1.5`

Canonical authority: `Valessc2/ValesSC2BotOps` → `docs/truth-model/VALESCODEX_ENGINEERING_STANDARD_V1.md`.

This file is a repository overlay, not a substitute copy of ValesCodex. Read the canonical standard before implementation and before any repository/publication state transition.

## SC2AIOptimize overlay

- SC2AIOptimize is a public generic policy-neutral optimisation engine. It accelerates compute; it does not own bot gameplay policy or commit authority.
- Inspect materially affected public C ABI, Python/C++ adapters, registry/data-version contracts and private consumers before changing shared interfaces.
- Preserve baseline/OFF fallback and total-cost correctness; optimisation wins must include packing/adapter/dispatch/materialisation cost where applicable.
- Apply Addendum 5 to repair families and preserve non-transitive repository/publication authority.
- Preserve failed/flaky correctness, sanitizer, ABI, package and performance evidence; do not relax proof or crossover criteria merely to obtain green status.
- Generated registry/data artefacts must follow their authoritative input/version and regeneration contract.

If a material prerequisite cannot be inspected or proved, fail closed or report the exact evidence ceiling.

When a local checkout and BotOps checkout are available, use:

```text
python <ValesSC2BotOps>/ops/harness/universal/valescodex_preflight.py --repo <path> --expected-repository Valessc2/SC2AIOptimize --expected-ref main
```

Remote/connector-only work must use the VC-1.5 remote evidence rules and must not claim local working-tree proof.

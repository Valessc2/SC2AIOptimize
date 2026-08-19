# SC2AIOptimize agent rules

All coding work in this repository is governed by **ValesCodex** before implementation begins.

Canonical authority is maintained in `Valessc2/ValesSC2BotOps` at `docs/truth-model/VALESCODEX_ENGINEERING_STANDARD_V1.md`. Repository-specific rules may strengthen ValesCodex but may not weaken it.

## Mandatory pre-code baseline

1. Resolve the exact repository, intended branch/ref, task scope, affected module and authorised files/behaviour.
2. Read repository-local instructions plus relevant README, architecture/contracts, CMake/build configuration and recent relevant changes before proposing code.
3. Verify origin, intended ref, exact HEAD, remote freshness and any supplied accepted-baseline ancestry when network access is available.
4. Inspect tracked and untracked working-tree state and active Git operations before mutation.
5. Inspect every materially affected provider/consumer repository before changing a public API, ABI, adapter, package or generated-data boundary.
6. Preserve existing user work; never silently discard/overwrite, force-push, rewrite history, amend or stage unrelated files.
7. Apply the C++, Python or mixed-language ValesCodex overlay as applicable.
8. Run correctness/build/test/static/sanitiser/package proof proportional to risk, then re-check repository state.
9. Report generated/applied/committed/pushed/PR-opened/merged/released/live states precisely.

If a material prerequisite cannot be inspected or proved, fail closed or explicitly report the unproved boundary rather than assuming it.

When a local checkout and BotOps checkout are available, use:

```text
python <ValesSC2BotOps>/ops/harness/universal/valescodex_preflight.py --repo <path> --expected-repository <owner/repo> --expected-ref <branch>
```

Normal preflight should prove remote freshness; `--offline` is deliberately weaker evidence.

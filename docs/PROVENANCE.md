# SC2AIOptimize source provenance

## Base75689 registry seed

- Source repository: `Valessc2/MMEvo`
- Source branch: `mmevo-v0.9.2z`
- Inspected source commit: `d704c4420818fc0a3af319e2ea8e18518f7e0b7a`
- Donor files: `MMEData75689.h/.cpp`, `MMEData75689_mp_a.inc`, `MMEData75689_mp_b.inc`
- SC2 build: `75689`
- SC2 data version: `B89B5D6FA7CBF6452E721311BFBC6CB2`
- Imported record count: `245`

The SC2AIOptimize representation removes MMEvo/SC2API coupling and exposes a framework-independent immutable registry view. The donor runtime bridge was intentionally not copied because it depends on SC2API and MMEvo ownership.

See `THIRD_PARTY_NOTICES.md` for the donor repository license notice.

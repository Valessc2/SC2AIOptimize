# Generic C++ adapter

C++ consumers use SC2AIOptimize's neutral headers directly and own translation from their framework types.

Rules:

- No `sc2::Unit`, MMEvo, MetaSwarm or other consumer type is allowed in SC2AIOptimize core headers.
- Translate once at a coarse boundary into compact neutral views.
- Prefer spans/caller-owned buffers; do not manufacture per-query object graphs.
- External type-ID to `SC2Registry::UnitIndex` mapping is an adapter/startup responsibility.
- The consumer owns policy and final decisions.

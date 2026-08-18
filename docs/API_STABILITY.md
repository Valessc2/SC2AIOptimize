# API stability

Public headers under `include/sc2opt/` are the intended consumer surface. Internal benchmark/test/generated machinery must not leak into that API.

Before V1, source compatibility may change when required to correct the architecture. V1 certification will define the compatibility/versioning policy explicitly.

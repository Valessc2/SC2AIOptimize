# Generic C++ adapter

The public C++ adapter contract is framework-free. `UnitView` / `WorldView` expose standard-layout records/spans; consuming bots map their own objects at their boundary.

`BuildDenseTypeMap` provides a startup-only, allocation-free mapping from consumer type IDs + catalog names to `SC2Registry` records. Unknown names remain null/unaccelerated; ambiguous duplicate consumer IDs fail closed.

SC2AIOptimize does not provide or own bot-specific adapters.

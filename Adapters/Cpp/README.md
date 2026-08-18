# Generic C++ adapter

The public C++ adapter contract is `sc2opt::adapters::cpp::WorldView` / `UnitView`: standard-layout, trivially-copyable records with no SC2API dependency.

A consuming C++ bot maps its native framework objects into these views at its boundary. SC2AIOptimize does not provide or own bot-specific adapters.

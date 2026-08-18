#pragma once

#include "sc2opt/model/WorldView.hpp"

namespace sc2opt::adapters::cpp {

// Backward-compatible C++ adapter aliases. The neutral data contract belongs to `model`, not
// to an adapter implementation, so Kernel code can consume it without depending on Adapters.
using UnitFlag = model::UnitFlag;
using UnitView = model::UnitView;
using WorldView = model::WorldView;

inline constexpr UnitFlag UnitAlive = model::UnitAlive;
inline constexpr UnitFlag UnitFlying = model::UnitFlying;
inline constexpr UnitFlag UnitStructure = model::UnitStructure;
inline constexpr UnitFlag UnitVisible = model::UnitVisible;

}  // namespace sc2opt::adapters::cpp

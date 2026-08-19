#pragma once

#include <cstdint>
#include <string_view>

// Package/source version. Keep this single authority available to all language adapters.
#define SC2OPT_VERSION_MAJOR 0
#define SC2OPT_VERSION_MINOR 1
#define SC2OPT_VERSION_PATCH 0
#define SC2OPT_VERSION_STRING "0.1.0-dev"

namespace sc2opt {

inline constexpr std::uint16_t kPackageVersionMajor = SC2OPT_VERSION_MAJOR;
inline constexpr std::uint16_t kPackageVersionMinor = SC2OPT_VERSION_MINOR;
inline constexpr std::uint16_t kPackageVersionPatch = SC2OPT_VERSION_PATCH;
inline constexpr std::string_view kPackageVersion = SC2OPT_VERSION_STRING;

// Stable consumer contract generation. The package itself remains pre-1.0 until the external
// S32 representative/integration/runtime certification gates close.
inline constexpr std::uint16_t kApiContractMajor = 1;
inline constexpr std::uint16_t kApiContractMinor = 1;
inline constexpr std::string_view kApiContractStatus = "v1-candidate";

}  // namespace sc2opt

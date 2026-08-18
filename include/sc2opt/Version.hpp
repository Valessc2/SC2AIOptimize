#pragma once

#include <cstdint>
#include <string_view>

namespace sc2opt {

// Stable consumer contract generation. The package itself remains pre-1.0 until the external
// S32 representative/integration/runtime certification gates close.
inline constexpr std::uint16_t kApiContractMajor = 1;
inline constexpr std::uint16_t kApiContractMinor = 0;
inline constexpr std::string_view kApiContractStatus = "v1-candidate";

}  // namespace sc2opt

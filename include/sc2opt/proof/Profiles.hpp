#pragma once

#include <cstdint>
#include <string_view>

namespace sc2opt::proof {

enum class WorkloadKind : std::uint8_t { Synthetic = 0, Representative = 1 };

struct HardwareProfileView {
    std::string_view id;
    std::string_view cpu;
    std::string_view os;
    std::string_view compiler;
    std::string_view build_config;
};

struct WorkloadProfileView {
    std::string_view id;
    std::string_view family;
    std::string_view version;
    WorkloadKind kind = WorkloadKind::Synthetic;
    std::string_view sc2_build;
    std::string_view data_version;
    std::string_view corpus_identity;
};

[[nodiscard]] bool IsValid(const HardwareProfileView& profile) noexcept;
[[nodiscard]] bool IsValid(const WorkloadProfileView& profile) noexcept;

}  // namespace sc2opt::proof

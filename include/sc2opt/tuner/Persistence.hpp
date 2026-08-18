#pragma once

#include "sc2opt/tuner/Context.hpp"

#include <cstdint>
#include <iosfwd>
#include <string>

namespace sc2opt::tuner {

inline constexpr std::uint32_t kChampionRecordSchemaVersion = 1;
inline constexpr std::uint64_t kInvalidDecisionId = 0;

struct EvidenceProvenance {
    std::string source_revision;
    std::string sc2_build;
    std::string data_version;
    std::string compiler_profile;
    std::string hardware_profile;
    std::string workload_profile;
};

struct ChampionRecord {
    std::uint32_t schema_version = kChampionRecordSchemaVersion;
    std::uint64_t decision_id = kInvalidDecisionId;
    ContextKey context{};
    CandidateId champion = kBaselineCandidate;
    std::uint64_t samples = 0;
    double mean_total_cost_ns = 0.0;
    double sample_variance_ns2 = 0.0;
    EvidenceProvenance provenance{};
};

[[nodiscard]] bool IsValid(const ChampionRecord& record) noexcept;
[[nodiscard]] bool WriteChampionRecord(std::ostream& out, const ChampionRecord& record);
[[nodiscard]] bool ReadChampionRecord(std::istream& in, ChampionRecord& record);

}  // namespace sc2opt::tuner

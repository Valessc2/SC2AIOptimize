#pragma once

#include "sc2opt/proof/Profiles.hpp"
#include "sc2opt/tuner/NetBenefit.hpp"

#include <cstddef>
#include <cstdint>
#include <iosfwd>
#include <span>
#include <string_view>

namespace sc2opt::proof {

inline constexpr std::uint32_t kCertificationManifestSchemaVersion = 1;

struct ProofLayers {
    bool source_design = false;
    bool build_tests = false;
    bool representative_benchmark = false;
    bool consumer_integration = false;
    bool ladder_runtime = false;
};

struct BenchmarkRecord {
    tuner::CandidateId candidate = tuner::kBaselineCandidate;
    std::size_t workload_size = 0;
    std::uint64_t samples = 0;
    double mean_total_cost_ns = 0.0;
    std::uint64_t p50_ns = 0;
    std::uint64_t p95_ns = 0;
    std::uint64_t p99_ns = 0;
    bool correctness_passed = false;
};

struct CertificationManifestView {
    std::uint32_t schema_version = kCertificationManifestSchemaVersion;
    std::string_view source_revision;
    std::string_view generated_utc;
    HardwareProfileView hardware{};
    WorkloadProfileView workload{};
    ProofLayers proof{};
    std::span<const BenchmarkRecord> benchmarks{};
};

enum class ManifestIssue : std::uint8_t {
    None,
    SchemaVersion,
    MissingIdentity,
    InvalidHardware,
    InvalidWorkload,
    InvalidProofOrder,
    MissingRepresentativeEvidence,
    InvalidBenchmark
};

[[nodiscard]] ManifestIssue ValidateManifest(const CertificationManifestView& manifest) noexcept;
[[nodiscard]] bool WriteCertificationManifestJson(std::ostream& out,
                                                  const CertificationManifestView& manifest);

}  // namespace sc2opt::proof

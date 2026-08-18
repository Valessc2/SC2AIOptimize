#include "sc2opt/proof/Manifest.hpp"

#include <array>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace {

int failures = 0;

void Check(bool condition, std::string_view message)
{
    if (!condition)
    {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}

void TestProfilesAndManifest()
{
    using namespace sc2opt::proof;

    const HardwareProfileView hardware{"test-hw", "Example CPU", "linux", "gcc-14", "Release"};
    const WorkloadProfileView workload{"dense-fight-v1", "spatial", "1", WorkloadKind::Representative,
                                       "75689", "B89", "sha256:abc123"};
    const std::array records{
        BenchmarkRecord{0, 128, 100, 1000.0, 900, 1200, 1400, true},
        BenchmarkRecord{1, 128, 100, 600.0, 550, 800, 950, true},
    };

    CertificationManifestView manifest{};
    manifest.source_revision = "abc\"def\\ghi";
    manifest.generated_utc = "2026-08-19T00:00:00Z";
    manifest.hardware = hardware;
    manifest.workload = workload;
    manifest.proof = {true, true, true, false, false};
    manifest.benchmarks = records;

    Check(ValidateManifest(manifest) == ManifestIssue::None, "valid manifest");
    std::ostringstream out;
    Check(WriteCertificationManifestJson(out, manifest), "manifest writer succeeds");
    const std::string json = out.str();
    Check(json.find("abc\\\"def\\\\ghi") != std::string::npos,
          "manifest JSON escapes identity strings");
    Check(json.find("\"candidate\": 1") != std::string::npos,
          "manifest contains candidate records");

    CertificationManifestView invalid_order = manifest;
    invalid_order.proof = {true, true, false, false, true};
    Check(ValidateManifest(invalid_order) == ManifestIssue::InvalidProofOrder,
          "proof layers cannot skip gates");

    CertificationManifestView synthetic_claim = manifest;
    synthetic_claim.workload.kind = WorkloadKind::Synthetic;
    Check(ValidateManifest(synthetic_claim) == ManifestIssue::MissingRepresentativeEvidence,
          "synthetic workload cannot claim representative benchmark proof");

    CertificationManifestView bad_benchmark = manifest;
    const std::array bad_records{BenchmarkRecord{1, 128, 10, 1.0, 100, 90, 110, true}};
    bad_benchmark.benchmarks = bad_records;
    Check(ValidateManifest(bad_benchmark) == ManifestIssue::InvalidBenchmark,
          "invalid benchmark quantile order rejected");
}

}  // namespace

int main()
{
    TestProfilesAndManifest();
    if (failures == 0)
    {
        std::cout << "SC2AIOptimize manifest tests passed\n";
        return 0;
    }
    return 1;
}

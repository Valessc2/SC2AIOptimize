#include "sc2opt/proof/Manifest.hpp"

#include <cmath>
#include <iomanip>
#include <ostream>

namespace sc2opt::proof {
namespace {

bool BenchmarkValid(const BenchmarkRecord& record) noexcept
{
    return record.samples > 0 && std::isfinite(record.mean_total_cost_ns) &&
           record.mean_total_cost_ns >= 0.0 && record.p50_ns <= record.p95_ns &&
           record.p95_ns <= record.p99_ns;
}

bool ProofOrderValid(const ProofLayers& proof) noexcept
{
    if (proof.build_tests && !proof.source_design)
        return false;
    if (proof.representative_benchmark && !proof.build_tests)
        return false;
    if (proof.consumer_integration && !proof.representative_benchmark)
        return false;
    if (proof.ladder_runtime && !proof.consumer_integration)
        return false;
    return true;
}

void WriteJsonString(std::ostream& out, std::string_view value)
{
    static constexpr char hex[] = "0123456789abcdef";
    out.put('"');
    for (const unsigned char ch : value)
    {
        switch (ch)
        {
        case '"': out << "\\\""; break;
        case '\\': out << "\\\\"; break;
        case '\b': out << "\\b"; break;
        case '\f': out << "\\f"; break;
        case '\n': out << "\\n"; break;
        case '\r': out << "\\r"; break;
        case '\t': out << "\\t"; break;
        default:
            if (ch < 0x20u)
            {
                out << "\\u00" << hex[(ch >> 4u) & 0x0fu] << hex[ch & 0x0fu];
            }
            else
            {
                out.put(static_cast<char>(ch));
            }
            break;
        }
    }
    out.put('"');
}

void WriteBool(std::ostream& out, bool value)
{
    out << (value ? "true" : "false");
}

}  // namespace

ManifestIssue ValidateManifest(const CertificationManifestView& manifest) noexcept
{
    if (manifest.schema_version != kCertificationManifestSchemaVersion)
        return ManifestIssue::SchemaVersion;
    if (manifest.source_revision.empty() || manifest.generated_utc.empty())
        return ManifestIssue::MissingIdentity;
    if (!IsValid(manifest.hardware))
        return ManifestIssue::InvalidHardware;
    if (!IsValid(manifest.workload))
        return ManifestIssue::InvalidWorkload;
    if (!ProofOrderValid(manifest.proof))
        return ManifestIssue::InvalidProofOrder;
    if (manifest.proof.representative_benchmark &&
        (manifest.workload.kind != WorkloadKind::Representative || manifest.benchmarks.empty()))
    {
        return ManifestIssue::MissingRepresentativeEvidence;
    }

    for (const BenchmarkRecord& record : manifest.benchmarks)
    {
        if (!BenchmarkValid(record))
            return ManifestIssue::InvalidBenchmark;
        if (manifest.proof.representative_benchmark && !record.correctness_passed)
            return ManifestIssue::InvalidBenchmark;
    }

    return ManifestIssue::None;
}

bool WriteCertificationManifestJson(std::ostream& out,
                                    const CertificationManifestView& manifest)
{
    if (ValidateManifest(manifest) != ManifestIssue::None)
        return false;

    out << "{\n  \"schema_version\": " << manifest.schema_version << ",\n  \"source_revision\": ";
    WriteJsonString(out, manifest.source_revision);
    out << ",\n  \"generated_utc\": ";
    WriteJsonString(out, manifest.generated_utc);
    out << ",\n  \"hardware\": {\n    \"id\": ";
    WriteJsonString(out, manifest.hardware.id);
    out << ",\n    \"cpu\": ";
    WriteJsonString(out, manifest.hardware.cpu);
    out << ",\n    \"os\": ";
    WriteJsonString(out, manifest.hardware.os);
    out << ",\n    \"compiler\": ";
    WriteJsonString(out, manifest.hardware.compiler);
    out << ",\n    \"build_config\": ";
    WriteJsonString(out, manifest.hardware.build_config);
    out << "\n  },\n  \"workload\": {\n    \"id\": ";
    WriteJsonString(out, manifest.workload.id);
    out << ",\n    \"family\": ";
    WriteJsonString(out, manifest.workload.family);
    out << ",\n    \"version\": ";
    WriteJsonString(out, manifest.workload.version);
    out << ",\n    \"kind\": ";
    WriteJsonString(out, manifest.workload.kind == WorkloadKind::Representative ? "representative"
                                                                                : "synthetic");
    out << ",\n    \"sc2_build\": ";
    WriteJsonString(out, manifest.workload.sc2_build);
    out << ",\n    \"data_version\": ";
    WriteJsonString(out, manifest.workload.data_version);
    out << ",\n    \"corpus_identity\": ";
    WriteJsonString(out, manifest.workload.corpus_identity);
    out << "\n  },\n  \"proof\": {\n    \"source_design\": ";
    WriteBool(out, manifest.proof.source_design);
    out << ",\n    \"build_tests\": ";
    WriteBool(out, manifest.proof.build_tests);
    out << ",\n    \"representative_benchmark\": ";
    WriteBool(out, manifest.proof.representative_benchmark);
    out << ",\n    \"consumer_integration\": ";
    WriteBool(out, manifest.proof.consumer_integration);
    out << ",\n    \"ladder_runtime\": ";
    WriteBool(out, manifest.proof.ladder_runtime);
    out << "\n  },\n  \"benchmarks\": [";

    for (std::size_t i = 0; i < manifest.benchmarks.size(); ++i)
    {
        const BenchmarkRecord& record = manifest.benchmarks[i];
        out << (i == 0 ? "\n" : ",\n") << "    {\"candidate\": " << record.candidate
            << ", \"workload_size\": " << record.workload_size
            << ", \"samples\": " << record.samples
            << ", \"mean_total_cost_ns\": " << std::setprecision(17)
            << record.mean_total_cost_ns << ", \"p50_ns\": " << record.p50_ns
            << ", \"p95_ns\": " << record.p95_ns << ", \"p99_ns\": " << record.p99_ns
            << ", \"correctness_passed\": ";
        WriteBool(out, record.correctness_passed);
        out << '}';
    }

    if (!manifest.benchmarks.empty())
        out << '\n';
    out << "  ]\n}\n";
    return static_cast<bool>(out);
}

}  // namespace sc2opt::proof

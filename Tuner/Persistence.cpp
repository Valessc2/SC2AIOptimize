#include "sc2opt/tuner/Persistence.hpp"

#include <cmath>
#include <iomanip>
#include <limits>
#include <locale>
#include <sstream>
#include <string>

namespace sc2opt::tuner {
namespace {

bool ProvenanceValid(const EvidenceProvenance& provenance) noexcept
{
    return !provenance.source_revision.empty() && !provenance.sc2_build.empty() &&
           !provenance.data_version.empty() && !provenance.compiler_profile.empty() &&
           !provenance.hardware_profile.empty() && !provenance.workload_profile.empty();
}

}  // namespace

bool IsValid(const ChampionRecord& record) noexcept
{
    return record.schema_version == kChampionRecordSchemaVersion &&
           record.decision_id != kInvalidDecisionId &&
           record.samples > 0 &&
           std::isfinite(record.mean_total_cost_ns) &&
           record.mean_total_cost_ns >= 0.0 &&
           std::isfinite(record.sample_variance_ns2) &&
           record.sample_variance_ns2 >= 0.0 &&
           ProvenanceValid(record.provenance);
}

bool WriteChampionRecord(std::ostream& out, const ChampionRecord& record)
{
    if (!IsValid(record))
        return false;

    std::ostringstream line;
    line.imbue(std::locale::classic());
    line << "SC2OPT_CHAMPION "
         << record.schema_version << ' '
         << record.decision_id << ' '
         << record.context.workload << ' '
         << record.context.hardware << ' '
         << record.context.state_flags << ' '
         << record.champion << ' '
         << record.samples << ' '
         << std::setprecision(std::numeric_limits<double>::max_digits10)
         << record.mean_total_cost_ns << ' '
         << record.sample_variance_ns2 << ' '
         << std::quoted(record.provenance.source_revision) << ' '
         << std::quoted(record.provenance.sc2_build) << ' '
         << std::quoted(record.provenance.data_version) << ' '
         << std::quoted(record.provenance.compiler_profile) << ' '
         << std::quoted(record.provenance.hardware_profile) << ' '
         << std::quoted(record.provenance.workload_profile)
         << '\n';

    if (!line)
        return false;

    out << line.str();
    return static_cast<bool>(out);
}

bool ReadChampionRecord(std::istream& in, ChampionRecord& record)
{
    std::string line;
    if (!std::getline(in, line))
        return false;

    std::istringstream parser(line);
    parser.imbue(std::locale::classic());

    std::string magic;
    ChampionRecord parsed{};
    if (!(parser >> magic) || magic != "SC2OPT_CHAMPION")
        return false;

    if (!(parser >> parsed.schema_version
                 >> parsed.decision_id
                 >> parsed.context.workload
                 >> parsed.context.hardware
                 >> parsed.context.state_flags
                 >> parsed.champion
                 >> parsed.samples
                 >> parsed.mean_total_cost_ns
                 >> parsed.sample_variance_ns2
                 >> std::quoted(parsed.provenance.source_revision)
                 >> std::quoted(parsed.provenance.sc2_build)
                 >> std::quoted(parsed.provenance.data_version)
                 >> std::quoted(parsed.provenance.compiler_profile)
                 >> std::quoted(parsed.provenance.hardware_profile)
                 >> std::quoted(parsed.provenance.workload_profile)))
    {
        return false;
    }

    parser >> std::ws;
    if (!parser.eof() || !IsValid(parsed))
        return false;

    record = std::move(parsed);
    return true;
}

}  // namespace sc2opt::tuner

#include "sc2opt/proof/Profiles.hpp"

namespace sc2opt::proof {

bool IsValid(const HardwareProfileView& profile) noexcept
{
    return !profile.id.empty() && !profile.cpu.empty() && !profile.os.empty() &&
           !profile.compiler.empty() && !profile.build_config.empty();
}

bool IsValid(const WorkloadProfileView& profile) noexcept
{
    return !profile.id.empty() && !profile.family.empty() && !profile.version.empty() &&
           !profile.sc2_build.empty() && !profile.data_version.empty() &&
           !profile.corpus_identity.empty();
}

}  // namespace sc2opt::proof

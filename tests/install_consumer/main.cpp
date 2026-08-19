#include "sc2opt/Version.hpp"
#include "sc2opt/adapters/IntegrationContract.hpp"
#include "sc2opt/kernel/hot/Geometry.hpp"
#include "sc2opt/tuner/NetBenefit.hpp"

#include <array>

int main()
{
    static_assert(sc2opt::kApiContractMajor == 1);
    static_assert(sc2opt::kApiContractMinor >= 1);

    const auto integration =
        sc2opt::adapters::CheckIntegrationCompatibility(sc2opt::adapters::IntegrationRequirements{});
    if (!integration.ready())
        return 1;

    using namespace sc2opt::kernel::hot;
    if (DistanceSquared({0.0f, 0.0f}, {3.0f, 4.0f}) != 25.0f)
        return 2;

    const std::array evidence{
        sc2opt::tuner::CandidateEvidence{0, 20, 100'000.0, true},
        sc2opt::tuner::CandidateEvidence{1, 20, 80'000.0, true},
    };
    return sc2opt::tuner::ChooseNetBenefitChampion(evidence).champion == 1 ? 0 : 3;
}

#include "sc2opt/adapters/cpp/WorldView.hpp"
#include "sc2opt/kernel/hot/Geometry.hpp"
#include "sc2opt/registry/SC2Registry.hpp"
#include "sc2opt/tuner/NetBenefit.hpp"
#include "sc2opt/tuner/TunableRegistry.hpp"

#include <array>
#include <cmath>
#include <iostream>
#include <string_view>
#include <unordered_set>

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

void TestRegistry()
{
    const auto registry = sc2opt::registry::Base75689();
    Check(registry.identity.sc2_build == "75689", "registry build identity");
    Check(registry.identity.data_version == "B89B5D6FA7CBF6452E721311BFBC6CB2",
          "registry data version");
    Check(registry.units.size() == sc2opt::registry::kBase75689ExpectedUnitCount,
          "registry expected unit count");

    const auto* marine = sc2opt::registry::FindByCatalogName(registry, "Marine");
    Check(marine != nullptr, "Marine exists");
    if (marine != nullptr)
    {
        Check(marine->race == sc2opt::registry::Race::Terran, "Marine race");
        Check(marine->minerals == 50, "Marine mineral cost");
        Check(std::fabs(marine->radius - 0.375f) < 0.0001f, "Marine radius");
    }

    Check(sc2opt::registry::FindByCatalogName(registry, "DefinitelyNotAUnit") == nullptr,
          "unknown unit fails closed");

    std::unordered_set<std::string_view> names;
    for (const auto& unit : registry.units)
    {
        Check(unit.catalog_name != nullptr && unit.catalog_name[0] != '\0',
              "registry names are non-empty");
        if (unit.catalog_name != nullptr)
            Check(names.insert(unit.catalog_name).second, "registry names are unique");
    }
}

void TestHotGeometry()
{
    using sc2opt::kernel::hot::DistanceSquared;
    using sc2opt::kernel::hot::Vec2;
    using sc2opt::kernel::hot::WithinRadius;

    Check(DistanceSquared(Vec2{0.0f, 0.0f}, Vec2{3.0f, 4.0f}) == 25.0f,
          "squared distance");
    Check(WithinRadius(Vec2{0.0f, 0.0f}, Vec2{3.0f, 4.0f}, 5.0f),
          "inclusive radius boundary");
    Check(!WithinRadius(Vec2{0.0f, 0.0f}, Vec2{3.0f, 4.0f}, -1.0f),
          "negative radius fails closed");
}

void TestNetBenefitGate()
{
    using namespace sc2opt::tuner;

    const std::array slower{{CandidateEvidence{0, 10, 100.0, true},
                             CandidateEvidence{1, 10, 120.0, true}}};
    Check(ChooseNetBenefitChampion(slower).champion == kBaselineCandidate,
          "slower optimisation is rejected");

    const std::array faster{{CandidateEvidence{0, 10, 100.0, true},
                             CandidateEvidence{1, 10, 70.0, true}}};
    Check(ChooseNetBenefitChampion(faster).champion == 1,
          "faster optimisation becomes champion");

    const std::array invalid{{CandidateEvidence{0, 10, 100.0, true},
                              CandidateEvidence{1, 10, 50.0, false}}};
    Check(ChooseNetBenefitChampion(invalid).champion == kBaselineCandidate,
          "incorrect optimisation is rejected regardless of speed");

    const std::array noisy{{CandidateEvidence{0, 10, 100.0, true},
                            CandidateEvidence{1, 2, 50.0, true}}};
    Check(ChooseNetBenefitChampion(noisy, NetBenefitPolicy{5, 0.0, 0.0}).champion ==
              kBaselineCandidate,
          "insufficient evidence cannot displace baseline");

    const std::array marginal{{CandidateEvidence{0, 10, 100.0, true},
                               CandidateEvidence{1, 10, 85.0, true}}};
    Check(ChooseNetBenefitChampion(marginal, NetBenefitPolicy{5, 0.0, 0.20}).champion ==
              kBaselineCandidate,
          "candidate below configured relative gain is rejected");
}

void TestTunableContract()
{
    using namespace sc2opt::tuner;
    Check(IsValid(TunableSpec{1, "grid.cell_size", TunableKind::Continuous, 4.0, 1.0, 16.0, 0.5,
                              TunableOnlineSafe}),
          "valid tunable specification");
    Check(!IsValid(TunableSpec{2, "bad", TunableKind::Continuous, 20.0, 1.0, 16.0, 0.5, 0}),
          "baseline outside range is invalid");
}

}  // namespace

int main()
{
    TestRegistry();
    TestHotGeometry();
    TestNetBenefitGate();
    TestTunableContract();

    if (failures == 0)
    {
        std::cout << "SC2AIOptimize core tests passed\n";
        return 0;
    }

    std::cerr << failures << " SC2AIOptimize core test(s) failed\n";
    return 1;
}

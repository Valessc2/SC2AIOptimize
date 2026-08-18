#include "sc2opt/kernel/geometry.hpp"
#include "sc2opt/kernel/spatial_index.hpp"
#include "sc2opt/registry/registry.hpp"
#include "sc2opt/tuner/net_benefit.hpp"
#include "sc2opt/tuner/tunable_registry.hpp"

#include <array>
#include <cassert>
#include <vector>

int main()
{
    using namespace sc2opt;

    static_assert(kernel::distance_squared({0.0f, 0.0f}, {3.0f, 4.0f}) == 25.0f);
    static_assert(kernel::within_radius({0.0f, 0.0f}, {3.0f, 4.0f}, 5.0f));

    constexpr std::array<registry::UnitStatic, 1> units{{
        {.catalog_name = "Example", .race = registry::Race::Terran, .movement = registry::Movement::Ground}
    }};
    constexpr registry::RegistryMetadata metadata{"75689", "example", 1};
    const registry::RegistryView registry_view(metadata, units);
    assert(registry_view.find_by_catalog_name("Example") == 0u);
    assert(registry_view.find_by_catalog_name("Missing") == registry::kInvalidUnitIndex);
    assert(registry_view.unit(0u) != nullptr);

    std::array<kernel::EntityView, 3> entities{{
        {1u, {0.0f, 0.0f}, 0.5f},
        {2u, {3.0f, 4.0f}, 0.5f},
        {3u, {20.0f, 20.0f}, 0.5f},
    }};
    kernel::SpatialIndex index(4.0f);
    index.rebuild(entities);
    std::vector<std::uint32_t> hits;
    index.query_radius({0.0f, 0.0f}, 5.0f, hits);
    assert(hits.size() == 2u);

    tuner::NetBenefitSample win{1000u, 700u, 50u, true, true};
    assert(tuner::decide_net_benefit(win).choice == tuner::CandidateChoice::Optimized);

    tuner::NetBenefitSample loss{1000u, 980u, 30u, true, true};
    assert(tuner::decide_net_benefit(loss).choice == tuner::CandidateChoice::Baseline);

    tuner::NetBenefitSample unproven{1000u, 500u, 0u, false, true};
    assert(tuner::decide_net_benefit(unproven).choice == tuner::CandidateChoice::Baseline);

    constexpr std::array<tuner::TunableSpec, 1> tunables{{
        {"kernel.spatial.cell_size", 4.0, 2.0, 12.0, 0.5, true}
    }};
    const tuner::TunableRegistryView tunable_view(tunables);
    assert(tunable_view.find("kernel.spatial.cell_size") != nullptr);

    return 0;
}

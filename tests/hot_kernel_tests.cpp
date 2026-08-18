#include "sc2opt/kernel/hot/CombatGeometry.hpp"
#include "sc2opt/kernel/hot/Filtering.hpp"
#include "sc2opt/kernel/hot/SpatialGrid.hpp"
#include "sc2opt/kernel/hot/SpatialStrategy.hpp"
#include "sc2opt/model/WorldView.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>

namespace {

int failures = 0;

void Check(bool condition, const char* message)
{
    if (!condition)
    {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}

void TestBoundarySemantics()
{
    using namespace sc2opt::kernel::hot;

    Check(WithinRadius({0.0f, 0.0f}, {3.0f, 4.0f}, 5.0f, BoundaryMode::Inclusive),
          "inclusive radius accepts exact boundary");
    Check(!WithinRadius({0.0f, 0.0f}, {3.0f, 4.0f}, 5.0f, BoundaryMode::Strict),
          "strict radius rejects exact boundary");

    const Circle2 source{{0.0f, 0.0f}, 1.0f};
    const Circle2 target{{5.0f, 0.0f}, 1.0f};
    Check(WithinEdgeRange(source, target, 3.0f, BoundaryMode::Inclusive),
          "edge range includes exact boundary");
    Check(!WithinEdgeRange(source, target, 3.0f, BoundaryMode::Strict),
          "edge range strict boundary");

    Check(WithinFacingCone({0.0f, 0.0f}, {1.0f, 0.0f}, {4.0f, 0.0f}, 1.0f,
                           BoundaryMode::Inclusive),
          "facing inclusive exact dot");
    Check(!WithinFacingCone({0.0f, 0.0f}, {1.0f, 0.0f}, {4.0f, 0.0f}, 1.0f,
                            BoundaryMode::Strict),
          "facing strict exact dot");
    Check(WithinFacingCone({2.0f, 2.0f}, {1.0f, 0.0f}, {2.0f, 2.0f}, 0.9f),
          "coincident target requires no turn");
}

void TestFiltering()
{
    using namespace sc2opt;
    const std::array<model::UnitView, 4> units{{
        {1, 7, model::UnitAlive | model::UnitVisible, 0, 0, 45, 0, 0, 0.375f},
        {2, 7, model::UnitAlive | model::UnitFlying, 0, 0, 125, 0, 0, 0.75f},
        {3, 9, model::UnitAlive | model::UnitVisible, 0, 0, 10, 0, 0, 0.5f},
        {4, 7, model::UnitVisible, 0, 0, 200, 0, 0, 1.0f},
    }};

    std::array<std::uint32_t, 4> output{};
    const kernel::hot::UnitFilter filter{model::UnitAlive, model::UnitFlying, 7, 20.0f};
    const auto result = kernel::hot::FilterUnitIndices(units, filter, output);
    Check(result && result.written == 1 && output[0] == 0, "unit filter exact result");

    std::array<std::uint32_t, 0> tiny{};
    const auto too_small = kernel::hot::FilterUnitIndices(units, filter, tiny);
    Check(too_small.status == kernel::hot::FilterStatus::OutputTooSmall &&
              too_small.required == 1 && too_small.written == 0,
          "unit filter reports required capacity");
}

void TestSpatialEquivalence()
{
    using namespace sc2opt::kernel::hot;

    const std::array<float, 10> xy{
        0.0f, 0.0f,
        3.0f, 4.0f,
        5.0f, 0.0f,
        6.0f, 0.0f,
        -5.0f, 0.0f,
    };

    SpatialGrid2D grid;
    Check(grid.SetCellSize(4.0f) == SpatialStatus::Ok, "valid cell size");
    Check(grid.Rebuild(xy) == SpatialStatus::Ok && grid.Ready(), "spatial rebuild");

    std::array<std::uint32_t, 5> baseline{};
    std::array<std::uint32_t, 5> stable{};
    std::array<std::uint32_t, 5> unordered{};

    const auto baseline_inclusive =
        RadiusQueryBruteForce(xy, {0.0f, 0.0f}, 5.0f, baseline, BoundaryMode::Inclusive);
    const auto grid_inclusive = grid.Query({0.0f, 0.0f}, 5.0f, stable,
                                           SpatialOrder::InputOrder,
                                           BoundaryMode::Inclusive);
    Check(baseline_inclusive && grid_inclusive &&
              baseline_inclusive.written == grid_inclusive.written,
          "grid inclusive count matches baseline");
    Check(std::equal(baseline.begin(), baseline.begin() + baseline_inclusive.written,
                     stable.begin()),
          "grid stable order exactly matches baseline");

    const auto grid_unordered = grid.Query({0.0f, 0.0f}, 5.0f, unordered,
                                            SpatialOrder::Unordered,
                                            BoundaryMode::Inclusive);
    Check(grid_unordered && grid_unordered.written == baseline_inclusive.written,
          "unordered grid count matches baseline");

    std::array<bool, 5> baseline_members{};
    std::array<bool, 5> unordered_members{};
    bool membership_indices_valid = true;
    for (std::size_t i = 0; i < baseline_inclusive.written; ++i)
    {
        if (baseline[i] >= baseline_members.size())
        {
            membership_indices_valid = false;
            break;
        }
        baseline_members[baseline[i]] = true;
    }
    for (std::size_t i = 0; i < grid_unordered.written; ++i)
    {
        if (unordered[i] >= unordered_members.size())
        {
            membership_indices_valid = false;
            break;
        }
        unordered_members[unordered[i]] = true;
    }
    Check(membership_indices_valid && baseline_members == unordered_members,
          "unordered grid membership matches baseline");

    const auto baseline_strict =
        RadiusQueryBruteForce(xy, {0.0f, 0.0f}, 5.0f, baseline, BoundaryMode::Strict);
    const auto grid_strict = grid.Query({0.0f, 0.0f}, 5.0f, stable,
                                        SpatialOrder::InputOrder,
                                        BoundaryMode::Strict);
    Check(baseline_strict && grid_strict && baseline_strict.written == 1 &&
              grid_strict.written == 1 && baseline[0] == 0 && stable[0] == 0,
          "strict boundary equivalence");

    std::array<std::uint32_t, 1> tiny{};
    const auto overflow = grid.Query({0.0f, 0.0f}, 5.0f, tiny);
    Check(overflow.status == SpatialStatus::OutputTooSmall && overflow.written == 0 &&
              overflow.required == baseline_inclusive.written,
          "spatial query reports required capacity");

    const std::array<float, 4> moved{100.0f, 100.0f, 104.0f, 100.0f};
    Check(grid.Rebuild(moved) == SpatialStatus::Ok && grid.PointCount() == 2,
          "spatial storage reusable across rebuilds");
    const auto old_area = grid.Query({0.0f, 0.0f}, 5.0f, stable);
    Check(old_area && old_area.written == 0, "rebuild clears prior active cells");

    Check(grid.SetCellSize(0.0f) == SpatialStatus::InvalidCellSize && !grid.Ready(),
          "invalid cell size fails closed");
}

void TestSpatialStrategy()
{
    using namespace sc2opt::kernel::hot;

    const SpatialCrossoverProfile uncertified{};
    Check(ChooseSpatialQueryStrategy(1000, true, true, uncertified) ==
              SpatialQueryStrategy::BruteForce,
          "uncertified stable grid remains off");
    Check(ChooseSpatialQueryStrategy(1000, false, true, uncertified) ==
              SpatialQueryStrategy::BruteForce,
          "uncertified unordered grid remains off");

    const SpatialCrossoverProfile certified{64, 32};
    Check(ChooseSpatialQueryStrategy(63, true, true, certified) ==
              SpatialQueryStrategy::BruteForce,
          "stable crossover respects threshold");
    Check(ChooseSpatialQueryStrategy(64, true, true, certified) ==
              SpatialQueryStrategy::GridInputOrder,
          "stable grid activates at certified threshold");
    Check(ChooseSpatialQueryStrategy(32, false, true, certified) ==
              SpatialQueryStrategy::GridUnordered,
          "unordered grid activates at certified threshold");
    Check(ChooseSpatialQueryStrategy(1000, false, false, certified) ==
              SpatialQueryStrategy::BruteForce,
          "unready grid always falls back to baseline");
}

}  // namespace

int main()
{
    TestBoundarySemantics();
    TestFiltering();
    TestSpatialEquivalence();
    TestSpatialStrategy();

    if (failures == 0)
    {
        std::cout << "SC2AIOptimize Hot kernel tests passed\n";
        return 0;
    }

    std::cerr << failures << " Hot kernel test(s) failed\n";
    return 1;
}

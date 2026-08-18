#include "sc2opt/kernel/compute/CombatEvaluation.hpp"
#include "sc2opt/kernel/compute/Influence.hpp"
#include "sc2opt/kernel/compute/Pathing.hpp"
#include "sc2opt/kernel/compute/Placement.hpp"
#include "sc2opt/kernel/compute/TargetSelection.hpp"

#include <array>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>

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

void TestTargetSelection()
{
    using namespace sc2opt::kernel::compute;

    const std::array<float, 5> scores{3.0f, 8.0f, 8.0f,
                                      std::numeric_limits<float>::quiet_NaN(), 2.0f};
    const auto first = SelectBestScore(scores);
    Check(first && first.index == 1 && first.score == 8.0f, "target first-tie selection");

    const auto last = SelectBestScore(scores, {}, {ScoreGoal::Maximize, TieBreak::Last});
    Check(last && last.index == 2, "target last-tie selection");

    const std::array<std::uint8_t, 5> eligible{1, 0, 0, 1, 1};
    const auto masked = SelectBestScore(scores, eligible);
    Check(masked && masked.index == 0, "target eligibility mask");

    const auto minimum = SelectBestScore(scores, {}, {ScoreGoal::Minimize, TieBreak::First});
    Check(minimum && minimum.index == 4 && minimum.score == 2.0f, "target minimize selection");

    const std::array<std::uint8_t, 1> bad_mask{1};
    Check(SelectBestScore(scores, bad_mask).status == SelectionStatus::LengthMismatch,
          "target mask length fails closed");
}

void TestInfluenceEquivalence()
{
    using namespace sc2opt;
    using namespace sc2opt::kernel::compute;

    std::array<float, 56> reference{};
    std::array<float, 56> bounded{};
    model::FloatGridView reference_grid{reference, 7, 7, 8};
    model::FloatGridView bounded_grid{bounded, 7, 7, 8};

    const RadialInfluenceSpec spec{{3.0f, 3.0f}, 1.0f, 3.0f, 10.0f, 2.0f,
                                   kernel::hot::BoundaryMode::Inclusive};
    const auto reference_result = ApplyRadialInfluenceReference(reference_grid, spec);
    const auto bounded_result = ApplyRadialInfluenceBounded(bounded_grid, spec);
    Check(reference_result && bounded_result && reference_result.touched == bounded_result.touched,
          "influence candidate touched count matches reference");

    bool equal = true;
    for (std::size_t i = 0; i < reference.size(); ++i)
    {
        if (std::fabs(reference[i] - bounded[i]) > 0.000001f)
        {
            equal = false;
            break;
        }
    }
    Check(equal, "influence bounded candidate matches reference values");

    Check(reference[7] == 0.0f && bounded[7] == 0.0f,
          "influence respects row stride padding");

    const RadialInfluenceSpec invalid{{3.0f, 3.0f}, 4.0f, 3.0f, 1.0f, 0.0f};
    Check(ApplyRadialInfluenceBounded(bounded_grid, invalid).status == InfluenceStatus::InvalidSpec,
          "invalid influence radii fail closed");
}

void TestPathing()
{
    using namespace sc2opt;
    using namespace sc2opt::kernel::compute;

    const float blocked = std::numeric_limits<float>::infinity();
    const std::array<float, 25> costs{
        1, 1, blocked, 1, 1,
        1, 1, 1,       1, 1,
        1, 1, 1,       1, 1,
        1, 1, 1,       1, 1,
        1, 1, 1,       1, 1,
    };
    const model::ConstFloatGridView grid{costs, 5, 5, 5};

    std::array<model::GridPoint, 25> dijkstra_path{};
    std::array<model::GridPoint, 25> astar_path{};
    AStarWorkspace dijkstra;
    AStarWorkspace astar;

    const auto reference = dijkstra.FindPath(grid, {0, 0}, {4, 0}, dijkstra_path,
                                              PathHeuristic::None);
    const auto candidate = astar.FindPath(grid, {0, 0}, {4, 0}, astar_path,
                                          PathHeuristic::Manhattan);
    Check(reference && candidate, "Dijkstra and A* both find path");
    Check(reference.total_cost == 6.0f && candidate.total_cost == reference.total_cost,
          "A* cost matches Dijkstra baseline");
    Check(reference.written == 7 && candidate.written == 7,
          "path length matches expected detour");
    Check(astar_path[0] == model::GridPoint{0, 0} &&
              astar_path[candidate.written - 1] == model::GridPoint{4, 0},
          "path endpoints preserved");
    Check(candidate.expanded <= reference.expanded,
          "Manhattan A* does not expand more than Dijkstra on deterministic fixture");

    std::array<model::GridPoint, 2> tiny{};
    const auto too_small = astar.FindPath(grid, {0, 0}, {4, 0}, tiny, PathHeuristic::Manhattan);
    Check(too_small.status == PathStatus::OutputTooSmall && too_small.written == 0 &&
              too_small.required == 7,
          "path output capacity fails closed with required size");

    const std::size_t warmed_capacity = astar.Capacity();
    const auto repeated = astar.FindPath(grid, {0, 0}, {4, 0}, astar_path,
                                         PathHeuristic::Manhattan);
    Check(repeated && astar.Capacity() == warmed_capacity,
          "path workspace retains allocation capacity across equal-size searches");

    std::array<float, 4> invalid_costs{1.0f, 0.5f, 1.0f, 1.0f};
    Check(astar.FindPath({invalid_costs, 2, 2, 2}, {0, 0}, {1, 1}, astar_path).status ==
              PathStatus::InvalidGrid,
          "path costs below contract fail closed");
}

void TestPlacement()
{
    using namespace sc2opt;
    using namespace sc2opt::kernel::compute;

    std::array<std::uint8_t, 25> buildable{};
    buildable.fill(1);
    buildable[2 * 5 + 2] = 0;
    const model::ByteGridView grid{buildable, 5, 5, 5};

    const std::array<std::uint8_t, 4> mask{1, 1, 1, 1};
    const FootprintMaskView footprint{mask, 2, 2, 2};
    Check(CanPlaceMask(grid, footprint, {0, 0}), "clear 2x2 placement accepted");
    Check(!CanPlaceMask(grid, footprint, {1, 1}), "blocked footprint rejected");
    Check(!CanPlaceMask(grid, footprint, {4, 4}), "out-of-bounds footprint rejected");

    const std::array<model::GridPoint, 4> candidates{{{0, 0}, {1, 1}, {3, 3}, {4, 4}}};
    std::array<std::uint32_t, 4> output{};
    const auto result = FilterValidPlacements(grid, footprint, candidates, output);
    Check(result && result.written == 2 && output[0] == 0 && output[1] == 2,
          "placement filtering preserves candidate order");
}

void TestCombatEvaluation()
{
    using namespace sc2opt::kernel::compute;

    const std::array<Combatant, 2> side_a{{{100.0f, 10.0f}, {100.0f, 10.0f}}};
    const std::array<Combatant, 2> side_b{{{50.0f, 12.5f}, {50.0f, 12.5f}}};

    const auto a = AggregateCombatants(side_a);
    Check(a && a.effective_health == 200.0 && a.effective_dps == 20.0 && a.active_units == 2,
          "combat aggregate");

    const auto exchange = EstimateContinuousExchange(side_a, side_b);
    Check(exchange && exchange.outcome == ExchangeOutcome::SideAPrevails,
          "continuous exchange outcome");
    Check(exchange.seconds_to_clear_b == 5.0 && exchange.seconds_to_clear_a == 8.0,
          "continuous exchange times");

    const std::array<Combatant, 1> harmless{{{100.0f, 0.0f}}};
    const auto stalemate = EstimateContinuousExchange(harmless, harmless);
    Check(stalemate && stalemate.outcome == ExchangeOutcome::Stalemate,
          "zero-dps exchange is stalemate");

    const std::array<Combatant, 1> invalid{{{-1.0f, 100.0f}}};
    Check(AggregateCombatants(invalid).status == CombatStatus::InvalidInput,
          "negative effective combat input fails closed");
}

}  // namespace

int main()
{
    TestTargetSelection();
    TestInfluenceEquivalence();
    TestPathing();
    TestPlacement();
    TestCombatEvaluation();

    if (failures == 0)
    {
        std::cout << "SC2AIOptimize Compute tests passed\n";
        return 0;
    }

    std::cerr << failures << " Compute test(s) failed\n";
    return 1;
}

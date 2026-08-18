#include "sc2opt/kernel/compute/Pathing.hpp"

#include <algorithm>
#include <array>
#include <cmath>
#include <limits>

namespace sc2opt::kernel::compute {
namespace {

[[nodiscard]] bool PositiveInfinity(float value) noexcept
{
    return std::isinf(value) && value > 0.0f;
}

[[nodiscard]] bool ValidTraversalGrid(model::ConstFloatGridView grid) noexcept
{
    if (!model::IsValid(grid))
        return false;

    const std::size_t cell_count = static_cast<std::size_t>(grid.width) * grid.height;
    if (cell_count > std::numeric_limits<std::uint32_t>::max())
        return false;

    for (std::uint32_t y = 0; y < grid.height; ++y)
    {
        const std::size_t row = static_cast<std::size_t>(y) * grid.stride;
        for (std::uint32_t x = 0; x < grid.width; ++x)
        {
            const float cost = grid.values[row + x];
            if (PositiveInfinity(cost))
                continue;
            if (!std::isfinite(cost) || cost < 1.0f)
                return false;
        }
    }
    return true;
}

[[nodiscard]] std::uint32_t Index(model::ConstFloatGridView grid,
                                  model::GridPoint point) noexcept
{
    return point.y * grid.width + point.x;
}

[[nodiscard]] model::GridPoint Point(model::ConstFloatGridView grid,
                                     std::uint32_t index) noexcept
{
    return {index % grid.width, index / grid.width};
}

[[nodiscard]] float CostAt(model::ConstFloatGridView grid, model::GridPoint point) noexcept
{
    return grid.values[static_cast<std::size_t>(point.y) * grid.stride + point.x];
}

[[nodiscard]] float Heuristic(model::GridPoint a,
                              model::GridPoint b,
                              PathHeuristic heuristic) noexcept
{
    if (heuristic == PathHeuristic::None)
        return 0.0f;

    const std::uint32_t dx = a.x > b.x ? a.x - b.x : b.x - a.x;
    const std::uint32_t dy = a.y > b.y ? a.y - b.y : b.y - a.y;
    return static_cast<float>(static_cast<std::uint64_t>(dx) + dy);
}

}  // namespace

PathResult AStarWorkspace::FindPath(model::ConstFloatGridView costs,
                                    model::GridPoint start,
                                    model::GridPoint goal,
                                    std::span<model::GridPoint> output,
                                    PathHeuristic heuristic)
{
    if (!ValidTraversalGrid(costs))
        return {PathStatus::InvalidGrid, 0, 0, std::numeric_limits<float>::infinity(), 0};

    if (start.x >= costs.width || start.y >= costs.height ||
        goal.x >= costs.width || goal.y >= costs.height ||
        PositiveInfinity(CostAt(costs, start)) || PositiveInfinity(CostAt(costs, goal)))
    {
        return {PathStatus::InvalidStartOrGoal, 0, 0,
                std::numeric_limits<float>::infinity(), 0};
    }

    const std::size_t cell_count = static_cast<std::size_t>(costs.width) * costs.height;
    const float infinity = std::numeric_limits<float>::infinity();
    const std::uint32_t no_parent = std::numeric_limits<std::uint32_t>::max();

    g_score_.assign(cell_count, infinity);
    parent_.assign(cell_count, no_parent);
    closed_.assign(cell_count, 0);
    open_.clear();
    if (open_.capacity() < cell_count)
        open_.reserve(cell_count);

    const std::uint32_t start_index = Index(costs, start);
    const std::uint32_t goal_index = Index(costs, goal);
    g_score_[start_index] = 0.0f;
    parent_[start_index] = start_index;
    open_.push_back({Heuristic(start, goal, heuristic), 0.0f, start_index});

    const auto worse = [](const OpenNode& left, const OpenNode& right) noexcept {
        if (left.f != right.f)
            return left.f > right.f;
        if (left.g != right.g)
            return left.g > right.g;
        return left.index > right.index;
    };
    std::make_heap(open_.begin(), open_.end(), worse);

    std::size_t expanded = 0;
    bool found = false;

    while (!open_.empty())
    {
        std::pop_heap(open_.begin(), open_.end(), worse);
        const OpenNode current = open_.back();
        open_.pop_back();

        if (closed_[current.index] != 0 || current.g != g_score_[current.index])
            continue;

        closed_[current.index] = 1;
        ++expanded;
        if (current.index == goal_index)
        {
            found = true;
            break;
        }

        const model::GridPoint point = Point(costs, current.index);
        const std::array<model::GridPoint, 4> neighbours{{
            {point.x > 0 ? point.x - 1 : point.x, point.y},
            {point.x + 1 < costs.width ? point.x + 1 : point.x, point.y},
            {point.x, point.y > 0 ? point.y - 1 : point.y},
            {point.x, point.y + 1 < costs.height ? point.y + 1 : point.y},
        }};

        for (const model::GridPoint neighbour : neighbours)
        {
            if (neighbour == point)
                continue;

            const float step_cost = CostAt(costs, neighbour);
            if (PositiveInfinity(step_cost))
                continue;

            const std::uint32_t neighbour_index = Index(costs, neighbour);
            if (closed_[neighbour_index] != 0)
                continue;

            const float tentative = current.g + step_cost;
            if (!std::isfinite(tentative) || tentative >= g_score_[neighbour_index])
                continue;

            g_score_[neighbour_index] = tentative;
            parent_[neighbour_index] = current.index;
            open_.push_back({tentative + Heuristic(neighbour, goal, heuristic),
                             tentative,
                             neighbour_index});
            std::push_heap(open_.begin(), open_.end(), worse);
        }
    }

    if (!found)
        return {PathStatus::NoPath, 0, 0, infinity, expanded};

    std::size_t required = 1;
    for (std::uint32_t index = goal_index; index != start_index;)
    {
        index = parent_[index];
        if (index == no_parent)
            return {PathStatus::NoPath, 0, 0, infinity, expanded};
        ++required;
    }

    if (output.size() < required)
        return {PathStatus::OutputTooSmall, 0, required, g_score_[goal_index], expanded};

    std::uint32_t index = goal_index;
    for (std::size_t position = required; position-- > 0;)
    {
        output[position] = Point(costs, index);
        if (index != start_index)
            index = parent_[index];
    }

    return {PathStatus::Ok, required, required, g_score_[goal_index], expanded};
}

}  // namespace sc2opt::kernel::compute

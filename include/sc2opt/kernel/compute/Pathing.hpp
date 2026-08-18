#pragma once

#include "sc2opt/model/GridView.hpp"

#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <vector>

namespace sc2opt::kernel::compute {

enum class PathHeuristic : std::uint8_t {
    None = 0,
    Manhattan
};

enum class PathStatus : std::uint8_t {
    Ok = 0,
    InvalidGrid,
    InvalidStartOrGoal,
    NoPath,
    OutputTooSmall
};

struct PathResult {
    PathStatus status = PathStatus::NoPath;
    std::size_t written = 0;
    std::size_t required = 0;
    float total_cost = std::numeric_limits<float>::infinity();
    std::size_t expanded = 0;

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return status == PathStatus::Ok;
    }
};

// Traversal grid contract: finite cells must have cost >= 1.0; +infinity is blocked.
// Movement is 4-neighbour and pays the destination cell's traversal cost. That makes a
// Manhattan heuristic with unit scale admissible. `None` is the exact Dijkstra/reference mode.
class AStarWorkspace {
public:
    [[nodiscard]] PathResult FindPath(model::ConstFloatGridView costs,
                                      model::GridPoint start,
                                      model::GridPoint goal,
                                      std::span<model::GridPoint> output,
                                      PathHeuristic heuristic = PathHeuristic::Manhattan);

    [[nodiscard]] std::size_t Capacity() const noexcept { return g_score_.capacity(); }

private:
    struct OpenNode {
        float f = 0.0f;
        float g = 0.0f;
        std::uint32_t index = 0;
    };

    std::vector<float> g_score_;
    std::vector<std::uint32_t> parent_;
    std::vector<std::uint8_t> closed_;
    std::vector<OpenNode> open_;
};

}  // namespace sc2opt::kernel::compute

#include "sc2opt/kernel/compute/TargetSelection.hpp"

#include <cmath>

namespace sc2opt::kernel::compute {

SelectionResult SelectBestScore(std::span<const float> scores,
                                std::span<const std::uint8_t> eligible,
                                SelectionPolicy policy) noexcept
{
    if (!eligible.empty() && eligible.size() != scores.size())
        return {SelectionStatus::LengthMismatch, 0, 0.0f};

    SelectionResult best;
    for (std::size_t i = 0; i < scores.size(); ++i)
    {
        if (!eligible.empty() && eligible[i] == 0)
            continue;

        const float score = scores[i];
        if (!std::isfinite(score))
            continue;

        if (!best)
        {
            best = {SelectionStatus::Ok, i, score};
            continue;
        }

        const bool better = policy.goal == ScoreGoal::Maximize ? score > best.score
                                                                : score < best.score;
        const bool equal_and_last = score == best.score && policy.tie_break == TieBreak::Last;
        if (better || equal_and_last)
            best = {SelectionStatus::Ok, i, score};
    }

    return best;
}

}  // namespace sc2opt::kernel::compute

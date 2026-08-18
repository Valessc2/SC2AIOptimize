#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>

namespace sc2opt::kernel::compute {

enum class ScoreGoal : std::uint8_t { Maximize = 0, Minimize };
enum class TieBreak : std::uint8_t { First = 0, Last };
enum class SelectionStatus : std::uint8_t { Ok = 0, LengthMismatch, NoCandidate };

struct SelectionPolicy {
    ScoreGoal goal = ScoreGoal::Maximize;
    TieBreak tie_break = TieBreak::First;
};

struct SelectionResult {
    SelectionStatus status = SelectionStatus::NoCandidate;
    std::size_t index = std::numeric_limits<std::size_t>::max();
    float score = 0.0f;

    [[nodiscard]] constexpr explicit operator bool() const noexcept
    {
        return status == SelectionStatus::Ok;
    }
};

// Linear best-of selection: no sorting/container construction. Empty `eligible` means all
// candidates. Non-finite scores are ineligible. Tie behaviour is explicit and deterministic.
[[nodiscard]] SelectionResult SelectBestScore(
    std::span<const float> scores,
    std::span<const std::uint8_t> eligible = {},
    SelectionPolicy policy = {}) noexcept;

}  // namespace sc2opt::kernel::compute

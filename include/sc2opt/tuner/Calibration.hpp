#pragma once

#include "sc2opt/tuner/Context.hpp"
#include "sc2opt/tuner/NetBenefit.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

namespace sc2opt::tuner {

struct CalibrationPoint {
    std::size_t workload_size = 0;
    std::span<const CandidateEvidence> candidates{};
};

enum class CalibrationIssue : std::uint8_t {
    None,
    EmptyEvidence,
    UnsortedWorkload,
    DuplicateCandidate,
    MissingBaseline,
    OutputTooSmall
};

struct CalibrationResult {
    CalibrationIssue issue = CalibrationIssue::None;
    std::size_t workload_points = 0;
    std::size_t bands_written = 0;

    [[nodiscard]] constexpr bool ok() const noexcept { return issue == CalibrationIssue::None; }
};

// Control-plane calibration. Candidate selection delegates to the single S20 net-benefit
// authority; this function only validates workload grouping and compresses adjacent champions.
[[nodiscard]] CalibrationResult BuildCrossoverCalibration(
    std::span<const CalibrationPoint> points,
    std::span<CrossoverBand> output,
    NetBenefitPolicy policy = {}) noexcept;

struct ControlOverheadPolicy {
    std::uint64_t maximum_absolute_ns = 0;  // 0 disables the absolute ceiling.
    double maximum_fraction_of_work = 0.01;
};

struct ControlOverheadDecision {
    bool accepted = false;
    double fraction_of_work = 0.0;
};

// Makes the tuner prove that its own measured control cost is cheap enough.
[[nodiscard]] ControlOverheadDecision CheckControlOverhead(
    std::uint64_t control_overhead_ns,
    std::uint64_t chosen_work_ns,
    ControlOverheadPolicy policy = {}) noexcept;

}  // namespace sc2opt::tuner

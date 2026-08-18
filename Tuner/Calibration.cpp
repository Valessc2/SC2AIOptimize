#include "sc2opt/tuner/Calibration.hpp"

#include <cmath>

namespace sc2opt::tuner {
namespace {

CalibrationIssue ValidateCandidates(std::span<const CandidateEvidence> candidates) noexcept
{
    bool baseline_present = false;
    for (std::size_t i = 0; i < candidates.size(); ++i)
    {
        if (candidates[i].id == kBaselineCandidate)
        {
            if (baseline_present)
                return CalibrationIssue::DuplicateCandidate;
            baseline_present = true;
        }

        for (std::size_t j = 0; j < i; ++j)
        {
            if (candidates[i].id == candidates[j].id)
                return CalibrationIssue::DuplicateCandidate;
        }
    }

    return baseline_present ? CalibrationIssue::None : CalibrationIssue::MissingBaseline;
}

}  // namespace

CalibrationResult BuildCrossoverCalibration(std::span<const CalibrationPoint> points,
                                            std::span<CrossoverBand> output,
                                            NetBenefitPolicy policy) noexcept
{
    if (points.empty())
        return {CalibrationIssue::EmptyEvidence, 0, 0};

    CalibrationResult result{};
    CandidateId previous_champion = kBaselineCandidate;
    bool have_previous = false;

    for (std::size_t i = 0; i < points.size(); ++i)
    {
        const CalibrationPoint& point = points[i];
        if (i > 0 && point.workload_size <= points[i - 1].workload_size)
            return {CalibrationIssue::UnsortedWorkload, result.workload_points,
                    result.bands_written};
        if (point.candidates.empty())
            return {CalibrationIssue::EmptyEvidence, result.workload_points, result.bands_written};

        const CalibrationIssue validation = ValidateCandidates(point.candidates);
        if (validation != CalibrationIssue::None)
            return {validation, result.workload_points, result.bands_written};

        const NetBenefitDecision decision = ChooseNetBenefitChampion(point.candidates, policy);
        const CandidateId champion = decision.champion;

        ++result.workload_points;
        if (!have_previous || champion != previous_champion)
        {
            if (result.bands_written >= output.size())
                return {CalibrationIssue::OutputTooSmall, result.workload_points,
                        result.bands_written};

            output[result.bands_written++] = {point.workload_size, champion};
            previous_champion = champion;
            have_previous = true;
        }
        else
        {
            output[result.bands_written - 1].maximum_workload_size = point.workload_size;
        }
    }

    return result;
}

ControlOverheadDecision CheckControlOverhead(std::uint64_t control_overhead_ns,
                                             std::uint64_t chosen_work_ns,
                                             ControlOverheadPolicy policy) noexcept
{
    if (!std::isfinite(policy.maximum_fraction_of_work) ||
        policy.maximum_fraction_of_work < 0.0 || chosen_work_ns == 0)
    {
        return {};
    }

    const double fraction =
        static_cast<double>(control_overhead_ns) / static_cast<double>(chosen_work_ns);
    const bool absolute_ok = policy.maximum_absolute_ns == 0 ||
                             control_overhead_ns <= policy.maximum_absolute_ns;
    return {absolute_ok && fraction <= policy.maximum_fraction_of_work, fraction};
}

}  // namespace sc2opt::tuner

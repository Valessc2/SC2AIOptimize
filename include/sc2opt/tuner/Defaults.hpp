#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace sc2opt::tuner::defaults {

inline constexpr std::uint64_t kOperationalMinimumSamples = 20;
inline constexpr std::uint64_t kCertificationMinimumSamples = 50;
inline constexpr std::uint64_t kMinimumAbsoluteGainNs = 1'000;
inline constexpr double kMinimumRelativeGain = 0.02;
inline constexpr double kChampionZScore = 1.96;
inline constexpr double kMaximumControlOverheadFraction = 0.01;
inline constexpr std::uint64_t kMaximumControlOverheadNs = 50'000;

// Seed candidate sets are engineering priors, not universal truths. Calibration is expected to
// reject, narrow or replace them for each hardware/workload/consumer context.
inline constexpr std::array<std::size_t, 8> kWorkloadSizeCandidates{
    8, 16, 32, 64, 128, 256, 512, 1024};
inline constexpr std::array<std::size_t, 7> kBatchSizeCandidates{
    16, 32, 64, 128, 256, 512, 1024};
inline constexpr std::array<float, 6> kSpatialCellSizeCandidates{
    2.0f, 3.0f, 4.0f, 6.0f, 8.0f, 12.0f};

inline constexpr std::uint32_t kBudgetResumePercent = 55;
inline constexpr std::uint32_t kBudgetSoftPercent = 70;
inline constexpr std::uint32_t kBudgetHardPercent = 95;

}  // namespace sc2opt::tuner::defaults

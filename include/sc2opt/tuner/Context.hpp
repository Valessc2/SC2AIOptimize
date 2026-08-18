#pragma once

#include "sc2opt/tuner/NetBenefit.hpp"

#include <cstddef>
#include <cstdint>
#include <span>

namespace sc2opt::tuner {

struct ContextKey {
    std::uint64_t workload = 0;
    std::uint64_t hardware = 0;
    std::uint32_t state_flags = 0;

    [[nodiscard]] constexpr bool operator==(const ContextKey&) const noexcept = default;
};

struct ContextChampion {
    ContextKey context{};
    CandidateId champion = kBaselineCandidate;
};

[[nodiscard]] CandidateId SelectContextChampion(
    std::span<const ContextChampion> champions,
    ContextKey context,
    CandidateId fallback = kBaselineCandidate) noexcept;

struct CrossoverBand {
    std::size_t maximum_workload_size = 0;
    CandidateId champion = kBaselineCandidate;
};

enum class CrossoverValidationIssue : std::uint8_t {
    None,
    Empty,
    NonIncreasingMaximum
};

[[nodiscard]] CrossoverValidationIssue ValidateCrossoverBands(
    std::span<const CrossoverBand> bands) noexcept;

[[nodiscard]] CandidateId SelectCrossoverChampion(
    std::span<const CrossoverBand> bands,
    std::size_t workload_size,
    CandidateId fallback = kBaselineCandidate) noexcept;

}  // namespace sc2opt::tuner

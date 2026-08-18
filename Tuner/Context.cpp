#include "sc2opt/tuner/Context.hpp"

namespace sc2opt::tuner {

CandidateId SelectContextChampion(std::span<const ContextChampion> champions,
                                  ContextKey context,
                                  CandidateId fallback) noexcept
{
    for (const ContextChampion& entry : champions)
    {
        if (entry.context == context)
            return entry.champion;
    }
    return fallback;
}

CrossoverValidationIssue ValidateCrossoverBands(std::span<const CrossoverBand> bands) noexcept
{
    if (bands.empty())
        return CrossoverValidationIssue::Empty;

    for (std::size_t i = 1; i < bands.size(); ++i)
    {
        if (bands[i].maximum_workload_size <= bands[i - 1].maximum_workload_size)
            return CrossoverValidationIssue::NonIncreasingMaximum;
    }
    return CrossoverValidationIssue::None;
}

CandidateId SelectCrossoverChampion(std::span<const CrossoverBand> bands,
                                    std::size_t workload_size,
                                    CandidateId fallback) noexcept
{
    if (ValidateCrossoverBands(bands) != CrossoverValidationIssue::None)
        return fallback;

    for (const CrossoverBand& band : bands)
    {
        if (workload_size <= band.maximum_workload_size)
            return band.champion;
    }
    return fallback;
}

}  // namespace sc2opt::tuner

#include "sc2opt/kernel/hot/SpatialGrid.hpp"

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

using Clock = std::chrono::steady_clock;

struct Lcg {
    std::uint32_t state = 0x12345678u;

    std::uint32_t Next() noexcept
    {
        state = state * 1664525u + 1013904223u;
        return state;
    }

    float Coordinate() noexcept
    {
        return static_cast<float>(Next() & 0xffffu) * (128.0f / 65535.0f);
    }
};

struct TimingSummary {
    std::uint64_t p50 = 0;
    std::uint64_t p95 = 0;
    std::uint64_t p99 = 0;
};

template <typename Function>
TimingSummary MeasureNanoseconds(Function&& function)
{
    std::array<std::uint64_t, 31> samples{};
    std::uint64_t checksum = 0;
    for (std::size_t i = 0; i < samples.size(); ++i)
    {
        const auto start = Clock::now();
        checksum ^= function();
        const auto stop = Clock::now();
        samples[i] = static_cast<std::uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count());
    }
    std::sort(samples.begin(), samples.end());
    if (checksum == 0xFFFFFFFFFFFFFFFFull)
        std::cerr << "checksum sentinel\n";

    const auto percentile = [&](std::size_t numerator) {
        const std::size_t rank = (samples.size() * numerator + 99u) / 100u;
        return samples[std::min(rank == 0 ? 0u : rank - 1u, samples.size() - 1u)];
    };
    return {percentile(50), percentile(95), percentile(99)};
}

std::uint64_t RunQueries(const std::vector<float>& xy,
                         const std::vector<sc2opt::kernel::hot::Vec2>& centers,
                         float radius,
                         std::vector<std::uint32_t>& output)
{
    std::uint64_t checksum = 0;
    for (const auto center : centers)
    {
        const auto result = sc2opt::kernel::hot::RadiusQueryBruteForce(xy, center, radius, output);
        if (!result)
            return 0;
        checksum += result.written;
        if (result.written != 0)
            checksum ^= output[result.written - 1];
    }
    return checksum;
}

std::uint64_t RunGridQueries(sc2opt::kernel::hot::SpatialGrid2D& grid,
                             const std::vector<float>& xy,
                             const std::vector<sc2opt::kernel::hot::Vec2>& centers,
                             float radius,
                             std::vector<std::uint32_t>& output,
                             sc2opt::kernel::hot::SpatialOrder order)
{
    if (grid.Rebuild(xy) != sc2opt::kernel::hot::SpatialStatus::Ok)
        return 0;

    std::uint64_t checksum = 0;
    for (const auto center : centers)
    {
        const auto result = grid.Query(center, radius, output, order);
        if (!result)
            return 0;
        checksum += result.written;
        if (result.written != 0)
            checksum ^= output[result.written - 1];
    }
    return checksum;
}

bool VerifyStableEquivalence(const std::vector<float>& xy,
                             const std::vector<sc2opt::kernel::hot::Vec2>& centers,
                             float radius,
                             sc2opt::kernel::hot::SpatialGrid2D& grid,
                             std::vector<std::uint32_t>& baseline,
                             std::vector<std::uint32_t>& candidate)
{
    if (grid.Rebuild(xy) != sc2opt::kernel::hot::SpatialStatus::Ok)
        return false;

    for (const auto center : centers)
    {
        const auto reference =
            sc2opt::kernel::hot::RadiusQueryBruteForce(xy, center, radius, baseline);
        const auto accelerated = grid.Query(center, radius, candidate,
                                             sc2opt::kernel::hot::SpatialOrder::InputOrder);
        if (!reference || !accelerated || reference.written != accelerated.written)
            return false;
        if (!std::equal(baseline.begin(), baseline.begin() + reference.written,
                        candidate.begin()))
        {
            return false;
        }
    }
    return true;
}

}  // namespace

int main()
{
    constexpr std::array<std::size_t, 7> sizes{8, 16, 32, 64, 128, 256, 512};
    constexpr std::size_t query_count = 256;
    constexpr float radius = 8.0f;
    constexpr float cell_size = 4.0f;

    std::cout << "# profile=spatial.synthetic.v1 seed=0x12345678 samples=31\n";
    std::cout << "# synthetic calibration only; CI timing is never certification\n";
    std::cout << "points,queries,cell_size,radius,baseline_p50_ns,baseline_p95_ns,baseline_p99_ns,"
                 "grid_stable_p50_ns,grid_stable_p95_ns,grid_stable_p99_ns,"
                 "grid_unordered_p50_ns,grid_unordered_p95_ns,grid_unordered_p99_ns\n";

    for (const std::size_t point_count : sizes)
    {
        Lcg rng;
        std::vector<float> xy(point_count * 2);
        for (float& value : xy)
            value = rng.Coordinate();

        std::vector<sc2opt::kernel::hot::Vec2> centers(query_count);
        for (auto& center : centers)
            center = {rng.Coordinate(), rng.Coordinate()};

        std::vector<std::uint32_t> baseline(point_count);
        std::vector<std::uint32_t> candidate(point_count);
        sc2opt::kernel::hot::SpatialGrid2D grid;
        if (grid.SetCellSize(cell_size) != sc2opt::kernel::hot::SpatialStatus::Ok ||
            !VerifyStableEquivalence(xy, centers, radius, grid, baseline, candidate))
        {
            std::cerr << "equivalence failure at point_count=" << point_count << '\n';
            return 2;
        }

        const TimingSummary baseline_ns =
            MeasureNanoseconds([&] { return RunQueries(xy, centers, radius, baseline); });
        const TimingSummary stable_ns = MeasureNanoseconds([&] {
            return RunGridQueries(grid, xy, centers, radius, candidate,
                                  sc2opt::kernel::hot::SpatialOrder::InputOrder);
        });
        const TimingSummary unordered_ns = MeasureNanoseconds([&] {
            return RunGridQueries(grid, xy, centers, radius, candidate,
                                  sc2opt::kernel::hot::SpatialOrder::Unordered);
        });

        std::cout << point_count << ',' << query_count << ',' << cell_size << ',' << radius << ','
                  << baseline_ns.p50 << ',' << baseline_ns.p95 << ',' << baseline_ns.p99 << ','
                  << stable_ns.p50 << ',' << stable_ns.p95 << ',' << stable_ns.p99 << ','
                  << unordered_ns.p50 << ',' << unordered_ns.p95 << ',' << unordered_ns.p99 << '\n';
    }

    return 0;
}

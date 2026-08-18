#include "sc2opt/kernel/hot/Filtering.hpp"
#include "sc2opt/kernel/hot/Geometry.hpp"
#include "sc2opt/kernel/hot/SpatialGrid.hpp"
#include "sc2opt/model/WorldView.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string_view>
#include <vector>

namespace {

int failures = 0;

void Check(bool condition, std::string_view message)
{
    if (!condition)
    {
        ++failures;
        std::cerr << "FAIL: " << message << '\n';
    }
}

struct Lcg {
    std::uint32_t state = 0x6d2b79f5u;

    std::uint32_t Next() noexcept
    {
        state = state * 1664525u + 1013904223u;
        return state;
    }

    float Coordinate() noexcept
    {
        const auto signed_value = static_cast<std::int32_t>(Next() % 32769u) - 16384;
        return static_cast<float>(signed_value) / 64.0f;
    }

    float Positive(float maximum) noexcept
    {
        return static_cast<float>(Next() & 0xffffu) * (maximum / 65535.0f);
    }
};

void TestGeometryProperties()
{
    using namespace sc2opt::kernel::hot;
    Lcg rng;
    for (int i = 0; i < 5000; ++i)
    {
        const Vec2 a{rng.Coordinate(), rng.Coordinate()};
        const Vec2 b{rng.Coordinate(), rng.Coordinate()};
        const float radius = rng.Positive(64.0f);
        const float ab = DistanceSquared(a, b);
        const float ba = DistanceSquared(b, a);
        Check(ab >= 0.0f && ab == ba, "distance squared symmetry/non-negative");
        if (WithinRadius(a, b, radius, BoundaryMode::Strict))
            Check(WithinRadius(a, b, radius, BoundaryMode::Inclusive),
                  "strict radius implies inclusive radius");
    }

    Check(!WithinRadius({0.0f, 0.0f}, {0.0f, 0.0f}, -1.0f),
          "negative radius rejected");
    Check(!WithinRadius({0.0f, 0.0f}, {1.0f, 0.0f},
                        std::numeric_limits<float>::quiet_NaN()),
          "NaN radius fails closed");
}

bool ReferenceMatch(const sc2opt::model::UnitView& unit,
                    sc2opt::kernel::hot::UnitFilter filter) noexcept
{
    using namespace sc2opt::kernel::hot;
    if ((unit.flags & filter.required_flags) != filter.required_flags)
        return false;
    if ((unit.flags & filter.excluded_flags) != 0u)
        return false;
    if (filter.consumer_type_id != kAnyConsumerType &&
        unit.consumer_type_id != filter.consumer_type_id)
    {
        return false;
    }
    return unit.health >= filter.minimum_health;
}

void TestFilteringProperties()
{
    using namespace sc2opt;
    Lcg rng;
    for (int iteration = 0; iteration < 300; ++iteration)
    {
        const std::size_t count = 1u + (rng.Next() % 128u);
        std::vector<model::UnitView> units(count);
        for (std::size_t i = 0; i < count; ++i)
        {
            units[i].tag = i + 1;
            units[i].consumer_type_id = rng.Next() % 8u;
            units[i].flags = rng.Next() & 0x0fu;
            units[i].health = rng.Positive(500.0f);
        }

        kernel::hot::UnitFilter filter{};
        filter.required_flags = rng.Next() & 0x0fu;
        filter.excluded_flags = rng.Next() & 0x0fu;
        filter.consumer_type_id = (rng.Next() & 1u) != 0u ? rng.Next() % 8u
                                                          : kernel::hot::kAnyConsumerType;
        filter.minimum_health = rng.Positive(500.0f);

        std::vector<std::uint32_t> expected;
        for (std::size_t i = 0; i < units.size(); ++i)
        {
            if (ReferenceMatch(units[i], filter))
                expected.push_back(static_cast<std::uint32_t>(i));
        }

        std::vector<std::uint32_t> actual(count);
        const auto result = kernel::hot::FilterUnitIndices(units, filter, actual);
        Check(result && result.written == expected.size(), "filter property count");
        Check(std::equal(expected.begin(), expected.end(), actual.begin()),
              "filter property ordering/content");
    }
}

void TestSpatialFuzzEquivalence()
{
    using namespace sc2opt::kernel::hot;
    Lcg rng;
    constexpr float cell_sizes[] = {1.0f, 2.0f, 4.0f, 8.0f, 16.0f};

    for (int world = 0; world < 80; ++world)
    {
        const std::size_t point_count = 1u + (rng.Next() % 192u);
        std::vector<float> xy(point_count * 2u);
        for (float& value : xy)
            value = rng.Coordinate();

        SpatialGrid2D grid;
        const float cell_size = cell_sizes[rng.Next() % 5u];
        Check(grid.SetCellSize(cell_size) == SpatialStatus::Ok, "fuzz cell size accepted");
        Check(grid.Rebuild(xy) == SpatialStatus::Ok, "fuzz grid rebuild");

        std::vector<std::uint32_t> reference(point_count);
        std::vector<std::uint32_t> stable(point_count);
        std::vector<std::uint32_t> unordered(point_count);

        for (int query = 0; query < 40; ++query)
        {
            const Vec2 center{rng.Coordinate(), rng.Coordinate()};
            const float radius = rng.Positive(48.0f);
            const BoundaryMode boundary = (rng.Next() & 1u) != 0u ? BoundaryMode::Inclusive
                                                                  : BoundaryMode::Strict;

            const auto baseline = RadiusQueryBruteForce(xy, center, radius, reference, boundary);
            const auto ordered =
                grid.Query(center, radius, stable, SpatialOrder::InputOrder, boundary);
            const auto loose = grid.Query(center, radius, unordered, SpatialOrder::Unordered,
                                          boundary);

            Check(baseline && ordered && loose, "fuzz spatial query statuses");
            if (!baseline || !ordered || !loose)
                continue;

            Check(baseline.written == ordered.written && baseline.written == loose.written,
                  "fuzz spatial count equivalence");
            Check(std::equal(reference.begin(), reference.begin() + baseline.written,
                             stable.begin()),
                  "fuzz stable spatial order equivalence");

            std::vector<std::uint32_t> expected(reference.begin(),
                                                reference.begin() + baseline.written);
            std::vector<std::uint32_t> actual(unordered.begin(), unordered.begin() + loose.written);
            std::sort(expected.begin(), expected.end());
            std::sort(actual.begin(), actual.end());
            Check(expected == actual, "fuzz unordered spatial membership equivalence");
        }
    }

    SpatialGrid2D grid;
    const float nan = std::numeric_limits<float>::quiet_NaN();
    const std::vector<float> invalid{0.0f, 0.0f, nan, 1.0f};
    Check(grid.Rebuild(invalid) == SpatialStatus::NonFiniteCoordinate,
          "non-finite spatial input fails closed");
}

}  // namespace

int main()
{
    TestGeometryProperties();
    TestFilteringProperties();
    TestSpatialFuzzEquivalence();

    if (failures == 0)
    {
        std::cout << "SC2AIOptimize deep contract tests passed\n";
        return 0;
    }

    std::cerr << failures << " deep contract test(s) failed\n";
    return 1;
}

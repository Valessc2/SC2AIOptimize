#include "sc2opt/adapters/cpp/DenseTypeMap.hpp"
#include "sc2opt/adapters/cpp/WorldView.hpp"
#include "sc2opt/kernel/hot/BatchGeometry.hpp"
#include "sc2opt/registry/SC2Registry.hpp"

#include <array>
#include <cstdint>
#include <iostream>

namespace third_party_bot {

struct TotallyDifferentUnit {
    std::uint64_t identity;
    std::uint32_t kind;
    float px;
    float py;
    float hp;
};

sc2opt::adapters::cpp::UnitView Adapt(const TotallyDifferentUnit& unit)
{
    return {unit.identity,
            unit.kind,
            sc2opt::adapters::cpp::UnitAlive,
            unit.px,
            unit.py,
            unit.hp,
            0.0f,
            0.0f,
            0.375f};
}

}  // namespace third_party_bot

int main()
{
    using namespace sc2opt;

    const third_party_bot::TotallyDifferentUnit original{42, 7, 3.0f, 4.0f, 45.0f};
    const auto adapted = third_party_bot::Adapt(original);
    if (adapted.tag != 42 || adapted.consumer_type_id != 7 || adapted.health != 45.0f)
        return 1;

    const std::array<float, 6> xy{0.0f, 0.0f, 3.0f, 4.0f, 6.0f, 8.0f};
    std::array<float, 3> distances{};
    const auto batch = kernel::hot::DistanceSquaredFromPoint(xy, {0.0f, 0.0f}, distances);
    if (!batch || batch.written != 3 || distances[1] != 25.0f || distances[2] != 100.0f)
        return 2;

    const std::array<adapters::cpp::TypeNameBinding, 3> bindings{{
        {7, "Marine"},
        {11, "Zergling"},
        {19, "ConsumerOnlyUnit"},
    }};
    std::array<const registry::UnitStatic*, 20> type_map{};
    const auto report = adapters::cpp::BuildDenseTypeMap(registry::Base75689(), bindings, type_map);
    if (!report || report.mapped != 2 || report.unmatched != 1)
        return 3;
    if (type_map[7] == nullptr || type_map[11] == nullptr || type_map[19] != nullptr)
        return 4;

    std::cout << "generic C++ consumer proof passed\n";
    return 0;
}

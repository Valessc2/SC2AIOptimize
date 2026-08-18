#include "sc2opt/adapters/cpp/DenseTypeMap.hpp"

#include <algorithm>

namespace sc2opt::adapters::cpp {

TypeMapReport BuildDenseTypeMap(registry::RegistryView source,
                                std::span<const TypeNameBinding> bindings,
                                std::span<const registry::UnitStatic*> output) noexcept
{
    TypeMapReport report;

    for (const TypeNameBinding& binding : bindings)
    {
        if (binding.catalog_name.empty())
        {
            report.status = TypeMapStatus::EmptyCatalogName;
            return report;
        }
        report.required_size =
            std::max(report.required_size, static_cast<std::size_t>(binding.consumer_type_id) + 1);
    }

    if (output.size() < report.required_size)
    {
        report.status = TypeMapStatus::OutputTooSmall;
        return report;
    }

    // Startup only. O(n^2) duplicate validation avoids a hidden heap allocation in the generic
    // adapter and keeps the runtime contract deterministic. If this ever matters, benchmark a
    // challenger rather than assuming a hash table is cheaper.
    for (std::size_t i = 0; i < bindings.size(); ++i)
    {
        for (std::size_t j = i + 1; j < bindings.size(); ++j)
        {
            if (bindings[i].consumer_type_id == bindings[j].consumer_type_id)
            {
                report.status = TypeMapStatus::DuplicateConsumerTypeId;
                return report;
            }
        }
    }

    std::fill(output.begin(), output.end(), nullptr);
    for (const TypeNameBinding& binding : bindings)
    {
        const registry::UnitStatic* unit =
            registry::FindByCatalogName(source, binding.catalog_name);
        if (unit == nullptr)
        {
            ++report.unmatched;
            continue;
        }

        output[binding.consumer_type_id] = unit;
        ++report.mapped;
    }

    return report;
}

}  // namespace sc2opt::adapters::cpp

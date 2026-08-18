#include "sc2opt/tuner/TunableRegistry.hpp"

namespace sc2opt::tuner {

TunableRegistryValidation ValidateTunableRegistry(std::span<const TunableSpec> specs) noexcept
{
    for (std::size_t i = 0; i < specs.size(); ++i)
    {
        if (!IsValid(specs[i]))
            return {TunableRegistryIssue::InvalidSpec, i, i};

        for (std::size_t j = 0; j < i; ++j)
        {
            if (specs[i].id == specs[j].id)
                return {TunableRegistryIssue::DuplicateId, i, j};
            if (specs[i].name == specs[j].name)
                return {TunableRegistryIssue::DuplicateName, i, j};
        }
    }

    return {};
}

const TunableSpec* FindTunableById(std::span<const TunableSpec> specs, std::uint32_t id) noexcept
{
    for (const TunableSpec& spec : specs)
    {
        if (spec.id == id)
            return &spec;
    }
    return nullptr;
}

const TunableSpec* FindTunableByName(std::span<const TunableSpec> specs,
                                    std::string_view name) noexcept
{
    for (const TunableSpec& spec : specs)
    {
        if (spec.name == name)
            return &spec;
    }
    return nullptr;
}

}  // namespace sc2opt::tuner

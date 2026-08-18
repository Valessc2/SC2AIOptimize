#pragma once

#include <span>
#include <string_view>

namespace sc2opt::tuner {

struct TunableSpec {
    std::string_view id;
    double baseline = 0.0;
    double minimum = 0.0;
    double maximum = 0.0;
    double step = 0.0;
    bool safe_online = false;
};

// Non-owning schema only. The consuming bot owns parameter semantics, storage and safety.
// SC2AIOptimize owns generic search/measurement mechanics and never introspects bot memory.
class TunableRegistryView final {
public:
    explicit constexpr TunableRegistryView(std::span<const TunableSpec> specs) noexcept : specs_(specs) {}

    [[nodiscard]] constexpr std::span<const TunableSpec> specs() const noexcept { return specs_; }

    [[nodiscard]] const TunableSpec* find(std::string_view id) const noexcept
    {
        for (const auto& spec : specs_)
        {
            if (spec.id == id)
                return &spec;
        }
        return nullptr;
    }

private:
    std::span<const TunableSpec> specs_;
};

} // namespace sc2opt::tuner

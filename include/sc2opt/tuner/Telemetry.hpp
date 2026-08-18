#pragma once

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace sc2opt::tuner {

inline constexpr std::size_t kLatencyHistogramBuckets = 65;

struct LatencySummary {
    std::uint64_t samples = 0;
    std::uint64_t total_ns = 0;
    std::uint64_t minimum_ns = 0;
    std::uint64_t maximum_ns = 0;
    std::uint64_t p50_upper_ns = 0;
    std::uint64_t p95_upper_ns = 0;
    std::uint64_t p99_upper_ns = 0;
};

// Fixed-storage log2 histogram. Percentiles are conservative bucket upper bounds rather than
// exact sample quantiles. Observe() performs no allocation and saturates instead of wrapping.
class FixedLatencyHistogram final {
public:
    void Reset() noexcept;
    void Observe(std::uint64_t duration_ns) noexcept;
    [[nodiscard]] LatencySummary Summary() const noexcept;

private:
    [[nodiscard]] static std::size_t BucketFor(std::uint64_t value) noexcept;
    [[nodiscard]] static std::uint64_t BucketUpper(std::size_t bucket) noexcept;
    [[nodiscard]] std::uint64_t PercentileUpper(std::uint32_t percentile) const noexcept;

    std::array<std::uint64_t, kLatencyHistogramBuckets> buckets_{};
    std::uint64_t samples_ = 0;
    std::uint64_t total_ns_ = 0;
    std::uint64_t minimum_ns_ = std::numeric_limits<std::uint64_t>::max();
    std::uint64_t maximum_ns_ = 0;
};

template <bool Enabled>
class ScopedLatencyTimer;

template <>
class ScopedLatencyTimer<true> final {
public:
    explicit ScopedLatencyTimer(FixedLatencyHistogram& histogram) noexcept
        : histogram_(&histogram), start_(Clock::now()) {}

    ~ScopedLatencyTimer() noexcept
    {
        const auto elapsed =
            std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now() - start_);
        if (elapsed.count() >= 0)
            histogram_->Observe(static_cast<std::uint64_t>(elapsed.count()));
    }

    ScopedLatencyTimer(const ScopedLatencyTimer&) = delete;
    ScopedLatencyTimer& operator=(const ScopedLatencyTimer&) = delete;

private:
    using Clock = std::chrono::steady_clock;
    FixedLatencyHistogram* histogram_;
    Clock::time_point start_;
};

// Compile-time disabled form: no clock call, no histogram mutation and no stored state.
template <>
class ScopedLatencyTimer<false> final {
public:
    explicit constexpr ScopedLatencyTimer(FixedLatencyHistogram&) noexcept {}
};

}  // namespace sc2opt::tuner

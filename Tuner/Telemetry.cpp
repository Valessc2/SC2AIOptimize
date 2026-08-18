#include "sc2opt/tuner/Telemetry.hpp"

#include <algorithm>
#include <bit>

namespace sc2opt::tuner {

void FixedLatencyHistogram::Reset() noexcept
{
    buckets_.fill(0);
    samples_ = 0;
    total_ns_ = 0;
    minimum_ns_ = std::numeric_limits<std::uint64_t>::max();
    maximum_ns_ = 0;
}

std::size_t FixedLatencyHistogram::BucketFor(std::uint64_t value) noexcept
{
    return static_cast<std::size_t>(std::bit_width(value));
}

std::uint64_t FixedLatencyHistogram::BucketUpper(std::size_t bucket) noexcept
{
    if (bucket == 0)
        return 0;
    if (bucket >= 64)
        return std::numeric_limits<std::uint64_t>::max();
    return (std::uint64_t{1} << bucket) - 1;
}

void FixedLatencyHistogram::Observe(std::uint64_t duration_ns) noexcept
{
    if (samples_ == std::numeric_limits<std::uint64_t>::max())
        return;

    const std::size_t bucket = BucketFor(duration_ns);
    ++buckets_[bucket];
    ++samples_;
    minimum_ns_ = std::min(minimum_ns_, duration_ns);
    maximum_ns_ = std::max(maximum_ns_, duration_ns);

    if (duration_ns <= std::numeric_limits<std::uint64_t>::max() - total_ns_)
        total_ns_ += duration_ns;
    else
        total_ns_ = std::numeric_limits<std::uint64_t>::max();
}

std::uint64_t FixedLatencyHistogram::PercentileUpper(std::uint32_t percentile) const noexcept
{
    if (samples_ == 0 || percentile == 0 || percentile > 100)
        return 0;

    const std::uint64_t quotient = samples_ / 100;
    const std::uint64_t remainder = samples_ % 100;
    const std::uint64_t rank = quotient * percentile + (remainder * percentile + 99) / 100;

    std::uint64_t cumulative = 0;
    for (std::size_t i = 0; i < buckets_.size(); ++i)
    {
        cumulative += buckets_[i];
        if (cumulative >= rank)
            return BucketUpper(i);
    }
    return maximum_ns_;
}

LatencySummary FixedLatencyHistogram::Summary() const noexcept
{
    if (samples_ == 0)
        return {};

    return {samples_, total_ns_, minimum_ns_, maximum_ns_, PercentileUpper(50),
            PercentileUpper(95), PercentileUpper(99)};
}

}  // namespace sc2opt::tuner

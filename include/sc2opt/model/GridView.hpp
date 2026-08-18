#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>

namespace sc2opt::model {

struct GridPoint {
    std::uint32_t x = 0;
    std::uint32_t y = 0;

    [[nodiscard]] constexpr bool operator==(const GridPoint&) const noexcept = default;
};

struct ConstFloatGridView {
    std::span<const float> values;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t stride = 0;
};

struct FloatGridView {
    std::span<float> values;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t stride = 0;
};

struct ByteGridView {
    std::span<const std::uint8_t> values;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
    std::uint32_t stride = 0;
};

[[nodiscard]] constexpr bool GridShapeFits(std::size_t storage_size,
                                           std::uint32_t width,
                                           std::uint32_t height,
                                           std::uint32_t stride) noexcept
{
    if (width == 0 || height == 0 || stride < width)
        return false;
    const std::size_t rows = static_cast<std::size_t>(height);
    const std::size_t row_stride = static_cast<std::size_t>(stride);
    if (rows > std::numeric_limits<std::size_t>::max() / row_stride)
        return false;
    return storage_size >= rows * row_stride;
}

[[nodiscard]] constexpr bool IsValid(ConstFloatGridView grid) noexcept
{
    return GridShapeFits(grid.values.size(), grid.width, grid.height, grid.stride);
}

[[nodiscard]] constexpr bool IsValid(FloatGridView grid) noexcept
{
    return GridShapeFits(grid.values.size(), grid.width, grid.height, grid.stride);
}

[[nodiscard]] constexpr bool IsValid(ByteGridView grid) noexcept
{
    return GridShapeFits(grid.values.size(), grid.width, grid.height, grid.stride);
}

}  // namespace sc2opt::model

#include "sc2opt/Version.hpp"
#include "sc2opt/kernel/hot/BatchGeometry.hpp"
#include "sc2opt/registry/SC2Registry.hpp"

#include <pybind11/pybind11.h>

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>

namespace py = pybind11;

namespace {

py::buffer_info RequireFloat32Vector(const py::buffer& buffer, bool writable)
{
    py::buffer_info info = buffer.request(writable);
    if (info.ndim != 1 || info.itemsize != static_cast<py::ssize_t>(sizeof(float)) ||
        info.format != py::format_descriptor<float>::format() ||
        info.strides[0] != static_cast<py::ssize_t>(sizeof(float)))
    {
        throw py::value_error("expected a contiguous 1-D float32 buffer");
    }
    return info;
}

py::buffer_info RequireByteVector(const py::buffer& buffer, bool writable)
{
    py::buffer_info info = buffer.request(writable);
    if (info.ndim != 1 || info.itemsize != 1 || info.strides[0] != 1)
        throw py::value_error("expected a contiguous writable byte buffer");
    return info;
}

[[noreturn]] void ThrowBatchStatus(sc2opt::kernel::hot::BatchStatus status)
{
    using sc2opt::kernel::hot::BatchStatus;
    switch (status)
    {
    case BatchStatus::InvalidCoordinateCount:
        throw py::value_error("xy buffer must contain x/y pairs");
    case BatchStatus::OutputTooSmall:
        throw py::value_error("output buffer is too small");
    case BatchStatus::InvalidRadius:
        throw py::value_error("radius must be non-negative");
    case BatchStatus::Ok:
        break;
    }
    throw py::value_error("unknown SC2AIOptimize batch status");
}

std::size_t DistanceSquaredInto(const py::buffer& xy,
                                const py::buffer& output,
                                float anchor_x,
                                float anchor_y)
{
    const py::buffer_info input_info = RequireFloat32Vector(xy, false);
    const py::buffer_info output_info = RequireFloat32Vector(output, true);

    const auto input = std::span<const float>(static_cast<const float*>(input_info.ptr),
                                              static_cast<std::size_t>(input_info.shape[0]));
    auto out = std::span<float>(static_cast<float*>(output_info.ptr),
                                static_cast<std::size_t>(output_info.shape[0]));

    const auto result = sc2opt::kernel::hot::DistanceSquaredFromPoint(
        input, {anchor_x, anchor_y}, out);
    if (!result)
        ThrowBatchStatus(result.status);
    return result.written;
}

std::size_t WithinRadiusMaskInto(const py::buffer& xy,
                                 const py::buffer& output,
                                 float anchor_x,
                                 float anchor_y,
                                 float radius)
{
    const py::buffer_info input_info = RequireFloat32Vector(xy, false);
    const py::buffer_info output_info = RequireByteVector(output, true);

    const auto input = std::span<const float>(static_cast<const float*>(input_info.ptr),
                                              static_cast<std::size_t>(input_info.shape[0]));
    auto out = std::span<std::uint8_t>(static_cast<std::uint8_t*>(output_info.ptr),
                                       static_cast<std::size_t>(output_info.shape[0]));

    const auto result = sc2opt::kernel::hot::WithinRadiusMaskFromPoint(
        input, {anchor_x, anchor_y}, radius, out);
    if (!result)
        ThrowBatchStatus(result.status);
    return result.written;
}

}  // namespace

PYBIND11_MODULE(_sc2opt, module)
{
    module.doc() = "Generic zero-framework Python adapter for SC2AIOptimize";
    module.attr("__version__") = SC2OPT_VERSION_STRING;

    module.def("distance_squared_into", &DistanceSquaredInto,
               py::arg("xy"), py::arg("output"), py::arg("anchor_x"), py::arg("anchor_y"),
               "Write squared distances from an anchor into a caller-owned float32 buffer.");
    module.def("within_radius_mask_into", &WithinRadiusMaskInto,
               py::arg("xy"), py::arg("output"), py::arg("anchor_x"), py::arg("anchor_y"),
               py::arg("radius"),
               "Write an inclusive radius mask into a caller-owned byte buffer.");

    module.def("registry_identity", [] {
        const auto registry = sc2opt::registry::Base75689();
        return py::make_tuple(std::string(registry.identity.sc2_build),
                              std::string(registry.identity.data_version));
    });
    module.def("registry_unit_count", [] {
        return sc2opt::registry::Base75689().units.size();
    });
}

#include "sc2opt/Version.hpp"
#include "sc2opt/adapters/IntegrationContract.hpp"
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

py::dict IntegrationContractDict()
{
    const auto contract = sc2opt::adapters::CurrentIntegrationContract();
    py::dict result;
    result["api_major"] = contract.api_major;
    result["api_minor"] = contract.api_minor;
    result["unit_view_abi"] = contract.unit_view_abi;
    result["package_version"] = std::string(contract.package_version);
    result["sc2_build"] = std::string(contract.sc2_build);
    result["data_version"] = std::string(contract.data_version);
    result["capabilities"] = contract.capabilities;
    return result;
}

py::dict CheckIntegration(std::uint32_t api_major,
                          std::uint32_t minimum_api_minor,
                          std::uint32_t unit_view_abi,
                          const std::string& sc2_build,
                          const std::string& data_version,
                          std::uint64_t required_capabilities)
{
    sc2opt::adapters::IntegrationRequirements requirements{};
    requirements.api_major = api_major;
    requirements.minimum_api_minor = minimum_api_minor;
    requirements.unit_view_abi = unit_view_abi;
    requirements.sc2_build = sc2_build;
    requirements.data_version = data_version;
    requirements.required_capabilities = required_capabilities;

    const auto report = sc2opt::adapters::CheckIntegrationCompatibility(requirements);
    py::dict result;
    result["ready"] = report.ready();
    result["status"] = std::string(sc2opt::adapters::IntegrationStatusName(report.status));
    result["missing_capabilities"] = report.missing_capabilities;
    return result;
}

}  // namespace

PYBIND11_MODULE(_sc2opt, module)
{
    module.doc() = "Generic zero-framework Python adapter for SC2AIOptimize";
    module.attr("__version__") = SC2OPT_VERSION_STRING;

    module.def("integration_contract", &IntegrationContractDict,
               "Return the bot-neutral SC2AIOptimize integration contract.");
    module.def("check_integration", &CheckIntegration,
               py::arg("api_major") = static_cast<std::uint32_t>(sc2opt::kApiContractMajor),
               py::arg("minimum_api_minor") = static_cast<std::uint32_t>(sc2opt::kApiContractMinor),
               py::arg("unit_view_abi") = sc2opt::model::kUnitViewAbiVersion,
               py::arg("sc2_build") = std::string(sc2opt::registry::kBase75689Build),
               py::arg("data_version") = std::string(sc2opt::registry::kBase75689DataVersion),
               py::arg("required_capabilities") = sc2opt::adapters::kSharedIntegrationCapabilities,
               "Check whether this SC2AIOptimize build satisfies a consumer integration contract.");

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

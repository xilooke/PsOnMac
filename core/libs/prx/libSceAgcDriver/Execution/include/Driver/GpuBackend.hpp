#ifndef CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_DRIVER_GPUBACKEND_HPP
#define CORE_LIBS_PRX_LIBSCEAGCDRIVER_EXECUTION_INCLUDE_DRIVER_GPUBACKEND_HPP

#include "gpu/rhi/RhiTypes.hpp"
#include <string_view>

namespace AgcDriver::DriverDetail {

AnyPS5::Gpu::Rhi::BackendType SelectedGpuBackend();
bool IsMetalBackendActive();
bool IsVulkanBackendActive();

} // namespace AgcDriver::DriverDetail

#endif
#include "prx/libSceAgcDriver/Execution/include/Driver/GpuBackend.hpp"
#include <cstdlib>
#include <cstring>

namespace AgcDriver::DriverDetail {

AnyPS5::Gpu::Rhi::BackendType SelectedGpuBackend() {
    const char* env = std::getenv("ANYPS5_GPU_BACKEND");
    if (env != nullptr) {
        if (std::strcmp(env, "metal") == 0 || std::strcmp(env, "Metal") == 0) {
            return AnyPS5::Gpu::Rhi::BackendType::Metal;
        }
        if (std::strcmp(env, "vulkan") == 0 || std::strcmp(env, "Vulkan") == 0) {
            return AnyPS5::Gpu::Rhi::BackendType::Vulkan;
        }
        if (std::strcmp(env, "null") == 0 || std::strcmp(env, "Null") == 0) {
            return AnyPS5::Gpu::Rhi::BackendType::Null;
        }
    }
#if defined(__APPLE__)
    return AnyPS5::Gpu::Rhi::BackendType::Metal;
#else
    return AnyPS5::Gpu::Rhi::BackendType::Vulkan;
#endif
}

bool IsMetalBackendActive() {
    return SelectedGpuBackend() == AnyPS5::Gpu::Rhi::BackendType::Metal;
}

bool IsVulkanBackendActive() {
    return SelectedGpuBackend() == AnyPS5::Gpu::Rhi::BackendType::Vulkan;
}

} // namespace AgcDriver::DriverDetail

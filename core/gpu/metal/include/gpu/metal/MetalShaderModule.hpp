#ifndef CORE_GPU_METAL_INCLUDE_GPU_METAL_METALSHADERMODULE_HPP
#define CORE_GPU_METAL_INCLUDE_GPU_METAL_METALSHADERMODULE_HPP

#include "gpu/rhi/IShaderModule.hpp"
#include <string>

namespace AnyPS5::Gpu::Metal {

class MetalShaderModule final : public Rhi::IShaderModule {
public:
    MetalShaderModule(void* mtlDevice, const Rhi::ShaderModuleDesc& desc);
    ~MetalShaderModule() override;

    [[nodiscard]] Rhi::ShaderStageFlag GetStage() const override { return stage; }
    [[nodiscard]] std::string_view GetEntryPoint() const override { return entryPoint; }

    [[nodiscard]] void* GetNativeFunction() const { return nativeFunction; }
    [[nodiscard]] void* GetNativeLibrary() const { return nativeLibrary; }

private:
    void* nativeLibrary = nullptr;
    void* nativeFunction = nullptr;
    Rhi::ShaderStageFlag stage = Rhi::ShaderStageFlag::Compute;
    std::string entryPoint;
};

} // namespace AnyPS5::Gpu::Metal

#endif

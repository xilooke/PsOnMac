#ifndef CORE_GPU_METAL_INCLUDE_GPU_METAL_METALSAMPLER_HPP
#define CORE_GPU_METAL_INCLUDE_GPU_METAL_METALSAMPLER_HPP

#include "gpu/rhi/ISampler.hpp"

namespace AnyPS5::Gpu::Metal {

class MetalSampler final : public Rhi::ISampler {
public:
    MetalSampler(void* mtlDevice, const Rhi::SamplerDesc& desc);
    ~MetalSampler() override;

    [[nodiscard]] const Rhi::SamplerDesc& GetDesc() const override { return desc; }
    [[nodiscard]] void* GetNativeSampler() const { return nativeSampler; }

private:
    void* nativeSampler = nullptr;
    Rhi::SamplerDesc desc{};
};

} // namespace AnyPS5::Gpu::Metal

#endif

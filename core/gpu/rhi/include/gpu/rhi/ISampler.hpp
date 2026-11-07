#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_ISAMPLER_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_ISAMPLER_HPP

#include "RhiTypes.hpp"

namespace AnyPS5::Gpu::Rhi {

struct SamplerDesc {
    Filter minFilter = Filter::Linear;
    Filter magFilter = Filter::Linear;
    Filter mipmapMode = Filter::Linear;
    SamplerAddressMode addressU = SamplerAddressMode::Repeat;
    SamplerAddressMode addressV = SamplerAddressMode::Repeat;
    SamplerAddressMode addressW = SamplerAddressMode::Repeat;
    float mipLodBias = 0.0f;
    bool anisotropyEnable = false;
    float maxAnisotropy = 1.0f;
    bool compareEnable = false;
    CompareOp compareOp = CompareOp::Never;
    float minLod = 0.0f;
    float maxLod = 1000.0f;
};

class ISampler {
public:
    virtual ~ISampler() = default;
    [[nodiscard]] virtual const SamplerDesc& GetDesc() const = 0;
};

} // namespace AnyPS5::Gpu::Rhi

#endif
#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_IDESCRIPTORTABLE_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_IDESCRIPTORTABLE_HPP

#include "IBuffer.hpp"
#include "ITexture.hpp"
#include "ISampler.hpp"
#include <cstdint>
#include <memory>

namespace AnyPS5::Gpu::Rhi {

class IDescriptorTable {
public:
    virtual ~IDescriptorTable() = default;
    virtual void SetBuffer(std::uint32_t slot, const std::shared_ptr<IBuffer>& buffer, std::size_t offset = 0) = 0;
    virtual void SetTexture(std::uint32_t slot, const std::shared_ptr<ITexture>& texture) = 0;
    virtual void SetSampler(std::uint32_t slot, const std::shared_ptr<ISampler>& sampler) = 0;
    virtual void Flush() = 0;
};

} // namespace AnyPS5::Gpu::Rhi

#endif
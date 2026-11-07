#ifndef CORE_GPU_METAL_INCLUDE_GPU_METAL_METALDESCRIPTORTABLE_HPP
#define CORE_GPU_METAL_INCLUDE_GPU_METAL_METALDESCRIPTORTABLE_HPP

#include "gpu/rhi/IDescriptorTable.hpp"
#include <unordered_map>

namespace AnyPS5::Gpu::Metal {

struct MetalBufferBinding {
    std::shared_ptr<Rhi::IBuffer> buffer;
    std::size_t offset = 0;
};

class MetalDescriptorTable final : public Rhi::IDescriptorTable {
public:
    MetalDescriptorTable() = default;
    ~MetalDescriptorTable() override = default;

    void SetBuffer(std::uint32_t slot, const std::shared_ptr<Rhi::IBuffer>& buffer, std::size_t offset = 0) override;
    void SetTexture(std::uint32_t slot, const std::shared_ptr<Rhi::ITexture>& texture) override;
    void SetSampler(std::uint32_t slot, const std::shared_ptr<Rhi::ISampler>& sampler) override;
    void Flush() override;

    [[nodiscard]] const std::unordered_map<std::uint32_t, MetalBufferBinding>& GetBuffers() const { return buffers; }
    [[nodiscard]] const std::unordered_map<std::uint32_t, std::shared_ptr<Rhi::ITexture>>& GetTextures() const { return textures; }
    [[nodiscard]] const std::unordered_map<std::uint32_t, std::shared_ptr<Rhi::ISampler>>& GetSamplers() const { return samplers; }

private:
    std::unordered_map<std::uint32_t, MetalBufferBinding> buffers;
    std::unordered_map<std::uint32_t, std::shared_ptr<Rhi::ITexture>> textures;
    std::unordered_map<std::uint32_t, std::shared_ptr<Rhi::ISampler>> samplers;
};

} // namespace AnyPS5::Gpu::Metal

#endif

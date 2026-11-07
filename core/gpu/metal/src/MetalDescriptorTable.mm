#include "gpu/metal/MetalDescriptorTable.hpp"

namespace AnyPS5::Gpu::Metal {

void MetalDescriptorTable::SetBuffer(std::uint32_t slot, const std::shared_ptr<Rhi::IBuffer>& buffer, std::size_t offset) {
    buffers[slot] = MetalBufferBinding{buffer, offset};
}

void MetalDescriptorTable::SetTexture(std::uint32_t slot, const std::shared_ptr<Rhi::ITexture>& texture) {
    textures[slot] = texture;
}

void MetalDescriptorTable::SetSampler(std::uint32_t slot, const std::shared_ptr<Rhi::ISampler>& sampler) {
    samplers[slot] = sampler;
}

void MetalDescriptorTable::Flush() {
}

} // namespace AnyPS5::Gpu::Metal

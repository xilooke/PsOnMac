#ifndef CORE_GPU_METAL_INCLUDE_GPU_METAL_METALBUFFER_HPP
#define CORE_GPU_METAL_INCLUDE_GPU_METAL_METALBUFFER_HPP

#include "gpu/rhi/IBuffer.hpp"

namespace AnyPS5::Gpu::Metal {

class MetalBuffer final : public Rhi::IBuffer {
public:
    MetalBuffer(void* mtlDevice, const Rhi::BufferDesc& desc);
    ~MetalBuffer() override;

    [[nodiscard]] std::size_t GetSize() const override;
    [[nodiscard]] Rhi::BufferUsage GetUsage() const override;
    [[nodiscard]] void* Map() override;
    void Unmap() override;
    [[nodiscard]] std::uint64_t GetDeviceAddress() const override;

    [[nodiscard]] void* GetNativeBuffer() const { return nativeBuffer; }

private:
    void* nativeBuffer = nullptr;
    std::size_t size = 0;
    Rhi::BufferUsage usage = Rhi::BufferUsage::None;
};

} // namespace AnyPS5::Gpu::Metal

#endif

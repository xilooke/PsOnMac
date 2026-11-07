#ifndef CORE_GPU_METAL_INCLUDE_GPU_METAL_METALTEXTURE_HPP
#define CORE_GPU_METAL_INCLUDE_GPU_METAL_METALTEXTURE_HPP

#include "gpu/rhi/ITexture.hpp"

namespace AnyPS5::Gpu::Metal {

class MetalTexture final : public Rhi::ITexture {
public:
    MetalTexture(void* mtlDevice, const Rhi::TextureDesc& desc);
    explicit MetalTexture(void* existingTexture);
    ~MetalTexture() override;

    [[nodiscard]] const Rhi::TextureDesc& GetDesc() const override { return desc; }
    [[nodiscard]] Rhi::Format GetFormat() const override { return desc.format; }
    [[nodiscard]] std::uint32_t GetWidth() const override { return desc.width; }
    [[nodiscard]] std::uint32_t GetHeight() const override { return desc.height; }
    [[nodiscard]] std::uint32_t GetDepth() const override { return desc.depth; }
    [[nodiscard]] std::uint32_t GetMipLevels() const override { return desc.mipLevels; }
    [[nodiscard]] std::uint32_t GetArrayLayers() const override { return desc.arrayLayers; }

    [[nodiscard]] void* GetNativeTexture() const { return nativeTexture; }

private:
    void* nativeTexture = nullptr;
    Rhi::TextureDesc desc{};
};

} // namespace AnyPS5::Gpu::Metal

#endif

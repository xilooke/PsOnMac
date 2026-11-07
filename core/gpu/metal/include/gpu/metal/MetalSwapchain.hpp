#ifndef CORE_GPU_METAL_INCLUDE_GPU_METAL_METALSWAPCHAIN_HPP
#define CORE_GPU_METAL_INCLUDE_GPU_METAL_METALSWAPCHAIN_HPP

#include "gpu/rhi/ISwapchain.hpp"
#include <vector>

namespace AnyPS5::Gpu::Metal {

class MetalSwapchain final : public Rhi::ISwapchain {
public:
    MetalSwapchain(void* mtlDevice, const Rhi::SwapchainDesc& desc);
    ~MetalSwapchain() override;

    bool AcquireNextImage(std::uint32_t& imageIndex, std::uint64_t timeoutNanoseconds = UINT64_MAX) override;
    void Present(std::uint32_t imageIndex) override;
    void Resize(std::uint32_t width, std::uint32_t height) override;
    [[nodiscard]] std::shared_ptr<Rhi::ITexture> GetTexture(std::uint32_t imageIndex) const override;
    [[nodiscard]] std::uint32_t GetWidth() const override { return desc.width; }
    [[nodiscard]] std::uint32_t GetHeight() const override { return desc.height; }
    [[nodiscard]] Rhi::Format GetFormat() const override { return desc.format; }

private:
    Rhi::SwapchainDesc desc;
    void* nativeLayer = nullptr;
    std::vector<void*> activeDrawables;
    std::vector<std::shared_ptr<Rhi::ITexture>> currentTextures;
};

} // namespace AnyPS5::Gpu::Metal

#endif

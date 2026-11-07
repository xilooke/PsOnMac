#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_ISWAPCHAIN_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_ISWAPCHAIN_HPP

#include "RhiTypes.hpp"
#include "ITexture.hpp"
#include <cstdint>
#include <memory>

namespace AnyPS5::Gpu::Rhi {

struct SwapchainDesc {
    void* nativeWindowHandle = nullptr;
    std::uint32_t width = 1920;
    std::uint32_t height = 1080;
    std::uint32_t bufferCount = 3;
    Format format = Format::B8G8R8A8_UNORM;
    bool vsync = true;
    bool hdr = false;
};

class ISwapchain {
public:
    virtual ~ISwapchain() = default;
    virtual bool AcquireNextImage(std::uint32_t& imageIndex, std::uint64_t timeoutNanoseconds = UINT64_MAX) = 0;
    virtual void Present(std::uint32_t imageIndex) = 0;
    virtual void Resize(std::uint32_t width, std::uint32_t height) = 0;
    [[nodiscard]] virtual std::shared_ptr<ITexture> GetTexture(std::uint32_t imageIndex) const = 0;
    [[nodiscard]] virtual std::uint32_t GetWidth() const = 0;
    [[nodiscard]] virtual std::uint32_t GetHeight() const = 0;
    [[nodiscard]] virtual Format GetFormat() const = 0;
};

} // namespace AnyPS5::Gpu::Rhi

#endif
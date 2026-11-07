#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_ITEXTURE_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_ITEXTURE_HPP

#include "RhiTypes.hpp"
#include <cstdint>

namespace AnyPS5::Gpu::Rhi {

struct TextureDesc {
    TextureType type = TextureType::Texture2D;
    Format format = Format::Undefined;
    std::uint32_t width = 1;
    std::uint32_t height = 1;
    std::uint32_t depth = 1;
    std::uint32_t mipLevels = 1;
    std::uint32_t arrayLayers = 1;
    std::uint32_t sampleCount = 1;
    TextureUsage usage = TextureUsage::None;
};

class ITexture {
public:
    virtual ~ITexture() = default;
    [[nodiscard]] virtual const TextureDesc& GetDesc() const = 0;
    [[nodiscard]] virtual Format GetFormat() const = 0;
    [[nodiscard]] virtual std::uint32_t GetWidth() const = 0;
    [[nodiscard]] virtual std::uint32_t GetHeight() const = 0;
    [[nodiscard]] virtual std::uint32_t GetDepth() const = 0;
    [[nodiscard]] virtual std::uint32_t GetMipLevels() const = 0;
    [[nodiscard]] virtual std::uint32_t GetArrayLayers() const = 0;
};

} // namespace AnyPS5::Gpu::Rhi

#endif
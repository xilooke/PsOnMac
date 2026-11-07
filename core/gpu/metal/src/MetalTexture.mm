#include "gpu/metal/MetalTexture.hpp"
#include "gpu/metal/MetalFormatMap.hpp"
#import <Metal/Metal.h>
#include <stdexcept>

namespace AnyPS5::Gpu::Metal {

MetalTexture::MetalTexture(void* mtlDevice, const Rhi::TextureDesc& description)
    : desc(description) {
    id<MTLDevice> device = (__bridge id<MTLDevice>)mtlDevice;
    if (!device) {
        throw std::runtime_error("Invalid MTLDevice for MetalTexture creation");
    }

    MTLTextureDescriptor* descriptor = [MTLTextureDescriptor new];
    descriptor.width = desc.width;
    descriptor.height = desc.height;
    descriptor.depth = desc.depth;
    descriptor.mipmapLevelCount = desc.mipLevels;
    descriptor.arrayLength = desc.arrayLayers;
    descriptor.pixelFormat = ToMetalPixelFormat(desc.format);

    switch (desc.type) {
    case Rhi::TextureType::Texture1D:
        descriptor.textureType = MTLTextureType1D;
        break;
    case Rhi::TextureType::Texture2D:
        descriptor.textureType = desc.sampleCount > 1 ? MTLTextureType2DMultisample : MTLTextureType2D;
        break;
    case Rhi::TextureType::Texture3D:
        descriptor.textureType = MTLTextureType3D;
        break;
    case Rhi::TextureType::TextureCube:
        descriptor.textureType = MTLTextureTypeCube;
        break;
    case Rhi::TextureType::Texture1DArray:
        descriptor.textureType = MTLTextureType1DArray;
        break;
    case Rhi::TextureType::Texture2DArray:
        descriptor.textureType = desc.sampleCount > 1 ? MTLTextureType2DMultisampleArray : MTLTextureType2DArray;
        break;
    case Rhi::TextureType::TextureCubeArray:
        descriptor.textureType = MTLTextureTypeCubeArray;
        break;
    }

    if (desc.sampleCount > 1) {
        descriptor.sampleCount = desc.sampleCount;
    }

    MTLTextureUsage usage = MTLTextureUsageShaderRead;
    if ((desc.usage & Rhi::TextureUsage::Storage) == Rhi::TextureUsage::Storage) {
        usage |= MTLTextureUsageShaderWrite;
    }
    if ((desc.usage & Rhi::TextureUsage::RenderTarget) == Rhi::TextureUsage::RenderTarget ||
        (desc.usage & Rhi::TextureUsage::DepthStencil) == Rhi::TextureUsage::DepthStencil) {
        usage |= MTLTextureUsageRenderTarget;
    }
    descriptor.usage = usage;
    descriptor.storageMode = MTLStorageModeShared;

    id<MTLTexture> tex = [device newTextureWithDescriptor:descriptor];
    if (!tex) {
        throw std::runtime_error("Failed to allocate MTLTexture");
    }

    nativeTexture = (__bridge_retained void*)tex;
}

MetalTexture::MetalTexture(void* existingTexture) {
    id<MTLTexture> tex = (__bridge id<MTLTexture>)existingTexture;
    if (tex) {
        desc.width = static_cast<std::uint32_t>(tex.width);
        desc.height = static_cast<std::uint32_t>(tex.height);
        desc.depth = static_cast<std::uint32_t>(tex.depth);
        desc.mipLevels = static_cast<std::uint32_t>(tex.mipmapLevelCount);
        desc.arrayLayers = static_cast<std::uint32_t>(tex.arrayLength);
        desc.format = FromMetalPixelFormat(tex.pixelFormat);
        nativeTexture = (__bridge_retained void*)tex;
    }
}

MetalTexture::~MetalTexture() {
    if (nativeTexture) {
        id<MTLTexture> tex = (__bridge_transfer id<MTLTexture>)nativeTexture;
        tex = nil;
        nativeTexture = nullptr;
    }
}

} // namespace AnyPS5::Gpu::Metal

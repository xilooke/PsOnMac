#include "gpu/metal/MetalFormatMap.hpp"
#import <Metal/Metal.h>

namespace AnyPS5::Gpu::Metal {

MTLPixelFormat ToMetalPixelFormat(Rhi::Format format) {
    switch (format) {
    case Rhi::Format::R8_UNORM:
        return MTLPixelFormatR8Unorm;
    case Rhi::Format::R8_SNORM:
        return MTLPixelFormatR8Snorm;
    case Rhi::Format::R8_UINT:
        return MTLPixelFormatR8Uint;
    case Rhi::Format::R8_SINT:
        return MTLPixelFormatR8Sint;
    case Rhi::Format::R8G8_UNORM:
        return MTLPixelFormatRG8Unorm;
    case Rhi::Format::R8G8_SNORM:
        return MTLPixelFormatRG8Snorm;
    case Rhi::Format::R8G8_UINT:
        return MTLPixelFormatRG8Uint;
    case Rhi::Format::R8G8_SINT:
        return MTLPixelFormatRG8Sint;
    case Rhi::Format::R8G8B8A8_UNORM:
        return MTLPixelFormatRGBA8Unorm;
    case Rhi::Format::R8G8B8A8_SRGB:
        return MTLPixelFormatRGBA8Unorm_sRGB;
    case Rhi::Format::R8G8B8A8_SNORM:
        return MTLPixelFormatRGBA8Snorm;
    case Rhi::Format::R8G8B8A8_UINT:
        return MTLPixelFormatRGBA8Uint;
    case Rhi::Format::R8G8B8A8_SINT:
        return MTLPixelFormatRGBA8Sint;
    case Rhi::Format::B8G8R8A8_UNORM:
        return MTLPixelFormatBGRA8Unorm;
    case Rhi::Format::B8G8R8A8_SRGB:
        return MTLPixelFormatBGRA8Unorm_sRGB;
    case Rhi::Format::R10G10B10A2_UNORM:
        return MTLPixelFormatRGB10A2Unorm;
    case Rhi::Format::R10G10B10A2_UINT:
        return MTLPixelFormatRGB10A2Uint;
    case Rhi::Format::R11G11B10_FLOAT:
        return MTLPixelFormatRG11B10Float;
    case Rhi::Format::R16_FLOAT:
        return MTLPixelFormatR16Float;
    case Rhi::Format::R16_UNORM:
        return MTLPixelFormatR16Unorm;
    case Rhi::Format::R16_UINT:
        return MTLPixelFormatR16Uint;
    case Rhi::Format::R16_SINT:
        return MTLPixelFormatR16Sint;
    case Rhi::Format::R16G16_FLOAT:
        return MTLPixelFormatRG16Float;
    case Rhi::Format::R16G16_UNORM:
        return MTLPixelFormatRG16Unorm;
    case Rhi::Format::R16G16_UINT:
        return MTLPixelFormatRG16Uint;
    case Rhi::Format::R16G16_SINT:
        return MTLPixelFormatRG16Sint;
    case Rhi::Format::R16G16B16A16_FLOAT:
        return MTLPixelFormatRGBA16Float;
    case Rhi::Format::R16G16B16A16_UNORM:
        return MTLPixelFormatRGBA16Unorm;
    case Rhi::Format::R16G16B16A16_UINT:
        return MTLPixelFormatRGBA16Uint;
    case Rhi::Format::R16G16B16A16_SINT:
        return MTLPixelFormatRGBA16Sint;
    case Rhi::Format::R32_FLOAT:
        return MTLPixelFormatR32Float;
    case Rhi::Format::R32_UINT:
        return MTLPixelFormatR32Uint;
    case Rhi::Format::R32_SINT:
        return MTLPixelFormatR32Sint;
    case Rhi::Format::R32G32_FLOAT:
        return MTLPixelFormatRG32Float;
    case Rhi::Format::R32G32_UINT:
        return MTLPixelFormatRG32Uint;
    case Rhi::Format::R32G32_SINT:
        return MTLPixelFormatRG32Sint;
    case Rhi::Format::R32G32B32A32_FLOAT:
        return MTLPixelFormatRGBA32Float;
    case Rhi::Format::R32G32B32A32_UINT:
        return MTLPixelFormatRGBA32Uint;
    case Rhi::Format::R32G32B32A32_SINT:
        return MTLPixelFormatRGBA32Sint;
    case Rhi::Format::D16_UNORM:
        return MTLPixelFormatDepth16Unorm;
    case Rhi::Format::D32_FLOAT:
        return MTLPixelFormatDepth32Float;
    case Rhi::Format::D32_FLOAT_S8_UINT:
    case Rhi::Format::D24_UNORM_S8_UINT:
        return MTLPixelFormatDepth32Float_Stencil8;
#if TARGET_OS_OSX
    case Rhi::Format::BC1_RGBA_UNORM:
        return MTLPixelFormatBC1_RGBA;
    case Rhi::Format::BC1_RGBA_SRGB:
        return MTLPixelFormatBC1_RGBA_sRGB;
    case Rhi::Format::BC2_RGBA_UNORM:
        return MTLPixelFormatBC2_RGBA;
    case Rhi::Format::BC2_RGBA_SRGB:
        return MTLPixelFormatBC2_RGBA_sRGB;
    case Rhi::Format::BC3_RGBA_UNORM:
        return MTLPixelFormatBC3_RGBA;
    case Rhi::Format::BC3_RGBA_SRGB:
        return MTLPixelFormatBC3_RGBA_sRGB;
    case Rhi::Format::BC4_UNORM:
        return MTLPixelFormatBC4_RUnorm;
    case Rhi::Format::BC4_SNORM:
        return MTLPixelFormatBC4_RSnorm;
    case Rhi::Format::BC5_UNORM:
        return MTLPixelFormatBC5_RGUnorm;
    case Rhi::Format::BC5_SNORM:
        return MTLPixelFormatBC5_RGSnorm;
    case Rhi::Format::BC6H_UFLOAT:
        return MTLPixelFormatBC6H_RGBUfloat;
    case Rhi::Format::BC6H_SFLOAT:
        return MTLPixelFormatBC6H_RGBFloat;
    case Rhi::Format::BC7_RGBA_UNORM:
        return MTLPixelFormatBC7_RGBAUnorm;
    case Rhi::Format::BC7_RGBA_SRGB:
        return MTLPixelFormatBC7_RGBAUnorm_sRGB;
#endif
    default:
        return MTLPixelFormatInvalid;
    }
}

Rhi::Format FromMetalPixelFormat(MTLPixelFormat format) {
    switch (format) {
    case MTLPixelFormatR8Unorm:
        return Rhi::Format::R8_UNORM;
    case MTLPixelFormatRGBA8Unorm:
        return Rhi::Format::R8G8B8A8_UNORM;
    case MTLPixelFormatRGBA8Unorm_sRGB:
        return Rhi::Format::R8G8B8A8_SRGB;
    case MTLPixelFormatBGRA8Unorm:
        return Rhi::Format::B8G8R8A8_UNORM;
    case MTLPixelFormatBGRA8Unorm_sRGB:
        return Rhi::Format::B8G8R8A8_SRGB;
    case MTLPixelFormatRGBA16Float:
        return Rhi::Format::R16G16B16A16_FLOAT;
    case MTLPixelFormatRGBA32Float:
        return Rhi::Format::R32G32B32A32_FLOAT;
    case MTLPixelFormatDepth32Float:
        return Rhi::Format::D32_FLOAT;
    case MTLPixelFormatDepth32Float_Stencil8:
        return Rhi::Format::D32_FLOAT_S8_UINT;
    default:
        return Rhi::Format::Undefined;
    }
}

} // namespace AnyPS5::Gpu::Metal

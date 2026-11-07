#include "gpu/metal/MetalSampler.hpp"
#import <Metal/Metal.h>
#include <stdexcept>

namespace AnyPS5::Gpu::Metal {

namespace {

MTLSamplerMinMagFilter ToMetalFilter(Rhi::Filter f) {
    return f == Rhi::Filter::Linear ? MTLSamplerMinMagFilterLinear : MTLSamplerMinMagFilterNearest;
}

MTLSamplerMipFilter ToMetalMipFilter(Rhi::Filter f) {
    return f == Rhi::Filter::Linear ? MTLSamplerMipFilterLinear : MTLSamplerMipFilterNearest;
}

MTLSamplerAddressMode ToMetalAddress(Rhi::SamplerAddressMode mode) {
    switch (mode) {
    case Rhi::SamplerAddressMode::Repeat:
        return MTLSamplerAddressModeRepeat;
    case Rhi::SamplerAddressMode::MirroredRepeat:
        return MTLSamplerAddressModeMirrorRepeat;
    case Rhi::SamplerAddressMode::ClampToEdge:
        return MTLSamplerAddressModeClampToEdge;
    case Rhi::SamplerAddressMode::ClampToBorder:
        return MTLSamplerAddressModeClampToBorderColor;
    case Rhi::SamplerAddressMode::MirrorClampToEdge:
        return MTLSamplerAddressModeMirrorClampToEdge;
    }
}

MTLCompareFunction ToMetalCompare(Rhi::CompareOp op) {
    switch (op) {
    case Rhi::CompareOp::Never:
        return MTLCompareFunctionNever;
    case Rhi::CompareOp::Less:
        return MTLCompareFunctionLess;
    case Rhi::CompareOp::Equal:
        return MTLCompareFunctionEqual;
    case Rhi::CompareOp::LessOrEqual:
        return MTLCompareFunctionLessEqual;
    case Rhi::CompareOp::Greater:
        return MTLCompareFunctionGreater;
    case Rhi::CompareOp::NotEqual:
        return MTLCompareFunctionNotEqual;
    case Rhi::CompareOp::GreaterOrEqual:
        return MTLCompareFunctionGreaterEqual;
    case Rhi::CompareOp::Always:
        return MTLCompareFunctionAlways;
    }
}

} // namespace

MetalSampler::MetalSampler(void* mtlDevice, const Rhi::SamplerDesc& description)
    : desc(description) {
    id<MTLDevice> device = (__bridge id<MTLDevice>)mtlDevice;
    if (!device) {
        throw std::runtime_error("Invalid MTLDevice for MetalSampler creation");
    }

    MTLSamplerDescriptor* descriptor = [MTLSamplerDescriptor new];
    descriptor.minFilter = ToMetalFilter(desc.minFilter);
    descriptor.magFilter = ToMetalFilter(desc.magFilter);
    descriptor.mipFilter = ToMetalMipFilter(desc.mipmapMode);
    descriptor.sAddressMode = ToMetalAddress(desc.addressU);
    descriptor.tAddressMode = ToMetalAddress(desc.addressV);
    descriptor.rAddressMode = ToMetalAddress(desc.addressW);
    descriptor.maxAnisotropy = desc.anisotropyEnable ? static_cast<NSUInteger>(desc.maxAnisotropy) : 1;
    descriptor.compareFunction = desc.compareEnable ? ToMetalCompare(desc.compareOp) : MTLCompareFunctionNever;
    descriptor.lodMinClamp = desc.minLod;
    descriptor.lodMaxClamp = desc.maxLod;

    id<MTLSamplerState> sampler = [device newSamplerStateWithDescriptor:descriptor];
    if (!sampler) {
        throw std::runtime_error("Failed to allocate MTLSamplerState");
    }

    nativeSampler = (__bridge_retained void*)sampler;
}

MetalSampler::~MetalSampler() {
    if (nativeSampler) {
        id<MTLSamplerState> sampler = (__bridge_transfer id<MTLSamplerState>)nativeSampler;
        sampler = nil;
        nativeSampler = nullptr;
    }
}

} // namespace AnyPS5::Gpu::Metal

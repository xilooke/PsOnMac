#include "gpu/metal/MetalPipeline.hpp"
#include "gpu/metal/MetalFormatMap.hpp"
#include "gpu/metal/MetalShaderModule.hpp"
#import <Metal/Metal.h>
#include <stdexcept>

namespace AnyPS5::Gpu::Metal {

namespace {

MTLBlendFactor ToMetalBlendFactor(Rhi::BlendFactor factor) {
    switch (factor) {
    case Rhi::BlendFactor::Zero:
        return MTLBlendFactorZero;
    case Rhi::BlendFactor::One:
        return MTLBlendFactorOne;
    case Rhi::BlendFactor::SrcColor:
        return MTLBlendFactorSourceColor;
    case Rhi::BlendFactor::OneMinusSrcColor:
        return MTLBlendFactorOneMinusSourceColor;
    case Rhi::BlendFactor::DstColor:
        return MTLBlendFactorDestinationColor;
    case Rhi::BlendFactor::OneMinusDstColor:
        return MTLBlendFactorOneMinusDestinationColor;
    case Rhi::BlendFactor::SrcAlpha:
        return MTLBlendFactorSourceAlpha;
    case Rhi::BlendFactor::OneMinusSrcAlpha:
        return MTLBlendFactorOneMinusSourceAlpha;
    case Rhi::BlendFactor::DstAlpha:
        return MTLBlendFactorDestinationAlpha;
    case Rhi::BlendFactor::OneMinusDstAlpha:
        return MTLBlendFactorOneMinusDestinationAlpha;
    case Rhi::BlendFactor::ConstantColor:
        return MTLBlendFactorBlendColor;
    case Rhi::BlendFactor::OneMinusConstantColor:
        return MTLBlendFactorOneMinusBlendColor;
    case Rhi::BlendFactor::ConstantAlpha:
        return MTLBlendFactorBlendAlpha;
    case Rhi::BlendFactor::OneMinusConstantAlpha:
        return MTLBlendFactorOneMinusBlendAlpha;
    case Rhi::BlendFactor::SrcAlphaSaturate:
        return MTLBlendFactorSourceAlphaSaturated;
    case Rhi::BlendFactor::Src1Color:
        return MTLBlendFactorSource1Color;
    case Rhi::BlendFactor::OneMinusSrc1Color:
        return MTLBlendFactorOneMinusSource1Color;
    case Rhi::BlendFactor::Src1Alpha:
        return MTLBlendFactorSource1Alpha;
    case Rhi::BlendFactor::OneMinusSrc1Alpha:
        return MTLBlendFactorOneMinusSource1Alpha;
    }
}

MTLBlendOperation ToMetalBlendOp(Rhi::BlendOp op) {
    switch (op) {
    case Rhi::BlendOp::Add:
        return MTLBlendOperationAdd;
    case Rhi::BlendOp::Subtract:
        return MTLBlendOperationSubtract;
    case Rhi::BlendOp::ReverseSubtract:
        return MTLBlendOperationReverseSubtract;
    case Rhi::BlendOp::Min:
        return MTLBlendOperationMin;
    case Rhi::BlendOp::Max:
        return MTLBlendOperationMax;
    }
}

MTLCompareFunction ToMetalCompareOp(Rhi::CompareOp op) {
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

MetalPipeline::MetalPipeline(void* mtlDevice, const Rhi::GraphicsPipelineDesc& desc)
    : isCompute(false), topology(desc.topology), rasterizer(desc.rasterizer) {
    id<MTLDevice> device = (__bridge id<MTLDevice>)mtlDevice;
    if (!device) {
        throw std::runtime_error("Invalid MTLDevice for MetalPipeline");
    }

    auto* vertModule = dynamic_cast<MetalShaderModule*>(desc.vertexShader.get());
    if (!vertModule || !vertModule->GetNativeFunction()) {
        throw std::runtime_error("Invalid vertex shader for MetalPipeline");
    }

    MTLRenderPipelineDescriptor* pipelineDesc = [MTLRenderPipelineDescriptor new];
    pipelineDesc.vertexFunction = (__bridge id<MTLFunction>)vertModule->GetNativeFunction();

    if (desc.fragmentShader) {
        auto* fragModule = dynamic_cast<MetalShaderModule*>(desc.fragmentShader.get());
        if (fragModule && fragModule->GetNativeFunction()) {
            pipelineDesc.fragmentFunction = (__bridge id<MTLFunction>)fragModule->GetNativeFunction();
        }
    }

    for (std::size_t i = 0; i < desc.colorAttachmentFormats.size(); ++i) {
        MTLPixelFormat pixelFormat = ToMetalPixelFormat(desc.colorAttachmentFormats[i]);
        pipelineDesc.colorAttachments[i].pixelFormat = pixelFormat;

        if (i < desc.blendAttachments.size()) {
            const auto& blend = desc.blendAttachments[i];
            pipelineDesc.colorAttachments[i].blendingEnabled = blend.blendEnable ? YES : NO;
            if (blend.blendEnable) {
                pipelineDesc.colorAttachments[i].sourceRGBBlendFactor = ToMetalBlendFactor(blend.srcColorBlendFactor);
                pipelineDesc.colorAttachments[i].destinationRGBBlendFactor = ToMetalBlendFactor(blend.dstColorBlendFactor);
                pipelineDesc.colorAttachments[i].rgbBlendOperation = ToMetalBlendOp(blend.colorBlendOp);
                pipelineDesc.colorAttachments[i].sourceAlphaBlendFactor = ToMetalBlendFactor(blend.srcAlphaBlendFactor);
                pipelineDesc.colorAttachments[i].destinationAlphaBlendFactor = ToMetalBlendFactor(blend.dstAlphaBlendFactor);
                pipelineDesc.colorAttachments[i].alphaBlendOperation = ToMetalBlendOp(blend.alphaBlendOp);
            }
            MTLColorWriteMask mask = MTLColorWriteMaskNone;
            if (blend.colorWriteMask & 0x1) mask |= MTLColorWriteMaskRed;
            if (blend.colorWriteMask & 0x2) mask |= MTLColorWriteMaskGreen;
            if (blend.colorWriteMask & 0x4) mask |= MTLColorWriteMaskBlue;
            if (blend.colorWriteMask & 0x8) mask |= MTLColorWriteMaskAlpha;
            pipelineDesc.colorAttachments[i].writeMask = mask;
        }
    }

    if (desc.depthAttachmentFormat != Rhi::Format::Undefined) {
        MTLPixelFormat depthFmt = ToMetalPixelFormat(desc.depthAttachmentFormat);
        pipelineDesc.depthAttachmentPixelFormat = depthFmt;
        if (depthFmt == MTLPixelFormatDepth32Float_Stencil8) {
            pipelineDesc.stencilAttachmentPixelFormat = depthFmt;
        }
    }

    pipelineDesc.rasterSampleCount = desc.sampleCount > 0 ? desc.sampleCount : 1;

    NSError* error = nil;
    id<MTLRenderPipelineState> renderState = [device newRenderPipelineStateWithDescriptor:pipelineDesc error:&error];
    if (!renderState || error) {
        std::string errMsg = error ? [[error localizedDescription] UTF8String] : "Unknown pipeline compilation error";
        throw std::runtime_error("Failed to create MTLRenderPipelineState: " + errMsg);
    }
    nativeRenderState = (__bridge_retained void*)renderState;

    MTLDepthStencilDescriptor* depthDesc = [MTLDepthStencilDescriptor new];
    depthDesc.depthWriteEnabled = desc.depthStencil.depthWriteEnable ? YES : NO;
    depthDesc.depthCompareFunction = desc.depthStencil.depthTestEnable
                                         ? ToMetalCompareOp(desc.depthStencil.depthCompareOp)
                                         : MTLCompareFunctionAlways;

    id<MTLDepthStencilState> depthState = [device newDepthStencilStateWithDescriptor:depthDesc];
    if (depthState) {
        nativeDepthStencilState = (__bridge_retained void*)depthState;
    }
}

MetalPipeline::MetalPipeline(void* mtlDevice, const Rhi::ComputePipelineDesc& desc)
    : isCompute(true) {
    id<MTLDevice> device = (__bridge id<MTLDevice>)mtlDevice;
    if (!device) {
        throw std::runtime_error("Invalid MTLDevice for MetalPipeline");
    }

    auto* compModule = dynamic_cast<MetalShaderModule*>(desc.computeShader.get());
    if (!compModule || !compModule->GetNativeFunction()) {
        throw std::runtime_error("Invalid compute shader for MetalPipeline");
    }

    id<MTLFunction> function = (__bridge id<MTLFunction>)compModule->GetNativeFunction();
    NSError* error = nil;
    id<MTLComputePipelineState> computeState = [device newComputePipelineStateWithFunction:function error:&error];
    if (!computeState || error) {
        std::string errMsg = error ? [[error localizedDescription] UTF8String] : "Unknown compute pipeline error";
        throw std::runtime_error("Failed to create MTLComputePipelineState: " + errMsg);
    }
    nativeComputeState = (__bridge_retained void*)computeState;
}

MetalPipeline::~MetalPipeline() {
    if (nativeRenderState) {
        id<MTLRenderPipelineState> state = (__bridge_transfer id<MTLRenderPipelineState>)nativeRenderState;
        state = nil;
        nativeRenderState = nullptr;
    }
    if (nativeComputeState) {
        id<MTLComputePipelineState> state = (__bridge_transfer id<MTLComputePipelineState>)nativeComputeState;
        state = nil;
        nativeComputeState = nullptr;
    }
    if (nativeDepthStencilState) {
        id<MTLDepthStencilState> state = (__bridge_transfer id<MTLDepthStencilState>)nativeDepthStencilState;
        state = nil;
        nativeDepthStencilState = nullptr;
    }
}

} // namespace AnyPS5::Gpu::Metal

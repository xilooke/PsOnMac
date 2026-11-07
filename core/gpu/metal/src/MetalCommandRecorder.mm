#include "gpu/metal/MetalCommandRecorder.hpp"
#include "gpu/metal/MetalBuffer.hpp"
#include "gpu/metal/MetalDescriptorTable.hpp"
#include "gpu/metal/MetalFormatMap.hpp"
#include "gpu/metal/MetalPipeline.hpp"
#include "gpu/metal/MetalSampler.hpp"
#include "gpu/metal/MetalTexture.hpp"
#import <Metal/Metal.h>
#include <stdexcept>

namespace AnyPS5::Gpu::Metal {

namespace {

MTLPrimitiveType ToMetalPrimitiveType(Rhi::PrimitiveTopology topology) {
    switch (topology) {
    case Rhi::PrimitiveTopology::PointList:
        return MTLPrimitiveTypePoint;
    case Rhi::PrimitiveTopology::LineList:
        return MTLPrimitiveTypeLine;
    case Rhi::PrimitiveTopology::LineStrip:
        return MTLPrimitiveTypeLineStrip;
    case Rhi::PrimitiveTopology::TriangleList:
        return MTLPrimitiveTypeTriangle;
    case Rhi::PrimitiveTopology::TriangleStrip:
        return MTLPrimitiveTypeTriangleStrip;
    case Rhi::PrimitiveTopology::TriangleFan:
    case Rhi::PrimitiveTopology::PatchList:
        throw std::runtime_error("Unsupported primitive topology for Metal draw command");
    }
}

MTLCullMode ToMetalCullMode(Rhi::CullMode mode) {
    switch (mode) {
    case Rhi::CullMode::None:
        return MTLCullModeNone;
    case Rhi::CullMode::Front:
        return MTLCullModeFront;
    case Rhi::CullMode::Back:
        return MTLCullModeBack;
    case Rhi::CullMode::FrontAndBack:
        throw std::runtime_error("FrontAndBack cull mode is unsupported on Metal");
    }
}

MTLWinding ToMetalFrontFace(Rhi::FrontFace face) {
    switch (face) {
    case Rhi::FrontFace::CounterClockwise:
        return MTLWindingCounterClockwise;
    case Rhi::FrontFace::Clockwise:
        return MTLWindingClockwise;
    }
}

} // namespace

MetalCommandRecorder::MetalCommandRecorder(void* mtlDevice, void* mtlQueue, Rhi::QueueType qType)
    : nativeDevice(mtlDevice), nativeQueue(mtlQueue), queueType(qType) {
    if (!nativeDevice || !nativeQueue) {
        throw std::runtime_error("Invalid device or queue for MetalCommandRecorder");
    }
}

MetalCommandRecorder::~MetalCommandRecorder() {
    EndActiveEncoders();
    if (nativeCommandBuffer) {
        id<MTLCommandBuffer> cb = (__bridge_transfer id<MTLCommandBuffer>)nativeCommandBuffer;
        cb = nil;
        nativeCommandBuffer = nullptr;
    }
}

void MetalCommandRecorder::EndActiveEncoders() {
    if (activeRenderEncoder) {
        id<MTLRenderCommandEncoder> enc = (__bridge_transfer id<MTLRenderCommandEncoder>)activeRenderEncoder;
        [enc endEncoding];
        activeRenderEncoder = nullptr;
    }
    if (activeComputeEncoder) {
        id<MTLComputeCommandEncoder> enc = (__bridge_transfer id<MTLComputeCommandEncoder>)activeComputeEncoder;
        [enc endEncoding];
        activeComputeEncoder = nullptr;
    }
    if (activeBlitEncoder) {
        id<MTLBlitCommandEncoder> enc = (__bridge_transfer id<MTLBlitCommandEncoder>)activeBlitEncoder;
        [enc endEncoding];
        activeBlitEncoder = nullptr;
    }
}

void MetalCommandRecorder::Begin() {
    EndActiveEncoders();
    if (nativeCommandBuffer) {
        id<MTLCommandBuffer> cb = (__bridge_transfer id<MTLCommandBuffer>)nativeCommandBuffer;
        cb = nil;
        nativeCommandBuffer = nullptr;
    }
    id<MTLCommandQueue> queue = (__bridge id<MTLCommandQueue>)nativeQueue;
    id<MTLCommandBuffer> cb = [queue commandBuffer];
    if (!cb) {
        throw std::runtime_error("Failed to allocate MTLCommandBuffer");
    }
    nativeCommandBuffer = (__bridge_retained void*)cb;
}

void MetalCommandRecorder::End() {
    EndActiveEncoders();
}

void MetalCommandRecorder::BeginRenderPass(const Rhi::RenderPassDesc& pass) {
    EndActiveEncoders();
    if (!nativeCommandBuffer) {
        Begin();
    }

    MTLRenderPassDescriptor* passDesc = [MTLRenderPassDescriptor new];
    for (std::size_t i = 0; i < pass.colorAttachments.size(); ++i) {
        const auto& ca = pass.colorAttachments[i];
        if (!ca.texture) {
            continue;
        }
        auto* mtlTex = dynamic_cast<MetalTexture*>(ca.texture.get());
        if (!mtlTex || !mtlTex->GetNativeTexture()) {
            continue;
        }

        passDesc.colorAttachments[i].texture = (__bridge id<MTLTexture>)mtlTex->GetNativeTexture();
        passDesc.colorAttachments[i].loadAction = ca.clear ? MTLLoadActionClear : MTLLoadActionLoad;
        passDesc.colorAttachments[i].storeAction = ca.store ? MTLStoreActionStore : MTLStoreActionDontCare;
        passDesc.colorAttachments[i].clearColor = MTLClearColorMake(ca.clearColor.r, ca.clearColor.g, ca.clearColor.b, ca.clearColor.a);
    }

    if (pass.depthStencilAttachment.texture) {
        auto* mtlTex = dynamic_cast<MetalTexture*>(pass.depthStencilAttachment.texture.get());
        if (mtlTex && mtlTex->GetNativeTexture()) {
            id<MTLTexture> dTex = (__bridge id<MTLTexture>)mtlTex->GetNativeTexture();
            passDesc.depthAttachment.texture = dTex;
            passDesc.depthAttachment.loadAction = pass.depthStencilAttachment.clearDepth ? MTLLoadActionClear : MTLLoadActionLoad;
            passDesc.depthAttachment.storeAction = pass.depthStencilAttachment.storeDepth ? MTLStoreActionStore : MTLStoreActionDontCare;
            passDesc.depthAttachment.clearDepth = pass.depthStencilAttachment.clearValue.depth;

            if (dTex.pixelFormat == MTLPixelFormatDepth32Float_Stencil8) {
                passDesc.stencilAttachment.texture = dTex;
                passDesc.stencilAttachment.loadAction = pass.depthStencilAttachment.clearStencil ? MTLLoadActionClear : MTLLoadActionLoad;
                passDesc.stencilAttachment.storeAction = pass.depthStencilAttachment.storeStencil ? MTLStoreActionStore : MTLStoreActionDontCare;
                passDesc.stencilAttachment.clearStencil = pass.depthStencilAttachment.clearValue.stencil;
            }
        }
    }

    id<MTLCommandBuffer> cb = (__bridge id<MTLCommandBuffer>)nativeCommandBuffer;
    id<MTLRenderCommandEncoder> enc = [cb renderCommandEncoderWithDescriptor:passDesc];
    if (!enc) {
        throw std::runtime_error("Failed to create MTLRenderCommandEncoder");
    }
    activeRenderEncoder = (__bridge_retained void*)enc;

    if (activePipeline && !activePipeline->IsCompute()) {
        SetPipeline(activePipeline);
    }
}

void MetalCommandRecorder::EndRenderPass() {
    if (activeRenderEncoder) {
        id<MTLRenderCommandEncoder> enc = (__bridge_transfer id<MTLRenderCommandEncoder>)activeRenderEncoder;
        [enc endEncoding];
        activeRenderEncoder = nullptr;
    }
}

void MetalCommandRecorder::SetPipeline(const std::shared_ptr<Rhi::IPipeline>& pipeline) {
    activePipeline = pipeline;
    auto* mtlPipe = dynamic_cast<MetalPipeline*>(pipeline.get());
    if (!mtlPipe) {
        return;
    }

    if (mtlPipe->IsCompute()) {
        if (activeComputeEncoder && mtlPipe->GetComputePipelineState()) {
            id<MTLComputeCommandEncoder> enc = (__bridge id<MTLComputeCommandEncoder>)activeComputeEncoder;
            [enc setComputePipelineState:(__bridge id<MTLComputePipelineState>)mtlPipe->GetComputePipelineState()];
        }
    } else {
        if (activeRenderEncoder && mtlPipe->GetRenderPipelineState()) {
            id<MTLRenderCommandEncoder> enc = (__bridge id<MTLRenderCommandEncoder>)activeRenderEncoder;
            [enc setRenderPipelineState:(__bridge id<MTLRenderPipelineState>)mtlPipe->GetRenderPipelineState()];
            if (mtlPipe->GetDepthStencilState()) {
                [enc setDepthStencilState:(__bridge id<MTLDepthStencilState>)mtlPipe->GetDepthStencilState()];
            }
            [enc setCullMode:ToMetalCullMode(mtlPipe->GetRasterizerState().cullMode)];
            [enc setFrontFacingWinding:ToMetalFrontFace(mtlPipe->GetRasterizerState().frontFace)];
        }
    }
}

void MetalCommandRecorder::SetViewport(const Rhi::Viewport& viewport) {
    if (activeRenderEncoder) {
        MTLViewport vp = {
            viewport.x,
            viewport.y,
            viewport.width,
            viewport.height,
            viewport.minDepth,
            viewport.maxDepth
        };
        id<MTLRenderCommandEncoder> enc = (__bridge id<MTLRenderCommandEncoder>)activeRenderEncoder;
        [enc setViewport:vp];
    }
}

void MetalCommandRecorder::SetScissor(const Rhi::Scissor& scissor) {
    if (activeRenderEncoder) {
        MTLScissorRect sc = {
            static_cast<NSUInteger>(scissor.x >= 0 ? scissor.x : 0),
            static_cast<NSUInteger>(scissor.y >= 0 ? scissor.y : 0),
            static_cast<NSUInteger>(scissor.width),
            static_cast<NSUInteger>(scissor.height)
        };
        id<MTLRenderCommandEncoder> enc = (__bridge id<MTLRenderCommandEncoder>)activeRenderEncoder;
        [enc setScissorRect:sc];
    }
}

void MetalCommandRecorder::SetVertexBuffer(std::uint32_t slot, const std::shared_ptr<Rhi::IBuffer>& buffer, std::size_t offset) {
    if (activeRenderEncoder && buffer) {
        auto* mtlBuf = dynamic_cast<MetalBuffer*>(buffer.get());
        if (mtlBuf && mtlBuf->GetNativeBuffer()) {
            id<MTLRenderCommandEncoder> enc = (__bridge id<MTLRenderCommandEncoder>)activeRenderEncoder;
            [enc setVertexBuffer:(__bridge id<MTLBuffer>)mtlBuf->GetNativeBuffer() offset:offset atIndex:slot];
        }
    }
}

void MetalCommandRecorder::SetIndexBuffer(const std::shared_ptr<Rhi::IBuffer>& buffer, std::size_t offset, bool is32Bit) {
    indexBuffer = buffer;
    indexBufferOffset = offset;
    is32BitIndex = is32Bit;
}

void MetalCommandRecorder::SetDescriptorTable(std::uint32_t slot, const std::shared_ptr<Rhi::IDescriptorTable>& table) {
    BindDescriptorTableToActiveEncoders(slot, table);
}

void MetalCommandRecorder::BindDescriptorTableToActiveEncoders(std::uint32_t slot, const std::shared_ptr<Rhi::IDescriptorTable>& table) {
    auto* mtlTable = dynamic_cast<MetalDescriptorTable*>(table.get());
    if (!mtlTable) {
        return;
    }

    if (activeRenderEncoder) {
        id<MTLRenderCommandEncoder> enc = (__bridge id<MTLRenderCommandEncoder>)activeRenderEncoder;
        for (const auto& [bSlot, binding] : mtlTable->GetBuffers()) {
            auto* b = dynamic_cast<MetalBuffer*>(binding.buffer.get());
            if (b && b->GetNativeBuffer()) {
                [enc setVertexBuffer:(__bridge id<MTLBuffer>)b->GetNativeBuffer() offset:binding.offset atIndex:slot + bSlot];
                [enc setFragmentBuffer:(__bridge id<MTLBuffer>)b->GetNativeBuffer() offset:binding.offset atIndex:slot + bSlot];
            }
        }
        for (const auto& [tSlot, tex] : mtlTable->GetTextures()) {
            auto* t = dynamic_cast<MetalTexture*>(tex.get());
            if (t && t->GetNativeTexture()) {
                [enc setVertexTexture:(__bridge id<MTLTexture>)t->GetNativeTexture() atIndex:slot + tSlot];
                [enc setFragmentTexture:(__bridge id<MTLTexture>)t->GetNativeTexture() atIndex:slot + tSlot];
            }
        }
        for (const auto& [sSlot, smp] : mtlTable->GetSamplers()) {
            auto* s = dynamic_cast<MetalSampler*>(smp.get());
            if (s && s->GetNativeSampler()) {
                [enc setVertexSamplerState:(__bridge id<MTLSamplerState>)s->GetNativeSampler() atIndex:slot + sSlot];
                [enc setFragmentSamplerState:(__bridge id<MTLSamplerState>)s->GetNativeSampler() atIndex:slot + sSlot];
            }
        }
    } else if (activeComputeEncoder) {
        id<MTLComputeCommandEncoder> enc = (__bridge id<MTLComputeCommandEncoder>)activeComputeEncoder;
        for (const auto& [bSlot, binding] : mtlTable->GetBuffers()) {
            auto* b = dynamic_cast<MetalBuffer*>(binding.buffer.get());
            if (b && b->GetNativeBuffer()) {
                [enc setBuffer:(__bridge id<MTLBuffer>)b->GetNativeBuffer() offset:binding.offset atIndex:slot + bSlot];
            }
        }
        for (const auto& [tSlot, tex] : mtlTable->GetTextures()) {
            auto* t = dynamic_cast<MetalTexture*>(tex.get());
            if (t && t->GetNativeTexture()) {
                [enc setTexture:(__bridge id<MTLTexture>)t->GetNativeTexture() atIndex:slot + tSlot];
            }
        }
        for (const auto& [sSlot, smp] : mtlTable->GetSamplers()) {
            auto* s = dynamic_cast<MetalSampler*>(smp.get());
            if (s && s->GetNativeSampler()) {
                [enc setSamplerState:(__bridge id<MTLSamplerState>)s->GetNativeSampler() atIndex:slot + sSlot];
            }
        }
    }
}

void MetalCommandRecorder::Draw(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t firstVertex, std::uint32_t firstInstance) {
    if (!activeRenderEncoder) {
        return;
    }
    id<MTLRenderCommandEncoder> enc = (__bridge id<MTLRenderCommandEncoder>)activeRenderEncoder;
    auto* mtlPipe = dynamic_cast<MetalPipeline*>(activePipeline.get());
    MTLPrimitiveType prim = mtlPipe ? ToMetalPrimitiveType(mtlPipe->GetTopology()) : MTLPrimitiveTypeTriangle;
    [enc drawPrimitives:prim vertexStart:firstVertex vertexCount:vertexCount instanceCount:instanceCount baseInstance:firstInstance];
}

void MetalCommandRecorder::DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t firstIndex, std::int32_t vertexOffset, std::uint32_t firstInstance) {
    if (!activeRenderEncoder || !indexBuffer) {
        return;
    }
    auto* mtlIdxBuf = dynamic_cast<MetalBuffer*>(indexBuffer.get());
    if (!mtlIdxBuf || !mtlIdxBuf->GetNativeBuffer()) {
        return;
    }

    id<MTLRenderCommandEncoder> enc = (__bridge id<MTLRenderCommandEncoder>)activeRenderEncoder;
    auto* mtlPipe = dynamic_cast<MetalPipeline*>(activePipeline.get());
    MTLPrimitiveType prim = mtlPipe ? ToMetalPrimitiveType(mtlPipe->GetTopology()) : MTLPrimitiveTypeTriangle;
    MTLIndexType idxType = is32BitIndex ? MTLIndexTypeUInt32 : MTLIndexTypeUInt16;
    std::size_t idxSize = is32BitIndex ? sizeof(std::uint32_t) : sizeof(std::uint16_t);

    [enc drawIndexedPrimitives:prim
                    indexCount:indexCount
                     indexType:idxType
                   indexBuffer:(__bridge id<MTLBuffer>)mtlIdxBuf->GetNativeBuffer()
             indexBufferOffset:indexBufferOffset + (firstIndex * idxSize)
                 instanceCount:instanceCount
                    baseVertex:vertexOffset
                  baseInstance:firstInstance];
}

void MetalCommandRecorder::DrawIndirect(const std::shared_ptr<Rhi::IBuffer>& argumentBuffer, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) {
    if (!activeRenderEncoder || !argumentBuffer) {
        return;
    }
    auto* mtlBuf = dynamic_cast<MetalBuffer*>(argumentBuffer.get());
    if (!mtlBuf || !mtlBuf->GetNativeBuffer()) {
        return;
    }

    id<MTLRenderCommandEncoder> enc = (__bridge id<MTLRenderCommandEncoder>)activeRenderEncoder;
    auto* mtlPipe = dynamic_cast<MetalPipeline*>(activePipeline.get());
    MTLPrimitiveType prim = mtlPipe ? ToMetalPrimitiveType(mtlPipe->GetTopology()) : MTLPrimitiveTypeTriangle;

    for (std::uint32_t i = 0; i < drawCount; ++i) {
        [enc drawPrimitives:prim
             indirectBuffer:(__bridge id<MTLBuffer>)mtlBuf->GetNativeBuffer()
       indirectBufferOffset:offset + (i * stride)];
    }
}

void MetalCommandRecorder::Dispatch(std::uint32_t groupCountX, std::uint32_t groupCountY, std::uint32_t groupCountZ) {
    if (activeRenderEncoder || activeBlitEncoder) {
        EndActiveEncoders();
    }

    if (!activeComputeEncoder) {
        if (!nativeCommandBuffer) {
            Begin();
        }
        id<MTLCommandBuffer> cb = (__bridge id<MTLCommandBuffer>)nativeCommandBuffer;
        id<MTLComputeCommandEncoder> enc = [cb computeCommandEncoder];
        if (!enc) {
            throw std::runtime_error("Failed to allocate MTLComputeCommandEncoder");
        }
        activeComputeEncoder = (__bridge_retained void*)enc;
        if (activePipeline && activePipeline->IsCompute()) {
            SetPipeline(activePipeline);
        }
    }

    id<MTLComputeCommandEncoder> enc = (__bridge id<MTLComputeCommandEncoder>)activeComputeEncoder;
    MTLSize threadgroups = MTLSizeMake(groupCountX, groupCountY, groupCountZ);
    MTLSize threadsPerGroup = MTLSizeMake(32, 1, 1);
    [enc dispatchThreadgroups:threadgroups threadsPerThreadgroup:threadsPerGroup];
}

void MetalCommandRecorder::DispatchIndirect(const std::shared_ptr<Rhi::IBuffer>& argumentBuffer, std::size_t offset) {
    if (activeRenderEncoder || activeBlitEncoder) {
        EndActiveEncoders();
    }

    if (!activeComputeEncoder) {
        if (!nativeCommandBuffer) {
            Begin();
        }
        id<MTLCommandBuffer> cb = (__bridge id<MTLCommandBuffer>)nativeCommandBuffer;
        id<MTLComputeCommandEncoder> enc = [cb computeCommandEncoder];
        if (!enc) {
            throw std::runtime_error("Failed to allocate MTLComputeCommandEncoder");
        }
        activeComputeEncoder = (__bridge_retained void*)enc;
        if (activePipeline && activePipeline->IsCompute()) {
            SetPipeline(activePipeline);
        }
    }

    auto* mtlBuf = dynamic_cast<MetalBuffer*>(argumentBuffer.get());
    if (!mtlBuf || !mtlBuf->GetNativeBuffer()) {
        return;
    }

    id<MTLComputeCommandEncoder> enc = (__bridge id<MTLComputeCommandEncoder>)activeComputeEncoder;
    MTLSize threadsPerGroup = MTLSizeMake(32, 1, 1);
    [enc dispatchThreadgroupsWithIndirectBuffer:(__bridge id<MTLBuffer>)mtlBuf->GetNativeBuffer()
                          indirectBufferOffset:offset
                         threadsPerThreadgroup:threadsPerGroup];
}

void MetalCommandRecorder::CopyBuffer(const std::shared_ptr<Rhi::IBuffer>& src, std::size_t srcOffset, const std::shared_ptr<Rhi::IBuffer>& dst, std::size_t dstOffset, std::size_t size) {
    EndActiveEncoders();
    if (!nativeCommandBuffer) {
        Begin();
    }

    id<MTLCommandBuffer> cb = (__bridge id<MTLCommandBuffer>)nativeCommandBuffer;
    id<MTLBlitCommandEncoder> enc = [cb blitCommandEncoder];
    if (!enc) {
        throw std::runtime_error("Failed to allocate MTLBlitCommandEncoder");
    }

    auto* s = dynamic_cast<MetalBuffer*>(src.get());
    auto* d = dynamic_cast<MetalBuffer*>(dst.get());
    if (s && d && s->GetNativeBuffer() && d->GetNativeBuffer()) {
        [enc copyFromBuffer:(__bridge id<MTLBuffer>)s->GetNativeBuffer()
               sourceOffset:srcOffset
                   toBuffer:(__bridge id<MTLBuffer>)d->GetNativeBuffer()
          destinationOffset:dstOffset
                       size:size];
    }
    [enc endEncoding];
}

void MetalCommandRecorder::CopyTexture(const std::shared_ptr<Rhi::ITexture>& src, const std::shared_ptr<Rhi::ITexture>& dst) {
    EndActiveEncoders();
    if (!nativeCommandBuffer) {
        Begin();
    }

    id<MTLCommandBuffer> cb = (__bridge id<MTLCommandBuffer>)nativeCommandBuffer;
    id<MTLBlitCommandEncoder> enc = [cb blitCommandEncoder];
    if (!enc) {
        throw std::runtime_error("Failed to allocate MTLBlitCommandEncoder");
    }

    auto* s = dynamic_cast<MetalTexture*>(src.get());
    auto* d = dynamic_cast<MetalTexture*>(dst.get());
    if (s && d && s->GetNativeTexture() && d->GetNativeTexture()) {
        [enc copyFromTexture:(__bridge id<MTLTexture>)s->GetNativeTexture()
                   toTexture:(__bridge id<MTLTexture>)d->GetNativeTexture()];
    }
    [enc endEncoding];
}

void MetalCommandRecorder::BufferBarrier(const std::shared_ptr<Rhi::IBuffer>& buffer) {
    (void)buffer;
    if (activeComputeEncoder) {
        id<MTLComputeCommandEncoder> enc = (__bridge id<MTLComputeCommandEncoder>)activeComputeEncoder;
        if (@available(macOS 10.14, *)) {
            [enc memoryBarrierWithScope:MTLBarrierScopeBuffers];
        }
    } else if (activeRenderEncoder) {
        id<MTLRenderCommandEncoder> enc = (__bridge id<MTLRenderCommandEncoder>)activeRenderEncoder;
        if (@available(macOS 10.14, *)) {
            [enc memoryBarrierWithScope:MTLBarrierScopeBuffers afterStages:MTLRenderStageVertex beforeStages:MTLRenderStageFragment];
        }
    }
}

void MetalCommandRecorder::TextureBarrier(const std::shared_ptr<Rhi::ITexture>& texture) {
    (void)texture;
    if (activeComputeEncoder) {
        id<MTLComputeCommandEncoder> enc = (__bridge id<MTLComputeCommandEncoder>)activeComputeEncoder;
        if (@available(macOS 10.14, *)) {
            [enc memoryBarrierWithScope:MTLBarrierScopeTextures];
        }
    } else if (activeRenderEncoder) {
        id<MTLRenderCommandEncoder> enc = (__bridge id<MTLRenderCommandEncoder>)activeRenderEncoder;
        if (@available(macOS 10.14, *)) {
            [enc memoryBarrierWithScope:MTLBarrierScopeTextures afterStages:MTLRenderStageVertex beforeStages:MTLRenderStageFragment];
        }
    }
}

} // namespace AnyPS5::Gpu::Metal

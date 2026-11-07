#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_GPUCOMMAND_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_GPUCOMMAND_HPP

#include "RhiTypes.hpp"
#include "ICommandRecorder.hpp"
#include <cstdint>
#include <memory>
#include <variant>

namespace AnyPS5::Gpu::Rhi {

struct CmdBeginRenderPass {
    RenderPassDesc pass;
};

struct CmdEndRenderPass {};

struct CmdSetPipeline {
    std::shared_ptr<IPipeline> pipeline;
};

struct CmdSetViewport {
    Viewport viewport;
};

struct CmdSetScissor {
    Scissor scissor;
};

struct CmdSetVertexBuffer {
    std::uint32_t slot = 0;
    std::shared_ptr<IBuffer> buffer;
    std::size_t offset = 0;
};

struct CmdSetIndexBuffer {
    std::shared_ptr<IBuffer> buffer;
    std::size_t offset = 0;
    bool is32Bit = true;
};

struct CmdSetDescriptorTable {
    std::uint32_t slot = 0;
    std::shared_ptr<IDescriptorTable> table;
};

struct CmdDraw {
    std::uint32_t vertexCount = 0;
    std::uint32_t instanceCount = 1;
    std::uint32_t firstVertex = 0;
    std::uint32_t firstInstance = 0;
};

struct CmdDrawIndexed {
    std::uint32_t indexCount = 0;
    std::uint32_t instanceCount = 1;
    std::uint32_t firstIndex = 0;
    std::int32_t vertexOffset = 0;
    std::uint32_t firstInstance = 0;
};

struct CmdDrawIndirect {
    std::shared_ptr<IBuffer> argumentBuffer;
    std::size_t offset = 0;
    std::uint32_t drawCount = 1;
    std::uint32_t stride = 0;
};

struct CmdDispatch {
    std::uint32_t groupCountX = 1;
    std::uint32_t groupCountY = 1;
    std::uint32_t groupCountZ = 1;
};

struct CmdDispatchIndirect {
    std::shared_ptr<IBuffer> argumentBuffer;
    std::size_t offset = 0;
};

struct CmdCopyBuffer {
    std::shared_ptr<IBuffer> src;
    std::size_t srcOffset = 0;
    std::shared_ptr<IBuffer> dst;
    std::size_t dstOffset = 0;
    std::size_t size = 0;
};

struct CmdCopyTexture {
    std::shared_ptr<ITexture> src;
    std::shared_ptr<ITexture> dst;
};

struct CmdBufferBarrier {
    std::shared_ptr<IBuffer> buffer;
};

struct CmdTextureBarrier {
    std::shared_ptr<ITexture> texture;
};

using GpuCommand = std::variant<
    CmdBeginRenderPass,
    CmdEndRenderPass,
    CmdSetPipeline,
    CmdSetViewport,
    CmdSetScissor,
    CmdSetVertexBuffer,
    CmdSetIndexBuffer,
    CmdSetDescriptorTable,
    CmdDraw,
    CmdDrawIndexed,
    CmdDrawIndirect,
    CmdDispatch,
    CmdDispatchIndirect,
    CmdCopyBuffer,
    CmdCopyTexture,
    CmdBufferBarrier,
    CmdTextureBarrier
>;

} // namespace AnyPS5::Gpu::Rhi

#endif
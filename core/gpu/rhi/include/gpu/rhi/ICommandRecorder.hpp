#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_ICOMMANDRECORDER_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_ICOMMANDRECORDER_HPP

#include "RhiTypes.hpp"
#include "IBuffer.hpp"
#include "ITexture.hpp"
#include "IPipeline.hpp"
#include "IDescriptorTable.hpp"
#include <cstdint>
#include <memory>
#include <span>

namespace AnyPS5::Gpu::Rhi {

struct RenderPassDesc {
    struct ColorAttachment {
        std::shared_ptr<ITexture> texture;
        ClearColor clearColor{};
        bool clear = false;
        bool store = true;
    };
    struct DepthStencilAttachment {
        std::shared_ptr<ITexture> texture;
        ClearDepthStencil clearValue{};
        bool clearDepth = false;
        bool storeDepth = true;
        bool clearStencil = false;
        bool storeStencil = true;
    };

    std::vector<ColorAttachment> colorAttachments{};
    DepthStencilAttachment depthStencilAttachment{};
};

class ICommandRecorder {
public:
    virtual ~ICommandRecorder() = default;

    virtual void Begin() = 0;
    virtual void End() = 0;

    virtual void BeginRenderPass(const RenderPassDesc& pass) = 0;
    virtual void EndRenderPass() = 0;

    virtual void SetPipeline(const std::shared_ptr<IPipeline>& pipeline) = 0;
    virtual void SetViewport(const Viewport& viewport) = 0;
    virtual void SetScissor(const Scissor& scissor) = 0;

    virtual void SetVertexBuffer(std::uint32_t slot, const std::shared_ptr<IBuffer>& buffer, std::size_t offset) = 0;
    virtual void SetIndexBuffer(const std::shared_ptr<IBuffer>& buffer, std::size_t offset, bool is32Bit) = 0;
    virtual void SetDescriptorTable(std::uint32_t slot, const std::shared_ptr<IDescriptorTable>& table) = 0;

    virtual void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t firstVertex, std::uint32_t firstInstance) = 0;
    virtual void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t firstIndex, std::int32_t vertexOffset, std::uint32_t firstInstance) = 0;
    virtual void DrawIndirect(const std::shared_ptr<IBuffer>& argumentBuffer, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) = 0;

    virtual void Dispatch(std::uint32_t groupCountX, std::uint32_t groupCountY, std::uint32_t groupCountZ) = 0;
    virtual void DispatchIndirect(const std::shared_ptr<IBuffer>& argumentBuffer, std::size_t offset) = 0;

    virtual void CopyBuffer(const std::shared_ptr<IBuffer>& src, std::size_t srcOffset, const std::shared_ptr<IBuffer>& dst, std::size_t dstOffset, std::size_t size) = 0;
    virtual void CopyTexture(const std::shared_ptr<ITexture>& src, const std::shared_ptr<ITexture>& dst) = 0;
    virtual void BufferBarrier(const std::shared_ptr<IBuffer>& buffer) = 0;
    virtual void TextureBarrier(const std::shared_ptr<ITexture>& texture) = 0;
};

} // namespace AnyPS5::Gpu::Rhi

#endif
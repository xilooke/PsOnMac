#ifndef CORE_GPU_METAL_INCLUDE_GPU_METAL_METALCOMMANDRECORDER_HPP
#define CORE_GPU_METAL_INCLUDE_GPU_METAL_METALCOMMANDRECORDER_HPP

#include "gpu/rhi/ICommandRecorder.hpp"
#include <memory>

namespace AnyPS5::Gpu::Metal {

class MetalCommandRecorder final : public Rhi::ICommandRecorder {
public:
    MetalCommandRecorder(void* mtlDevice, void* mtlQueue, Rhi::QueueType queueType);
    ~MetalCommandRecorder() override;

    void Begin() override;
    void End() override;

    void BeginRenderPass(const Rhi::RenderPassDesc& pass) override;
    void EndRenderPass() override;

    void SetPipeline(const std::shared_ptr<Rhi::IPipeline>& pipeline) override;
    void SetViewport(const Rhi::Viewport& viewport) override;
    void SetScissor(const Rhi::Scissor& scissor) override;

    void SetVertexBuffer(std::uint32_t slot, const std::shared_ptr<Rhi::IBuffer>& buffer, std::size_t offset) override;
    void SetIndexBuffer(const std::shared_ptr<Rhi::IBuffer>& buffer, std::size_t offset, bool is32Bit) override;
    void SetDescriptorTable(std::uint32_t slot, const std::shared_ptr<Rhi::IDescriptorTable>& table) override;

    void Draw(std::uint32_t vertexCount, std::uint32_t instanceCount, std::uint32_t firstVertex, std::uint32_t firstInstance) override;
    void DrawIndexed(std::uint32_t indexCount, std::uint32_t instanceCount, std::uint32_t firstIndex, std::int32_t vertexOffset, std::uint32_t firstInstance) override;
    void DrawIndirect(const std::shared_ptr<Rhi::IBuffer>& argumentBuffer, std::size_t offset, std::uint32_t drawCount, std::uint32_t stride) override;

    void Dispatch(std::uint32_t groupCountX, std::uint32_t groupCountY, std::uint32_t groupCountZ) override;
    void DispatchIndirect(const std::shared_ptr<Rhi::IBuffer>& argumentBuffer, std::size_t offset) override;

    void CopyBuffer(const std::shared_ptr<Rhi::IBuffer>& src, std::size_t srcOffset, const std::shared_ptr<Rhi::IBuffer>& dst, std::size_t dstOffset, std::size_t size) override;
    void CopyTexture(const std::shared_ptr<Rhi::ITexture>& src, const std::shared_ptr<Rhi::ITexture>& dst) override;
    void BufferBarrier(const std::shared_ptr<Rhi::IBuffer>& buffer) override;
    void TextureBarrier(const std::shared_ptr<Rhi::ITexture>& texture) override;

    [[nodiscard]] void* GetNativeCommandBuffer() const { return nativeCommandBuffer; }
    [[nodiscard]] Rhi::QueueType GetQueueType() const { return queueType; }

private:
    void EndActiveEncoders();
    void BindDescriptorTableToActiveEncoders(std::uint32_t slot, const std::shared_ptr<Rhi::IDescriptorTable>& table);

    void* nativeDevice = nullptr;
    void* nativeQueue = nullptr;
    void* nativeCommandBuffer = nullptr;
    void* activeRenderEncoder = nullptr;
    void* activeComputeEncoder = nullptr;
    void* activeBlitEncoder = nullptr;

    Rhi::QueueType queueType = Rhi::QueueType::Graphics;
    std::shared_ptr<Rhi::IPipeline> activePipeline;
    std::shared_ptr<Rhi::IBuffer> indexBuffer;
    std::size_t indexBufferOffset = 0;
    bool is32BitIndex = false;
};

} // namespace AnyPS5::Gpu::Metal

#endif

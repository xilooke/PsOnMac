#include "gpu/rhi/GpuCommandStream.hpp"
#include <cassert>

namespace {

class MockRecorder final : public AnyPS5::Gpu::Rhi::ICommandRecorder {
public:
    void Begin() override {}
    void End() override {}

    void BeginRenderPass(const AnyPS5::Gpu::Rhi::RenderPassDesc&) override {
        renderPassBegun = true;
    }
    void EndRenderPass() override {
        renderPassEnded = true;
    }

    void SetPipeline(const std::shared_ptr<AnyPS5::Gpu::Rhi::IPipeline>&) override {}
    void SetViewport(const AnyPS5::Gpu::Rhi::Viewport&) override {}
    void SetScissor(const AnyPS5::Gpu::Rhi::Scissor&) override {}

    void SetVertexBuffer(std::uint32_t, const std::shared_ptr<AnyPS5::Gpu::Rhi::IBuffer>&, std::size_t) override {}
    void SetIndexBuffer(const std::shared_ptr<AnyPS5::Gpu::Rhi::IBuffer>&, std::size_t, bool) override {}
    void SetDescriptorTable(std::uint32_t, const std::shared_ptr<AnyPS5::Gpu::Rhi::IDescriptorTable>&) override {}

    void Draw(std::uint32_t count, std::uint32_t instances, std::uint32_t, std::uint32_t) override {
        drawCount += count;
        instanceCount += instances;
    }
    void DrawIndexed(std::uint32_t, std::uint32_t, std::uint32_t, std::int32_t, std::uint32_t) override {}
    void DrawIndirect(const std::shared_ptr<AnyPS5::Gpu::Rhi::IBuffer>&, std::size_t, std::uint32_t, std::uint32_t) override {}

    void Dispatch(std::uint32_t x, std::uint32_t, std::uint32_t) override {
        dispatchX += x;
    }
    void DispatchIndirect(const std::shared_ptr<AnyPS5::Gpu::Rhi::IBuffer>&, std::size_t) override {}

    void CopyBuffer(const std::shared_ptr<AnyPS5::Gpu::Rhi::IBuffer>&, std::size_t, const std::shared_ptr<AnyPS5::Gpu::Rhi::IBuffer>&, std::size_t, std::size_t) override {}
    void CopyTexture(const std::shared_ptr<AnyPS5::Gpu::Rhi::ITexture>&, const std::shared_ptr<AnyPS5::Gpu::Rhi::ITexture>&) override {}
    void BufferBarrier(const std::shared_ptr<AnyPS5::Gpu::Rhi::IBuffer>&) override {}
    void TextureBarrier(const std::shared_ptr<AnyPS5::Gpu::Rhi::ITexture>&) override {}

    bool renderPassBegun = false;
    bool renderPassEnded = false;
    std::uint32_t drawCount = 0;
    std::uint32_t instanceCount = 0;
    std::uint32_t dispatchX = 0;
};

} // namespace

int main() {
    using namespace AnyPS5::Gpu::Rhi;

    GpuCommandStream stream;
    if (!stream.Empty()) {
        return 1;
    }

    stream.Append(CmdBeginRenderPass{});
    stream.Append(CmdDraw{3, 1, 0, 0});
    stream.Append(CmdEndRenderPass{});
    stream.Append(CmdDispatch{16, 1, 1});

    if (stream.Size() != 4) {
        return 2;
    }

    MockRecorder recorder;
    stream.Playback(recorder);

    if (!recorder.renderPassBegun || !recorder.renderPassEnded) {
        return 3;
    }
    if (recorder.drawCount != 3 || recorder.instanceCount != 1) {
        return 4;
    }
    if (recorder.dispatchX != 16) {
        return 5;
    }

    stream.Clear();
    if (!stream.Empty()) {
        return 6;
    }

    return 0;
}

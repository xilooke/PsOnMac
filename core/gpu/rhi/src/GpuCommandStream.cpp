#include "gpu/rhi/GpuCommandStream.hpp"

namespace AnyPS5::Gpu::Rhi {

void GpuCommandStream::Append(GpuCommand cmd) {
    commands.push_back(std::move(cmd));
}

void GpuCommandStream::Clear() {
    commands.clear();
}

std::size_t GpuCommandStream::Size() const {
    return commands.size();
}

bool GpuCommandStream::Empty() const {
    return commands.empty();
}

void GpuCommandStream::Playback(ICommandRecorder& recorder) const {
    for (const auto& cmd : commands) {
        std::visit([&recorder](const auto& c) {
            using T = std::decay_t<decltype(c)>;
            if constexpr (std::is_same_v<T, CmdBeginRenderPass>) {
                recorder.BeginRenderPass(c.pass);
            } else if constexpr (std::is_same_v<T, CmdEndRenderPass>) {
                recorder.EndRenderPass();
            } else if constexpr (std::is_same_v<T, CmdSetPipeline>) {
                recorder.SetPipeline(c.pipeline);
            } else if constexpr (std::is_same_v<T, CmdSetViewport>) {
                recorder.SetViewport(c.viewport);
            } else if constexpr (std::is_same_v<T, CmdSetScissor>) {
                recorder.SetScissor(c.scissor);
            } else if constexpr (std::is_same_v<T, CmdSetVertexBuffer>) {
                recorder.SetVertexBuffer(c.slot, c.buffer, c.offset);
            } else if constexpr (std::is_same_v<T, CmdSetIndexBuffer>) {
                recorder.SetIndexBuffer(c.buffer, c.offset, c.is32Bit);
            } else if constexpr (std::is_same_v<T, CmdSetDescriptorTable>) {
                recorder.SetDescriptorTable(c.slot, c.table);
            } else if constexpr (std::is_same_v<T, CmdDraw>) {
                recorder.Draw(c.vertexCount, c.instanceCount, c.firstVertex, c.firstInstance);
            } else if constexpr (std::is_same_v<T, CmdDrawIndexed>) {
                recorder.DrawIndexed(c.indexCount, c.instanceCount, c.firstIndex, c.vertexOffset, c.firstInstance);
            } else if constexpr (std::is_same_v<T, CmdDrawIndirect>) {
                recorder.DrawIndirect(c.argumentBuffer, c.offset, c.drawCount, c.stride);
            } else if constexpr (std::is_same_v<T, CmdDispatch>) {
                recorder.Dispatch(c.groupCountX, c.groupCountY, c.groupCountZ);
            } else if constexpr (std::is_same_v<T, CmdDispatchIndirect>) {
                recorder.DispatchIndirect(c.argumentBuffer, c.offset);
            } else if constexpr (std::is_same_v<T, CmdCopyBuffer>) {
                recorder.CopyBuffer(c.src, c.srcOffset, c.dst, c.dstOffset, c.size);
            } else if constexpr (std::is_same_v<T, CmdCopyTexture>) {
                recorder.CopyTexture(c.src, c.dst);
            } else if constexpr (std::is_same_v<T, CmdBufferBarrier>) {
                recorder.BufferBarrier(c.buffer);
            } else if constexpr (std::is_same_v<T, CmdTextureBarrier>) {
                recorder.TextureBarrier(c.texture);
            }
        }, cmd);
    }
}

} // namespace AnyPS5::Gpu::Rhi

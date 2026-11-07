#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_GPUCOMMANDSTREAM_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_GPUCOMMANDSTREAM_HPP

#include "GpuCommand.hpp"
#include "ICommandRecorder.hpp"
#include <vector>

namespace AnyPS5::Gpu::Rhi {

class GpuCommandStream {
public:
    void Append(GpuCommand cmd);
    void Clear();
    [[nodiscard]] std::size_t Size() const;
    [[nodiscard]] bool Empty() const;
    void Playback(ICommandRecorder& recorder) const;

private:
    std::vector<GpuCommand> commands;
};

} // namespace AnyPS5::Gpu::Rhi

#endif
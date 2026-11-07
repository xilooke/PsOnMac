#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_IQUEUE_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_IQUEUE_HPP

#include "RhiTypes.hpp"
#include "ICommandRecorder.hpp"
#include "IFence.hpp"
#include <memory>
#include <span>

namespace AnyPS5::Gpu::Rhi {

struct SubmitInfo {
    std::span<const std::shared_ptr<ICommandRecorder>> commandRecorders{};
    std::span<const std::shared_ptr<IFence>> waitFences{};
    std::span<const std::shared_ptr<IFence>> signalFences{};
    std::shared_ptr<IFence> completionFence{};
};

class IQueue {
public:
    virtual ~IQueue() = default;
    [[nodiscard]] virtual QueueType GetType() const = 0;
    virtual void Submit(const SubmitInfo& submitInfo) = 0;
    virtual void WaitIdle() = 0;
};

} // namespace AnyPS5::Gpu::Rhi

#endif
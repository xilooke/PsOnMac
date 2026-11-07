#ifndef CORE_GPU_METAL_INCLUDE_GPU_METAL_METALQUEUE_HPP
#define CORE_GPU_METAL_INCLUDE_GPU_METAL_METALQUEUE_HPP

#include "gpu/rhi/IQueue.hpp"

namespace AnyPS5::Gpu::Metal {

class MetalQueue final : public Rhi::IQueue {
public:
    MetalQueue(void* mtlDevice, Rhi::QueueType queueType);
    ~MetalQueue() override;

    [[nodiscard]] Rhi::QueueType GetType() const override { return type; }
    void Submit(const Rhi::SubmitInfo& submitInfo) override;
    void WaitIdle() override;

    [[nodiscard]] void* GetNativeQueue() const { return nativeQueue; }

private:
    void* nativeQueue = nullptr;
    Rhi::QueueType type = Rhi::QueueType::Graphics;
};

} // namespace AnyPS5::Gpu::Metal

#endif

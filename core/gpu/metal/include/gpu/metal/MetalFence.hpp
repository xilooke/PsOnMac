#ifndef CORE_GPU_METAL_INCLUDE_GPU_METAL_METALFENCE_HPP
#define CORE_GPU_METAL_INCLUDE_GPU_METAL_METALFENCE_HPP

#include "gpu/rhi/IFence.hpp"
#include <atomic>
#include <cstdint>

namespace AnyPS5::Gpu::Metal {

class MetalFence final : public Rhi::IFence {
public:
    MetalFence(void* mtlDevice, bool signaled = false);
    ~MetalFence() override;

    void Wait(std::uint64_t timeoutNanoseconds = UINT64_MAX) override;
    void Reset() override;
    [[nodiscard]] bool IsSignaled() const override;

    [[nodiscard]] void* GetSharedEvent() const { return nativeEvent; }
    [[nodiscard]] std::uint64_t GetTargetValue() const { return targetValue.load(); }
    void PrepareNextSignal();

private:
    void* nativeEvent = nullptr;
    std::atomic<std::uint64_t> targetValue{1};
};

class MetalTimelineSemaphore final : public Rhi::ITimelineSemaphore {
public:
    MetalTimelineSemaphore(void* mtlDevice, std::uint64_t initialValue = 0);
    ~MetalTimelineSemaphore() override;

    [[nodiscard]] std::uint64_t GetCurrentValue() const override;
    void Signal(std::uint64_t value) override;
    bool Wait(std::uint64_t value, std::uint64_t timeoutNanoseconds = UINT64_MAX) override;

    [[nodiscard]] void* GetSharedEvent() const { return nativeEvent; }

private:
    void* nativeEvent = nullptr;
};

} // namespace AnyPS5::Gpu::Metal

#endif

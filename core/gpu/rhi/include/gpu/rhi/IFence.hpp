#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_IFENCE_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_IFENCE_HPP

#include <cstdint>

namespace AnyPS5::Gpu::Rhi {

class IFence {
public:
    virtual ~IFence() = default;
    virtual void Wait(std::uint64_t timeoutNanoseconds = UINT64_MAX) = 0;
    virtual void Reset() = 0;
    [[nodiscard]] virtual bool IsSignaled() const = 0;
};

class ITimelineSemaphore {
public:
    virtual ~ITimelineSemaphore() = default;
    [[nodiscard]] virtual std::uint64_t GetCurrentValue() const = 0;
    virtual void Signal(std::uint64_t value) = 0;
    virtual bool Wait(std::uint64_t value, std::uint64_t timeoutNanoseconds = UINT64_MAX) = 0;
};

} // namespace AnyPS5::Gpu::Rhi

#endif
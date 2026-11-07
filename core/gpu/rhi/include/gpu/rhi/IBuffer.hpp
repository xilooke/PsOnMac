#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_IBUFFER_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_IBUFFER_HPP

#include "RhiTypes.hpp"
#include <cstddef>
#include <cstdint>

namespace AnyPS5::Gpu::Rhi {

struct BufferDesc {
    std::size_t size = 0;
    BufferUsage usage = BufferUsage::None;
    bool hostVisible = false;
    bool hostCoherent = false;
    void* existingHostAddress = nullptr;
};

class IBuffer {
public:
    virtual ~IBuffer() = default;
    [[nodiscard]] virtual std::size_t GetSize() const = 0;
    [[nodiscard]] virtual BufferUsage GetUsage() const = 0;
    [[nodiscard]] virtual void* Map() = 0;
    virtual void Unmap() = 0;
    [[nodiscard]] virtual std::uint64_t GetDeviceAddress() const = 0;
};

} // namespace AnyPS5::Gpu::Rhi

#endif
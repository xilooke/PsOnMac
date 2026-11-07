#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_IPIPELINECACHE_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_IPIPELINECACHE_HPP

#include <cstddef>
#include <cstdint>
#include <vector>

namespace AnyPS5::Gpu::Rhi {

class IPipelineCache {
public:
    virtual ~IPipelineCache() = default;
    [[nodiscard]] virtual std::vector<std::uint8_t> GetData() const = 0;
};

} // namespace AnyPS5::Gpu::Rhi

#endif
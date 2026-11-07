#ifndef CORE_GPU_METAL_INCLUDE_GPU_METAL_METALPIPELINECACHE_HPP
#define CORE_GPU_METAL_INCLUDE_GPU_METAL_METALPIPELINECACHE_HPP

#include "gpu/rhi/IPipelineCache.hpp"
#include <span>
#include <vector>

namespace AnyPS5::Gpu::Metal {

class MetalPipelineCache final : public Rhi::IPipelineCache {
public:
    MetalPipelineCache(void* mtlDevice, std::span<const std::uint8_t> initialData);
    ~MetalPipelineCache() override;

    [[nodiscard]] std::vector<std::uint8_t> GetData() const override;

private:
    std::vector<std::uint8_t> cachedData;
};

} // namespace AnyPS5::Gpu::Metal

#endif

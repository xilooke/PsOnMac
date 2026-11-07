#include "gpu/metal/MetalPipelineCache.hpp"
#import <Metal/Metal.h>

namespace AnyPS5::Gpu::Metal {

MetalPipelineCache::MetalPipelineCache(void* mtlDevice, std::span<const std::uint8_t> initialData)
    : cachedData(initialData.begin(), initialData.end()) {
    (void)mtlDevice;
}

MetalPipelineCache::~MetalPipelineCache() = default;

std::vector<std::uint8_t> MetalPipelineCache::GetData() const {
    return cachedData;
}

} // namespace AnyPS5::Gpu::Metal

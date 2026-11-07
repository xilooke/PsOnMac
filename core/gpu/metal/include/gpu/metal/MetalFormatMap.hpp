#ifndef CORE_GPU_METAL_INCLUDE_GPU_METAL_METALFORMATMAP_HPP
#define CORE_GPU_METAL_INCLUDE_GPU_METAL_METALFORMATMAP_HPP

#include "gpu/rhi/RhiTypes.hpp"
#include <cstdint>

#ifdef __OBJC__
#import <Metal/Metal.h>
#else
typedef unsigned long MTLPixelFormat;
#endif

namespace AnyPS5::Gpu::Metal {

MTLPixelFormat ToMetalPixelFormat(Rhi::Format format);
Rhi::Format FromMetalPixelFormat(MTLPixelFormat format);

} // namespace AnyPS5::Gpu::Metal

#endif
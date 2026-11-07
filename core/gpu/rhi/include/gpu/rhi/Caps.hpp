#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_CAPS_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_CAPS_HPP

#include <cstddef>
#include <cstdint>

namespace AnyPS5::Gpu::Rhi {

enum class ArgumentBufferTier : std::uint8_t {
    None,
    Tier1,
    Tier2
};

struct Caps {
    std::uint32_t minWaveSize = 32;
    std::uint32_t maxWaveSize = 64;
    bool supportsWave32 = true;
    bool supportsWave64 = false;

    bool supportsMeshShaders = false;
    bool supportsNativeTessellation = false;
    bool requiresGeometryShaderEmulation = true;

    bool supportsD24UnormS8Uint = false;
    bool supportsD32FloatS8Uint = true;
    bool supportsBcTextureCompression = true;

    bool supportsInt64Atomics = false;
    bool supportsFloatAtomics = false;

    ArgumentBufferTier argumentBufferTier = ArgumentBufferTier::Tier2;
    bool hasUnifiedMemory = false;
    bool supportsExternalMemoryHost = false;
    bool requiresExplicitResidency = false;

    std::uint32_t maxTextureDimension1D = 16384;
    std::uint32_t maxTextureDimension2D = 16384;
    std::uint32_t maxTextureDimension3D = 2048;
    std::uint32_t maxTextureDimensionCube = 16384;
    std::uint32_t maxTextureArrayLayers = 2048;

    std::uint64_t maxBufferSize = 1073741824ULL;
    std::uint32_t maxComputeSharedMemoryBytes = 65536;
    std::uint32_t maxComputeWorkGroupInvocations = 1024;
    std::uint32_t maxComputeWorkGroupSize[3] = {1024, 1024, 64};
};

} // namespace AnyPS5::Gpu::Rhi

#endif
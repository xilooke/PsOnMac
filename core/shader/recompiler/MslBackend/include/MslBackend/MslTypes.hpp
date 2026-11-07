#ifndef CORE_SHADER_RECOMPILIER_MSLBACKEND_INCLUDE_MSLBACKEND_MSLTYPES_HPP
#define CORE_SHADER_RECOMPILIER_MSLBACKEND_INCLUDE_MSLBACKEND_MSLTYPES_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace ShaderRecompiler::Msl {

enum class MslVersion : std::uint32_t {
    Version2_4 = 24,
    Version3_0 = 30,
    Version3_1 = 31
};

struct MslTargetOptions {
    MslVersion version = MslVersion::Version3_0;
    bool spannedWaves = true;
    bool useArgumentBuffers = true;
    bool fastMath = false;
    bool enableSimdIntrinsics = true;
    std::uint32_t simdWidth = 32;
};

struct MslEmitResult {
    std::string source;
    std::string entryPoint = "main0";
    bool usesArgumentBuffers = true;
    std::uint32_t workGroupSize[3] = {1, 1, 1};
    std::size_t instructionCount = 0;
};

} // namespace ShaderRecompiler::Msl

#endif
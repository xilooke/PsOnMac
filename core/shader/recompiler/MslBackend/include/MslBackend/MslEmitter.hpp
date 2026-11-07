#ifndef CORE_SHADER_RECOMPILIER_MSLBACKEND_INCLUDE_MSLBACKEND_MSLEMITTER_HPP
#define CORE_SHADER_RECOMPILIER_MSLBACKEND_INCLUDE_MSLBACKEND_MSLEMITTER_HPP

#include "MslTypes.hpp"
#include "IntermediateRepresentation/IrProgram.hpp"

namespace ShaderRecompiler::Msl {

class MslEmitter {
public:
    [[nodiscard]] MslEmitResult Emit(const IrProgram& program, const MslTargetOptions& options = {}) const;
};

} // namespace ShaderRecompiler::Msl

#endif
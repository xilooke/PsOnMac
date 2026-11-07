#include "MslBackend/MslEmitter.hpp"
#include "IntermediateRepresentation/IrProgram.hpp"
#include "IntermediateRepresentation/IrBlock.hpp"
#include "IntermediateRepresentation/IrValue.hpp"
#include <cassert>
#include <string_view>

namespace {

bool Contains(std::string_view haystack, std::string_view needle) {
    return haystack.find(needle) != std::string_view::npos;
}

void TestComputeShader() {
    using namespace ShaderRecompiler;
    using namespace ShaderRecompiler::Msl;

    IrProgram program;
    auto& block = program.CreateBlock();

    auto& v0 = program.CreateValue(IrOpcode::Identity, IrType::U32);
    v0.SetImmediateU32(10);
    block.AppendInstruction(&v0);

    auto& v1 = program.CreateValue(IrOpcode::Identity, IrType::U32);
    v1.SetImmediateU32(20);
    block.AppendInstruction(&v1);

    auto& v2 = program.CreateValue(IrOpcode::IAdd32, IrType::U32);
    v2.AddArgument(&v0);
    v2.AddArgument(&v1);
    block.AppendInstruction(&v2);

    auto& v3 = program.CreateValue(IrOpcode::ReadFirstLane, IrType::U32);
    v3.AddArgument(&v2);
    block.AppendInstruction(&v3);

    auto& v4 = program.CreateValue(IrOpcode::Barrier, IrType::Void);
    block.AppendInstruction(&v4);

    MslEmitter emitter;
    MslEmitResult result = emitter.Emit(program);

    assert(Contains(result.source, "#include <metal_stdlib>"));
    assert(Contains(result.source, "using namespace metal;"));
    assert(Contains(result.source, "kernel void main0("));
    assert(Contains(result.source, "threadgroup_barrier"));
    assert(Contains(result.source, "simd_broadcast_first"));
    assert(result.instructionCount >= 5);
}

void TestVertexShader() {
    using namespace ShaderRecompiler;
    using namespace ShaderRecompiler::Msl;

    IrProgram program;
    auto& block = program.CreateBlock();
    auto& v0 = program.CreateValue(IrOpcode::Identity, IrType::F32);
    v0.SetImmediateU32(0x3F800000u);
    block.AppendInstruction(&v0);

    auto& v1 = program.CreateValue(IrOpcode::FAdd32, IrType::F32);
    v1.AddArgument(&v0);
    v1.AddArgument(&v0);
    block.AppendInstruction(&v1);

    MslEmitter emitter;
    MslEmitResult result = emitter.Emit(program);

    assert(Contains(result.source, "vertex VertexOutput main0(") || Contains(result.source, "main0("));
    assert(Contains(result.source, "ArgumentTable"));
}

void TestFragmentShader() {
    using namespace ShaderRecompiler;
    using namespace ShaderRecompiler::Msl;

    IrProgram program;
    auto& block = program.CreateBlock();

    auto& v0 = program.CreateValue(IrOpcode::Identity, IrType::Bool);
    v0.SetImmediateU32(1);
    block.AppendInstruction(&v0);

    auto& v1 = program.CreateValue(IrOpcode::Discard, IrType::Void);
    block.AppendInstruction(&v1);

    MslEmitter emitter;
    MslEmitResult result = emitter.Emit(program);

    assert(Contains(result.source, "discard_fragment();"));
}

} // namespace

int main() {
    TestComputeShader();
    TestVertexShader();
    TestFragmentShader();
    return 0;
}

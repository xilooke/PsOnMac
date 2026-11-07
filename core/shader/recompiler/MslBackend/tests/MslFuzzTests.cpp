#include "MslBackend/MslEmitter.hpp"
#include "IntermediateRepresentation/IrProgram.hpp"
#include "IntermediateRepresentation/IrBlock.hpp"
#include "IntermediateRepresentation/IrValue.hpp"
#include <cassert>
#include <vector>

namespace {

void TestTruncatedArguments() {
    using namespace ShaderRecompiler;
    using namespace ShaderRecompiler::Msl;

    IrProgram program;
    auto& block = program.CreateBlock();

    auto& vAdd = program.CreateValue(IrOpcode::IAdd32, IrType::U32);
    block.AppendInstruction(&vAdd);

    auto& vMul = program.CreateValue(IrOpcode::FMul32, IrType::F32);
    block.AppendInstruction(&vMul);

    auto& vSelect = program.CreateValue(IrOpcode::Select, IrType::U32);
    block.AppendInstruction(&vSelect);

    auto& vCmp = program.CreateValue(IrOpcode::CompareEqual, IrType::Bool);
    block.AppendInstruction(&vCmp);

    auto& vLane = program.CreateValue(IrOpcode::ReadLane, IrType::U32);
    block.AppendInstruction(&vLane);

    MslEmitter emitter;
    MslEmitResult result = emitter.Emit(program);
    assert(!result.source.empty());
}

void TestRandomizedStreamFuzz() {
    using namespace ShaderRecompiler;
    using namespace ShaderRecompiler::Msl;

    IrProgram program;
    auto& block = program.CreateBlock();

    std::vector<IrValue*> values;
    auto& seed = program.CreateValue(IrOpcode::Identity, IrType::U32);
    seed.SetImmediateU32(42);
    block.AppendInstruction(&seed);
    values.push_back(&seed);

    const IrOpcode opcodes[] = {
        IrOpcode::IAdd32,
        IrOpcode::ISub32,
        IrOpcode::IMul32,
        IrOpcode::BitwiseAnd32,
        IrOpcode::BitwiseOr32,
        IrOpcode::BitwiseXor32,
        IrOpcode::ShiftLeftLogical32,
        IrOpcode::ShiftRightArithmetic32,
        IrOpcode::ShiftRightLogical32,
        IrOpcode::CompareEqual,
        IrOpcode::CompareNotEqual,
        IrOpcode::Barrier
    };

    uint32_t rng = 0x12345678;
    for (int i = 0; i < 500; ++i) {
        rng = rng * 1664525u + 1013904223u;
        IrOpcode op = opcodes[rng % (sizeof(opcodes) / sizeof(opcodes[0]))];
        auto& inst = program.CreateValue(op, IrType::U32);

        if (op != IrOpcode::Barrier && !values.empty()) {
            size_t idx1 = (rng >> 8) % values.size();
            inst.AddArgument(values[idx1]);
            if (op != IrOpcode::ReadFirstLane) {
                size_t idx2 = (rng >> 16) % values.size();
                inst.AddArgument(values[idx2]);
            }
        }
        block.AppendInstruction(&inst);
        values.push_back(&inst);
    }

    MslEmitter emitter;
    MslEmitResult result = emitter.Emit(program);
    assert(!result.source.empty());
    assert(result.instructionCount >= 500);
}

} // namespace

int main() {
    TestTruncatedArguments();
    TestRandomizedStreamFuzz();
    return 0;
}

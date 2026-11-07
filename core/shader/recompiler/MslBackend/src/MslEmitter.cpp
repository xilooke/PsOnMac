#include "MslBackend/MslEmitter.hpp"
#include "MslBackend/MslEmitterState.hpp"
#include <sstream>

namespace ShaderRecompiler::Msl {

namespace {

std::string MslTypeString(IrType type) {
    switch (type) {
    case IrType::Void:
        return "void";
    case IrType::Bool:
        return "bool";
    case IrType::U32:
        return "uint";
    case IrType::S32:
        return "int";
    case IrType::F32:
        return "float";
    case IrType::U64:
        return "ulong";
    case IrType::S64:
        return "long";
    case IrType::F64:
        return "double";
    case IrType::Vec2U32:
        return "uint2";
    case IrType::Vec3U32:
        return "uint3";
    case IrType::Vec4U32:
        return "uint4";
    case IrType::Vec2F32:
        return "float2";
    case IrType::Vec3F32:
        return "float3";
    case IrType::Vec4F32:
        return "float4";
    case IrType::U8:
        return "uchar";
    case IrType::U16:
        return "ushort";
    case IrType::F16:
        return "half";
    default:
        return "uint";
    }
}

std::string FormatOperand(MslEmitterState& state, const IrValue* op) {
    if (!op) {
        return "0";
    }
    if (op->HasImmediate()) {
        if (op->Type() == IrType::F32) {
            float f = 0.0f;
            std::uint32_t u = op->ImmediateU32();
            std::memcpy(&f, &u, sizeof(float));
            std::ostringstream ss;
            ss << f;
            std::string s = ss.str();
            if (s.find('.') == std::string::npos && s.find('e') == std::string::npos) {
                s += ".0f";
            } else {
                s += "f";
            }
            return s;
        }
        if (op->Type() == IrType::Bool) {
            return op->ImmediateU32() != 0 ? "true" : "false";
        }
        if (op->Type() == IrType::S32) {
            return std::to_string(static_cast<std::int32_t>(op->ImmediateU32()));
        }
        return std::to_string(op->ImmediateU32()) + "u";
    }
    return state.GetValueName(op);
}

void EmitInstruction(MslEmitterState& state, const IrValue* inst) {
    if (!inst) {
        return;
    }

    const auto& args = inst->Arguments();
    const IrOpcode op = inst->Opcode();
    const IrType type = inst->Type();

    std::string lhs = state.GetValueName(inst);
    std::string typeStr = MslTypeString(type);

    switch (op) {
    case IrOpcode::Identity:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + ";");
        }
        break;
    case IrOpcode::IAdd32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + " + " + FormatOperand(state, args[1]) + ";");
        }
        break;
    case IrOpcode::ISub32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + " - " + FormatOperand(state, args[1]) + ";");
        }
        break;
    case IrOpcode::IMul32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + " * " + FormatOperand(state, args[1]) + ";");
        }
        break;
    case IrOpcode::IDiv32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + " / " + FormatOperand(state, args[1]) + ";");
        }
        break;
    case IrOpcode::IMod32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + " % " + FormatOperand(state, args[1]) + ";");
        }
        break;
    case IrOpcode::IAnd32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + " & " + FormatOperand(state, args[1]) + ";");
        }
        break;
    case IrOpcode::IOr32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + " | " + FormatOperand(state, args[1]) + ";");
        }
        break;
    case IrOpcode::IXor32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + " ^ " + FormatOperand(state, args[1]) + ";");
        }
        break;
    case IrOpcode::IShiftLeft32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + " << " + FormatOperand(state, args[1]) + ";");
        }
        break;
    case IrOpcode::IShiftRightLogical32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + " >> " + FormatOperand(state, args[1]) + ";");
        }
        break;
    case IrOpcode::IShiftRightArithmetic32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = static_cast<uint>(static_cast<int>(" + FormatOperand(state, args[0]) + ") >> " + FormatOperand(state, args[1]) + ");");
        }
        break;
    case IrOpcode::INegate32:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = -" + FormatOperand(state, args[0]) + ";");
        }
        break;
    case IrOpcode::INot32:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = ~" + FormatOperand(state, args[0]) + ";");
        }
        break;
    case IrOpcode::FAdd32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + " + " + FormatOperand(state, args[1]) + ";");
        }
        break;
    case IrOpcode::FSub32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + " - " + FormatOperand(state, args[1]) + ";");
        }
        break;
    case IrOpcode::FMul32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + " * " + FormatOperand(state, args[1]) + ";");
        }
        break;
    case IrOpcode::FDiv32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = " + FormatOperand(state, args[0]) + " / " + FormatOperand(state, args[1]) + ";");
        }
        break;
    case IrOpcode::FNegate32:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = -" + FormatOperand(state, args[0]) + ";");
        }
        break;
    case IrOpcode::FAbs32:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = metal::abs(" + FormatOperand(state, args[0]) + ");");
        }
        break;
    case IrOpcode::FFma32:
        if (args.size() >= 3) {
            state.AppendLine(typeStr + " " + lhs + " = metal::fma(" + FormatOperand(state, args[0]) + ", " + FormatOperand(state, args[1]) + ", " + FormatOperand(state, args[2]) + ");");
        }
        break;
    case IrOpcode::FSqrt32:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = metal::sqrt(" + FormatOperand(state, args[0]) + ");");
        }
        break;
    case IrOpcode::FRcp32:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = 1.0f / (" + FormatOperand(state, args[0]) + ");");
        }
        break;
    case IrOpcode::FRsq32:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = metal::rsqrt(" + FormatOperand(state, args[0]) + ");");
        }
        break;
    case IrOpcode::FMin32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = metal::min(" + FormatOperand(state, args[0]) + ", " + FormatOperand(state, args[1]) + ");");
        }
        break;
    case IrOpcode::FMax32:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = metal::max(" + FormatOperand(state, args[0]) + ", " + FormatOperand(state, args[1]) + ");");
        }
        break;
    case IrOpcode::FFloor32:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = metal::floor(" + FormatOperand(state, args[0]) + ");");
        }
        break;
    case IrOpcode::FCeil32:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = metal::ceil(" + FormatOperand(state, args[0]) + ");");
        }
        break;
    case IrOpcode::FFract32:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = metal::fract(" + FormatOperand(state, args[0]) + ");");
        }
        break;
    case IrOpcode::ConvertFToI32:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = static_cast<int>(" + FormatOperand(state, args[0]) + ");");
        }
        break;
    case IrOpcode::ConvertIToF32:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = static_cast<float>(" + FormatOperand(state, args[0]) + ");");
        }
        break;
    case IrOpcode::ConvertFToU32:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = static_cast<uint>(" + FormatOperand(state, args[0]) + ");");
        }
        break;
    case IrOpcode::ConvertUToF32:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = static_cast<float>(" + FormatOperand(state, args[0]) + ");");
        }
        break;
    case IrOpcode::CompareEqual:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = (" + FormatOperand(state, args[0]) + " == " + FormatOperand(state, args[1]) + ");");
        }
        break;
    case IrOpcode::CompareNotEqual:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = (" + FormatOperand(state, args[0]) + " != " + FormatOperand(state, args[1]) + ");");
        }
        break;
    case IrOpcode::CompareLessThan:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = (" + FormatOperand(state, args[0]) + " < " + FormatOperand(state, args[1]) + ");");
        }
        break;
    case IrOpcode::CompareGreaterThan:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = (" + FormatOperand(state, args[0]) + " > " + FormatOperand(state, args[1]) + ");");
        }
        break;
    case IrOpcode::Select:
        if (args.size() >= 3) {
            state.AppendLine(typeStr + " " + lhs + " = (" + FormatOperand(state, args[0]) + ") ? (" + FormatOperand(state, args[1]) + ") : (" + FormatOperand(state, args[2]) + ");");
        }
        break;
    case IrOpcode::ReadFirstLane:
        if (!args.empty()) {
            state.AppendLine(typeStr + " " + lhs + " = simd_broadcast_first(" + FormatOperand(state, args[0]) + ");");
        }
        break;
    case IrOpcode::ReadLane:
        if (args.size() >= 2) {
            state.AppendLine(typeStr + " " + lhs + " = simd_shuffle(" + FormatOperand(state, args[0]) + ", static_cast<ushort>(" + FormatOperand(state, args[1]) + "));");
        }
        break;
    case IrOpcode::Barrier:
        state.AppendLine("threadgroup_barrier(mem_flags::mem_device | mem_flags::mem_threadgroup);");
        break;
    case IrOpcode::Discard:
        state.AppendLine("discard_fragment();");
        break;
    case IrOpcode::Return:
        state.AppendLine("return;");
        break;
    default:
        break;
    }
}

} // namespace

MslEmitResult MslEmitter::Emit(const IrProgram& program, const MslTargetOptions& options) const {
    MslEmitterState state(options);

    state.header << "#include <metal_stdlib>\n";
    state.header << "using namespace metal;\n\n";

    state.declarations << "struct ArgumentTable {\n";
    state.declarations << "    device uint* buffers[16] [[id(0)]];\n";
    state.declarations << "    texture2d<float> textures[16] [[id(16)]];\n";
    state.declarations << "    sampler samplers[16] [[id(32)]];\n";
    state.declarations << "};\n\n";

    state.declarations << "struct VertexOutput {\n";
    state.declarations << "    float4 position [[position]];\n";
    state.declarations << "    float4 color [[user(locn0)]];\n";
    state.declarations << "    float2 uv [[user(locn1)]];\n";
    state.declarations << "};\n\n";

    state.declarations << "struct FragmentOutput {\n";
    state.declarations << "    float4 color0 [[color(0)]];\n";
    state.declarations << "};\n\n";

    const auto& blocks = program.Blocks();
    const bool isVertex = !blocks.empty() && blocks.front()->Id() == 100;
    const bool isFragment = !blocks.empty() && blocks.front()->Id() == 200;

    if (isVertex) {
        state.body << "vertex VertexOutput main0(\n";
        state.body << "    uint vertex_id [[vertex_id]],\n";
        state.body << "    uint instance_id [[instance_id]],\n";
        state.body << "    constant ArgumentTable& args [[buffer(0)]]\n";
        state.body << ") {\n";
    } else if (isFragment) {
        state.body << "fragment FragmentOutput main0(\n";
        state.body << "    VertexOutput in [[stage_in]],\n";
        state.body << "    constant ArgumentTable& args [[buffer(0)]]\n";
        state.body << ") {\n";
    } else {
        state.body << "kernel void main0(\n";
        state.body << "    uint3 thread_pos [[thread_position_in_grid]],\n";
        state.body << "    uint3 threadgroup_pos [[threadgroup_position_in_grid]],\n";
        state.body << "    uint3 thread_in_group [[thread_position_in_threadgroup]],\n";
        state.body << "    uint simd_lane [[thread_index_in_simdgroup]],\n";
        state.body << "    constant ArgumentTable& args [[buffer(0)]]\n";
        state.body << ") {\n";
    }

    state.Indent();

    if (isVertex) {
        state.AppendLine("VertexOutput out{};");
        state.AppendLine("out.position = float4(0.0f, 0.0f, 0.0f, 1.0f);");
    } else if (isFragment) {
        state.AppendLine("FragmentOutput out{};");
        state.AppendLine("out.color0 = float4(0.0f, 0.0f, 0.0f, 1.0f);");
    }

    std::size_t count = 0;
    for (const auto& block : blocks) {
        for (const auto* inst : block->Instructions()) {
            EmitInstruction(state, inst);
            count++;
        }
    }

    if (isVertex) {
        state.AppendLine("return out;");
    } else if (isFragment) {
        state.AppendLine("return out;");
    }

    state.Dedent();
    state.body << "}\n";

    MslEmitResult result;
    result.source = state.BuildString();
    result.entryPoint = "main0";
    result.usesArgumentBuffers = true;
    result.instructionCount = count;
    return result;
}

} // namespace ShaderRecompiler::Msl

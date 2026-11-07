#ifndef CORE_SHADER_RECOMPILIER_MSLBACKEND_INCLUDE_MSLBACKEND_MSLEMITTERSTATE_HPP
#define CORE_SHADER_RECOMPILIER_MSLBACKEND_INCLUDE_MSLBACKEND_MSLEMITTERSTATE_HPP

#include "MslTypes.hpp"
#include "IntermediateRepresentation/IrProgram.hpp"
#include <string>
#include <string_view>
#include <unordered_map>
#include <sstream>

namespace ShaderRecompiler::Msl {

class MslEmitterState {
public:
    explicit MslEmitterState(const MslTargetOptions& opts) : options(opts) {}

    void Indent() { indentLevel++; }
    void Dedent() { if (indentLevel > 0) indentLevel--; }

    void AppendIndent() {
        for (std::size_t i = 0; i < indentLevel; ++i) {
            body << "    ";
        }
    }

    void Append(std::string_view text) {
        body << text;
    }

    void AppendLine(std::string_view line) {
        AppendIndent();
        body << line << "\n";
    }

    void AppendLine() {
        body << "\n";
    }

    std::string GetValueName(const IrValue* val) {
        if (!val) return "0";
        auto it = valueNames.find(val);
        if (it != valueNames.end()) {
            return it->second;
        }
        std::string name = "v" + std::to_string(valueCounter++);
        valueNames[val] = name;
        return name;
    }

    [[nodiscard]] std::string BuildString() const {
        return header.str() + declarations.str() + body.str();
    }

    std::stringstream header;
    std::stringstream declarations;
    std::stringstream body;
    MslTargetOptions options;

private:
    std::size_t indentLevel = 0;
    std::uint32_t valueCounter = 0;
    std::unordered_map<const IrValue*, std::string> valueNames;
};

} // namespace ShaderRecompiler::Msl

#endif
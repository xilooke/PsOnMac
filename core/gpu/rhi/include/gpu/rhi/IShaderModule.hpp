#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_ISHADERMODULE_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_ISHADERMODULE_HPP

#include <cstddef>
#include <cstdint>
#include <span>
#include <string_view>

namespace AnyPS5::Gpu::Rhi {

enum class ShaderStageFlag : std::uint32_t {
    Vertex = 1 << 0,
    Fragment = 1 << 1,
    Compute = 1 << 2,
    Geometry = 1 << 3,
    TessControl = 1 << 4,
    TessEval = 1 << 5,
    Mesh = 1 << 6
};

inline constexpr ShaderStageFlag operator|(ShaderStageFlag a, ShaderStageFlag b) {
    return static_cast<ShaderStageFlag>(static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));
}

inline constexpr ShaderStageFlag operator&(ShaderStageFlag a, ShaderStageFlag b) {
    return static_cast<ShaderStageFlag>(static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b));
}

struct ShaderModuleDesc {
    ShaderStageFlag stage = ShaderStageFlag::Compute;
    std::span<const std::uint32_t> spirvCode{};
    std::string_view mslSource{};
    std::string_view entryPoint = "main";
};

class IShaderModule {
public:
    virtual ~IShaderModule() = default;
    [[nodiscard]] virtual ShaderStageFlag GetStage() const = 0;
    [[nodiscard]] virtual std::string_view GetEntryPoint() const = 0;
};

} // namespace AnyPS5::Gpu::Rhi

#endif
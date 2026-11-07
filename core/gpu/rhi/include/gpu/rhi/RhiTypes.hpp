#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_RHITYPES_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_RHITYPES_HPP

#include <cstddef>
#include <cstdint>

namespace AnyPS5::Gpu::Rhi {

enum class BackendType : std::uint8_t {
    Vulkan,
    Metal,
    Null
};

enum class QueueType : std::uint8_t {
    Graphics,
    Compute,
    Transfer
};

enum class Format : std::uint32_t {
    Undefined = 0,
    R8_UNORM,
    R8_SNORM,
    R8_UINT,
    R8_SINT,
    R8G8_UNORM,
    R8G8_SNORM,
    R8G8_UINT,
    R8G8_SINT,
    R8G8B8A8_UNORM,
    R8G8B8A8_SRGB,
    R8G8B8A8_SNORM,
    R8G8B8A8_UINT,
    R8G8B8A8_SINT,
    B8G8R8A8_UNORM,
    B8G8R8A8_SRGB,
    R10G10B10A2_UNORM,
    R10G10B10A2_UINT,
    R11G11B10_FLOAT,
    R16_FLOAT,
    R16_UNORM,
    R16_UINT,
    R16_SINT,
    R16G16_FLOAT,
    R16G16_UNORM,
    R16G16_UINT,
    R16G16_SINT,
    R16G16B16A16_FLOAT,
    R16G16B16A16_UNORM,
    R16G16B16A16_UINT,
    R16G16B16A16_SINT,
    R32_FLOAT,
    R32_UINT,
    R32_SINT,
    R32G32_FLOAT,
    R32G32_UINT,
    R32G32_SINT,
    R32G32B32A32_FLOAT,
    R32G32B32A32_UINT,
    R32G32B32A32_SINT,
    D16_UNORM,
    D32_FLOAT,
    D32_FLOAT_S8_UINT,
    D24_UNORM_S8_UINT,
    BC1_RGBA_UNORM,
    BC1_RGBA_SRGB,
    BC2_RGBA_UNORM,
    BC2_RGBA_SRGB,
    BC3_RGBA_UNORM,
    BC3_RGBA_SRGB,
    BC4_UNORM,
    BC4_SNORM,
    BC5_UNORM,
    BC5_SNORM,
    BC6H_UFLOAT,
    BC6H_SFLOAT,
    BC7_RGBA_UNORM,
    BC7_RGBA_SRGB
};

enum class TextureType : std::uint8_t {
    Texture1D,
    Texture2D,
    Texture3D,
    TextureCube,
    Texture1DArray,
    Texture2DArray,
    TextureCubeArray
};

enum class BufferUsage : std::uint32_t {
    None = 0,
    Vertex = 1 << 0,
    Index = 1 << 1,
    Uniform = 1 << 2,
    Storage = 1 << 3,
    Indirect = 1 << 4,
    TransferSrc = 1 << 5,
    TransferDst = 1 << 6
};

inline constexpr BufferUsage operator|(BufferUsage a, BufferUsage b) {
    return static_cast<BufferUsage>(static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));
}

inline constexpr BufferUsage operator&(BufferUsage a, BufferUsage b) {
    return static_cast<BufferUsage>(static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b));
}

enum class TextureUsage : std::uint32_t {
    None = 0,
    Sampled = 1 << 0,
    Storage = 1 << 1,
    RenderTarget = 1 << 2,
    DepthStencil = 1 << 3,
    TransferSrc = 1 << 4,
    TransferDst = 1 << 5
};

inline constexpr TextureUsage operator|(TextureUsage a, TextureUsage b) {
    return static_cast<TextureUsage>(static_cast<std::uint32_t>(a) | static_cast<std::uint32_t>(b));
}

inline constexpr TextureUsage operator&(TextureUsage a, TextureUsage b) {
    return static_cast<TextureUsage>(static_cast<std::uint32_t>(a) & static_cast<std::uint32_t>(b));
}

enum class PrimitiveTopology : std::uint8_t {
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip,
    TriangleFan,
    PatchList
};

enum class CullMode : std::uint8_t {
    None,
    Front,
    Back,
    FrontAndBack
};

enum class FrontFace : std::uint8_t {
    CounterClockwise,
    Clockwise
};

enum class CompareOp : std::uint8_t {
    Never,
    Less,
    Equal,
    LessOrEqual,
    Greater,
    NotEqual,
    GreaterOrEqual,
    Always
};

enum class Filter : std::uint8_t {
    Nearest,
    Linear
};

enum class SamplerAddressMode : std::uint8_t {
    Repeat,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
    MirrorClampToEdge
};

enum class BlendFactor : std::uint8_t {
    Zero,
    One,
    SrcColor,
    OneMinusSrcColor,
    DstColor,
    OneMinusDstColor,
    SrcAlpha,
    OneMinusSrcAlpha,
    DstAlpha,
    OneMinusDstAlpha,
    ConstantColor,
    OneMinusConstantColor,
    ConstantAlpha,
    OneMinusConstantAlpha,
    SrcAlphaSaturate,
    Src1Color,
    OneMinusSrc1Color,
    Src1Alpha,
    OneMinusSrc1Alpha
};

enum class BlendOp : std::uint8_t {
    Add,
    Subtract,
    ReverseSubtract,
    Min,
    Max
};

struct Viewport {
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    float minDepth = 0.0f;
    float maxDepth = 1.0f;
};

struct Scissor {
    std::int32_t x = 0;
    std::int32_t y = 0;
    std::uint32_t width = 0;
    std::uint32_t height = 0;
};

struct ClearColor {
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;
    float a = 0.0f;
};

struct ClearDepthStencil {
    float depth = 1.0f;
    std::uint32_t stencil = 0;
};

} // namespace AnyPS5::Gpu::Rhi

#endif
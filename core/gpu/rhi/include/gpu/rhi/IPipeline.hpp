#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_IPIPELINE_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_IPIPELINE_HPP

#include "RhiTypes.hpp"
#include "IShaderModule.hpp"
#include <memory>
#include <vector>

namespace AnyPS5::Gpu::Rhi {

struct BlendAttachmentState {
    bool blendEnable = false;
    BlendFactor srcColorBlendFactor = BlendFactor::One;
    BlendFactor dstColorBlendFactor = BlendFactor::Zero;
    BlendOp colorBlendOp = BlendOp::Add;
    BlendFactor srcAlphaBlendFactor = BlendFactor::One;
    BlendFactor dstAlphaBlendFactor = BlendFactor::Zero;
    BlendOp alphaBlendOp = BlendOp::Add;
    std::uint8_t colorWriteMask = 0xF;
};

struct RasterizerState {
    CullMode cullMode = CullMode::None;
    FrontFace frontFace = FrontFace::CounterClockwise;
    bool depthClampEnable = false;
    bool depthBiasEnable = false;
    float depthBiasConstantFactor = 0.0f;
    float depthBiasClamp = 0.0f;
    float depthBiasSlopeFactor = 0.0f;
    float lineWidth = 1.0f;
};

struct DepthStencilState {
    bool depthTestEnable = false;
    bool depthWriteEnable = false;
    CompareOp depthCompareOp = CompareOp::Always;
    bool stencilTestEnable = false;
};

struct GraphicsPipelineDesc {
    std::shared_ptr<IShaderModule> vertexShader;
    std::shared_ptr<IShaderModule> fragmentShader;
    PrimitiveTopology topology = PrimitiveTopology::TriangleList;
    RasterizerState rasterizer{};
    DepthStencilState depthStencil{};
    std::vector<BlendAttachmentState> blendAttachments{};
    std::vector<Format> colorAttachmentFormats{};
    Format depthAttachmentFormat = Format::Undefined;
    std::uint32_t sampleCount = 1;
};

struct ComputePipelineDesc {
    std::shared_ptr<IShaderModule> computeShader;
};

class IPipeline {
public:
    virtual ~IPipeline() = default;
    [[nodiscard]] virtual bool IsCompute() const = 0;
};

} // namespace AnyPS5::Gpu::Rhi

#endif
#ifndef CORE_GPU_METAL_INCLUDE_GPU_METAL_METALPIPELINE_HPP
#define CORE_GPU_METAL_INCLUDE_GPU_METAL_METALPIPELINE_HPP

#include "gpu/rhi/IPipeline.hpp"

namespace AnyPS5::Gpu::Metal {

class MetalPipeline final : public Rhi::IPipeline {
public:
    MetalPipeline(void* mtlDevice, const Rhi::GraphicsPipelineDesc& desc);
    MetalPipeline(void* mtlDevice, const Rhi::ComputePipelineDesc& desc);
    ~MetalPipeline() override;

    [[nodiscard]] bool IsCompute() const override { return isCompute; }

    [[nodiscard]] void* GetRenderPipelineState() const { return nativeRenderState; }
    [[nodiscard]] void* GetComputePipelineState() const { return nativeComputeState; }
    [[nodiscard]] void* GetDepthStencilState() const { return nativeDepthStencilState; }
    [[nodiscard]] Rhi::PrimitiveTopology GetTopology() const { return topology; }
    [[nodiscard]] const Rhi::RasterizerState& GetRasterizerState() const { return rasterizer; }

private:
    bool isCompute = false;
    void* nativeRenderState = nullptr;
    void* nativeComputeState = nullptr;
    void* nativeDepthStencilState = nullptr;
    Rhi::PrimitiveTopology topology = Rhi::PrimitiveTopology::TriangleList;
    Rhi::RasterizerState rasterizer{};
};

} // namespace AnyPS5::Gpu::Metal

#endif

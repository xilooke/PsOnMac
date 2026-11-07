#include "gpu/metal/MetalDevice.hpp"
#include <cassert>
#include <string_view>

namespace {

constexpr std::string_view computeShaderSource = R"(
#include <metal_stdlib>
using namespace metal;

kernel void test_compute(device uint* outData [[buffer(0)]], uint tid [[thread_position_in_grid]]) {
    outData[tid] = tid * 2;
}
)";

constexpr std::string_view renderShaderSource = R"(
#include <metal_stdlib>
using namespace metal;

struct VertexOut {
    float4 position [[position]];
    float4 color;
};

vertex VertexOut test_vert(uint vid [[vertex_id]]) {
    VertexOut out;
    float2 positions[3] = { float2(0.0, 0.5), float2(-0.5, -0.5), float2(0.5, -0.5) };
    out.position = float4(positions[vid], 0.0, 1.0);
    out.color = float4(1.0, 0.0, 0.0, 1.0);
    return out;
}

fragment float4 test_frag(VertexOut in [[stage_in]]) {
    return in.color;
}
)";

} // namespace

int main() {
    auto device = AnyPS5::Gpu::Metal::CreateMetalDevice();
    assert(device != nullptr);

    AnyPS5::Gpu::Rhi::ShaderModuleDesc compShaderDesc{};
    compShaderDesc.stage = AnyPS5::Gpu::Rhi::ShaderStageFlag::Compute;
    compShaderDesc.mslSource = computeShaderSource;
    compShaderDesc.entryPoint = "test_compute";

    auto compShader = device->CreateShaderModule(compShaderDesc);
    assert(compShader != nullptr);

    AnyPS5::Gpu::Rhi::ComputePipelineDesc compPipeDesc{};
    compPipeDesc.computeShader = compShader;
    auto compPipeline = device->CreateComputePipeline(compPipeDesc);
    assert(compPipeline != nullptr);
    assert(compPipeline->IsCompute());

    AnyPS5::Gpu::Rhi::BufferDesc bufDesc{};
    bufDesc.size = 64 * sizeof(std::uint32_t);
    bufDesc.usage = AnyPS5::Gpu::Rhi::BufferUsage::Storage;
    auto outBuffer = device->CreateBuffer(bufDesc);
    assert(outBuffer != nullptr);

    std::uint32_t* mapped = static_cast<std::uint32_t*>(outBuffer->Map());
    for (std::size_t i = 0; i < 64; ++i) {
        mapped[i] = 0;
    }
    outBuffer->Unmap();

    auto descTable = device->CreateDescriptorTable();
    descTable->SetBuffer(0, outBuffer, 0);

    auto computeQueue = device->GetQueue(AnyPS5::Gpu::Rhi::QueueType::Compute);
    assert(computeQueue != nullptr);

    auto recorder = device->CreateCommandRecorder(AnyPS5::Gpu::Rhi::QueueType::Compute);
    assert(recorder != nullptr);

    recorder->Begin();
    recorder->SetPipeline(compPipeline);
    recorder->SetDescriptorTable(0, descTable);
    recorder->Dispatch(2, 1, 1);
    recorder->End();

    AnyPS5::Gpu::Rhi::SubmitInfo submitInfo{};
    const std::shared_ptr<AnyPS5::Gpu::Rhi::ICommandRecorder> recorders[] = { recorder };
    submitInfo.commandRecorders = recorders;
    computeQueue->Submit(submitInfo);
    computeQueue->WaitIdle();

    mapped = static_cast<std::uint32_t*>(outBuffer->Map());
    for (std::size_t i = 0; i < 64; ++i) {
        assert(mapped[i] == static_cast<std::uint32_t>(i * 2));
    }
    outBuffer->Unmap();

    AnyPS5::Gpu::Rhi::ShaderModuleDesc vertDesc{};
    vertDesc.stage = AnyPS5::Gpu::Rhi::ShaderStageFlag::Vertex;
    vertDesc.mslSource = renderShaderSource;
    vertDesc.entryPoint = "test_vert";
    auto vertShader = device->CreateShaderModule(vertDesc);
    assert(vertShader != nullptr);

    AnyPS5::Gpu::Rhi::ShaderModuleDesc fragDesc{};
    fragDesc.stage = AnyPS5::Gpu::Rhi::ShaderStageFlag::Fragment;
    fragDesc.mslSource = renderShaderSource;
    fragDesc.entryPoint = "test_frag";
    auto fragShader = device->CreateShaderModule(fragDesc);
    assert(fragShader != nullptr);

    AnyPS5::Gpu::Rhi::GraphicsPipelineDesc gfxPipeDesc{};
    gfxPipeDesc.vertexShader = vertShader;
    gfxPipeDesc.fragmentShader = fragShader;
    gfxPipeDesc.colorAttachmentFormats = { AnyPS5::Gpu::Rhi::Format::R8G8B8A8_UNORM };
    auto gfxPipeline = device->CreateGraphicsPipeline(gfxPipeDesc);
    assert(gfxPipeline != nullptr);
    assert(!gfxPipeline->IsCompute());

    AnyPS5::Gpu::Rhi::TextureDesc rtDesc{};
    rtDesc.type = AnyPS5::Gpu::Rhi::TextureType::Texture2D;
    rtDesc.width = 256;
    rtDesc.height = 256;
    rtDesc.depth = 1;
    rtDesc.mipLevels = 1;
    rtDesc.arrayLayers = 1;
    rtDesc.format = AnyPS5::Gpu::Rhi::Format::R8G8B8A8_UNORM;
    rtDesc.usage = AnyPS5::Gpu::Rhi::TextureUsage::RenderTarget | AnyPS5::Gpu::Rhi::TextureUsage::Sampled;
    auto renderTarget = device->CreateTexture(rtDesc);
    assert(renderTarget != nullptr);

    auto graphicsQueue = device->GetQueue(AnyPS5::Gpu::Rhi::QueueType::Graphics);
    assert(graphicsQueue != nullptr);

    auto gfxRecorder = device->CreateCommandRecorder(AnyPS5::Gpu::Rhi::QueueType::Graphics);
    assert(gfxRecorder != nullptr);

    gfxRecorder->Begin();

    AnyPS5::Gpu::Rhi::RenderPassDesc passDesc{};
    AnyPS5::Gpu::Rhi::RenderPassDesc::ColorAttachment colorAtt{};
    colorAtt.texture = renderTarget;
    colorAtt.clear = true;
    colorAtt.store = true;
    colorAtt.clearColor = { 0.0f, 0.0f, 1.0f, 1.0f };
    passDesc.colorAttachments.push_back(colorAtt);

    gfxRecorder->BeginRenderPass(passDesc);
    gfxRecorder->SetPipeline(gfxPipeline);

    AnyPS5::Gpu::Rhi::Viewport vp{ 0.0f, 0.0f, 256.0f, 256.0f, 0.0f, 1.0f };
    gfxRecorder->SetViewport(vp);

    AnyPS5::Gpu::Rhi::Scissor sc{ 0, 0, 256, 256 };
    gfxRecorder->SetScissor(sc);

    gfxRecorder->Draw(3, 1, 0, 0);
    gfxRecorder->EndRenderPass();
    gfxRecorder->End();

    const std::shared_ptr<AnyPS5::Gpu::Rhi::ICommandRecorder> gfxRecorders[] = { gfxRecorder };
    submitInfo.commandRecorders = gfxRecorders;
    graphicsQueue->Submit(submitInfo);
    graphicsQueue->WaitIdle();

    AnyPS5::Gpu::Rhi::SwapchainDesc swDesc{};
    swDesc.width = 256;
    swDesc.height = 256;
    swDesc.bufferCount = 2;
    swDesc.format = AnyPS5::Gpu::Rhi::Format::B8G8R8A8_UNORM;

    auto swapchain = device->CreateSwapchain(swDesc);
    assert(swapchain != nullptr);
    assert(swapchain->GetWidth() == 256);
    assert(swapchain->GetHeight() == 256);

    std::uint32_t imgIndex = 0;
    bool acquired = swapchain->AcquireNextImage(imgIndex, 100000000);
    assert(acquired);
    auto swapchainTex = swapchain->GetTexture(imgIndex);
    assert(swapchainTex != nullptr);
    swapchain->Present(imgIndex);

    swapchain->Resize(512, 512);
    assert(swapchain->GetWidth() == 512);
    assert(swapchain->GetHeight() == 512);

    return 0;
}

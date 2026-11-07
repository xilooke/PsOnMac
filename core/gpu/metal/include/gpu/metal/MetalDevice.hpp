#ifndef CORE_GPU_METAL_INCLUDE_GPU_METAL_METALDEVICE_HPP
#define CORE_GPU_METAL_INCLUDE_GPU_METAL_METALDEVICE_HPP

#include "gpu/rhi/IDevice.hpp"

namespace AnyPS5::Gpu::Metal {

class MetalDevice final : public Rhi::IDevice {
public:
    explicit MetalDevice(void* mtlDevice = nullptr);
    ~MetalDevice() override;

    [[nodiscard]] Rhi::BackendType GetBackendType() const override { return Rhi::BackendType::Metal; }
    [[nodiscard]] const Rhi::Caps& GetCaps() const override { return caps; }

    [[nodiscard]] std::shared_ptr<Rhi::IQueue> GetQueue(Rhi::QueueType type) override;

    [[nodiscard]] std::shared_ptr<Rhi::IBuffer> CreateBuffer(const Rhi::BufferDesc& desc) override;
    [[nodiscard]] std::shared_ptr<Rhi::ITexture> CreateTexture(const Rhi::TextureDesc& desc) override;
    [[nodiscard]] std::shared_ptr<Rhi::ISampler> CreateSampler(const Rhi::SamplerDesc& desc) override;
    [[nodiscard]] std::shared_ptr<Rhi::IShaderModule> CreateShaderModule(const Rhi::ShaderModuleDesc& desc) override;

    [[nodiscard]] std::shared_ptr<Rhi::IPipeline> CreateGraphicsPipeline(const Rhi::GraphicsPipelineDesc& desc) override;
    [[nodiscard]] std::shared_ptr<Rhi::IPipeline> CreateComputePipeline(const Rhi::ComputePipelineDesc& desc) override;

    [[nodiscard]] std::shared_ptr<Rhi::IPipelineCache> CreatePipelineCache(std::span<const std::uint8_t> initialData) override;
    [[nodiscard]] std::shared_ptr<Rhi::IFence> CreateFence(bool signaled = false) override;
    [[nodiscard]] std::shared_ptr<Rhi::ITimelineSemaphore> CreateTimelineSemaphore(std::uint64_t initialValue = 0) override;

    [[nodiscard]] std::shared_ptr<Rhi::ISwapchain> CreateSwapchain(const Rhi::SwapchainDesc& desc) override;
    [[nodiscard]] std::shared_ptr<Rhi::IDescriptorTable> CreateDescriptorTable() override;
    [[nodiscard]] std::shared_ptr<Rhi::ICommandRecorder> CreateCommandRecorder(Rhi::QueueType queueType) override;

    void WaitIdle() override;

    [[nodiscard]] void* GetNativeDevice() const { return nativeDevice; }

private:
    void InitCaps();

    void* nativeDevice = nullptr;
    Rhi::Caps caps{};
    std::shared_ptr<Rhi::IQueue> graphicsQueue;
    std::shared_ptr<Rhi::IQueue> computeQueue;
    std::shared_ptr<Rhi::IQueue> transferQueue;
};

std::shared_ptr<Rhi::IDevice> CreateMetalDevice(void* mtlDevice = nullptr);

} // namespace AnyPS5::Gpu::Metal

#endif

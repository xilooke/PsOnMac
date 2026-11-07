#ifndef CORE_GPU_RHI_INCLUDE_GPU_RHI_IDEVICE_HPP
#define CORE_GPU_RHI_INCLUDE_GPU_RHI_IDEVICE_HPP

#include "RhiTypes.hpp"
#include "Caps.hpp"
#include "IBuffer.hpp"
#include "ITexture.hpp"
#include "ISampler.hpp"
#include "IShaderModule.hpp"
#include "IPipeline.hpp"
#include "IPipelineCache.hpp"
#include "IFence.hpp"
#include "ISwapchain.hpp"
#include "IDescriptorTable.hpp"
#include "ICommandRecorder.hpp"
#include "IQueue.hpp"
#include <memory>
#include <span>

namespace AnyPS5::Gpu::Rhi {

class IDevice {
public:
    virtual ~IDevice() = default;

    [[nodiscard]] virtual BackendType GetBackendType() const = 0;
    [[nodiscard]] virtual const Caps& GetCaps() const = 0;

    [[nodiscard]] virtual std::shared_ptr<IQueue> GetQueue(QueueType type) = 0;

    [[nodiscard]] virtual std::shared_ptr<IBuffer> CreateBuffer(const BufferDesc& desc) = 0;
    [[nodiscard]] virtual std::shared_ptr<ITexture> CreateTexture(const TextureDesc& desc) = 0;
    [[nodiscard]] virtual std::shared_ptr<ISampler> CreateSampler(const SamplerDesc& desc) = 0;
    [[nodiscard]] virtual std::shared_ptr<IShaderModule> CreateShaderModule(const ShaderModuleDesc& desc) = 0;

    [[nodiscard]] virtual std::shared_ptr<IPipeline> CreateGraphicsPipeline(const GraphicsPipelineDesc& desc) = 0;
    [[nodiscard]] virtual std::shared_ptr<IPipeline> CreateComputePipeline(const ComputePipelineDesc& desc) = 0;

    [[nodiscard]] virtual std::shared_ptr<IPipelineCache> CreatePipelineCache(std::span<const std::uint8_t> initialData) = 0;
    [[nodiscard]] virtual std::shared_ptr<IFence> CreateFence(bool signaled = false) = 0;
    [[nodiscard]] virtual std::shared_ptr<ITimelineSemaphore> CreateTimelineSemaphore(std::uint64_t initialValue = 0) = 0;

    [[nodiscard]] virtual std::shared_ptr<ISwapchain> CreateSwapchain(const SwapchainDesc& desc) = 0;
    [[nodiscard]] virtual std::shared_ptr<IDescriptorTable> CreateDescriptorTable() = 0;
    [[nodiscard]] virtual std::shared_ptr<ICommandRecorder> CreateCommandRecorder(QueueType queueType) = 0;

    virtual void WaitIdle() = 0;
};

} // namespace AnyPS5::Gpu::Rhi

#endif
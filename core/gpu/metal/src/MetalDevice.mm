#include "gpu/metal/MetalDevice.hpp"
#include "gpu/metal/MetalBuffer.hpp"
#include "gpu/metal/MetalCommandRecorder.hpp"
#include "gpu/metal/MetalDescriptorTable.hpp"
#include "gpu/metal/MetalFence.hpp"
#include "gpu/metal/MetalPipeline.hpp"
#include "gpu/metal/MetalPipelineCache.hpp"
#include "gpu/metal/MetalQueue.hpp"
#include "gpu/metal/MetalSampler.hpp"
#include "gpu/metal/MetalShaderModule.hpp"
#include "gpu/metal/MetalSwapchain.hpp"
#include "gpu/metal/MetalTexture.hpp"
#import <Metal/Metal.h>
#include <stdexcept>

namespace AnyPS5::Gpu::Metal {

MetalDevice::MetalDevice(void* mtlDevice) {
    id<MTLDevice> device = nil;
    if (mtlDevice != nullptr) {
        device = (__bridge id<MTLDevice>)mtlDevice;
    } else {
        device = MTLCreateSystemDefaultDevice();
    }

    if (!device) {
        throw std::runtime_error("No Metal device available");
    }

    nativeDevice = (__bridge_retained void*)device;
    InitCaps();

    graphicsQueue = std::make_shared<MetalQueue>(nativeDevice, Rhi::QueueType::Graphics);
    computeQueue = std::make_shared<MetalQueue>(nativeDevice, Rhi::QueueType::Compute);
    transferQueue = std::make_shared<MetalQueue>(nativeDevice, Rhi::QueueType::Transfer);
}

MetalDevice::~MetalDevice() {
    graphicsQueue.reset();
    computeQueue.reset();
    transferQueue.reset();

    if (nativeDevice) {
        id<MTLDevice> device = (__bridge_transfer id<MTLDevice>)nativeDevice;
        device = nil;
        nativeDevice = nullptr;
    }
}

void MetalDevice::InitCaps() {
    id<MTLDevice> device = (__bridge id<MTLDevice>)nativeDevice;

    caps.hasUnifiedMemory = [device hasUnifiedMemory];
    caps.minWaveSize = 32;
    caps.maxWaveSize = 32;
    caps.supportsWave32 = true;
    caps.supportsWave64 = false;

    if (@available(macOS 11.0, *)) {
        caps.supportsMeshShaders = [device supportsFamily:MTLGPUFamilyApple7];
    } else {
        caps.supportsMeshShaders = false;
    }

    caps.supportsBcTextureCompression = true;
    caps.supportsD32FloatS8Uint = true;
    caps.supportsD24UnormS8Uint = false;
    caps.argumentBufferTier = Rhi::ArgumentBufferTier::Tier2;

    caps.maxTextureDimension1D = 16384;
    caps.maxTextureDimension2D = 16384;
    caps.maxTextureDimension3D = 2048;
    caps.maxTextureDimensionCube = 16384;
    caps.maxTextureArrayLayers = 2048;

    caps.maxBufferSize = 1073741824ULL;
    caps.maxComputeSharedMemoryBytes = 32768;
    caps.maxComputeWorkGroupInvocations = 1024;
    caps.maxComputeWorkGroupSize[0] = 1024;
    caps.maxComputeWorkGroupSize[1] = 1024;
    caps.maxComputeWorkGroupSize[2] = 64;
}

std::shared_ptr<Rhi::IQueue> MetalDevice::GetQueue(Rhi::QueueType type) {
    switch (type) {
    case Rhi::QueueType::Graphics:
        return graphicsQueue;
    case Rhi::QueueType::Compute:
        return computeQueue;
    case Rhi::QueueType::Transfer:
        return transferQueue;
    }
}

std::shared_ptr<Rhi::IBuffer> MetalDevice::CreateBuffer(const Rhi::BufferDesc& desc) {
    return std::make_shared<MetalBuffer>(nativeDevice, desc);
}

std::shared_ptr<Rhi::ITexture> MetalDevice::CreateTexture(const Rhi::TextureDesc& desc) {
    return std::make_shared<MetalTexture>(nativeDevice, desc);
}

std::shared_ptr<Rhi::ISampler> MetalDevice::CreateSampler(const Rhi::SamplerDesc& desc) {
    return std::make_shared<MetalSampler>(nativeDevice, desc);
}

std::shared_ptr<Rhi::IShaderModule> MetalDevice::CreateShaderModule(const Rhi::ShaderModuleDesc& desc) {
    return std::make_shared<MetalShaderModule>(nativeDevice, desc);
}

std::shared_ptr<Rhi::IPipeline> MetalDevice::CreateGraphicsPipeline(const Rhi::GraphicsPipelineDesc& desc) {
    return std::make_shared<MetalPipeline>(nativeDevice, desc);
}

std::shared_ptr<Rhi::IPipeline> MetalDevice::CreateComputePipeline(const Rhi::ComputePipelineDesc& desc) {
    return std::make_shared<MetalPipeline>(nativeDevice, desc);
}

std::shared_ptr<Rhi::IPipelineCache> MetalDevice::CreatePipelineCache(std::span<const std::uint8_t> initialData) {
    return std::make_shared<MetalPipelineCache>(nativeDevice, initialData);
}

std::shared_ptr<Rhi::IFence> MetalDevice::CreateFence(bool signaled) {
    return std::make_shared<MetalFence>(nativeDevice, signaled);
}

std::shared_ptr<Rhi::ITimelineSemaphore> MetalDevice::CreateTimelineSemaphore(std::uint64_t initialValue) {
    return std::make_shared<MetalTimelineSemaphore>(nativeDevice, initialValue);
}

std::shared_ptr<Rhi::ISwapchain> MetalDevice::CreateSwapchain(const Rhi::SwapchainDesc& desc) {
    return std::make_shared<MetalSwapchain>(nativeDevice, desc);
}

std::shared_ptr<Rhi::IDescriptorTable> MetalDevice::CreateDescriptorTable() {
    return std::make_shared<MetalDescriptorTable>();
}

std::shared_ptr<Rhi::ICommandRecorder> MetalDevice::CreateCommandRecorder(Rhi::QueueType queueType) {
    auto q = GetQueue(queueType);
    auto* mtlQ = dynamic_cast<MetalQueue*>(q.get());
    return std::make_shared<MetalCommandRecorder>(nativeDevice, mtlQ->GetNativeQueue(), queueType);
}

void MetalDevice::WaitIdle() {
    if (graphicsQueue) graphicsQueue->WaitIdle();
    if (computeQueue) computeQueue->WaitIdle();
    if (transferQueue) transferQueue->WaitIdle();
}

std::shared_ptr<Rhi::IDevice> CreateMetalDevice(void* mtlDevice) {
    return std::make_shared<MetalDevice>(mtlDevice);
}

} // namespace AnyPS5::Gpu::Metal

namespace AnyPS5::Gpu::Rhi {

std::shared_ptr<IDevice> CreateDevice(BackendType type) {
    if (type == BackendType::Metal) {
        return Metal::CreateMetalDevice();
    }
    throw std::runtime_error("Unsupported backend type for platform");
}

} // namespace AnyPS5::Gpu::Rhi

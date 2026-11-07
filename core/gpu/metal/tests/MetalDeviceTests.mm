#include "gpu/metal/MetalDevice.hpp"
#include <cassert>

int main() {
    auto device = AnyPS5::Gpu::Metal::CreateMetalDevice();
    assert(device != nullptr);
    assert(device->GetBackendType() == AnyPS5::Gpu::Rhi::BackendType::Metal);

    const auto& caps = device->GetCaps();
    assert(caps.supportsWave32);
    assert(caps.minWaveSize == 32);
    assert(caps.maxTextureDimension2D >= 16384);

    auto graphicsQueue = device->GetQueue(AnyPS5::Gpu::Rhi::QueueType::Graphics);
    assert(graphicsQueue != nullptr);
    assert(graphicsQueue->GetType() == AnyPS5::Gpu::Rhi::QueueType::Graphics);

    auto computeQueue = device->GetQueue(AnyPS5::Gpu::Rhi::QueueType::Compute);
    assert(computeQueue != nullptr);
    assert(computeQueue->GetType() == AnyPS5::Gpu::Rhi::QueueType::Compute);

    auto transferQueue = device->GetQueue(AnyPS5::Gpu::Rhi::QueueType::Transfer);
    assert(transferQueue != nullptr);
    assert(transferQueue->GetType() == AnyPS5::Gpu::Rhi::QueueType::Transfer);

    auto semaphore = device->CreateTimelineSemaphore(42);
    assert(semaphore != nullptr);
    assert(semaphore->GetCurrentValue() == 42);
    semaphore->Signal(100);
    assert(semaphore->GetCurrentValue() == 100);
    assert(semaphore->Wait(100, 1000000));

    auto fence = device->CreateFence(false);
    assert(fence != nullptr);
    assert(!fence->IsSignaled());
    fence->Reset();

    device->WaitIdle();
    return 0;
}

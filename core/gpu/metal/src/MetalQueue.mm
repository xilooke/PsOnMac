#include "gpu/metal/MetalQueue.hpp"
#include "gpu/metal/MetalCommandRecorder.hpp"
#include "gpu/metal/MetalFence.hpp"
#import <Metal/Metal.h>
#include <stdexcept>

namespace AnyPS5::Gpu::Metal {

MetalQueue::MetalQueue(void* mtlDevice, Rhi::QueueType queueType)
    : type(queueType) {
    id<MTLDevice> device = (__bridge id<MTLDevice>)mtlDevice;
    if (!device) {
        throw std::runtime_error("Invalid MTLDevice for MetalQueue");
    }

    id<MTLCommandQueue> queue = [device newCommandQueue];
    if (!queue) {
        throw std::runtime_error("Failed to allocate MTLCommandQueue");
    }

    nativeQueue = (__bridge_retained void*)queue;
}

MetalQueue::~MetalQueue() {
    if (nativeQueue) {
        id<MTLCommandQueue> queue = (__bridge_transfer id<MTLCommandQueue>)nativeQueue;
        queue = nil;
        nativeQueue = nullptr;
    }
}

void MetalQueue::Submit(const Rhi::SubmitInfo& submitInfo) {
    for (const auto& rec : submitInfo.commandRecorders) {
        if (!rec) {
            continue;
        }
        auto* mtlRec = dynamic_cast<MetalCommandRecorder*>(rec.get());
        if (!mtlRec || !mtlRec->GetNativeCommandBuffer()) {
            continue;
        }

        id<MTLCommandBuffer> cb = (__bridge id<MTLCommandBuffer>)mtlRec->GetNativeCommandBuffer();

        for (const auto& waitFence : submitInfo.waitFences) {
            auto* mf = dynamic_cast<MetalFence*>(waitFence.get());
            if (mf && mf->GetSharedEvent()) {
                id<MTLSharedEvent> ev = (__bridge id<MTLSharedEvent>)mf->GetSharedEvent();
                [cb encodeWaitForEvent:ev value:mf->GetTargetValue()];
            }
        }

        for (const auto& signalFence : submitInfo.signalFences) {
            auto* mf = dynamic_cast<MetalFence*>(signalFence.get());
            if (mf && mf->GetSharedEvent()) {
                id<MTLSharedEvent> ev = (__bridge id<MTLSharedEvent>)mf->GetSharedEvent();
                mf->PrepareNextSignal();
                [cb encodeSignalEvent:ev value:mf->GetTargetValue()];
            }
        }

        if (submitInfo.completionFence) {
            auto* mf = dynamic_cast<MetalFence*>(submitInfo.completionFence.get());
            if (mf && mf->GetSharedEvent()) {
                id<MTLSharedEvent> ev = (__bridge id<MTLSharedEvent>)mf->GetSharedEvent();
                mf->PrepareNextSignal();
                [cb encodeSignalEvent:ev value:mf->GetTargetValue()];
            }
        }

        [cb commit];
    }
}

void MetalQueue::WaitIdle() {
    id<MTLCommandQueue> queue = (__bridge id<MTLCommandQueue>)nativeQueue;
    if (!queue) {
        return;
    }
    id<MTLCommandBuffer> cb = [queue commandBuffer];
    if (cb) {
        [cb commit];
        [cb waitUntilCompleted];
    }
}

} // namespace AnyPS5::Gpu::Metal

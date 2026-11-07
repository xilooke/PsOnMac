#include "gpu/metal/MetalFence.hpp"
#import <Metal/Metal.h>
#include <dispatch/dispatch.h>
#include <stdexcept>

namespace AnyPS5::Gpu::Metal {

MetalFence::MetalFence(void* mtlDevice, bool signaled) {
    id<MTLDevice> device = (__bridge id<MTLDevice>)mtlDevice;
    if (!device) {
        throw std::runtime_error("Invalid MTLDevice for MetalFence");
    }

    id<MTLSharedEvent> event = [device newSharedEvent];
    if (!event) {
        throw std::runtime_error("Failed to allocate MTLSharedEvent");
    }

    if (signaled) {
        event.signaledValue = 1;
        targetValue.store(1);
    } else {
        event.signaledValue = 0;
        targetValue.store(1);
    }

    nativeEvent = (__bridge_retained void*)event;
}

MetalFence::~MetalFence() {
    if (nativeEvent) {
        id<MTLSharedEvent> event = (__bridge_transfer id<MTLSharedEvent>)nativeEvent;
        event = nil;
        nativeEvent = nullptr;
    }
}

void MetalFence::Wait(std::uint64_t timeoutNanoseconds) {
    id<MTLSharedEvent> event = (__bridge id<MTLSharedEvent>)nativeEvent;
    std::uint64_t target = targetValue.load();
    if ([event signaledValue] >= target) {
        return;
    }

    dispatch_semaphore_t sem = dispatch_semaphore_create(0);
    MTLSharedEventListener* listener = [[MTLSharedEventListener alloc] init];
    [event notifyListener:listener atValue:target block:^(id<MTLSharedEvent>, uint64_t) {
        dispatch_semaphore_signal(sem);
    }];

    dispatch_time_t dTimeout = (timeoutNanoseconds == UINT64_MAX)
                                   ? DISPATCH_TIME_FOREVER
                                   : dispatch_time(DISPATCH_TIME_NOW, static_cast<int64_t>(timeoutNanoseconds));
    dispatch_semaphore_wait(sem, dTimeout);
}

void MetalFence::Reset() {
    id<MTLSharedEvent> event = (__bridge id<MTLSharedEvent>)nativeEvent;
    std::uint64_t current = targetValue.load();
    std::uint64_t next = current + 1;
    event.signaledValue = current;
    targetValue.store(next);
}

bool MetalFence::IsSignaled() const {
    id<MTLSharedEvent> event = (__bridge id<MTLSharedEvent>)nativeEvent;
    return [event signaledValue] >= targetValue.load();
}

void MetalFence::PrepareNextSignal() {
    id<MTLSharedEvent> event = (__bridge id<MTLSharedEvent>)nativeEvent;
    std::uint64_t current = targetValue.load();
    if ([event signaledValue] >= current) {
        targetValue.store(current + 1);
    }
}

MetalTimelineSemaphore::MetalTimelineSemaphore(void* mtlDevice, std::uint64_t initialValue) {
    id<MTLDevice> device = (__bridge id<MTLDevice>)mtlDevice;
    if (!device) {
        throw std::runtime_error("Invalid MTLDevice for MetalTimelineSemaphore");
    }

    id<MTLSharedEvent> event = [device newSharedEvent];
    if (!event) {
        throw std::runtime_error("Failed to allocate MTLSharedEvent for timeline semaphore");
    }

    event.signaledValue = initialValue;
    nativeEvent = (__bridge_retained void*)event;
}

MetalTimelineSemaphore::~MetalTimelineSemaphore() {
    if (nativeEvent) {
        id<MTLSharedEvent> event = (__bridge_transfer id<MTLSharedEvent>)nativeEvent;
        event = nil;
        nativeEvent = nullptr;
    }
}

std::uint64_t MetalTimelineSemaphore::GetCurrentValue() const {
    id<MTLSharedEvent> event = (__bridge id<MTLSharedEvent>)nativeEvent;
    return [event signaledValue];
}

void MetalTimelineSemaphore::Signal(std::uint64_t value) {
    id<MTLSharedEvent> event = (__bridge id<MTLSharedEvent>)nativeEvent;
    event.signaledValue = value;
}

bool MetalTimelineSemaphore::Wait(std::uint64_t value, std::uint64_t timeoutNanoseconds) {
    id<MTLSharedEvent> event = (__bridge id<MTLSharedEvent>)nativeEvent;
    if ([event signaledValue] >= value) {
        return true;
    }

    dispatch_semaphore_t sem = dispatch_semaphore_create(0);
    MTLSharedEventListener* listener = [[MTLSharedEventListener alloc] init];
    [event notifyListener:listener atValue:value block:^(id<MTLSharedEvent>, uint64_t) {
        dispatch_semaphore_signal(sem);
    }];

    dispatch_time_t dTimeout = (timeoutNanoseconds == UINT64_MAX)
                                   ? DISPATCH_TIME_FOREVER
                                   : dispatch_time(DISPATCH_TIME_NOW, static_cast<int64_t>(timeoutNanoseconds));
    long status = dispatch_semaphore_wait(sem, dTimeout);
    return status == 0;
}

} // namespace AnyPS5::Gpu::Metal

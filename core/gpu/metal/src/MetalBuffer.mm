#include "gpu/metal/MetalBuffer.hpp"
#import <Metal/Metal.h>
#include <cstring>
#include <stdexcept>

namespace AnyPS5::Gpu::Metal {

MetalBuffer::MetalBuffer(void* mtlDevice, const Rhi::BufferDesc& desc)
    : size(desc.size), usage(desc.usage) {
    id<MTLDevice> device = (__bridge id<MTLDevice>)mtlDevice;
    if (!device) {
        throw std::runtime_error("Invalid MTLDevice for MetalBuffer creation");
    }

    id<MTLBuffer> buf = nil;
    if (desc.existingHostAddress != nullptr) {
        std::uintptr_t addr = reinterpret_cast<std::uintptr_t>(desc.existingHostAddress);
        if ((addr & 0x3FFF) == 0 && (desc.size & 0x3FFF) == 0) {
            buf = [device newBufferWithBytesNoCopy:desc.existingHostAddress
                                            length:desc.size
                                           options:MTLResourceStorageModeShared
                                       deallocator:nil];
        }
    }

    if (!buf) {
        buf = [device newBufferWithLength:desc.size options:MTLResourceStorageModeShared];
        if (desc.existingHostAddress != nullptr && buf != nil) {
            std::memcpy([buf contents], desc.existingHostAddress, desc.size);
        }
    }

    if (!buf) {
        throw std::runtime_error("Failed to allocate MTLBuffer");
    }

    nativeBuffer = (__bridge_retained void*)buf;
}

MetalBuffer::~MetalBuffer() {
    if (nativeBuffer) {
        id<MTLBuffer> buf = (__bridge_transfer id<MTLBuffer>)nativeBuffer;
        buf = nil;
        nativeBuffer = nullptr;
    }
}

std::size_t MetalBuffer::GetSize() const {
    return size;
}

Rhi::BufferUsage MetalBuffer::GetUsage() const {
    return usage;
}

void* MetalBuffer::Map() {
    id<MTLBuffer> buf = (__bridge id<MTLBuffer>)nativeBuffer;
    return [buf contents];
}

void MetalBuffer::Unmap() {
}

std::uint64_t MetalBuffer::GetDeviceAddress() const {
    id<MTLBuffer> buf = (__bridge id<MTLBuffer>)nativeBuffer;
    if (@available(macOS 13.0, *)) {
        return [buf gpuAddress];
    }
    return 0;
}

} // namespace AnyPS5::Gpu::Metal

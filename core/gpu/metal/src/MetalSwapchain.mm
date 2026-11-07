#include "gpu/metal/MetalSwapchain.hpp"
#include "gpu/metal/MetalFormatMap.hpp"
#include "gpu/metal/MetalTexture.hpp"
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import <AppKit/AppKit.h>
#include <stdexcept>

namespace AnyPS5::Gpu::Metal {

MetalSwapchain::MetalSwapchain(void* mtlDevice, const Rhi::SwapchainDesc& description)
    : desc(description) {
    id<MTLDevice> device = (__bridge id<MTLDevice>)mtlDevice;
    if (!device) {
        throw std::runtime_error("Invalid MTLDevice for MetalSwapchain");
    }

    CAMetalLayer* layer = nil;
    if (desc.nativeWindowHandle != nullptr) {
        id obj = (__bridge id)desc.nativeWindowHandle;
        if ([obj isKindOfClass:[CAMetalLayer class]]) {
            layer = (CAMetalLayer*)obj;
        } else if ([obj isKindOfClass:[NSView class]]) {
            NSView* view = (NSView*)obj;
            if ([view.layer isKindOfClass:[CAMetalLayer class]]) {
                layer = (CAMetalLayer*)view.layer;
            } else {
                layer = [CAMetalLayer layer];
                view.layer = layer;
                view.wantsLayer = YES;
            }
        }
    }

    if (!layer) {
        layer = [CAMetalLayer layer];
    }

    layer.device = device;
    layer.pixelFormat = ToMetalPixelFormat(desc.format);
    layer.drawableSize = CGSizeMake(desc.width, desc.height);
    layer.maximumDrawableCount = desc.bufferCount;
    layer.displaySyncEnabled = desc.vsync ? YES : NO;
    layer.presentsWithTransaction = NO;

    nativeLayer = (__bridge_retained void*)layer;
    activeDrawables.resize(desc.bufferCount, nullptr);
    currentTextures.resize(desc.bufferCount, nullptr);
}

MetalSwapchain::~MetalSwapchain() {
    for (auto& drawable : activeDrawables) {
        if (drawable) {
            id<CAMetalDrawable> d = (__bridge_transfer id<CAMetalDrawable>)drawable;
            d = nil;
            drawable = nullptr;
        }
    }
    if (nativeLayer) {
        CAMetalLayer* layer = (__bridge_transfer CAMetalLayer*)nativeLayer;
        layer = nil;
        nativeLayer = nullptr;
    }
}

bool MetalSwapchain::AcquireNextImage(std::uint32_t& imageIndex, std::uint64_t timeoutNanoseconds) {
    (void)timeoutNanoseconds;
    CAMetalLayer* layer = (__bridge CAMetalLayer*)nativeLayer;
    if (!layer) {
        return false;
    }

    id<CAMetalDrawable> drawable = [layer nextDrawable];
    if (!drawable) {
        return false;
    }

    for (std::uint32_t i = 0; i < activeDrawables.size(); ++i) {
        if (activeDrawables[i] == nullptr) {
            imageIndex = i;
            activeDrawables[i] = (__bridge_retained void*)drawable;
            currentTextures[i] = std::make_shared<MetalTexture>((__bridge void*)drawable.texture);
            return true;
        }
    }

    imageIndex = 0;
    if (activeDrawables[0]) {
        id<CAMetalDrawable> old = (__bridge_transfer id<CAMetalDrawable>)activeDrawables[0];
        old = nil;
    }
    activeDrawables[0] = (__bridge_retained void*)drawable;
    currentTextures[0] = std::make_shared<MetalTexture>((__bridge void*)drawable.texture);
    return true;
}

void MetalSwapchain::Present(std::uint32_t imageIndex) {
    if (imageIndex >= activeDrawables.size() || !activeDrawables[imageIndex]) {
        return;
    }

    id<CAMetalDrawable> drawable = (__bridge_transfer id<CAMetalDrawable>)activeDrawables[imageIndex];
    activeDrawables[imageIndex] = nullptr;
    currentTextures[imageIndex].reset();
    [drawable present];
}

void MetalSwapchain::Resize(std::uint32_t width, std::uint32_t height) {
    desc.width = width;
    desc.height = height;
    CAMetalLayer* layer = (__bridge CAMetalLayer*)nativeLayer;
    if (layer) {
        layer.drawableSize = CGSizeMake(width, height);
    }
}

std::shared_ptr<Rhi::ITexture> MetalSwapchain::GetTexture(std::uint32_t imageIndex) const {
    if (imageIndex < currentTextures.size()) {
        return currentTextures[imageIndex];
    }
    return nullptr;
}

} // namespace AnyPS5::Gpu::Metal

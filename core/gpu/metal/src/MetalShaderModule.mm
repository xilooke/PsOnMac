#include "gpu/metal/MetalShaderModule.hpp"
#import <Metal/Metal.h>
#include <stdexcept>

namespace AnyPS5::Gpu::Metal {

MetalShaderModule::MetalShaderModule(void* mtlDevice, const Rhi::ShaderModuleDesc& desc)
    : stage(desc.stage), entryPoint(desc.entryPoint) {
    id<MTLDevice> device = (__bridge id<MTLDevice>)mtlDevice;
    if (!device) {
        throw std::runtime_error("Invalid MTLDevice for MetalShaderModule");
    }

    if (desc.mslSource.empty()) {
        throw std::runtime_error("MSL source code is required for MetalShaderModule");
    }

    NSString* source = [NSString stringWithUTF8String:std::string(desc.mslSource).c_str()];
    MTLCompileOptions* options = [MTLCompileOptions new];
    options.fastMathEnabled = NO;

    NSError* error = nil;
    id<MTLLibrary> library = [device newLibraryWithSource:source options:options error:&error];
    if (!library || error) {
        std::string errMsg = error ? [[error localizedDescription] UTF8String] : "Unknown MSL compile error";
        throw std::runtime_error("Failed to compile MSL shader: " + errMsg);
    }

    NSString* funcName = [NSString stringWithUTF8String:entryPoint.c_str()];
    id<MTLFunction> function = [library newFunctionWithName:funcName];
    if (!function) {
        throw std::runtime_error("Entry point not found in MSL library: " + entryPoint);
    }

    nativeLibrary = (__bridge_retained void*)library;
    nativeFunction = (__bridge_retained void*)function;
}

MetalShaderModule::~MetalShaderModule() {
    if (nativeFunction) {
        id<MTLFunction> func = (__bridge_transfer id<MTLFunction>)nativeFunction;
        func = nil;
        nativeFunction = nullptr;
    }
    if (nativeLibrary) {
        id<MTLLibrary> lib = (__bridge_transfer id<MTLLibrary>)nativeLibrary;
        lib = nil;
        nativeLibrary = nullptr;
    }
}

} // namespace AnyPS5::Gpu::Metal

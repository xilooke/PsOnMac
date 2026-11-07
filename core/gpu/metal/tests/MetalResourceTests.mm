#include "gpu/metal/MetalDevice.hpp"
#include <cassert>
#include <cstdlib>
#include <cstring>

int main() {
    auto device = AnyPS5::Gpu::Metal::CreateMetalDevice();
    assert(device != nullptr);

    AnyPS5::Gpu::Rhi::BufferDesc bufDesc{};
    bufDesc.size = 65536;
    bufDesc.usage = AnyPS5::Gpu::Rhi::BufferUsage::Storage;
    auto buffer = device->CreateBuffer(bufDesc);
    assert(buffer != nullptr);
    assert(buffer->GetSize() == 65536);

    void* mapped = buffer->Map();
    assert(mapped != nullptr);
    std::uint32_t* u32Ptr = static_cast<std::uint32_t*>(mapped);
    u32Ptr[0] = 0x12345678;
    assert(u32Ptr[0] == 0x12345678);
    buffer->Unmap();

    void* hostMem = nullptr;
    int memalignRes = posix_memalign(&hostMem, 16384, 16384);
    assert(memalignRes == 0 && hostMem != nullptr);
    std::memset(hostMem, 0xAB, 16384);

    AnyPS5::Gpu::Rhi::BufferDesc zeroCopyDesc{};
    zeroCopyDesc.size = 16384;
    zeroCopyDesc.usage = AnyPS5::Gpu::Rhi::BufferUsage::Storage;
    zeroCopyDesc.existingHostAddress = hostMem;

    auto zeroCopyBuffer = device->CreateBuffer(zeroCopyDesc);
    assert(zeroCopyBuffer != nullptr);
    void* zcMapped = zeroCopyBuffer->Map();
    assert(zcMapped == hostMem);
    assert(static_cast<std::uint8_t*>(zcMapped)[0] == 0xAB);
    zeroCopyBuffer->Unmap();
    std::free(hostMem);

    AnyPS5::Gpu::Rhi::TextureDesc texDesc{};
    texDesc.type = AnyPS5::Gpu::Rhi::TextureType::Texture2D;
    texDesc.width = 256;
    texDesc.height = 256;
    texDesc.depth = 1;
    texDesc.mipLevels = 1;
    texDesc.arrayLayers = 1;
    texDesc.format = AnyPS5::Gpu::Rhi::Format::R8G8B8A8_UNORM;
    texDesc.usage = AnyPS5::Gpu::Rhi::TextureUsage::Sampled | AnyPS5::Gpu::Rhi::TextureUsage::RenderTarget;

    auto texture = device->CreateTexture(texDesc);
    assert(texture != nullptr);
    assert(texture->GetWidth() == 256);
    assert(texture->GetHeight() == 256);
    assert(texture->GetFormat() == AnyPS5::Gpu::Rhi::Format::R8G8B8A8_UNORM);

    AnyPS5::Gpu::Rhi::TextureDesc depthDesc{};
    depthDesc.type = AnyPS5::Gpu::Rhi::TextureType::Texture2D;
    depthDesc.width = 256;
    depthDesc.height = 256;
    depthDesc.depth = 1;
    depthDesc.mipLevels = 1;
    depthDesc.arrayLayers = 1;
    depthDesc.format = AnyPS5::Gpu::Rhi::Format::D32_FLOAT_S8_UINT;
    depthDesc.usage = AnyPS5::Gpu::Rhi::TextureUsage::DepthStencil;

    auto depthTex = device->CreateTexture(depthDesc);
    assert(depthTex != nullptr);
    assert(depthTex->GetFormat() == AnyPS5::Gpu::Rhi::Format::D32_FLOAT_S8_UINT);

    AnyPS5::Gpu::Rhi::SamplerDesc sampDesc{};
    sampDesc.minFilter = AnyPS5::Gpu::Rhi::Filter::Linear;
    sampDesc.magFilter = AnyPS5::Gpu::Rhi::Filter::Linear;
    sampDesc.mipmapMode = AnyPS5::Gpu::Rhi::Filter::Linear;
    sampDesc.addressU = AnyPS5::Gpu::Rhi::SamplerAddressMode::ClampToEdge;
    sampDesc.addressV = AnyPS5::Gpu::Rhi::SamplerAddressMode::ClampToEdge;
    sampDesc.addressW = AnyPS5::Gpu::Rhi::SamplerAddressMode::ClampToEdge;

    auto sampler = device->CreateSampler(sampDesc);
    assert(sampler != nullptr);

    auto descTable = device->CreateDescriptorTable();
    assert(descTable != nullptr);
    descTable->SetBuffer(0, buffer, 0);
    descTable->SetTexture(1, texture);
    descTable->SetSampler(2, sampler);
    descTable->Flush();

    return 0;
}

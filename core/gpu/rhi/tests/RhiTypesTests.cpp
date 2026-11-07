#include "gpu/rhi/RhiTypes.hpp"
#include "gpu/rhi/Caps.hpp"
#include <cassert>
#include <cstdlib>

int main() {
    using namespace AnyPS5::Gpu::Rhi;

    BufferUsage usage = BufferUsage::Vertex | BufferUsage::Uniform;
    if ((usage & BufferUsage::Vertex) != BufferUsage::Vertex) {
        return 1;
    }
    if ((usage & BufferUsage::Index) == BufferUsage::Index) {
        return 2;
    }

    TextureUsage texUsage = TextureUsage::Sampled | TextureUsage::RenderTarget;
    if ((texUsage & TextureUsage::Sampled) != TextureUsage::Sampled) {
        return 3;
    }

    Caps caps{};
    if (caps.minWaveSize != 32) {
        return 4;
    }
    if (caps.maxTextureDimension2D < 4096) {
        return 5;
    }

    return 0;
}

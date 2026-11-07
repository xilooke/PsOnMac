#include "gpu/rhi/IDevice.hpp"
#include <memory>

namespace {

class MockBuffer final : public AnyPS5::Gpu::Rhi::IBuffer {
public:
    explicit MockBuffer(std::size_t size) : bufferSize(size) {}
    [[nodiscard]] std::size_t GetSize() const override { return bufferSize; }
    [[nodiscard]] AnyPS5::Gpu::Rhi::BufferUsage GetUsage() const override { return AnyPS5::Gpu::Rhi::BufferUsage::Vertex; }
    [[nodiscard]] void* Map() override { return nullptr; }
    void Unmap() override {}
    [[nodiscard]] std::uint64_t GetDeviceAddress() const override { return 0x1000; }
private:
    std::size_t bufferSize = 0;
};

} // namespace

int main() {
    std::shared_ptr<AnyPS5::Gpu::Rhi::IBuffer> buffer = std::make_shared<MockBuffer>(1024);
    if (buffer->GetSize() != 1024) {
        return 1;
    }
    if (buffer->GetDeviceAddress() != 0x1000) {
        return 2;
    }
    return 0;
}

#include <platform/macos/MachExceptionHandler.hpp>
#include <platform/macos/MachVmMemory.hpp>
#include <cassert>
#include <unistd.h>

int main() {
    using namespace Platform::MacOs;

    std::size_t pageSize = getpagesize();
    std::uintptr_t page = 0;
    bool allocOk = MachVmMemory::Allocate(page, pageSize, true);
    assert(allocOk);

    bool protOk = MachVmMemory::Protect(page, pageSize, true, false, false);
    assert(protOk);

    bool handled = false;
    auto callback = [&](std::uintptr_t faultAddr, bool isWrite) {
        if (faultAddr >= page && faultAddr < page + pageSize) {
            MachVmMemory::Protect(page, pageSize, true, true, false);
            handled = true;
            return true;
        }
        return false;
    };

    auto& handler = MachExceptionHandler::Instance();
    bool started = handler.Start(callback);
    assert(started);
    assert(handler.IsRunning());

    int* ptr = reinterpret_cast<int*>(page);
    *ptr = 12345;
    assert(*ptr == 12345);
    assert(handled);

    handler.Stop();
    assert(!handler.IsRunning());

    MachVmMemory::Deallocate(page, pageSize);
    return 0;
}

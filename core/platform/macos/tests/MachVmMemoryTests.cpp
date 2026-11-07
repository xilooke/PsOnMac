#include <platform/macos/MachVmMemory.hpp>
#include <cassert>
#include <unistd.h>

int main() {
    using namespace Platform::MacOs;
    std::size_t size = getpagesize() * 4;
    std::uintptr_t addr = 0;

    bool allocOk = MachVmMemory::Allocate(addr, size, true);
    assert(allocOk);
    assert(addr != 0);

    bool protectOk = MachVmMemory::Protect(addr, size, true, true, false);
    assert(protectOk);

    int* ptr = reinterpret_cast<int*>(addr);
    *ptr = 42;
    assert(*ptr == 42);

    std::uintptr_t remapAddr = 0;
    bool remapOk = MachVmMemory::Remap(remapAddr, size, addr, false);
    assert(remapOk);
    assert(remapAddr != 0);

    int* remapPtr = reinterpret_cast<int*>(remapAddr);
    assert(*remapPtr == 42);
    *remapPtr = 84;
    assert(*ptr == 84);

    bool deallocRemapOk = MachVmMemory::Deallocate(remapAddr, size);
    assert(deallocRemapOk);

    bool deallocOk = MachVmMemory::Deallocate(addr, size);
    assert(deallocOk);

    return 0;
}

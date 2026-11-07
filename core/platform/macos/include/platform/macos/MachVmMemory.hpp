#ifndef PLATFORM_MACOS_MACHVMMEMORY_HPP
#define PLATFORM_MACOS_MACHVMMEMORY_HPP

#include <cstddef>
#include <cstdint>

namespace Platform::MacOs {

class MachVmMemory {
public:
    static bool Allocate(std::uintptr_t& address, std::size_t size, bool anywhere);
    static bool Deallocate(std::uintptr_t address, std::size_t size);
    static bool Remap(std::uintptr_t& targetAddress, std::size_t size, std::uintptr_t sourceAddress, bool copyOnWrite);
    static bool Protect(std::uintptr_t address, std::size_t size, bool readable, bool writable, bool executable);
};

} // namespace Platform::MacOs

#endif

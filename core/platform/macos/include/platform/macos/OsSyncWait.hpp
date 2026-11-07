#ifndef PLATFORM_MACOS_OSSYNCWAIT_HPP
#define PLATFORM_MACOS_OSSYNCWAIT_HPP

#include <cstddef>
#include <cstdint>

namespace Platform::MacOs {

class OsSyncWait {
public:
    static bool WaitOnAddress(const volatile void* address, std::uint64_t expectedValue, std::uint32_t timeoutUs = 0, std::size_t size = sizeof(std::uint64_t));
    static bool WakeByAddress(const volatile void* address, bool wakeAll = false, std::size_t size = sizeof(std::uint64_t));
};

} // namespace Platform::MacOs

#endif

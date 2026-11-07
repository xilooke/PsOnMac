#ifndef PLATFORM_MACOS_OSSYNCWAIT_HPP
#define PLATFORM_MACOS_OSSYNCWAIT_HPP

#include <cstdint>

namespace Platform::MacOs {

class OsSyncWait {
public:
    static bool WaitOnAddress(const volatile void* address, std::uint64_t expectedValue, std::uint32_t timeoutUs);
    static bool WakeByAddress(const volatile void* address, bool wakeAll);
};

} // namespace Platform::MacOs

#endif

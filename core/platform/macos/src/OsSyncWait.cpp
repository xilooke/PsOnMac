#include <platform/macos/OsSyncWait.hpp>

#ifdef __APPLE__
#include <cstdint>

extern "C" {
int __ulock_wait(std::uint32_t operation, void* addr, std::uint64_t value, std::uint32_t timeout_us);
int __ulock_wake(std::uint32_t operation, void* addr, std::uint64_t wake_value);
}

namespace {
constexpr std::uint32_t kUlCompareAndWait = 1;
constexpr std::uint32_t kUlfWakeAll = 0x00000100;
}
#endif

namespace Platform::MacOs {

bool OsSyncWait::WaitOnAddress(const volatile void* address, std::uint64_t expectedValue, std::uint32_t timeoutUs) {
#ifdef __APPLE__
    const int ret = __ulock_wait(kUlCompareAndWait, const_cast<void*>(address), expectedValue, timeoutUs);
    return ret >= 0;
#else
    (void)address;
    (void)expectedValue;
    (void)timeoutUs;
    return false;
#endif
}

bool OsSyncWait::WakeByAddress(const volatile void* address, bool wakeAll) {
#ifdef __APPLE__
    const std::uint32_t op = kUlCompareAndWait | (wakeAll ? kUlfWakeAll : 0);
    const int ret = __ulock_wake(op, const_cast<void*>(address), 0);
    return ret >= 0;
#else
    (void)address;
    (void)wakeAll;
    return false;
#endif
}

} // namespace Platform::MacOs

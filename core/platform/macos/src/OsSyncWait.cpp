#include <platform/macos/OsSyncWait.hpp>

#ifdef __APPLE__
#include <cerrno>
#include <os/os_sync_wait_on_address.h>

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

bool OsSyncWait::WaitOnAddress(const volatile void* address, std::uint64_t expectedValue, std::uint32_t timeoutUs, std::size_t size) {
#ifdef __APPLE__
    void* addr = const_cast<void*>(address);
    if (size != 4 && size != 8) {
        size = 8;
    }
    if (__builtin_available(macOS 14.4, *)) {
        int ret = 0;
        if (timeoutUs == 0) {
            ret = os_sync_wait_on_address(addr, expectedValue, size, OS_SYNC_WAIT_ON_ADDRESS_NONE);
        } else {
            const uint64_t timeoutNs = static_cast<uint64_t>(timeoutUs) * 1000ULL;
            ret = os_sync_wait_on_address_with_timeout(addr, expectedValue, size, OS_SYNC_WAIT_ON_ADDRESS_NONE, OS_CLOCK_MACH_ABSOLUTE_TIME, timeoutNs);
        }
        return ret == 0;
    }
    const int ret = __ulock_wait(kUlCompareAndWait, addr, expectedValue, timeoutUs);
    return ret >= 0;
#else
    (void)address;
    (void)expectedValue;
    (void)timeoutUs;
    (void)size;
    return false;
#endif
}

bool OsSyncWait::WakeByAddress(const volatile void* address, bool wakeAll, std::size_t size) {
#ifdef __APPLE__
    void* addr = const_cast<void*>(address);
    if (size != 4 && size != 8) {
        size = 8;
    }
    if (__builtin_available(macOS 14.4, *)) {
        int ret = 0;
        if (wakeAll) {
            ret = os_sync_wake_by_address_all(addr, size, OS_SYNC_WAKE_BY_ADDRESS_NONE);
        } else {
            ret = os_sync_wake_by_address_any(addr, size, OS_SYNC_WAKE_BY_ADDRESS_NONE);
        }
        return ret == 0 || errno == ENOENT;
    }
    const std::uint32_t op = kUlCompareAndWait | (wakeAll ? kUlfWakeAll : 0);
    const int ret = __ulock_wake(op, addr, 0);
    return ret >= 0;
#else
    (void)address;
    (void)wakeAll;
    (void)size;
    return false;
#endif
}

} // namespace Platform::MacOs

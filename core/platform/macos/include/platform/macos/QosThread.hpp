#ifndef PLATFORM_MACOS_QOSTHREAD_HPP
#define PLATFORM_MACOS_QOSTHREAD_HPP

#include <cstdint>

namespace Platform::MacOs {

enum class ThreadQosClass {
    UserInteractive,
    UserInitiated,
    Default,
    Utility,
    Background
};

class QosThread {
public:
    static ThreadQosClass FromGuestPriority(int prio);
    static bool SetCurrentThreadQos(ThreadQosClass qos, int relativePriority = 0);
    static bool SetCurrentThreadGuestPriority(int prio);
    static bool SetCurrentThreadAffinityTag(std::uint32_t tag);
    static bool SetThreadAffinityTag(void* nativePthread, std::uint32_t tag);
};

} // namespace Platform::MacOs

#endif

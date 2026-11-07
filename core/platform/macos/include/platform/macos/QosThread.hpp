#ifndef PLATFORM_MACOS_QOSTHREAD_HPP
#define PLATFORM_MACOS_QOSTHREAD_HPP

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
    static bool SetCurrentThreadQos(ThreadQosClass qos, int relativePriority = 0);
};

} // namespace Platform::MacOs

#endif

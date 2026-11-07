#include <platform/macos/QosThread.hpp>

#ifdef __APPLE__
#include <pthread.h>
#include <sys/qos.h>
#include <mach/mach.h>
#include <mach/thread_policy.h>
#endif

namespace Platform::MacOs {

ThreadQosClass QosThread::FromGuestPriority(int prio) {
    if (prio < 512) {
        return ThreadQosClass::UserInteractive;
    }
    if (prio < 650) {
        return ThreadQosClass::UserInitiated;
    }
    if (prio < 800) {
        return ThreadQosClass::Default;
    }
    if (prio < 900) {
        return ThreadQosClass::Utility;
    }
    return ThreadQosClass::Background;
}

bool QosThread::SetCurrentThreadQos(ThreadQosClass qos, int relativePriority) {
#ifdef __APPLE__
    qos_class_t qosClass = QOS_CLASS_DEFAULT;
    switch (qos) {
    case ThreadQosClass::UserInteractive:
        qosClass = QOS_CLASS_USER_INTERACTIVE;
        break;
    case ThreadQosClass::UserInitiated:
        qosClass = QOS_CLASS_USER_INITIATED;
        break;
    case ThreadQosClass::Default:
        qosClass = QOS_CLASS_DEFAULT;
        break;
    case ThreadQosClass::Utility:
        qosClass = QOS_CLASS_UTILITY;
        break;
    case ThreadQosClass::Background:
        qosClass = QOS_CLASS_BACKGROUND;
        break;
    }
    const int ret = pthread_set_qos_class_self_np(qosClass, relativePriority);
    return ret == 0;
#else
    (void)qos;
    (void)relativePriority;
    return false;
#endif
}

bool QosThread::SetCurrentThreadGuestPriority(int prio) {
    return SetCurrentThreadQos(FromGuestPriority(prio));
}

bool QosThread::SetCurrentThreadAffinityTag(std::uint32_t tag) {
#ifdef __APPLE__
    mach_port_t machThread = pthread_mach_thread_np(pthread_self());
    thread_affinity_policy_data_t policy = { static_cast<integer_t>(tag) };
    kern_return_t kr = thread_policy_set(machThread, THREAD_AFFINITY_POLICY, reinterpret_cast<thread_policy_t>(&policy), THREAD_AFFINITY_POLICY_COUNT);
    return kr == KERN_SUCCESS;
#else
    (void)tag;
    return false;
#endif
}

bool QosThread::SetThreadAffinityTag(void* nativePthread, std::uint32_t tag) {
#ifdef __APPLE__
    if (!nativePthread) {
        return false;
    }
    pthread_t thread = static_cast<pthread_t>(nativePthread);
    mach_port_t machThread = pthread_mach_thread_np(thread);
    thread_affinity_policy_data_t policy = { static_cast<integer_t>(tag) };
    kern_return_t kr = thread_policy_set(machThread, THREAD_AFFINITY_POLICY, reinterpret_cast<thread_policy_t>(&policy), THREAD_AFFINITY_POLICY_COUNT);
    return kr == KERN_SUCCESS;
#else
    (void)nativePthread;
    (void)tag;
    return false;
#endif
}

} // namespace Platform::MacOs

#include <platform/macos/QosThread.hpp>

#ifdef __APPLE__
#include <pthread.h>
#include <sys/qos.h>
#endif

namespace Platform::MacOs {

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

} // namespace Platform::MacOs

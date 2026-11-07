#include <platform/macos/QosThread.hpp>
#include <cassert>
#include <pthread.h>

int main() {
    using namespace Platform::MacOs;

    assert(QosThread::FromGuestPriority(300) == ThreadQosClass::UserInteractive);
    assert(QosThread::FromGuestPriority(600) == ThreadQosClass::UserInitiated);
    assert(QosThread::FromGuestPriority(700) == ThreadQosClass::Default);
    assert(QosThread::FromGuestPriority(850) == ThreadQosClass::Utility);
    assert(QosThread::FromGuestPriority(950) == ThreadQosClass::Background);

    bool ok1 = QosThread::SetCurrentThreadQos(ThreadQosClass::UserInteractive, 0);
    assert(ok1);

    bool ok2 = QosThread::SetCurrentThreadGuestPriority(700);
    assert(ok2);

    bool affOk1 = QosThread::SetCurrentThreadAffinityTag(1);
    assert(affOk1);

    bool affOk2 = QosThread::SetThreadAffinityTag(pthread_self(), 2);
    assert(affOk2);

    return 0;
}

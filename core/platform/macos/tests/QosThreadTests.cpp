#include <platform/macos/QosThread.hpp>
#include <cassert>

int main() {
    using namespace Platform::MacOs;
    bool ok1 = QosThread::SetCurrentThreadQos(ThreadQosClass::UserInteractive, 0);
    assert(ok1);

    bool ok2 = QosThread::SetCurrentThreadQos(ThreadQosClass::Default, 0);
    assert(ok2);

    bool ok3 = QosThread::SetCurrentThreadQos(ThreadQosClass::Background, 0);
    assert(ok3);

    return 0;
}

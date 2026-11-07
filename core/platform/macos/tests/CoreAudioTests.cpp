#include <platform/macos/CoreAudioOut.hpp>
#include <cassert>

int main() {
    using namespace Platform::MacOs;

    bool available = CoreAudioOut::IsDefaultOutputAvailable();
    assert(available);

    bool initOk = CoreAudioOut::Initialize(48000, 2);
    assert(initOk);

    CoreAudioOut::Shutdown();

    return 0;
}

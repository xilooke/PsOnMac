#ifndef PLATFORM_MACOS_COREAUDIOOUT_HPP
#define PLATFORM_MACOS_COREAUDIOOUT_HPP

#include <cstdint>

namespace Platform::MacOs {

class CoreAudioOut {
public:
    static bool IsDefaultOutputAvailable();
    static bool Initialize(std::uint32_t sampleRate = 48000, std::uint32_t channels = 2);
    static void Shutdown();
};

} // namespace Platform::MacOs

#endif

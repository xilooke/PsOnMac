#include <platform/macos/CoreAudioOut.hpp>

#ifdef __APPLE__
#include <AudioToolbox/AudioToolbox.h>
#include <CoreAudio/CoreAudio.h>

namespace {
AudioComponentInstance g_audioUnit = nullptr;
}
#endif

namespace Platform::MacOs {

bool CoreAudioOut::IsDefaultOutputAvailable() {
#ifdef __APPLE__
    AudioComponentDescription desc{};
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_DefaultOutput;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;

    AudioComponent comp = AudioComponentFindNext(nullptr, &desc);
    return comp != nullptr;
#else
    return false;
#endif
}

bool CoreAudioOut::Initialize(std::uint32_t sampleRate, std::uint32_t channels) {
#ifdef __APPLE__
    if (g_audioUnit != nullptr) {
        return true;
    }

    AudioComponentDescription desc{};
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_DefaultOutput;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;

    AudioComponent comp = AudioComponentFindNext(nullptr, &desc);
    if (!comp) {
        return false;
    }

    OSStatus status = AudioComponentInstanceNew(comp, &g_audioUnit);
    if (status != noErr || !g_audioUnit) {
        g_audioUnit = nullptr;
        return false;
    }

    AudioStreamBasicDescription streamDesc{};
    streamDesc.mSampleRate = static_cast<Float64>(sampleRate);
    streamDesc.mFormatID = kAudioFormatLinearPCM;
    streamDesc.mFormatFlags = kAudioFormatFlagIsSignedInteger | kAudioFormatFlagIsPacked;
    streamDesc.mBytesPerPacket = channels * sizeof(std::int16_t);
    streamDesc.mFramesPerPacket = 1;
    streamDesc.mBytesPerFrame = channels * sizeof(std::int16_t);
    streamDesc.mChannelsPerFrame = channels;
    streamDesc.mBitsPerChannel = 16;

    status = AudioUnitSetProperty(g_audioUnit, kAudioUnitProperty_StreamFormat, kAudioUnitScope_Input, 0, &streamDesc, sizeof(streamDesc));
    if (status != noErr) {
        AudioComponentInstanceDispose(g_audioUnit);
        g_audioUnit = nullptr;
        return false;
    }

    status = AudioUnitInitialize(g_audioUnit);
    if (status != noErr) {
        AudioComponentInstanceDispose(g_audioUnit);
        g_audioUnit = nullptr;
        return false;
    }

    return true;
#else
    (void)sampleRate;
    (void)channels;
    return false;
#endif
}

void CoreAudioOut::Shutdown() {
#ifdef __APPLE__
    if (g_audioUnit != nullptr) {
        AudioUnitUninitialize(g_audioUnit);
        AudioComponentInstanceDispose(g_audioUnit);
        g_audioUnit = nullptr;
    }
#endif
}

} // namespace Platform::MacOs

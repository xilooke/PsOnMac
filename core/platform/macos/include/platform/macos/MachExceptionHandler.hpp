#ifndef PLATFORM_MACOS_MACHEXCEPTIONHANDLER_HPP
#define PLATFORM_MACOS_MACHEXCEPTIONHANDLER_HPP

#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>

namespace Platform::MacOs {

using BadAccessCallback = std::function<bool(std::uintptr_t faultAddress, bool isWrite)>;

class MachExceptionHandler {
public:
    static MachExceptionHandler& Instance();
    bool Start(BadAccessCallback callback);
    void Stop();
    [[nodiscard]] bool IsRunning() const;
    bool HandleBadAccess(std::uintptr_t faultAddress, bool isWrite);

private:
    MachExceptionHandler();
    ~MachExceptionHandler();

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace Platform::MacOs

#endif

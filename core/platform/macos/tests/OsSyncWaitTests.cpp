#include <platform/macos/OsSyncWait.hpp>
#include <atomic>
#include <cassert>
#include <thread>
#include <chrono>
#include <memory>

int main() {
    using namespace Platform::MacOs;

    auto syncWord64 = std::make_unique<uint64_t>(0);
    std::atomic<bool> worker64Started{false};
    std::atomic<bool> wait64Finished{false};

    std::thread worker64([&] {
        worker64Started.store(true);
        bool ok = OsSyncWait::WaitOnAddress(syncWord64.get(), 0, 5000000, sizeof(uint64_t));
        assert(ok);
        wait64Finished.store(true);
    });

    while (!worker64Started.load()) {
        std::this_thread::yield();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    *syncWord64 = 1;
    bool wake64Ok = OsSyncWait::WakeByAddress(syncWord64.get(), true, sizeof(uint64_t));
    assert(wake64Ok);

    worker64.join();
    assert(wait64Finished.load());

    auto syncWord32 = std::make_unique<uint32_t>(10);
    std::atomic<bool> worker32Started{false};
    std::atomic<bool> wait32Finished{false};

    std::thread worker32([&] {
        worker32Started.store(true);
        bool ok = OsSyncWait::WaitOnAddress(syncWord32.get(), 10, 5000000, sizeof(uint32_t));
        assert(ok);
        wait32Finished.store(true);
    });

    while (!worker32Started.load()) {
        std::this_thread::yield();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    *syncWord32 = 20;
    bool wake32Ok = OsSyncWait::WakeByAddress(syncWord32.get(), false, sizeof(uint32_t));
    assert(wake32Ok);

    worker32.join();
    assert(wait32Finished.load());

    bool emptyWakeOk = OsSyncWait::WakeByAddress(syncWord32.get(), true, sizeof(uint32_t));
    assert(emptyWakeOk);

    return 0;
}

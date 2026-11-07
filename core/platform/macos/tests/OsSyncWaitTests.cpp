#include <platform/macos/OsSyncWait.hpp>
#include <atomic>
#include <cassert>
#include <thread>
#include <chrono>

int main() {
    using namespace Platform::MacOs;

    alignas(8) volatile std::uint64_t syncWord = 0;
    std::atomic<bool> workerStarted{false};
    std::atomic<bool> waitFinished{false};

    std::thread worker([&] {
        workerStarted.store(true);
        bool ok = OsSyncWait::WaitOnAddress(&syncWord, 0, 5000000);
        assert(ok);
        waitFinished.store(true);
    });

    while (!workerStarted.load()) {
        std::this_thread::yield();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    syncWord = 1;
    bool wakeOk = OsSyncWait::WakeByAddress(&syncWord, true);
    assert(wakeOk);

    worker.join();
    assert(waitFinished.load());

    return 0;
}

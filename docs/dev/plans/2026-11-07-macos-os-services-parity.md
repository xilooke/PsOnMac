# macOS OS Services Parity Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement Phase 5 macOS OS services parity covering `libkernel` (thread affinity/QoS on Darwin, `sceKernelWaitOnAddress` via `OsSyncWait`, timers/kqueue), filesystem semantics (APFS case sensitivity, `F_NOCACHE` streaming reads, file descriptor limit scaling), and audio/multimedia verification.

**Architecture:** Integrate `core/platform/macos/` abstractions (`OsSyncWait`, `QosThread`, `MachVmMemory`) into guest libraries (`libkernel`, `libc`, `libSceAudioOut`), preserving FreeBSD/PS5 binary ABI.

**Tech Stack:** C++20, Objective-C++, POSIX/Darwin APIs (`kqueue`, `kevent`, `pthread_mach_thread_np`, `thread_policy_set`, `setrlimit`, `fcntl(F_NOCACHE)`).

**Spec:** `anyps5-metal-port-prompt.md` (Phase 5 - OS services parity on macOS).

## Global Constraints

- Zero comments in C++ and Objective-C++ code (except `#endif` and namespace-closing markers `} // namespace ...`).
- Fail-fast policy: Unimplemented functions call `NotImplemented_nid_no_patch(__func__)` and throw `std::runtime_error`.
- Dependencies: Only submodules under `3rdparty/` or Apple system frameworks.
- Conventional Commits for commit messages.
- `tools/check_conventions.py --base origin/main` must pass without findings.
- After the whole part is completed, stop and test it first before proceeding to Phase 6.

---

### Task 1: `libkernel` Threading, Affinity & QoS on macOS

**Files:**
- Modify: `core/libs/prx/libkernel/Pthread/src/Thread.cpp`
- Modify: `core/libs/prx/libkernel/Pthread/Posix/Thread.cpp`
- Test: `core/libs/prx/libkernel/Pthread/tests/MacOsThreadTests.cpp`

- [ ] On macOS (`__APPLE__`), route `SetLiveGuestThreadAffinity` and thread affinity updates through `QosThread::SetCurrentThreadAffinityTag` using Mach thread affinity policy.
- [ ] Map thread priority to macOS QoS classes (`QOS_CLASS_USER_INTERACTIVE`, `QOS_CLASS_DEFAULT`, `QOS_CLASS_UTILITY`, `QOS_CLASS_BACKGROUND`).
- [ ] Add unit test verifying thread QoS and affinity application on macOS.

### Task 2: `sceKernelWaitOnAddress` Acceleration via `OsSyncWait`

**Files:**
- Modify: `core/libs/prx/libkernel/SyncOnAddress/src/SyncOnAddress.cpp`
- Test: `core/libs/prx/libkernel/SyncOnAddress/tests/MacOsSyncOnAddressTests.cpp`

- [ ] Integrate `Platform::MacOs::OsSyncWait::WaitOnAddress` and `WakeByAddress` for macOS builds in `SyncOnAddress.cpp` to leverage native Darwin `__ulock_wait` / `os_sync_wait_on_address`.
- [ ] Add unit test verifying wait-on-address and wake-by-address in multi-threaded guest synchronization.

### Task 3: Filesystem, APFS Semantics & File Limits

**Files:**
- Modify: `core/libs/prx/libkernel/File/src/Open.cpp`
- Modify: `core/libs/prx/libc/src/General.cpp`
- Test: `core/libs/prx/libkernel/File/tests/MacOsFileSemanticsTests.cpp`

- [ ] Raise `RLIMIT_NOFILE` soft limit to `OPEN_MAX` / hard limit at initialization on macOS to support high-descriptor PS5 workloads.
- [ ] Add `fcntl(fd, F_NOCACHE, 1)` on macOS for direct I/O and streaming read descriptors.
- [ ] Document APFS case sensitivity handling in `docs/dev/macos/FILESYSTEM.md`.
- [ ] Add unit test verifying descriptor limit expansion and `F_NOCACHE` behavior.

### Task 4: Audio & Multimedia Submodule Verification

**Files:**
- Modify: `core/libs/prx/libSceAudioOut/src/AudioOutSdl.cpp`
- Test: `core/libs/prx/libSceAudioOut/tests/MacOsAudioTests.cpp`

- [ ] Verify SDL2 CoreAudio output backend initialization on macOS.
- [ ] Add unit test verifying audio device querying and PCM buffer submission.

### Task 5: Verification & Stop for Review

- [ ] Run full test suite: `ctest --test-dir build-macos-arm64 --output-on-failure`.
- [ ] Run conventions check: `python3 tools/check_conventions.py --base origin/main`.
- [ ] Confirm zero comments in C++ and Objective-C++ code.
- [ ] Create Conventional Commit: `feat(platform): implement macOS OS services parity for libkernel, fs, and audio`.
- [ ] **Stop and present Phase 5 completion and test verification results to user.**

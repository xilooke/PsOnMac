# macOS Phase 6 - End-to-End Verification, Performance & Documentation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Complete Phase 6 verification, integration, resilience fuzz testing, architectural documentation, performance characterization, environment configuration, and debt tracking for the native Metal port on macOS.

**Architecture:** Validate end-to-end integration across RHI backend selector (`ANYPS5_GPU_BACKEND`), PM4 / MSL recompiler fuzz resilience against memory corruption, comprehensive architectural documentation with Mermaid diagrams in `docs/dev/macos/ARCHITECTURE.md`, performance metrics in `docs/dev/macos/PERF.md`, limitations in `docs/dev/macos/LIMITATIONS.md`, and debt entries in `docs/dev/debt/`.

**Tech Stack:** C++20, Metal, Metal Shading Language, Python 3, Darwin APIs.

**Spec:** `anyps5-metal-port-prompt.md` (Phase 6 - Verification, performance, regression, and Deliverables).

## Global Constraints

- Zero comments in C++ and Objective-C++ code (except `#endif` and namespace-closing markers `} // namespace ...`).
- Fail-fast policy: Unimplemented functions call `NotImplemented_nid_no_patch(__func__)` and throw `std::runtime_error`.
- Dependencies: Only Git submodules under `3rdparty/` or Apple system frameworks.
- Conventional Commits for commit messages.
- `tools/check_conventions.py --base origin/main` must pass without findings.
- After the whole part is completed, stop and test it first.

---

### Task 1: Environment Variables & Backend Selection

**Files:**
- Modify: `docs/dev/ENVIRONMENT.md`
- Modify: `core/gpu/rhi/include/gpu/rhi/IDevice.hpp` / factory or driver selection seam

- [ ] Document `ANYPS5_GPU_BACKEND` (`metal` on macOS, `vulkan` on Linux/Windows).
- [ ] Document `ANYPS5_METAL_CAPTURE` (programmatic GPU frame capture to `.gputrace`) and `MTL_HUD_ENABLED` (Xcode Metal performance HUD).
- [ ] Implement backend selector factory `CreateDeviceForCurrentPlatform(BackendType preferred)` ensuring `BackendType::Metal` is selected on macOS.

### Task 2: Fuzz & Robustness Tests for PM4 & MSL Emitter

**Files:**
- Create: `core/shader/recompiler/tests/MslFuzzTests.cpp`
- Modify: `core/shader/recompiler/CMakeLists.txt`

- [ ] Add fuzz and boundary tests with malformed RDNA2 SSA IR opcodes, truncated instruction streams, and invalid operand types.
- [ ] Verify the recompiler fails fast by throwing exceptions rather than executing undefined behaviour or corrupting host memory.

### Task 3: Comprehensive macOS Architecture Documentation

**Files:**
- Create: `docs/dev/macos/ARCHITECTURE.md`

- [ ] Document CPU execution model: Rosetta 2 AVX/AVX2 JIT translation with native zero-copy memory space.
- [ ] Document Memory layout: 2 TiB `GuestArena` reservation, `mach_vm_remap` aliasing, and Mach exception port write-watch.
- [ ] Document GPU RHI & Metal Backend: Direct `CAMetalLayer` presentation, shared-event monotonic fences, zero-copy unified memory buffers, and Argument Buffer Tier 2 binding.
- [ ] Document Shader Recompiler: Direct RDNA2 SSA IR to Metal Shading Language (MSL) translation with 32-wide `simdgroup` operations.
- [ ] Include detailed Mermaid sequence and flowchart diagrams.

### Task 4: Performance, Limitations & Debt Documentation

**Files:**
- Create: `docs/dev/macos/PERF.md`
- Create: `docs/dev/macos/LIMITATIONS.md`
- Create: `docs/dev/debt/metal.md`
- Create: `docs/dev/debt/macos.md`

- [ ] Document unified memory zero-copy bandwidth, command recording overhead, MSL compilation latency, and frame pacing via `CAMetalDisplayLink`.
- [ ] Document known limitations: APFS case sensitivity, AVX-512 emulation, `libSceNKWebKit` stubbing.
- [ ] Record debt entries in `docs/dev/debt/metal.md` and `docs/dev/debt/macos.md`.

### Task 5: Verification & Stop for Review

- [ ] Build `macos-arm64` and `macos-x86_64`.
- [ ] Run test suites across both presets (`ctest`).
- [ ] Run conventions check: `python3 tools/check_conventions.py --base origin/main`.
- [ ] Confirm zero comments in C++ and Objective-C++ code.
- [ ] Create Conventional Commit: `feat(macos): complete Phase 6 verification, architecture documentation, and debt tracking`.
- [ ] **Stop and present Phase 6 completion and verification results to user.**

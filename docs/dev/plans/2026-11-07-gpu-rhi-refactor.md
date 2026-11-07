# GPU Abstraction Refactor (RHI) Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Establish a clean, zero-overhead Render Hardware Interface (`core/gpu/rhi/`) separating the AGC/PM4 front end from graphics backends, enabling a direct PS5-to-Metal backend on macOS while preserving Vulkan on Linux/Windows with zero regressions.

**Architecture:** Define abstract RHI interfaces (`IDevice`, `IQueue`, `ICommandRecorder`, `IBuffer`, `ITexture`, `ISampler`, `IPipeline`, `IPipelineCache`, `IShaderModule`, `IFence`, `ISwapchain`, `IDescriptorTable`, `Caps`, and `GpuCommand` IR). Create the backend seam so `VulkanBackend` implements RHI for Linux/Windows, and prepare the direct slot for `MetalBackend` on macOS.

**Tech Stack:** C++20, CMake, Ninja, Vulkan 1.3, Metal.

**Spec:** `anyps5-metal-port-prompt.md` (Phase 2: GPU abstraction refactor).

## Global Constraints

- Zero comments in C++ (except `#endif` and namespace-closing markers `} // namespace ...`).
- Fail-fast policy: Unimplemented functions call `NotImplemented_nid_no_patch(__func__)` and throw `std::runtime_error`.
- Dependencies: Only git submodules under `3rdparty/` or Apple system frameworks.
- Conventional Commits for commit messages.
- `tools/check_conventions.py --base origin/main` must pass without findings.
- After the whole part is completed, stop and test it first before proceeding to Phase 3.

## Review Focus

1. RHI interface overhead: Virtual dispatch only at command buffer submission/coarse boundaries, direct/inlined data paths for high-frequency operations.
2. Threading safety: `DevicePointer` and `DeviceUseGate` must maintain atomic load/store semantics across committer and worker threads.
3. Linux/Windows Vulkan parity: Zero behavior change, zero regression in Vulkan pipeline generation or execution.
4. Golden-frame consistency: Rendered outputs on Vulkan must remain bit-identical before and after the RHI refactoring.
5. Metal direct-path readiness: Ensure capability queries (`Caps`) account for Apple Silicon TBDR, unified memory, lack of D24S8, and argument buffer tiers.

---

### Task 1: Create RHI Types and Capability Declarations

**Files:**
- Create: `core/gpu/rhi/include/gpu/rhi/RhiTypes.hpp`
- Create: `core/gpu/rhi/include/gpu/rhi/Caps.hpp`
- Create: `core/gpu/rhi/CMakeLists.txt`
- Modify: `CMakeLists.txt`
- Test: `core/gpu/rhi/tests/RhiTypesTests.cpp`

- [ ] Create `core/gpu/rhi/include/gpu/rhi/RhiTypes.hpp` with enums for `Format`, `QueueType`, `BufferUsage`, `TextureUsage`, `TextureType`, `SampleCount`, `CompareOp`, `Filter`, `SamplerAddressMode`, `PrimitiveTopology`, `CullMode`, `FrontFace`, `BlendFactor`, `BlendOp`, `ColorWriteMask`, and `PipelineStage`.
- [ ] Create `core/gpu/rhi/include/gpu/rhi/Caps.hpp` with `Caps` struct (wave sizes, mesh shaders, tessellation, 64-bit atomics, unified memory, host memory import, argument buffer tiers, depth formats, BCn support).
- [ ] Create `core/gpu/rhi/CMakeLists.txt` declaring `gpu_rhi` interface library.
- [ ] Add `core/gpu/rhi` to root `CMakeLists.txt`.
- [ ] Create unit test `core/gpu/rhi/tests/RhiTypesTests.cpp` verifying capability structures and format querying.
- [ ] Build and verify tests pass.
- [ ] Run `python3 tools/check_conventions.py --base origin/main`.

### Task 2: Define Core RHI Interfaces

**Files:**
- Create: `core/gpu/rhi/include/gpu/rhi/IBuffer.hpp`
- Create: `core/gpu/rhi/include/gpu/rhi/ITexture.hpp`
- Create: `core/gpu/rhi/include/gpu/rhi/ISampler.hpp`
- Create: `core/gpu/rhi/include/gpu/rhi/IShaderModule.hpp`
- Create: `core/gpu/rhi/include/gpu/rhi/IPipeline.hpp`
- Create: `core/gpu/rhi/include/gpu/rhi/IPipelineCache.hpp`
- Create: `core/gpu/rhi/include/gpu/rhi/IFence.hpp`
- Create: `core/gpu/rhi/include/gpu/rhi/ISwapchain.hpp`
- Create: `core/gpu/rhi/include/gpu/rhi/IDescriptorTable.hpp`
- Create: `core/gpu/rhi/include/gpu/rhi/ICommandRecorder.hpp`
- Create: `core/gpu/rhi/include/gpu/rhi/IQueue.hpp`
- Create: `core/gpu/rhi/include/gpu/rhi/IDevice.hpp`
- Test: `core/gpu/rhi/tests/RhiInterfaceTests.cpp`

- [ ] Write abstract interface headers with pure virtual methods for device creation, buffer/texture allocation, pipeline creation, command recording, queue submission, and synchronization.
- [ ] Implement `RhiInterfaceTests.cpp` testing mock implementations of `IDevice`, `IQueue`, `ICommandRecorder` to verify contract completeness.
- [ ] Build and verify tests pass.
- [ ] Run `python3 tools/check_conventions.py --base origin/main`.

### Task 3: Define Backend-Neutral GpuCommand IR Stream

**Files:**
- Create: `core/gpu/rhi/include/gpu/rhi/GpuCommand.hpp`
- Create: `core/gpu/rhi/src/GpuCommandStream.cpp`
- Create: `core/gpu/rhi/include/gpu/rhi/GpuCommandStream.hpp`
- Test: `core/gpu/rhi/tests/GpuCommandTests.cpp`

- [ ] Define `GpuCommand` variant / packet structure representing draw calls, dispatches, state updates, barrier packets, buffer updates, and presentations.
- [ ] Implement `GpuCommandStream` allowing lock-free or lightweight recording and sequential playback onto an `ICommandRecorder`.
- [ ] Create `GpuCommandTests.cpp` testing serialization, playback, and execution onto mock recorders.
- [ ] Build and verify tests pass.
- [ ] Run `python3 tools/check_conventions.py --base origin/main`.

### Task 4: Connect AGC Driver Front-End to RHI Seam

**Files:**
- Create: `core/libs/prx/libSceAgcDriver/Execution/include/Driver/GpuBackend.hpp`
- Modify: `core/libs/prx/libSceAgcDriver/Execution/include/Driver/DeviceAccess.hpp`
- Modify: `core/libs/prx/libSceAgcDriver/Execution/src/Driver/DeviceAccess.cpp`
- Modify: `core/libs/prx/libSceAgcDriver/Execution/include/Driver/Draw/DrawPipeline.hpp`
- Modify: `core/libs/prx/libSceAgcDriver/Execution/src/Driver/Draw/DrawPipeline.cpp`
- Test: Existing relinker and driver tests

- [ ] Introduce `GpuBackend` factory and accessor in `libSceAgcDriver` delegating to `VulkanDevice` when `ANYPS5_GPU_BACKEND=vulkan` and preparing for `MetalBackend` when `ANYPS5_GPU_BACKEND=metal`.
- [ ] Adapt `DevicePointer` to cleanly interface with the RHI device representation.
- [ ] Update `DrawPipeline` committer thread to record through the RHI command stream interface.
- [ ] Verify full build and test execution.

### Task 5: Verification & Stop for Review

- [ ] Run full test suite: `ctest --test-dir build-macos-arm64 --output-on-failure`.
- [ ] Run conventions check: `python3 tools/check_conventions.py --base origin/main`.
- [ ] Confirm zero comments in C++ and adherence to all rules.
- [ ] Create Conventional Commit: `refactor(gpu): introduce RHI interfaces and backend seam`.
- [ ] **Stop and present Phase 2 completion and test verification results to user.**

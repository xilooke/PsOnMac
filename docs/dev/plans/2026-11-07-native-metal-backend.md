# Native Metal Backend Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement the concrete, direct Native Metal graphics backend (`core/gpu/metal/`) against the RHI interfaces, driving Apple Silicon GPUs natively with zero runtime translation.

**Architecture:** Use Objective-C++ (`.mm`) with direct Apple Metal framework APIs (`MTLDevice`, `MTLCommandQueue`, `MTLCommandBuffer`, `MTLRenderCommandEncoder`, `MTLComputeCommandEncoder`, `MTLBuffer`, `MTLTexture`, `MTLSharedEvent`, `CAMetalLayer`). Exploit Apple Silicon unified memory with `newBufferWithBytesNoCopy` and `MTLResourceStorageModeShared`. Coalesce TBDR render passes and compile MSL shaders into `MTLRenderPipelineState` and `MTLComputePipelineState`.

**Tech Stack:** C++20, Objective-C++ ARC (`-fobjc-arc`), Metal 3/4, MetalKit, QuartzCore (`CAMetalLayer`), CMake, Ninja.

**Spec:** `anyps5-metal-port-prompt.md` (Phase 4: Native Metal backend).

## Global Constraints

- Zero comments in C++ and Objective-C++ code (except `#endif` and namespace-closing markers `} // namespace ...`).
- Fail-fast policy: Unimplemented functions call `NotImplemented_nid_no_patch(__func__)` and throw `std::runtime_error`.
- Dependencies: Only Apple system frameworks (Metal, QuartzCore, Foundation).
- Conventional Commits for commit messages.
- `tools/check_conventions.py --base origin/main` must pass without findings.
- After the whole part is completed, stop and test it first before proceeding to Phase 5.

## Review Focus

1. Direct Metal integration: Verify zero MoltenVK or translation layer dependencies; calls go directly to `id<MTLDevice>`, `id<MTLCommandBuffer>`, etc.
2. Unified memory zero-copy: `newBufferWithBytesNoCopy` must require 16 KiB alignment on Apple Silicon; provide page-aligned allocation fallback.
3. TBDR pass lifecycle: Each `MTLRenderPassDescriptor` must specify correct load/store actions (`MTLLoadActionClear`/`Load`/`DontCare`, `MTLStoreActionStore`/`DontCare`).
4. Format mapping: Validate depth/stencil mapping (`Depth32Float_Stencil8` for PS5 depth targets) and compressed BCn formats.
5. Timeline synchronization: Map PS5 fences and labels using `MTLSharedEvent` with monotonic value signaling.

---

### Task 1: Format Mapping & Device Setup

**Files:**
- Create: `core/gpu/metal/include/gpu/metal/MetalFormatMap.hpp`
- Create: `core/gpu/metal/src/MetalFormatMap.mm`
- Create: `core/gpu/metal/include/gpu/metal/MetalDevice.hpp`
- Create: `core/gpu/metal/src/MetalDevice.mm`
- Create: `core/gpu/metal/CMakeLists.txt`
- Modify: `CMakeLists.txt`
- Test: `core/gpu/metal/tests/MetalDeviceTests.mm`

- [x] Implement `MetalFormatMap` translating RHI `Format` to `MTLPixelFormat`.
- [x] Implement `MetalDevice` querying hardware capabilities (`Caps`) and discovering default `MTLDevice`.
- [x] Add `core/gpu/metal` library and `metal_device_tests` to CMake.
- [x] Build and verify on macOS Apple Silicon.

### Task 2: Buffers, Textures, and Unified Memory

**Files:**
- Create: `core/gpu/metal/include/gpu/metal/MetalBuffer.hpp`
- Create: `core/gpu/metal/src/MetalBuffer.mm`
- Create: `core/gpu/metal/include/gpu/metal/MetalTexture.hpp`
- Create: `core/gpu/metal/src/MetalTexture.mm`
- Create: `core/gpu/metal/include/gpu/metal/MetalSampler.hpp`
- Create: `core/gpu/metal/src/MetalSampler.mm`
- Test: `core/gpu/metal/tests/MetalResourceTests.mm`

- [x] Implement `MetalBuffer` supporting `newBufferWithLength` and `newBufferWithBytesNoCopy` with `MTLResourceStorageModeShared`.
- [x] Implement `MetalTexture` creating `id<MTLTexture>` matching descriptor dimensions and pixel formats.
- [x] Implement `MetalSampler` creating `id<MTLSamplerState>`.
- [x] Write unit tests verifying zero-copy memory access and texture creation.

### Task 3: Command Recording, Pipelines, and Queues

**Files:**
- Create: `core/gpu/metal/include/gpu/metal/MetalPipeline.hpp`
- Create: `core/gpu/metal/src/MetalPipeline.mm`
- Create: `core/gpu/metal/include/gpu/metal/MetalFence.hpp`
- Create: `core/gpu/metal/src/MetalFence.mm`
- Create: `core/gpu/metal/include/gpu/metal/MetalCommandRecorder.hpp`
- Create: `core/gpu/metal/src/MetalCommandRecorder.mm`
- Create: `core/gpu/metal/include/gpu/metal/MetalQueue.hpp`
- Create: `core/gpu/metal/src/MetalQueue.mm`
- Test: `core/gpu/metal/tests/MetalExecutionTests.mm`

- [x] Implement `MetalPipeline` compiling MSL shader sources into `MTLRenderPipelineState` and `MTLComputePipelineState`.
- [x] Implement `MetalFence` and `MetalTimelineSemaphore` via `MTLSharedEvent`.
- [x] Implement `MetalCommandRecorder` encoding render passes, draws, dispatches, and copies.
- [x] Implement `MetalQueue` submitting command buffers and managing wait/signal events.
- [x] Write unit test executing an actual offscreen render pass and compute dispatch on Apple Silicon GPU.

### Task 4: Swapchain & Window Presentation

**Files:**
- Create: `core/gpu/metal/include/gpu/metal/MetalSwapchain.hpp`
- Create: `core/gpu/metal/src/MetalSwapchain.mm`
- Test: `core/gpu/metal/tests/MetalSwapchainTests.mm`

- [x] Implement `MetalSwapchain` using `CAMetalLayer` with double/triple buffering and drawable acquisition.
- [x] Verify test suite and integration.

### Task 5: Verification & Stop for Review

- [x] Run full test suite: `ctest --test-dir build-macos-arm64 --output-on-failure`.
- [x] Run conventions check: `python3 tools/check_conventions.py --base origin/main`.
- [x] Confirm zero comments in C++ and Objective-C++ code.
- [x] Create Conventional Commit: `feat(gpu): implement native Metal graphics backend`.
- [x] **Stop and present Phase 4 completion and test verification results to user.**

# Direct RDNA2 to MSL Shader Recompiler Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Implement a direct, native Metal Shading Language (MSL) recompiler backend (`core/shader/recompiler/MslBackend/`) translating PS5 RDNA2 SSA Intermediate Representation directly into valid, high-performance MSL without any intermediate translation layers.

**Architecture:** Extend `core/shader/recompiler` with `MslBackend`, providing `MslEmitter`. Map RDNA2 wave32 operations to Apple GPU `simdgroup` primitives, structured control flow to MSL branching/loops, PS5 resource tables to Metal Tier 2 Argument Buffers, and shader stage exports to MSL vertex/fragment/kernel attributes.

**Tech Stack:** C++20, Metal Shading Language (MSL 3.0+), CMake, Ninja.

**Spec:** `anyps5-metal-port-prompt.md` (Phase 3: Shader path: RDNA2 -> Metal).

## Global Constraints

- Zero comments in C++ (except `#endif` and namespace-closing markers `} // namespace ...`).
- Fail-fast policy: Unimplemented functions call `NotImplemented_nid_no_patch(__func__)` and throw `std::runtime_error`.
- Dependencies: Only git submodules under `3rdparty/` or Apple system frameworks.
- Conventional Commits for commit messages.
- `tools/check_conventions.py --base origin/main` must pass without findings.
- After the whole part is completed, stop and test it first before proceeding to Phase 4.

## Review Focus

1. Direct translation integrity: Ensure MSL is generated cleanly from SSA IR with zero runtime SPIR-V or translation layer dependencies.
2. SIMD wave width: Verify wave32 maps to Apple GPU 32-wide `simdgroup` intrinsics (`simd_broadcast_first`, `simd_shuffle`, `simd_ballot`).
3. Memory and resource mapping: Map T#/S#/V# descriptors to Metal argument buffer structures with explicit address space qualifiers (`device`, `constant`, `threadgroup`).
4. Pipeline stage attributes: Ensure correct entry point keywords (`[[vertex]]`, `[[fragment]]`, `[[kernel]]`) and stage inputs/outputs (`[[position]]`, `[[color(0)]]`, `[[thread_position_in_grid]]`).
5. Math precision: Use IEEE-compliant standard functions and avoid fast-math shortcuts that diverge from RDNA2 rounding/denorm behavior.

---

### Task 1: Create MSL Backend Types and Emitter Interface

**Files:**
- Create: `core/shader/recompiler/MslBackend/include/MslBackend/MslTypes.hpp`
- Create: `core/shader/recompiler/MslBackend/include/MslBackend/MslEmitter.hpp`

- [ ] Create `MslTypes.hpp` defining MSL version, target options, language attributes, and resource bindings.
- [ ] Create `MslEmitter.hpp` declaring `MslEmitter` class with `Emit()` interface accepting `IrProgram` and emitting complete MSL shader text.

### Task 2: Implement Direct SSA IR to MSL Code Generation

**Files:**
- Create: `core/shader/recompiler/MslBackend/src/MslEmitter.cpp`
- Create: `core/shader/recompiler/MslBackend/src/MslAlu.cpp`
- Create: `core/shader/recompiler/MslBackend/include/MslBackend/MslEmitterState.hpp`

- [ ] Implement `MslEmitterState` tracking indentation, variable naming, argument buffers, and temp values.
- [ ] Implement ALU and intrinsic lowering in `MslAlu.cpp` (integers, floats, comparisons, conversions, SIMD ops).
- [ ] Implement `MslEmitter.cpp` generating stage headers, argument buffers, control flow, and stage exports.

### Task 3: Unit Testing and CMake Integration

**Files:**
- Create: `core/shader/recompiler/MslBackend/tests/MslEmitterTests.cpp`
- Modify: `core/shader/recompiler/CMakeLists.txt`
- Modify: `core/shader/recompiler/Recompiler.hpp`

- [ ] Add `msl` source field to `CompiledShaderArtifact` in `Recompiler.hpp`.
- [ ] Add `MslBackend` sources and tests to `core/shader/recompiler/CMakeLists.txt`.
- [ ] Write comprehensive unit tests in `MslEmitterTests.cpp` testing compute, vertex, fragment, ALU, and SIMD code generation.
- [ ] Build and verify all tests pass on arm64 and x86_64.

### Task 4: Verification & Stop for Review

- [ ] Run full test suite: `ctest --test-dir build-macos-arm64 --output-on-failure`.
- [ ] Run conventions check: `python3 tools/check_conventions.py --base origin/main`.
- [ ] Confirm zero comments in C++ and adherence to all rules.
- [ ] Create Conventional Commit: `feat(shader): implement direct RDNA2 to MSL recompiler backend`.
- [ ] **Stop and present Phase 3 completion and test verification results to user.**

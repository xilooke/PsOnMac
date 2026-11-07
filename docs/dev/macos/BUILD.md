# Building AnyPS5 on macOS

This guide covers building the AnyPS5 platform layer and relinker on macOS (Apple Silicon and Intel).

## Prerequisites

- macOS 14.0 (Sonoma) or newer.
- AppleClang from Xcode Command Line Tools:
  ```sh
  xcode-select --install
  ```
- Build tools via Homebrew:
  ```sh
  brew install cmake ninja
  ```
- Python 3.9+ with `tomllib` (Python 3.11+ recommended) for running test suites.

## Submodules

Initialize the required submodules:

```sh
git submodule update --init --recursive
```

## Native Apple Silicon Build (arm64)

To configure, build, and test the native arm64 platform layer and relinker using CMake Presets:

```sh
cmake --preset macos-arm64
cmake --build --preset macos-arm64
ctest --preset macos-arm64 --output-on-failure
```

To run only the macOS platform-specific unit tests:

```sh
ctest --test-dir build-macos-arm64 -R "mach_|qos_|os_sync_" -V
```

The platform tests cover:
- `mach_vm_memory`: 2 TiB address space reservations, direct page mapping, aliasing, and remapping.
- `os_sync_wait`: Futex-style synchronization backed by Darwin private `__ulock_wait` and `__ulock_wake`.
- `qos_thread`: Mapping PS5 thread priorities to Darwin Quality of Service (QoS) classes via `pthread_set_qos_class_self_np`.
- `mach_exception_handler`: Thread-safe Mach exception server handling `EXC_BAD_ACCESS` page-protection faults without interfering with signals or debuggers.

## Rosetta 2 Build (x86_64)

To build and test the x86_64 slice targeting Rosetta 2 execution:

```sh
cmake --preset macos-x86_64
cmake --build --preset macos-x86_64
ctest --preset macos-x86_64 -R "mach_|qos_|os_sync_" -V
```

## Environment Variables

- `ANYPS5_GPU_BACKEND`: Selects the graphics backend. Defaults to `metal` on macOS and `vulkan` on Linux/Windows. Set `ANYPS5_GPU_BACKEND=metal` or `ANYPS5_GPU_BACKEND=vulkan`.

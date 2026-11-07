# AnyPS5 macOS & Apple Silicon Known Limitations

This document enumerates architectural differences, platform boundaries, and deferred capabilities on macOS.

---

## 1. Filesystem & APFS

- **Case Sensitivity**: Standard Apple APFS volumes are case-insensitive and case-preserving. PlayStation 5 games assuming case-sensitive file separation cannot store two files with names differing only by case in the same folder.
- **Direct I/O (`O_DIRECT`)**: Darwin does not support the POSIX `O_DIRECT` open flag. AnyPS5 translates `SCE_KERNEL_O_DIRECT` to `fcntl(fd, F_NOCACHE, 1)` on descriptor open, which bypasses the Unified Buffer Cache (UBC) for streaming reads.

---

## 2. Hardware Architecture & Rosetta 2

- **Page Size**: Apple Silicon uses 16 KiB virtual memory pages, whereas x86_64 PS5 uses 4 KiB pages. `GuestArena` aligns base allocations to 16 KiB boundaries. Partial-page write-watch protections track dirtied state at 16 KiB granularity.
- **AVX-512**: Rosetta 2 supports AVX, AVX2, and FMA, but not AVX-512. Since PS5 Zen 2 hardware does not feature AVX-512, commercial PS5 software is completely unaffected.
- **Raytracing Support**: Hardware raytracing intersection (`MTLAccelerationStructure`) requires Apple Silicon M3, M4, or newer chips with Metal 3.1 RT cores. On M1 and M2 series, BVH intersections fall back to compute traversal.

---

## 3. Runtime Services & Subsystems

- **WebKit UI (`libSceNKWebKit`)**: WebKit-based overlay UI modules are currently fail-fast stubs on macOS, documented in `docs/dev/debt/`.
- **Global Buffer Device Address (BDA) Atomics**: Flat 64-bit atomics directly on untyped buffer addresses without typed binding require Metal buffer binding emulation.

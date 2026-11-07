**English** | [Português (Brasil)](docs/user/README.pt-BR.md) | [Español](docs/user/README.es.md) | [Italiano](docs/user/README.it.md) | [简体中文](docs/user/README.zh-CN.md) | [Русский](docs/user/README.ru.md) | [Bahasa Indonesia](docs/user/README.id.md)

# About

**PsOnMac** is a high-performance native macOS Apple Silicon port of AnyPS5, relinking PlayStation 5 executables into native Mach-O binaries running against reimplemented system libraries and a direct Apple Metal 3.1 graphics backend.

The tool provides automatic executable porting to macOS, Linux, and Windows with zero emulation overhead.

---

## PsOnMac Highlights (macOS on Apple Silicon)

- **Zero Translation Layers**: AGC command buffers execute directly via Apple Metal (`core/gpu/metal/`). No MoltenVK, no Wine, and no translation wrappers.
- **Direct RDNA2 -> Metal Shading Language Recompiler**: RDNA2 SSA IR is lowered directly to MSL 3.1 shaders (`core/shader/recompiler/MslBackend/`) utilizing 32-wide `simdgroup` operations.
- **Unified Memory Architecture (UMA)**: Zero-copy guest-host arena using a 2 TiB virtual address space mapped via `mach_vm_remap`.
- **Low-Overhead Synchronization**: Ultra-fast atomic wait/wake powered by native macOS 14.4+ `os_sync_wait_on_address`.
- **Native Audio Unit**: Low-latency 48kHz stereo PCM presentation via Apple CoreAudio HAL (`CoreAudioOut.cpp`).
- **Native Metal GUI Launcher (`anyps5-ui`)**: Sleek Cocoa + Metal launcher featuring a live 60 FPS Metal shader canvas, DualSense controller diagnostics, drag-and-drop game relinking, and standalone homebrew ELF support.

---

## Quick Start on macOS

### 1. Build Native Apple Silicon Binaries
```sh
cmake --preset macos-arm64
cmake --build build-macos-arm64
```

### 2. Launch the Graphical User Interface
```sh
./build-macos-arm64/anyps5-ui
```
*(Or launch via the relinker: `./build-macos-arm64/core/relinker/relinker --ui`)*

### 3. Relink PS5 Executables via CLI
```sh
./build-macos-arm64/core/relinker/relinker --macos --to-rosetta /path/to/game/eboot.bin ./game-macos
```
*(For standalone homebrew ELFs without a `sce_module` directory, add `--skip-sce-module --skip-syscall-check`)*

---

## Technical Documentation

- [macOS Architecture & Subsystems](docs/dev/macos/ARCHITECTURE.md)
- [Performance & Bandwidth Characterization](docs/dev/macos/PERF.md)
- [Hardware & Architecture Limitations](docs/dev/macos/LIMITATIONS.md)
- [macOS Direct I/O & Filesystem](docs/dev/macos/FILESYSTEM.md)
- [Spike Verification & Research Findings](docs/dev/macos/RESEARCH.md)

---

[**Web converter**](https://boykopovar.github.io/AnyPS5/) - files stay on your device.

![anyps5_collage1](https://github.com/user-attachments/assets/ed2f0b2d-33d3-46d7-ab88-767ca872fc99)

Includes a [relinker](core/relinker) that converts executable to the target system's native format and implementations of [system prx libraries](core/libs/prx) suitable for dynamic linking. No emulation or separate runtime process.

[**Discord**](https://discord.gg/x9naWVzC6A), [Usage](docs/user/USAGE.md), [Build instructions](docs/dev/BUILD.md), [Architecture](docs/dev/ARCHITECTURE.md), [Technical debt of the project](docs/dev/TechnicalDebt.md), [code style conventions](docs/dev/CONVENTIONS.md), [contributing](CONTRIBUTING.md)

## Status

[![libraries](https://boykopovar.github.io/AnyPS5/badge-libraries.svg)](https://boykopovar.github.io/AnyPS5/progress.html) [![shader forms decoded, counted from the code](https://boykopovar.github.io/AnyPS5/badge-shaders.svg)](https://boykopovar.github.io/AnyPS5/progress.html) [![implemented in 7 days](https://boykopovar.github.io/AnyPS5/badge-week.svg)](https://boykopovar.github.io/AnyPS5/history.html)

[![progress map](https://boykopovar.github.io/AnyPS5/progress.svg)](https://boykopovar.github.io/AnyPS5/progress.html)

[Progress history](https://boykopovar.github.io/AnyPS5/history.html)

<sub>* System libraries: percentage of the functions known to the project so far (declared in [core/libs/prx](core/libs/prx)), not of every PS5 system function. The total grows as more functions are declared.</sub>

Unsupported or unexpected states throw a C++ exception, usually `std::runtime_error`. An exception that nothing catches prints its `what()` to stderr and ends the process.

The [shader recompiler](core/shader/recompiler/Recompiler.cpp) successfully produces SPIR-V (validated via [Spirv-Tools](3rdparty/SPIRV-Tools) when built with `ANYPS5_ENABLE_SPIRV_TOOLS`).

## Compatibility

Per-game status and test reports live in the [Game bring-up](https://github.com/boykopovar/AnyPS5/discussions/categories/game-bring-up) discussions, one thread per title.

## Input mapping

SDL-mapped game controllers are supported, including analog sticks and triggers. Keyboard and mouse controls can be configured with an `anyps5-input.ini` file. See [input mapping](docs/user/INPUT_MAPPING.md) for the supported devices and configuration format.

## Disclaimer

This project is intended for interoperability, research, preservation, and compatibility purposes. It does not include, distribute, or require copyrighted software, firmware, cryptographic keys, or proprietary libraries. Users are responsible for ensuring that any binaries used with this project are obtained and used in accordance with applicable laws and their respective license terms.

AnyPS5 is an independent project and is not affiliated with, authorized or endorsed by Sony Interactive Entertainment. "PlayStation" and "PS5" are trademarks of Sony Interactive Entertainment Inc.

## License

This project is licensed under the GNU General Public License version 2 only.

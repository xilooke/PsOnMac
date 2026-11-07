# macOS Filesystem Semantics & I/O

This document describes the filesystem semantics, direct I/O adaptations, and file descriptor management for AnyPS5 on macOS.

## 1. APFS Case Sensitivity

- Standard macOS Apple File System (APFS) volumes are case-insensitive and case-preserving.
- The PlayStation 5 native kernel environment uses case-sensitive filesystems.
- On macOS, path resolution preserves case on file creation and looks up paths case-insensitively. Titles relying on exact case match without colliding filenames with distinct case operate transparently.
- Titles that create duplicate filenames differing only by case in the same folder are not supported on case-insensitive APFS formatted volumes. Case-sensitive APFS volumes can be used if exact case distinction is required.

## 2. Direct I/O and Uncached Streaming (`F_NOCACHE`)

- Darwin and APFS do not implement the `O_DIRECT` open flag found in Linux and FreeBSD.
- When opening files with `SCE_KERNEL_O_DIRECT`, AnyPS5 strips `O_DIRECT` from the initial open call and immediately issues:
  ```c
  fcntl(fd, F_NOCACHE, 1);
  ```
- This instructs the Darwin kernel to bypass the Unified Buffer Cache (UBC), enabling high-throughput direct streaming I/O with minimal cache pollution.

## 3. File Descriptor Limit Scaling

- Darwin defaults to a conservative per-process file limit (often 256 file descriptors).
- During startup, `OpenFileLimit` queries `kern.maxfilesperproc` via `sysctlbyname`:
  ```c
  sysctlbyname("kern.maxfilesperproc", &perProcess, &size, nullptr, 0);
  ```
- `RLIMIT_NOFILE` (`rlim_cur`) is raised to the hard limit or `kern.maxfilesperproc` to accommodate titles that maintain thousands of simultaneously open package streams.

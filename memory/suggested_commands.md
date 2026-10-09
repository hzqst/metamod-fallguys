---
title: suggested_commands
type: note
permalink: metamod-fallguys/suggested-commands
---

# Suggested Commands

```sh
git clone --recursive https://github.com/hzqst/metamod-fallguys
git submodule update --init --recursive
git submodule status --recursive
cmake -S . -B build-cmake/native -A Win32
cmake --build build-cmake/native --config Release --parallel 6
cmake --install build-cmake/native --config Release --prefix build
```

Linux: omit -A Win32 and add -DCMAKE_BUILD_TYPE=Release; install gcc-multilib/g++-multilib.
Debug: use --config Debug on Windows or -DCMAKE_BUILD_TYPE=Debug on Linux.
Legacy scripts/build-*.bat/sh are unified build compatibility entry points.

Standalone plugin: cmake -S <plugin> -B <build> -DMETAMOD_SOURCE_PATH=<metamod-clone>
-DASEXT_SOURCE_PATH=<asext-clone>, or omit overrides for pinned FetchContent.
Native outputs install under addons/metamod/dlls. Only Windows Win32/Linux i386 are supported.

Formatting: install clang-format==23.1.3; configure -DFORMAT_VALIDATION_ONLY=ON;
build format-check or format. Component-prefixed targets also exist in the aggregate.
Commit component changes in their own repositories before updating aggregate gitlinks.

#!/usr/bin/env bash
set -euo pipefail
MMFG_ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
git -C "$MMFG_ROOT" submodule update --init --recursive
cmake -S "$MMFG_ROOT" -B "$MMFG_ROOT/build-cmake/Debug" -DCMAKE_BUILD_TYPE=Debug
cmake --build "$MMFG_ROOT/build-cmake/Debug" --parallel
cmake --install "$MMFG_ROOT/build-cmake/Debug" --prefix "$MMFG_ROOT/build"

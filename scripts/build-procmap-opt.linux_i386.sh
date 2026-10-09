#!/usr/bin/env bash
set -euo pipefail
MMFG_ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
git -C "$MMFG_ROOT" submodule update --init --recursive
cmake -S "$MMFG_ROOT" -B "$MMFG_ROOT/build-cmake/Release" -DCMAKE_BUILD_TYPE=Release
cmake --build "$MMFG_ROOT/build-cmake/Release" --parallel
cmake --install "$MMFG_ROOT/build-cmake/Release" --prefix "$MMFG_ROOT/build"

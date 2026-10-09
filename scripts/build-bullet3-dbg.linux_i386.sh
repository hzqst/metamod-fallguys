#!/usr/bin/env bash
set -euo pipefail
MMFG_ROOT="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
git -C "$MMFG_ROOT" submodule update --init --recursive
cmake -S "$MMFG_ROOT" -B "$MMFG_ROOT/build/x86/Debug" -DCMAKE_BUILD_TYPE=Debug
cmake --build "$MMFG_ROOT/build/x86/Debug" --parallel
cmake --install "$MMFG_ROOT/build/x86/Debug"

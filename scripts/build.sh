#!/usr/bin/env bash
# Build + test on Linux/macOS. Mirrors build.ps1 so the same workflow runs on
# both platforms -- the JD explicitly asks for Windows AND Linux support.
set -euo pipefail

BUILD_DIR="${1:-build}"

cmake -S . -B "$BUILD_DIR" -DCMAKE_BUILD_TYPE=Release
cmake --build "$BUILD_DIR" -j
ctest --test-dir "$BUILD_DIR" --output-on-failure

echo "Build + tests complete. Binary: $BUILD_DIR/sensor_stats"

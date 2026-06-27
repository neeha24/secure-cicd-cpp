# Build + test on Windows. Mirrors build.sh so the same workflow runs on both
# platforms -- the JD explicitly asks for Windows AND Linux support.
#Requires -Version 5.1
param([string]$BuildDir = "build")

$ErrorActionPreference = "Stop"

cmake -S . -B $BuildDir -DCMAKE_BUILD_TYPE=Release
cmake --build $BuildDir --config Release
ctest --test-dir $BuildDir -C Release --output-on-failure

Write-Host "Build + tests complete. Binary in $BuildDir"

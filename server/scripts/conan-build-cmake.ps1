param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",

    [switch]$Clean,

    [switch]$Help
)

function Show-Help {
    Write-Host @"
Conan + CMake build script

Usage:
  .\scripts\conan-build-cmake.ps1 [-Configuration <Debug|Release>] [-Clean]

Parameters:
  -Configuration  Build type (default: Release)
  -Clean          Clean build and conan output directories
  -Help           Show this help

"@ -ForegroundColor Cyan
}

if ($Help) {
    Show-Help
    exit 0
}

$ErrorActionPreference = "Stop"
$projectRoot = Split-Path -Parent $PSScriptRoot
Set-Location $projectRoot

Write-Host "==================================================" -ForegroundColor Cyan
Write-Host "  Badchat CMake Build ($Configuration)" -ForegroundColor Cyan
Write-Host "==================================================" -ForegroundColor Cyan

if ($Clean) {
    Write-Host "Cleaning build directories..." -ForegroundColor Yellow
    Remove-Item -Path "$projectRoot\build" -Recurse -Force -ErrorAction SilentlyContinue
    Remove-Item -Path "$projectRoot\conan_output" -Recurse -Force -ErrorAction SilentlyContinue
}

Write-Host "[1/3] Installing Conan dependencies..." -ForegroundColor Cyan
conan install . `
    --profile=conan/profiles/windows-msvc-2019 `
    --settings=build_type=$Configuration `
    --build=missing `
    --output-folder=conan_output

if ($LASTEXITCODE -ne 0) {
    Write-Host "Conan install failed!" -ForegroundColor Red
    exit 1
}

if (-not (Test-Path "$projectRoot\build")) {
    New-Item -ItemType Directory -Path "$projectRoot\build" | Out-Null
}

Set-Location "$projectRoot\build"

Write-Host "[2/3] Configuring CMake..." -ForegroundColor Cyan
cmake -G "Visual Studio 16 2019" -A x64 `
    -DCMAKE_TOOLCHAIN_FILE="$projectRoot\conan_output\conan_toolchain.cmake" `
    -DCMAKE_BUILD_TYPE=$Configuration `
    "$projectRoot"

if ($LASTEXITCODE -ne 0) {
    Write-Host "CMake configuration failed!" -ForegroundColor Red
    exit 1
}

Write-Host "[3/3] Building..." -ForegroundColor Cyan
cmake --build . --config $Configuration

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build failed!" -ForegroundColor Red
    exit 1
}

Set-Location $projectRoot

Write-Host ""
Write-Host "==================================================" -ForegroundColor Green
Write-Host "  Build succeeded!" -ForegroundColor Green
Write-Host "==================================================" -ForegroundColor Green
Write-Host ""
Write-Host "Executables:" -ForegroundColor Cyan
Write-Host "  build\bin\$Configuration\GateServer.exe" -ForegroundColor White
Write-Host "  build\bin\$Configuration\StatusServer.exe" -ForegroundColor White
Write-Host "  build\bin\$Configuration\ChatServer.exe" -ForegroundColor White
Write-Host "  build\bin\$Configuration\ResourceServer.exe" -ForegroundColor White
Write-Host ""
Write-Host "Next: .\scripts\run-all-cpp.ps1 -Configuration $Configuration" -ForegroundColor Cyan

param(
    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release"
)

$ErrorActionPreference = "Stop"
$scriptDir = $PSScriptRoot

Write-Host "==================================================" -ForegroundColor Cyan
Write-Host "  Starting all C++ services ($Configuration)" -ForegroundColor Cyan
Write-Host "==================================================" -ForegroundColor Cyan

$servers = @("Status", "Chat1", "Chat2", "Gate", "Resource")

foreach ($server in $servers) {
    Write-Host "Starting $server..." -ForegroundColor Yellow
    & "$scriptDir\run-cpp-server.ps1" -Server $server -Configuration $Configuration
    Start-Sleep -Milliseconds 500
}

Write-Host ""
Write-Host "All servers started." -ForegroundColor Green

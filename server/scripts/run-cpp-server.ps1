param(
    [Parameter(Mandatory = $true)]
    [ValidateSet("Status", "Gate", "Chat1", "Chat2", "Resource")]
    [string]$Server,

    [ValidateSet("Debug", "Release")]
    [string]$Configuration = "Release",

    [string]$RepoRoot = "",

    [switch]$Foreground
)

$ErrorActionPreference = "Stop"

if ($RepoRoot -eq "") {
    $RepoRoot = (Resolve-Path (Join-Path $PSScriptRoot "..")).Path
}

$map = @{
    "Status"   = @{
        ExeName    = "StatusServer.exe"
        ConfigFile = "config\status.ini"
    }
    "Gate"     = @{
        ExeName    = "GateServer.exe"
        ConfigFile = "config\gate.ini"
    }
    "Chat1"    = @{
        ExeName    = "ChatServer.exe"
        ConfigFile = "config\chat1.ini"
    }
    "Chat2"    = @{
        ExeName    = "ChatServer.exe"
        ConfigFile = "config\chat2.ini"
    }
    "Resource" = @{
        ExeName    = "ResourceServer.exe"
        ConfigFile = "config\resource.ini"
    }
}

$entry = $map[$Server]
$exePath = Join-Path $RepoRoot "build\bin\$Configuration\$($entry.ExeName)"
$configPath = Join-Path $RepoRoot $entry.ConfigFile

if (-not (Test-Path $exePath)) {
    throw @"
Executable not found: $exePath
Run .\scripts\conan-build-cmake.ps1 -Configuration $Configuration first.
"@
}

if (-not (Test-Path $configPath)) {
    Write-Warning "Config not found: $configPath. Run .\scripts\sync-config.ps1 first."
}

$configAbsolute = (Resolve-Path $configPath).Path

Write-Host "Starting $Server" -ForegroundColor Cyan
Write-Host "  Executable: $exePath"
Write-Host "  Config:     $configAbsolute"

$arguments = "--config `"$configAbsolute`""

if ($Foreground) {
    & $exePath --config $configAbsolute
    return
}

Start-Process -FilePath $exePath -ArgumentList "--config", "`"$configAbsolute`""

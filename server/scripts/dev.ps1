param(
    [Parameter(Mandatory = $true)]
    [ValidateSet("Up", "Down", "Check")]
    [string]$Action,

    [string]$EnvFile = "..\config\.env"
)

$ErrorActionPreference = "Stop"

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path (Join-Path $scriptDir "..")
$composeFile = Join-Path $repoRoot "deploy\docker-compose.yml"
$envRawPath = Join-Path $scriptDir $EnvFile

if (-not (Test-Path $envRawPath)) {
    throw "Env file not found: $envRawPath. Copy config/.env.example to config/.env first."
}
$envPath = Resolve-Path $envRawPath

function Read-EnvFile {
    param([string]$Path)
    $map = @{}
    foreach ($line in Get-Content -Path $Path) {
        $trimmed = $line.Trim()
        if ($trimmed -eq "" -or $trimmed.StartsWith("#")) { continue }
        $eqIndex = $trimmed.IndexOf("=")
        if ($eqIndex -lt 1) { continue }
        $map[$trimmed.Substring(0, $eqIndex).Trim()] = $trimmed.Substring($eqIndex + 1).Trim()
    }
    return $map
}

function Get-Value {
    param([hashtable]$Map, [string]$Key, [string]$DefaultValue = "")
    if ($Map.ContainsKey($Key) -and $Map[$Key] -ne "") { return $Map[$Key] }
    return $DefaultValue
}

function Read-Ini {
    param([string]$Path)
    $ini = @{}
    $section = ""
    foreach ($line in Get-Content -Path $Path) {
        $trimmed = $line.Trim()
        if ($trimmed -eq "" -or $trimmed.StartsWith(";") -or $trimmed.StartsWith("#")) { continue }
        if ($trimmed.StartsWith("[") -and $trimmed.EndsWith("]")) {
            $section = $trimmed.Substring(1, $trimmed.Length - 2)
            if (-not $ini.ContainsKey($section)) { $ini[$section] = @{} }
            continue
        }
        $eqIndex = $trimmed.IndexOf("=")
        if ($eqIndex -gt 0 -and $section -ne "") {
            $ini[$section][$trimmed.Substring(0, $eqIndex).Trim()] = $trimmed.Substring($eqIndex + 1).Trim()
        }
    }
    return $ini
}

function Check-PortListening {
    param([int]$Port)
    try {
        $conn = Get-NetTCPConnection -LocalPort $Port -State Listen -ErrorAction SilentlyContinue
        return ($null -ne $conn)
    } catch { return $false }
}

switch ($Action) {
    "Up" {
        if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
            Write-Host "ERROR: docker command not found. Install Docker Desktop first." -ForegroundColor Red
            exit 1
        }
        docker info --format '{{.ServerVersion}}' 1>$null 2>$null
        if ($LASTEXITCODE -ne 0) {
            Write-Host "ERROR: Cannot connect to Docker daemon. Start Docker Desktop and wait for it to be ready." -ForegroundColor Red
            exit 1
        }
        Write-Host "[1/2] Starting MySQL, Redis and VarifyServer containers..." -ForegroundColor Cyan
        docker compose --env-file $envPath -f $composeFile up -d --build
        if ($LASTEXITCODE -ne 0) {
            Write-Host "ERROR: docker compose up failed." -ForegroundColor Red
            exit 1
        }
        Write-Host "[2/2] Containers started. Run 'docker compose ps' to verify." -ForegroundColor Green
    }

    "Down" {
        if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
            Write-Host "ERROR: docker command not found." -ForegroundColor Red
            exit 1
        }
        docker compose --env-file $envPath -f $composeFile down
        if ($LASTEXITCODE -ne 0) {
            Write-Host "ERROR: docker compose down failed." -ForegroundColor Red
            exit 1
        }
        Write-Host "Containers stopped." -ForegroundColor Green
    }

    "Check" {
        $cfg = Read-EnvFile -Path $envPath
        $results = New-Object System.Collections.ArrayList

        $requiredKeys = @(
            "MYSQL_PORT", "MYSQL_USER", "MYSQL_PASSWORD", "MYSQL_DB",
            "REDIS_PORT", "REDIS_PASSWORD",
            "GATE_PORT", "VARIFY_GRPC_PORT", "STATUS_GRPC_PORT",
            "CHAT1_PORT", "CHAT1_RPC_PORT", "CHAT2_PORT", "CHAT2_RPC_PORT",
            "GATE_HOST", "VARIFY_HOST", "STATUS_HOST", "CHAT1_HOST", "CHAT2_HOST",
            "EMAIL_USER", "EMAIL_PASS"
        )
        foreach ($key in $requiredKeys) {
            if (-not $cfg.ContainsKey($key) -or $cfg[$key] -eq "") {
                [void]$results.Add([PSCustomObject]@{ Level = "ERROR"; Message = ".env missing required key: $key" })
            }
        }

        $mysqlPort  = Get-Value $cfg "MYSQL_PORT"       "3308"
        $redisPort  = Get-Value $cfg "REDIS_PORT"       "6379"
        $gatePort   = Get-Value $cfg "GATE_PORT"        "8080"
        $varifyPort = Get-Value $cfg "VARIFY_GRPC_PORT" "50051"
        $statusPort = Get-Value $cfg "STATUS_GRPC_PORT" "50052"
        $chat1Port  = Get-Value $cfg "CHAT1_PORT"       "8090"
        $chat1Rpc   = Get-Value $cfg "CHAT1_RPC_PORT"   "50055"
        $chat2Port  = Get-Value $cfg "CHAT2_PORT"       "8091"
        $chat2Rpc   = Get-Value $cfg "CHAT2_RPC_PORT"   "50056"

        $gateIni   = Join-Path $repoRoot "config\gate.ini"
        $statusIni = Join-Path $repoRoot "config\status.ini"
        $chat1Ini  = Join-Path $repoRoot "config\chat1.ini"
        $chat2Ini  = Join-Path $repoRoot "config\chat2.ini"
        $resIni    = Join-Path $repoRoot "config\resource.ini"

        foreach ($p in @($gateIni, $statusIni, $chat1Ini, $chat2Ini, $resIni)) {
            if (-not (Test-Path $p)) {
                [void]$results.Add([PSCustomObject]@{ Level = "ERROR"; Message = "Missing config file: $p" })
            }
        }

        if (Test-Path $gateIni) {
            $ini = Read-Ini $gateIni
            if ($ini["GateServer"]["Port"]   -ne $gatePort)   { [void]$results.Add([PSCustomObject]@{Level="ERROR";Message="gate.ini mismatch: GateServer.Port != GATE_PORT"}) }
            if ($ini["VarifyServer"]["Port"] -ne $varifyPort) { [void]$results.Add([PSCustomObject]@{Level="ERROR";Message="gate.ini mismatch: VarifyServer.Port != VARIFY_GRPC_PORT"}) }
            if ($ini["StatusServer"]["Port"] -ne $statusPort) { [void]$results.Add([PSCustomObject]@{Level="ERROR";Message="gate.ini mismatch: StatusServer.Port != STATUS_GRPC_PORT"}) }
            if ($ini["Mysql"]["Port"]        -ne $mysqlPort)  { [void]$results.Add([PSCustomObject]@{Level="ERROR";Message="gate.ini mismatch: Mysql.Port != MYSQL_PORT"}) }
            if ($ini["Redis"]["Port"]        -ne $redisPort)  { [void]$results.Add([PSCustomObject]@{Level="ERROR";Message="gate.ini mismatch: Redis.Port != REDIS_PORT"}) }
        }
        if (Test-Path $statusIni) {
            $ini = Read-Ini $statusIni
            if ($ini["StatusServer"]["Port"] -ne $statusPort) { [void]$results.Add([PSCustomObject]@{Level="ERROR";Message="status.ini mismatch: StatusServer.Port != STATUS_GRPC_PORT"}) }
            if ($ini["chatserver1"]["Port"]  -ne $chat1Port)  { [void]$results.Add([PSCustomObject]@{Level="ERROR";Message="status.ini mismatch: chatserver1.Port != CHAT1_PORT"}) }
            if ($ini["chatserver2"]["Port"]  -ne $chat2Port)  { [void]$results.Add([PSCustomObject]@{Level="ERROR";Message="status.ini mismatch: chatserver2.Port != CHAT2_PORT"}) }
        }
        if (Test-Path $chat1Ini) {
            $ini = Read-Ini $chat1Ini
            if ($ini["SelfServer"]["Port"]    -ne $chat1Port) { [void]$results.Add([PSCustomObject]@{Level="ERROR";Message="chat1.ini mismatch: SelfServer.Port != CHAT1_PORT"}) }
            if ($ini["SelfServer"]["RPCPort"] -ne $chat1Rpc)  { [void]$results.Add([PSCustomObject]@{Level="ERROR";Message="chat1.ini mismatch: SelfServer.RPCPort != CHAT1_RPC_PORT"}) }
            if ($ini["chatserver2"]["Port"]   -ne $chat2Rpc)  { [void]$results.Add([PSCustomObject]@{Level="ERROR";Message="chat1.ini mismatch: chatserver2.Port != CHAT2_RPC_PORT"}) }
        }
        if (Test-Path $chat2Ini) {
            $ini = Read-Ini $chat2Ini
            if ($ini["SelfServer"]["Port"]    -ne $chat2Port) { [void]$results.Add([PSCustomObject]@{Level="ERROR";Message="chat2.ini mismatch: SelfServer.Port != CHAT2_PORT"}) }
            if ($ini["SelfServer"]["RPCPort"] -ne $chat2Rpc)  { [void]$results.Add([PSCustomObject]@{Level="ERROR";Message="chat2.ini mismatch: SelfServer.RPCPort != CHAT2_RPC_PORT"}) }
            if ($ini["chatserver1"]["Port"]   -ne $chat1Rpc)  { [void]$results.Add([PSCustomObject]@{Level="ERROR";Message="chat2.ini mismatch: chatserver1.Port != CHAT1_RPC_PORT"}) }
        }

        if (-not (Get-Command docker -ErrorAction SilentlyContinue)) {
            [void]$results.Add([PSCustomObject]@{Level="WARN";Message="docker command not found. Skip container state checks."})
        } else {
            try {
                $composePs = docker compose --env-file $envPath -f $composeFile ps --format json 2>$null
                if ($LASTEXITCODE -ne 0) {
                    [void]$results.Add([PSCustomObject]@{Level="WARN";Message="Cannot read docker compose status. Is Docker Desktop running?"})
                } else {
                    $containers = @()
                    if ($composePs) {
                        foreach ($line in ($composePs -split "`r?`n" | Where-Object { $_.Trim() -ne "" })) {
                            try { $containers += ($line | ConvertFrom-Json) } catch {}
                        }
                    }
                    foreach ($svc in @("mysql", "redis", "varifyserver")) {
                        $matched = $containers | Where-Object { $_.Service -eq $svc }
                        if (-not $matched) {
                            [void]$results.Add([PSCustomObject]@{Level="WARN";Message="Container not found: $svc"})
                        } elseif ($matched[0].State -ne "running") {
                            [void]$results.Add([PSCustomObject]@{Level="WARN";Message="Container not running: $svc ($($matched[0].State))"})
                        }
                    }
                }
            } catch {
                [void]$results.Add([PSCustomObject]@{Level="WARN";Message="Failed to query docker compose status."})
            }
        }

        foreach ($pair in @(
            @{Name="MYSQL_PORT";       Port=$mysqlPort},
            @{Name="REDIS_PORT";       Port=$redisPort},
            @{Name="VARIFY_GRPC_PORT"; Port=$varifyPort},
            @{Name="STATUS_GRPC_PORT"; Port=$statusPort},
            @{Name="CHAT1_PORT";       Port=$chat1Port},
            @{Name="CHAT2_PORT";       Port=$chat2Port},
            @{Name="GATE_PORT";        Port=$gatePort}
        )) {
            if (Check-PortListening -Port ([int]$pair.Port)) {
                [void]$results.Add([PSCustomObject]@{Level="INFO";Message="Port listening: $($pair.Name)=$($pair.Port)"})
            } else {
                [void]$results.Add([PSCustomObject]@{Level="WARN";Message="Port not listening: $($pair.Name)=$($pair.Port)"})
            }
        }

        $errors = $results | Where-Object { $_.Level -eq "ERROR" }
        $warns  = $results | Where-Object { $_.Level -eq "WARN" }
        $infos  = $results | Where-Object { $_.Level -eq "INFO" }

        Write-Host ""
        Write-Host "=== badchat dev-check report ===" -ForegroundColor Cyan
        Write-Host "INFO : $($infos.Count)"
        Write-Host "WARN : $($warns.Count)"
        Write-Host "ERROR: $($errors.Count)"
        Write-Host ""
        foreach ($r in $results) {
            Write-Host ("[{0}] {1}" -f $r.Level, $r.Message)
        }
        if ($errors.Count -gt 0) { exit 1 }
    }
}

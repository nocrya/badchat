param(
    [string]$EnvFile = "..\config\.env"
)

$ErrorActionPreference = "Stop"

function Read-EnvFile {
    param([string]$Path)
    $map = @{}
    foreach ($line in Get-Content -Path $Path) {
        $trimmed = $line.Trim()
        if ($trimmed -eq "" -or $trimmed.StartsWith("#")) {
            continue
        }

        $eqIndex = $trimmed.IndexOf("=")
        if ($eqIndex -lt 1) {
            continue
        }

        $key = $trimmed.Substring(0, $eqIndex).Trim()
        $value = $trimmed.Substring($eqIndex + 1).Trim()
        $map[$key] = $value
    }
    return $map
}

function Get-Value {
    param(
        [hashtable]$Map,
        [string]$Key,
        [string]$DefaultValue = ""
    )
    if ($Map.ContainsKey($Key) -and $Map[$Key] -ne "") {
        return $Map[$Key]
    }
    return $DefaultValue
}

function Write-IniWithBackup {
    param(
        [string]$Path,
        [string]$Content
    )
    if (Test-Path $Path) {
        Copy-Item -Path $Path -Destination "$Path.bak" -Force
    }
    [System.IO.File]::WriteAllText($Path, $Content, [System.Text.Encoding]::ASCII)
    Write-Host "Updated: $Path"
}

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$repoRoot = Resolve-Path (Join-Path $scriptDir "..")
$envRawPath = Join-Path $scriptDir $EnvFile

if (-not (Test-Path $envRawPath)) {
    throw "Env file not found: $envRawPath. Copy config/.env.example to config/.env first."
}

$envPath = Resolve-Path $envRawPath
$cfg = Read-EnvFile -Path $envPath

$mysqlHost = Get-Value $cfg "MYSQL_HOST" "127.0.0.1"
$mysqlPort = Get-Value $cfg "MYSQL_PORT" "3308"
$mysqlUser = Get-Value $cfg "MYSQL_USER" "root"
$mysqlPass = Get-Value $cfg "MYSQL_PASSWORD" "change-me"
$mysqlDb = Get-Value $cfg "MYSQL_DB" "qiqi"

$redisHost = Get-Value $cfg "REDIS_HOST" "127.0.0.1"
$redisPort = Get-Value $cfg "REDIS_PORT" "6379"
$redisPass = Get-Value $cfg "REDIS_PASSWORD" "change-me"

$gateHost = Get-Value $cfg "GATE_HOST" "127.0.0.1"
$gatePort = Get-Value $cfg "GATE_PORT" "8080"
$varifyHost = Get-Value $cfg "VARIFY_HOST" "127.0.0.1"
$varifyPort = Get-Value $cfg "VARIFY_GRPC_PORT" "50051"
$statusHost = Get-Value $cfg "STATUS_HOST" "127.0.0.1"
$statusPort = Get-Value $cfg "STATUS_GRPC_PORT" "50052"
$chat1Host = Get-Value $cfg "CHAT1_HOST" "127.0.0.1"
$chat1Port = Get-Value $cfg "CHAT1_PORT" "8090"
$chat1Rpc = Get-Value $cfg "CHAT1_RPC_PORT" "50055"
$chat2Host = Get-Value $cfg "CHAT2_HOST" "127.0.0.1"
$chat2Port = Get-Value $cfg "CHAT2_PORT" "8091"
$chat2Rpc = Get-Value $cfg "CHAT2_RPC_PORT" "50056"
$resHost = Get-Value $cfg "RESOURCE_HOST" "127.0.0.1"
$resPort = Get-Value $cfg "RESOURCE_PORT" "9090"
$resRpc = Get-Value $cfg "RESOURCE_RPC_PORT" "51055"

$configDir = Join-Path $repoRoot "config"
$gateIniPath     = Join-Path $configDir "gate.ini"
$statusIniPath   = Join-Path $configDir "status.ini"
$chat1IniPath    = Join-Path $configDir "chat1.ini"
$chat2IniPath    = Join-Path $configDir "chat2.ini"
$resourceIniPath = Join-Path $configDir "resource.ini"
$clientIniPath   = Join-Path (Resolve-Path (Join-Path $repoRoot "..")) "client\config.ini"

$gateIni = @"
[GateServer]
Port = $gatePort
[VarifyServer]
Host = $varifyHost
Port = $varifyPort
[StatusServer]
Host = $statusHost
Port = $statusPort
[Mysql]
Host = $mysqlHost
Port = $mysqlPort
User = $mysqlUser
Passwd = $mysqlPass
Schema = $mysqlDb
[Redis]
Host = $redisHost
Port = $redisPort
Passwd = $redisPass
"@

$statusIni = @"
[StatusServer]
Host = $statusHost
Port = $statusPort
[Mysql]
Host = $mysqlHost
Port = $mysqlPort
User = $mysqlUser
Passwd = $mysqlPass
Schema = $mysqlDb
[Redis]
Host = $redisHost
Port = $redisPort
Passwd = $redisPass
[chatservers]
Name = chatserver1,chatserver2
[chatserver1]
Name = chatserver1
Host = $chat1Host
Port = $chat1Port
[chatserver2]
Name = chatserver2
Host = $chat2Host
Port = $chat2Port
"@

$chat1Ini = @"
[GateServer]
Port = $gatePort
[VarifyServer]
Host = $varifyHost
Port = $varifyPort
[StatusServer]
Host = $statusHost
Port = $statusPort
[SelfServer]
Name = chatserver1
Host = $chat1Host
Port = $chat1Port
RPCPort = $chat1Rpc
[Mysql]
Host = $mysqlHost
Port = $mysqlPort
User = $mysqlUser
Passwd = $mysqlPass
Schema = $mysqlDb
[Redis]
Host = $redisHost
Port = $redisPort
Passwd = $redisPass
[PeerServer]
Servers = chatserver2
[chatserver2]
Name = chatserver2
Host = $chat2Host
Port = $chat2Rpc
"@

$chat2Ini = @"
[GateServer]
Port = $gatePort
[VarifyServer]
Host = $varifyHost
Port = $varifyPort
[StatusServer]
Host = $statusHost
Port = $statusPort
[SelfServer]
Name = chatserver2
Host = $chat2Host
Port = $chat2Port
RPCPort = $chat2Rpc
[Mysql]
Host = $mysqlHost
Port = $mysqlPort
User = $mysqlUser
Passwd = $mysqlPass
Schema = $mysqlDb
[Redis]
Host = $redisHost
Port = $redisPort
Passwd = $redisPass
[PeerServer]
Servers = chatserver1
[chatserver1]
Name = chatserver1
Host = $chat1Host
Port = $chat1Rpc
"@

$resourceIni = @"
[GateServer]
Port = $gatePort
[VarifyServer]
Host = $varifyHost
Port = $varifyPort
[StatusServer]
Host = $statusHost
Port = $statusPort
[SelfServer]
Name = resserver
Host = $resHost
Port = $resPort
RPCPort = $resRpc
[Mysql]
Host = $mysqlHost
Port = $mysqlPort
User = $mysqlUser
Passwd = $mysqlPass
Schema = $mysqlDb
[Redis]
Host = $redisHost
Port = $redisPort
Passwd = $redisPass
[Static]
Path = static
[Output]
Path = bin
"@

$clientIni = @"
[GateServer]
host=$gateHost
port=$gatePort
"@

Write-IniWithBackup -Path $gateIniPath     -Content $gateIni
Write-IniWithBackup -Path $statusIniPath   -Content $statusIni
Write-IniWithBackup -Path $chat1IniPath    -Content $chat1Ini
Write-IniWithBackup -Path $chat2IniPath    -Content $chat2Ini
Write-IniWithBackup -Path $resourceIniPath -Content $resourceIni

if (Test-Path (Split-Path $clientIniPath -Parent)) {
    Write-IniWithBackup -Path $clientIniPath -Content $clientIni
}

Write-Host "All config.ini files are synchronized from $envPath"

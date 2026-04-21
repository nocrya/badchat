# conan-clean.ps1
# 清理 Conan 缓存和输出文件

param(
    [switch]$All,
    [switch]$Help
)

function Show-Help {
    Write-Host @"
清理 Conan 缓存和输出文件

用法:
  .\conan-clean.ps1 [-All]

参数:
  -All    清理所有 Conan 缓存 (包括全局缓存)

示例:
  .\conan-clean.ps1           # 仅清理项目输出
  .\conan-clean.ps1 -All      # 清理所有缓存

"@ -ForegroundColor Cyan
}

if ($Help) {
    Show-Help
    exit 0
}

$projectRoot = Split-Path -Parent $PSScriptRoot
Set-Location $projectRoot

Write-Host "==================================================" -ForegroundColor Cyan
Write-Host "  清理 Conan 文件" -ForegroundColor Cyan
Write-Host "==================================================" -ForegroundColor Cyan
Write-Host ""

# 清理项目输出
if (Test-Path "$projectRoot\conan_output") {
    Write-Host "删除 conan_output 目录..." -ForegroundColor Yellow
    Remove-Item -Path "$projectRoot\conan_output" -Recurse -Force
    Write-Host "✓ 已清理" -ForegroundColor Green
} else {
    Write-Host "✓ conan_output 不存在" -ForegroundColor Gray
}

# 清理 Conan 配置文件
if (Test-Path "$projectRoot\conanbuildinfo.txt") {
    Remove-Item -Path "$projectRoot\conanbuildinfo.txt" -Force
    Write-Host "✓ 已清理 conanbuildinfo.txt" -ForegroundColor Green
}

# 如果指定 -All，清理全局缓存
if ($All) {
    Write-Host ""
    Write-Host "清理全局 Conan 缓存..." -ForegroundColor Yellow
    conan remove '*' -c -q
    Write-Host "✓ 已清理全局缓存" -ForegroundColor Green
}

Write-Host ""
Write-Host "==================================================" -ForegroundColor Green
Write-Host "  清理完成!" -ForegroundColor Green
Write-Host "==================================================" -ForegroundColor Green

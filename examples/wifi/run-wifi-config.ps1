# SPDX-License-Identifier: Apache-2.0
# WiFi Configuration Launcher Script
# Simplified runner for WiFi priority configuration

param(
    [string]$Port,
    [switch]$COM7,
    [switch]$Help
)

# Display help
if ($Help) {
    Write-Host @"
WiFi Configuration Launcher
════════════════════════════════════════

USAGE:
  .\run-wifi-config.ps1              # Default (no port specified)
  .\run-wifi-config.ps1 -COM7        # Run with COM7 port
  .\run-wifi-config.ps1 -Port COM6   # Run with specific port

EXAMPLES:
  .\run-wifi-config.ps1
  .\run-wifi-config.ps1 -COM7
  .\run-wifi-config.ps1 -Port COM8

"@
    exit 0
}

# Determine port
$TargetPort = $null
if ($COM7) {
    $TargetPort = "-Port COM7"
}
elseif ($Port) {
    $TargetPort = "-Port $Port"
}

# Change to script directory
$ScriptDir = "c:\Users\chuky\ESP-IDF\examples\wifi"
Set-Location $ScriptDir

Write-Host "📁 Working directory: $((Get-Location).Path)" -ForegroundColor Cyan
Write-Host ""

# Run configuration
if ($TargetPort) {
    Write-Host "🚀 Running: .\configure-wifi-priorities.ps1 $TargetPort" -ForegroundColor Green
    & .\configure-wifi-priorities.ps1 $TargetPort
}
else {
    Write-Host "🚀 Running: .\configure-wifi-priorities.ps1" -ForegroundColor Green
    & .\configure-wifi-priorities.ps1
}

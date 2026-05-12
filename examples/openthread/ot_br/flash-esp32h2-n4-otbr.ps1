# Flash and monitor OpenThread Border Router (ot_br) on ESP32-H2 N4
# Usage: .\flash-esp32h2-n4-otbr.ps1 [-Port COM6] [-BuildOnly] [-MonitorOnly]

param(
    [string]$Port = "COM6",
    [switch]$BuildOnly,
    [switch]$MonitorOnly
)

$WifiScript = (Resolve-Path (Join-Path $PSScriptRoot "..\..\wifi\build-flash-otbr.ps1")).Path

if ($BuildOnly) {
    & $WifiScript -BuildOnly -Port $Port
} elseif ($MonitorOnly) {
    & $WifiScript -Monitor -Port $Port
} else {
    & $WifiScript -BuildFlashMonitor -Port $Port
}
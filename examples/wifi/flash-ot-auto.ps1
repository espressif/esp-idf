# SPDX-License-Identifier: Apache-2.0
# Auto-flash OpenThread Border Router for ESP32-H2 N4 on a COM port.

param(
    [string]$Port = "COM6",
    [switch]$ShowOnly,
    [switch]$Help
)

$BuildFlashScript = Join-Path $PSScriptRoot "build-flash-otbr.ps1"

function Show-Help {
    $helpText = @(
        "Auto OpenThread Flash Utility"
        ""
        "USAGE:"
        "  .\flash-ot-auto.ps1 [-Port COM6] [-ShowOnly]"
        ""
        "BEHAVIOR:"
        "  Detects chip on selected COM port:"
        "    - ESP32-H2 -> builds and flashes the OTBR firmware for ESP32-H2 N4"
        ""
        "OPTIONS:"
        "  -Port COM#   Serial port to use (default: COM6)"
        "  -ShowOnly    Print selected command without flashing"
        "  -Help        Show this help"
    )

    Write-Host ($helpText -join "`n")
}

function Test-RequiredPath([string]$path) {
    if (-not (Test-Path $path)) {
        throw "Required path not found: $path"
    }
}

function Get-ChipType([string]$comPort) {
    $output = python -m esptool --port $comPort chip-id 2>&1
    $joined = $output -join "`n"

    if ($LASTEXITCODE -ne 0) {
        throw "Unable to query chip on $comPort.`n$joined"
    }

    if ($joined -match "ESP32-H2") {
        return "ESP32-H2"
    }

    return "UNKNOWN"
}

function Invoke-Flash([string]$comPort, [switch]$showOnly) {
    Test-RequiredPath $BuildFlashScript

    $command = "& `"$BuildFlashScript`" -BuildFlashMonitor -Port $comPort"
    Write-Host "Selected ESP32-H2 N4 flash flow:" -ForegroundColor Cyan
    Write-Host $command -ForegroundColor Gray

    if ($showOnly) {
        return
    }

    & $BuildFlashScript -BuildFlashMonitor -Port $comPort
    if ($LASTEXITCODE -ne 0) {
        throw "Flash failed with exit code $LASTEXITCODE"
    }

    Write-Host "Flash completed successfully." -ForegroundColor Green
}

if ($Help) {
    Show-Help
    exit 0
}

try {
    Write-Host "Detecting chip on $Port..." -ForegroundColor Yellow
    $chip = Get-ChipType -comPort $Port
    Write-Host "Detected chip: $chip" -ForegroundColor Cyan

    switch ($chip) {
        "ESP32-H2" {
            Write-Host "Role selected: OTBR (ESP32-H2 N4 profile)" -ForegroundColor Green
            Invoke-Flash -comPort $Port -showOnly:$ShowOnly
        }
        default {
            throw "Unsupported/unknown chip on $Port. Expected ESP32-H2."
        }
    }
}
catch {
    Write-Host "ERROR: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

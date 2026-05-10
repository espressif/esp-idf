# SPDX-License-Identifier: Apache-2.0
# Build and Flash OpenThread Border Router for ESP32-C6

param(
    [string]$Port = "COM6",
    [switch]$BuildOnly,
    [switch]$FlashOnly,
    [switch]$Monitor,
    [switch]$BuildFlashMonitor,
    [switch]$Clean,
    [switch]$Menuconfig,
    [switch]$CheckPortOnly,
    [switch]$SkipChipCheck,
    [switch]$Help
)

$OTBRPath = "c:\Users\chuky\esp-idf\examples\openthread\ot_br"
$Target = "esp32c6"

function Write-Header {
    Write-Host "`n╔════════════════════════════════════════════════════════╗" -ForegroundColor Cyan
    Write-Host "║   OpenThread Border Router - ESP32-C6 Builder/Flasher   ║" -ForegroundColor Cyan
    Write-Host "╚════════════════════════════════════════════════════════╝`n" -ForegroundColor Cyan
}

function Show-Help {
    Write-Host @"
OpenThread Border Router Build & Flash Script for ESP32-C6

USAGE:
  .\build-flash-otbr.ps1 [options]

OPTIONS:
  -Port COM#                  Serial port (default: COM6)
  -BuildOnly                  Build firmware only (no flash)
  -FlashOnly                  Flash only (assumes build exists)
  -Monitor                    Start serial monitor after flash
  -BuildFlashMonitor          Build, flash, and monitor in one go
  -Clean                      Clean build (remove build directory)
  -Menuconfig                 Open configuration menu
    -CheckPortOnly              Enumerate and validate selected OTBR port
    -SkipChipCheck              Skip chip type validation (not recommended)
  -Help                       Show this help message

EXAMPLES:
  # Full cycle: build, flash, and monitor
  .\build-flash-otbr.ps1 -BuildFlashMonitor

  # Build only
  .\build-flash-otbr.ps1 -BuildOnly

  # Flash to COM7
  .\build-flash-otbr.ps1 -Port COM7

  # Flash with monitoring
  .\build-flash-otbr.ps1 -Monitor

  # Configure options before build
  .\build-flash-otbr.ps1 -Menuconfig -BuildOnly

  # Verify COM6 is OTBR-capable before flashing
  .\build-flash-otbr.ps1 -Port COM6 -CheckPortOnly

"@
}

function Get-PortInfo {
    param(
        [string]$ComPort
    )

    $allPorts = Get-PnpDevice -PresentOnly -Class Ports -ErrorAction SilentlyContinue
    if (-not $allPorts) {
        return $null
    }

    foreach ($entry in $allPorts) {
        if ($entry.FriendlyName -match "\(($ComPort)\)") {
            return $entry
        }
    }

    return $null
}

function Get-EspChipType {
    param(
        [string]$ComPort
    )

    $esptoolOutput = python -m esptool --port $ComPort chip-id 2>&1
    if ($LASTEXITCODE -ne 0) {
        return [PSCustomObject]@{
            Success = $false
            ChipType = $null
            RawOutput = ($esptoolOutput -join "`n")
        }
    }

    $joined = $esptoolOutput -join "`n"
    $match = [regex]::Match($joined, "Connected to\s+(ESP32-[A-Za-z0-9]+)")
    if ($match.Success) {
        return [PSCustomObject]@{
            Success = $true
            ChipType = $match.Groups[1].Value
            RawOutput = $joined
        }
    }

    return [PSCustomObject]@{
        Success = $true
        ChipType = "Unknown"
        RawOutput = $joined
    }
}

function Show-PortCapability {
    param(
        [string]$ComPort,
        [bool]$RequireC6 = $false
    )

    Write-Host "Checking serial port capability for $ComPort ..." -ForegroundColor Yellow

    $portInfo = Get-PortInfo -ComPort $ComPort
    if (-not $portInfo) {
        Write-Host "ERROR: $ComPort is not currently present." -ForegroundColor Red
        Write-Host "Hint: reconnect the board and run this check again." -ForegroundColor Red
        return $false
    }

    Write-Host "Port detected: $($portInfo.FriendlyName)" -ForegroundColor Green
    Write-Host "Instance ID:  $($portInfo.InstanceId)" -ForegroundColor Gray

    if ($SkipChipCheck) {
        Write-Host "Chip check skipped by user request." -ForegroundColor Yellow
        return $true
    }

    $chip = Get-EspChipType -ComPort $ComPort
    if (-not $chip.Success) {
        Write-Host "ERROR: unable to query chip on $ComPort." -ForegroundColor Red
        Write-Host $chip.RawOutput -ForegroundColor DarkGray
        return $false
    }

    Write-Host "Detected chip: $($chip.ChipType)" -ForegroundColor Cyan

    if ($RequireC6 -and $chip.ChipType -ne "ESP32-C6") {
        Write-Host "ERROR: OTBR requires ESP32-C6, but $ComPort is $($chip.ChipType)." -ForegroundColor Red
        Write-Host "Do not flash OTBR firmware to ESP32-H2 (RCP) ports." -ForegroundColor Red
        return $false
    }

    if ($chip.ChipType -eq "ESP32-C6") {
        Write-Host "OK: $ComPort is OTBR-capable (ESP32-C6)." -ForegroundColor Green
    }
    elseif ($chip.ChipType -eq "ESP32-H2") {
        Write-Host "Note: $ComPort is ESP32-H2 (RCP role)." -ForegroundColor Yellow
    }

    return $true
}

function Test-Path-Exists {
    if (-not (Test-Path $OTBRPath)) {
        Write-Host "❌ Error: OTBR project not found at: $OTBRPath" -ForegroundColor Red
        exit 1
    }
}

function Set-Target {
    Write-Host "🎯 Setting target to $Target..." -ForegroundColor Yellow
    Push-Location $OTBRPath
    idf.py set-target $Target
    Pop-Location
}

function Invoke-Menuconfig {
    Write-Host "⚙️  Opening configuration menu..." -ForegroundColor Yellow
    Push-Location $OTBRPath
    idf.py menuconfig
    Pop-Location
}

function Invoke-Clean {
    Write-Host "🧹 Cleaning build directory..." -ForegroundColor Yellow
    Push-Location $OTBRPath
    Remove-Item -Path "build" -Recurse -Force -ErrorAction SilentlyContinue
    idf.py fullclean
    Pop-Location
    Write-Host "✓ Build directory cleaned`n" -ForegroundColor Green
}

function Invoke-Build {
    Write-Host "🔨 Building OTBR firmware for $Target..." -ForegroundColor Yellow
    Write-Host "   Path: $OTBRPath`n" -ForegroundColor Gray
    
    Push-Location $OTBRPath
    $StartTime = Get-Date
    
    idf.py build
    
    $Duration = ((Get-Date) - $StartTime).TotalSeconds
    if ($LASTEXITCODE -eq 0) {
        Write-Host "`n✓ Build completed successfully in $([math]::Round($Duration, 1))s" -ForegroundColor Green
    }
    else {
        Write-Host "`n❌ Build failed!" -ForegroundColor Red
        Pop-Location
        exit 1
    }
    
    Pop-Location
}

function Invoke-Flash {
    param(
        [string]$ComPort
    )
    
    if (-not (Show-PortCapability -ComPort $ComPort -RequireC6 $true)) {
        exit 1
    }

    Write-Host "📤 Flashing OTBR to $ComPort..." -ForegroundColor Yellow
    
    Push-Location $OTBRPath
    idf.py -p $ComPort flash
    
    if ($LASTEXITCODE -eq 0) {
        Write-Host "✓ Flash completed successfully`n" -ForegroundColor Green
    }
    else {
        Write-Host "❌ Flash failed!" -ForegroundColor Red
        Pop-Location
        exit 1
    }
    
    Pop-Location
}

function Invoke-Monitor {
    param(
        [string]$ComPort
    )
    
    if (-not (Show-PortCapability -ComPort $ComPort -RequireC6 $false)) {
        exit 1
    }

    Write-Host "📡 Opening serial monitor on $ComPort..." -ForegroundColor Yellow
    Write-Host "   (Press Ctrl+] to exit monitor)`n" -ForegroundColor Gray
    
    Push-Location $OTBRPath
    idf.py -p $ComPort monitor
    Pop-Location
}

function Show-Summary {
    Write-Host "╔════════════════════════════════════════════════════════╗" -ForegroundColor Green
    Write-Host "║                    Summary                             ║" -ForegroundColor Green
    Write-Host "╠════════════════════════════════════════════════════════╣" -ForegroundColor Green
    Write-Host "║ ✓ Target: $Target" -PadRight 56 -ForegroundColor Green
    Write-Host "║ ✓ Project: OTBR (ot_br)" -PadRight 56 -ForegroundColor Green
    Write-Host "║ ✓ Serial Port: $Port" -PadRight 56 -ForegroundColor Green
    Write-Host "║" -ForegroundColor Green
    Write-Host "║ Next Steps:" -ForegroundColor Green
    Write-Host "║ 1. Configure WiFi in OTBR console:" -ForegroundColor Green
    Write-Host "║    wifi_set_sta_config --ssid <SSID> --password <PASS>" -ForegroundColor Green
    Write-Host "║" -ForegroundColor Green
    Write-Host "║ 2. Verify network formation:" -ForegroundColor Green
    Write-Host "║    state    # Should show 'leader' or 'router'" -ForegroundColor Green
    Write-Host "║" -ForegroundColor Green
    Write-Host "║ 3. Add to Home Assistant:" -ForegroundColor Green
    Write-Host "║    Settings → Devices & Services → Thread" -ForegroundColor Green
    Write-Host "║" -ForegroundColor Green
    Write-Host "╚════════════════════════════════════════════════════════╝" -ForegroundColor Green
}

# Main execution
Write-Header

if ($Help) {
    Show-Help
    exit 0
}

if ($CheckPortOnly) {
    if (Show-PortCapability -ComPort $Port -RequireC6 $true) {
        exit 0
    }
    exit 1
}

# Verify project exists
Test-Path-Exists

# Set target
Set-Target

# Handle clean
if ($Clean) {
    Invoke-Clean
}

# Handle menuconfig
if ($Menuconfig) {
    Invoke-Menuconfig
}

# Determine what to do
if ($BuildFlashMonitor) {
    Invoke-Build
    Invoke-Flash -ComPort $Port
    Invoke-Monitor -ComPort $Port
}
elseif ($BuildOnly) {
    Invoke-Build
}
elseif ($FlashOnly) {
    Invoke-Flash -ComPort $Port
    if ($Monitor) {
        Invoke-Monitor -ComPort $Port
    }
}
elseif ($Monitor) {
    Invoke-Monitor -ComPort $Port
}
else {
    # Default: build only
    Invoke-Build
}

Show-Summary

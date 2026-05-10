# SPDX-License-Identifier: Apache-2.0
# Auto-select OpenThread image by detected chip on a COM port.
# - ESP32-C6 -> flash OTBR image set (esp32-otbr-binaries)
# - ESP32-H2 -> flash RCP image set (esp32-radio-binaries)

param(
    [string]$Port = "COM6",
    [int]$Baud = 460800,
    [switch]$NoStub,
    [switch]$ShowOnly,
    [switch]$Help
)

$RepoRoot = "c:\Users\chuky\esp-idf"
$OtbrBinDir = Join-Path $RepoRoot "esp32-otbr-binaries"
$RcpBinDir = Join-Path $RepoRoot "esp32-radio-binaries"

function Show-Help {
    Write-Host @"
Auto OpenThread Flash Utility

USAGE:
  .\flash-ot-auto.ps1 [-Port COM6] [-Baud 460800] [-NoStub] [-ShowOnly]

BEHAVIOR:
  Detects chip on selected COM port:
    - ESP32-C6 -> flashes OTBR binaries (esp32-otbr-binaries)
    - ESP32-H2 -> flashes RCP binaries (esp32-radio-binaries)

OPTIONS:
  -Port COM#   Serial port to use (default: COM6)
  -Baud N      Flash baudrate (default: 460800)
  -NoStub      Disable esptool stub loader
  -ShowOnly    Print selected command without flashing
  -Help        Show this help
"@
}

function Test-RequiredPath([string]$path) {
    if (-not (Test-Path $path)) {
        throw "Required path not found: $path"
    }
}

function Get-ChipType([string]$comPort) {
    $output = python -m esptool --port $comPort chip_id 2>&1
    $joined = $output -join "`n"

    if ($LASTEXITCODE -ne 0) {
        throw "Unable to query chip on $comPort.`n$joined"
    }

    if ($joined -match "ESP32-C6") {
        return "ESP32-C6"
    }

    if ($joined -match "ESP32-H2") {
        return "ESP32-H2"
    }

    return "UNKNOWN"
}

function Get-FlashSpec([string]$jsonPath) {
    $spec = Get-Content $jsonPath -Raw | ConvertFrom-Json

    $chip = $spec.extra_esptool_args.chip
    $before = $spec.extra_esptool_args.before
    $after = $spec.extra_esptool_args.after
    $stub = [bool]$spec.extra_esptool_args.stub
    $flashMode = $spec.flash_settings.flash_mode
    $flashSize = $spec.flash_settings.flash_size
    $flashFreq = $spec.flash_settings.flash_freq

    $entries = @()
    foreach ($p in ($spec.flash_files.PSObject.Properties | Sort-Object Name)) {
        $entries += $p.Name
        $entries += $p.Value
    }

    return [PSCustomObject]@{
        Chip = $chip
        Before = $before
        After = $after
        Stub = $stub
        FlashMode = $flashMode
        FlashSize = $flashSize
        FlashFreq = $flashFreq
        Entries = $entries
    }
}

function Normalize-ResetArg([string]$value) {
    if ([string]::IsNullOrWhiteSpace($value)) {
        return $value
    }

    return $value.Replace("-", "_")
}

function Invoke-Flash([string]$binDir, [string]$comPort, [int]$baudRate, [switch]$disableStub, [switch]$showOnly) {
    $jsonPath = Join-Path $binDir "flasher_args.json"
    Test-RequiredPath $jsonPath

    $spec = Get-FlashSpec $jsonPath

    $args = @(
        "-m", "esptool",
        "--chip", $spec.Chip,
        "--port", $comPort,
        "--baud", "$baudRate",
        "--before", (Normalize-ResetArg $spec.Before),
        "--after", (Normalize-ResetArg $spec.After),
        "write_flash",
        "--flash_mode", $spec.FlashMode,
        "--flash_size", $spec.FlashSize,
        "--flash_freq", $spec.FlashFreq
    )

    if (-not $disableStub -and -not $spec.Stub) {
        $args += "--no-stub"
    }

    if ($disableStub) {
        $args += "--no-stub"
    }

    $args += $spec.Entries

    Write-Host "Selected flash spec from: $jsonPath" -ForegroundColor Cyan
    Write-Host "Command:" -ForegroundColor Yellow
    Write-Host ("python " + ($args -join " ")) -ForegroundColor Gray

    if ($showOnly) {
        return
    }

    Push-Location $binDir
    try {
        & python @args
        if ($LASTEXITCODE -ne 0) {
            throw "Flash failed with exit code $LASTEXITCODE"
        }
    }
    finally {
        Pop-Location
    }

    Write-Host "Flash completed successfully." -ForegroundColor Green
}

if ($Help) {
    Show-Help
    exit 0
}

try {
    Test-RequiredPath $OtbrBinDir
    Test-RequiredPath $RcpBinDir

    Write-Host "Detecting chip on $Port..." -ForegroundColor Yellow
    $chip = Get-ChipType -comPort $Port
    Write-Host "Detected chip: $chip" -ForegroundColor Cyan

    switch ($chip) {
        "ESP32-C6" {
            Write-Host "Role selected: OTBR (Border Router)" -ForegroundColor Green
            Invoke-Flash -binDir $OtbrBinDir -comPort $Port -baudRate $Baud -disableStub:$NoStub -showOnly:$ShowOnly
        }
        "ESP32-H2" {
            Write-Host "Role selected: RCP (Radio Co-Processor)" -ForegroundColor Green
            Invoke-Flash -binDir $RcpBinDir -comPort $Port -baudRate $Baud -disableStub:$NoStub -showOnly:$ShowOnly
        }
        default {
            throw "Unsupported/unknown chip on $Port. Expected ESP32-C6 or ESP32-H2."
        }
    }
}
catch {
    Write-Host "ERROR: $($_.Exception.Message)" -ForegroundColor Red
    exit 1
}

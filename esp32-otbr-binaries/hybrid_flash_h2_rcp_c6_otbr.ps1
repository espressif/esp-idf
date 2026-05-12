param(
    [string]$OtbrPort = "COM6",
    [string]$RcpPort = "COM8",
    [int]$Baud = 460800
)

$ErrorActionPreference = "Stop"
if ($null -ne (Get-Variable -Name PSNativeCommandUseErrorActionPreference -ErrorAction SilentlyContinue)) {
    $PSNativeCommandUseErrorActionPreference = $false
}

$otbrBase = Split-Path -Parent $MyInvocation.MyCommand.Path
$rcpBuild = "C:\Users\chuky\esp-idf\examples\openthread\ot_rcp\build"

$otbrBoot = Join-Path $otbrBase "bootloader.bin"
$otbrPart = Join-Path $otbrBase "partition-table.bin"
$otbrApp = Join-Path $otbrBase "esp_ot_br.bin"

$rcpBoot = Join-Path $rcpBuild "bootloader\bootloader.bin"
$rcpPart = Join-Path $rcpBuild "partition_table\partition-table.bin"
$rcpApp = Join-Path $rcpBuild "esp_ot_rcp.bin"

function Fail([string]$msg) {
    Write-Output "ERR $msg"
    exit 1
}

function Check-File([string]$path, [string]$name) {
    if (-not (Test-Path $path)) {
        Fail "MISSING_FILE $name"
    }
}

function Read-ChipType([string]$port) {
    $out = cmd /c "python -m esptool --port $port chip-id 2>nul"
    if ($LASTEXITCODE -ne 0) {
        return $null
    }

    $chipLine = $out | Select-String -Pattern "Detecting chip type\.\.\." | Select-Object -First 1
    if ($chipLine) {
        return ($chipLine.Line -replace "^.*Detecting chip type\.\.\.\s*", "").Trim()
    }

    $chipLine = $out | Select-String -Pattern "Chip is\s+" | Select-Object -First 1
    if ($chipLine) {
        return ($chipLine.Line -replace "^.*Chip is\s+", "").Trim()
    }

    return $null
}

function Assert-Chip([string]$port, [string]$mustContain, [string]$role) {
    $chip = Read-ChipType $port
    if (-not $chip) {
        Fail "NO_CHIP_ID port=$port role=$role"
    }

    if ($chip -notlike "*$mustContain*") {
        Fail "CHIP_MISMATCH port=$port role=$role got=$chip expected=$mustContain"
    }

    Write-Output "OK CHIP port=$port role=$role chip=$chip"
}

function Flash-C6Otbr([string]$port, [int]$baudRate) {
    python -m esptool --chip esp32c6 --port $port --baud $baudRate --before default_reset --after hard_reset write_flash --flash_mode dout --flash_size 2MB --flash_freq 80m 0x0 $otbrBoot 0x8000 $otbrPart 0x10000 $otbrApp
    if ($LASTEXITCODE -ne 0) {
        Fail "FLASH_FAIL role=OTBR port=$port"
    }
    Write-Output "OK FLASH role=OTBR port=$port"
}

function Flash-H2Rcp([string]$port, [int]$baudRate) {
    python -m esptool --chip esp32h2 --port $port --baud $baudRate --before default_reset --after hard_reset write_flash --flash_mode dio --flash_size 2MB --flash_freq 48m 0x0 $rcpBoot 0x8000 $rcpPart 0x10000 $rcpApp
    if ($LASTEXITCODE -ne 0) {
        Fail "FLASH_FAIL role=RCP port=$port"
    }
    Write-Output "OK FLASH role=RCP port=$port"
}

Check-File $otbrBoot "bootloader.bin"
Check-File $otbrPart "partition-table.bin"
Check-File $otbrApp "esp_ot_br.bin"
Check-File $rcpBoot "ot_rcp bootloader"
Check-File $rcpPart "ot_rcp partition-table"
Check-File $rcpApp "esp_ot_rcp.bin"

# Hybrid role assignment:
# - OTBR (C6) on COM6 by default
# - RCP (H2) on COM8 by default
Assert-Chip $OtbrPort "ESP32-C6" "OTBR"
Assert-Chip $RcpPort "ESP32-H2" "RCP"

Flash-C6Otbr $OtbrPort $Baud
Flash-H2Rcp $RcpPort $Baud

Write-Output "OK HYBRID_READY otbr=$OtbrPort rcp=$RcpPort"

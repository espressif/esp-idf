#!/usr/bin/env pwsh

# Emergency backup option to use previous export.ps1 (export_legacy.ps1) if the new export approach fails.
# To use it, set environmental variable like: $Env:ESP_IDF_LEGACY_EXPORT=1
if ($env:ESP_IDF_LEGACY_EXPORT) {
    . ./tools/legacy_exports/export_legacy.ps1
    exit $LASTEXITCODE
}

$idf_path = "$PSScriptRoot"

if (-not (Test-Path "$idf_path/tools/idf.py") -or
    -not (Test-Path "$idf_path/tools/idf_tools.py") -or
    -not (Test-Path "$idf_path/tools/activate.py")) {

    Write-Output "Could not detect IDF_PATH. Please navigate to your ESP-IDF directory and run:"
    Write-Output ".\export.ps1"

    $env:IDF_PATH = ""

    exit 1
}

$idf_exports = python "$idf_path/tools/activate.py" --export
# The dot sourcing is added here in PowerShell since
# Win PSAnalyzer complains about using `Invoke-Expression` command
. $idf_exports

#!/usr/bin/env pwsh

# Emergency backup option to use previous export.ps1 (export_legacy.ps1) if the new export approach fails.
# To use it, set environmental variable like: $Env:ESP_IDF_LEGACY_EXPORT=1
if ($env:ESP_IDF_LEGACY_EXPORT) {
    . ./tools/legacy_exports/export_legacy.ps1
    exit $LASTEXITCODE
}

if ("$env:IDF_PATH" -ne "") {
    $idf_path = "$env:IDF_PATH"
} else {
    Write-Output "No IDF_PATH enviroment variable detected. Using the root directory as IDF_PATH"
    $idf_path = "$PSScriptRoot"
}

if (-not (Test-Path "$idf_path/tools/idf.py") -or
    -not (Test-Path "$idf_path/tools/idf_tools.py") -or
    -not (Test-Path "$idf_path/tools/activate.py")) {
    
    Write-Output "`"$idf_path`" doesn't contain needed scripts. Please, add IDF_PATH enviroment variable:"
    Write-Output '  $env:IDF_PATH=(add path here), '
    Write-Output 'or navigate to your ESP-IDF directory and run:'
    Write-Output '  .\export.ps1'
    
    exit 1
}

$idf_exports = python "$idf_path/tools/activate.py" --export
# The dot sourcing is added here in PowerShell since
# Win PSAnalyzer complains about using `Invoke-Expression` command
. $idf_exports

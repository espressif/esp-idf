#!/usr/bin/env pwsh

$idf_path = "$PSScriptRoot"

if (-not (Test-Path "$idf_path/tools/idf.py") -or
    -not (Test-Path "$idf_path/tools/idf_tools.py") -or
    -not (Test-Path "$idf_path/tools/activate.py")) {

    Write-Output "Could not detect IDF_PATH. Please set it before running this script:"
    Write-Output '  $env:IDF_PATH=(add path here)'

    $env:IDF_PATH = ""

    exit 1
}

$idf_exports = python "$idf_path/tools/activate.py" --export
# The dot sourcing is added here in PowerShell since
# Win PSAnalyzer complains about using `Invoke-Expression` command
. $idf_exports

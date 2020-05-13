#!/usr/bin/env pwsh
$IDF_PATH = $PSScriptRoot


Write-Output "Installing ESP-IDF tools"
Start-Process -Wait -NoNewWindow -FilePath "python" -Args "$IDF_PATH/tools/idf_tools.py install"
if ($LASTEXITCODE -ne 0)  { exit $LASTEXITCODE } # if error

Write-Output "Setting up Python environment"
Start-Process -Wait  -NoNewWindow -FilePath "python" -Args "$IDF_PATH/tools/idf_tools.py install-python-env"
if ($LASTEXITCODE -ne 0)  { exit $LASTEXITCODE} # if error


Write-Output "
All done! You can now run:
    export.ps1
"

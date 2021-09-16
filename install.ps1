#!/usr/bin/env pwsh
$IDF_PATH = $PSScriptRoot

$TARGETS = (python "$IDF_PATH/tools/install_util.py" extract targets "$args")

Write-Output "Installing ESP-IDF tools"
Start-Process -Wait -NoNewWindow -FilePath "python" -Args "$IDF_PATH/tools/idf_tools.py install --targets=${TARGETS}"
if ($LASTEXITCODE -ne 0)  { exit $LASTEXITCODE } # if error

$FEATURES = (python "$IDF_PATH/tools/install_util.py" extract features "$args")

Write-Output "Setting up Python environment"
Start-Process -Wait  -NoNewWindow -FilePath "python" -Args "$IDF_PATH/tools/idf_tools.py install-python-env --features=${FEATURES}"
if ($LASTEXITCODE -ne 0)  { exit $LASTEXITCODE} # if error


Write-Output "
All done! You can now run:
    export.ps1
"

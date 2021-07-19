#!/usr/bin/env pwsh
$IDF_PATH = $PSScriptRoot

if($args.count -eq 0){
    $TARGETS = "all"
}else
{
    $TARGETS = $args[0] -join ','
}
Write-Output "Installing ESP-IDF tools"
Start-Process -Wait -NoNewWindow -FilePath "python" -Args "$IDF_PATH/tools/idf_tools.py install --targets=${TARGETS}"
if ($LASTEXITCODE -ne 0)  { exit $LASTEXITCODE } # if error

Write-Output "Setting up Python environment"
Start-Process -Wait  -NoNewWindow -FilePath "python" -Args "$IDF_PATH/tools/idf_tools.py install-python-env"
if ($LASTEXITCODE -ne 0)  { exit $LASTEXITCODE} # if error


Write-Output "
All done! You can now run:
    export.ps1
"

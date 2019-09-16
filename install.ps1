if ($env:MSYSTEM -ne $null) {
    Write-Output "This .ps1 file is for Windows Powershell only. When using MSYS, run:`n. ./export.sh."
    exit 1
}


$IDF_PATH = $PSScriptRoot


Write-Output "Installing ESP-IDF tools"
Start-Process -Wait -NoNewWindow -FilePath "python.exe" -Args "$IDF_PATH/tools/idf_tools.py install"
if ($LASTEXITCODE -ne 0)  { exit $LASTEXITCODE } # if error

Write-Output "Setting up Python environment"
Start-Process -Wait  -NoNewWindow -FilePath "python.exe" -Args "$IDF_PATH/tools/idf_tools.py install-python-env"
if ($LASTEXITCODE -ne 0)  { exit $LASTEXITCODE} # if error


Write-Output "
All done! You can now run:
    export.ps1
"

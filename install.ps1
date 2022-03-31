#!/usr/bin/env pwsh
$IDF_PATH = $PSScriptRoot

if($args.count -eq 0){
    $TARGETS = "all"
}else
{
    $TARGETS = $args[0] -join ','
}
Write-Output "Installing ESP-IDF tools"
$proces_tools = Start-Process -Wait -PassThru -NoNewWindow -FilePath "python" -Args "$IDF_PATH/tools/idf_tools.py install --targets=${TARGETS}"
$exit_code_tools = $proces_tools.ExitCode
if ($exit_code_tools -ne 0) { exit $exit_code_tools }  # if error

Write-Output "Setting up Python environment"
$proces_py_env = Start-Process -Wait -PassThru  -NoNewWindow -FilePath "python" -Args "$IDF_PATH/tools/idf_tools.py install-python-env --features=${FEATURES}"
$exit_code_py_env = $proces_py_env.ExitCode
if ($exit_code_py_env -ne 0) { exit $exit_code_py_env } # if error


Write-Output "
All done! You can now run:
    export.ps1
"

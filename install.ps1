#!/usr/bin/env pwsh

param(
    [Switch]$h
)

$IDF_PATH = $PSScriptRoot

if($h){
python "$IDF_PATH/tools/install_util.py" print_help ps1
Exit
}

$TARGETS = (python "$IDF_PATH/tools/install_util.py" extract targets "$args")

Write-Output "Installing ESP-IDF tools"
$proces_tools = Start-Process -Wait -PassThru -NoNewWindow -FilePath "python" -Args "`"$IDF_PATH/tools/idf_tools.py`" install --targets=${TARGETS}"
$exit_code_tools = $proces_tools.ExitCode
if ($exit_code_tools -ne 0) { exit $exit_code_tools }  # if error

$FEATURES = (python "$IDF_PATH/tools/install_util.py" extract features "$args")

Write-Output "Setting up Python environment"
$proces_py_env = Start-Process -Wait -PassThru  -NoNewWindow -FilePath "python" -Args "`"$IDF_PATH/tools/idf_tools.py`" install-python-env --features=${FEATURES}"
$exit_code_py_env = $proces_py_env.ExitCode
if ($exit_code_py_env -ne 0) { exit $exit_code_py_env } # if error


Write-Output "
All done! You can now run:
    export.ps1
"

#!/usr/bin/env pwsh

param(
    [Switch]$h
)

$IDF_PATH = $PSScriptRoot

if($h){
python "$IDF_PATH/tools/install_util.py" print_help ps1
Exit
}

Write-Output "INFO: Using IDF_PATH '$IDF_PATH' for installation."

$TARGETS = (python "$IDF_PATH/tools/install_util.py" extract targets "$args")

Write-Output "Installing ESP-IDF tools"
# PowerShell does not propagate variables to new process (spawned python), so we pass detected IDF_PATH as argument
# to avoid using any previously set IDF_PATH values in the terminal environment.
$proces_tools = Start-Process -Wait -PassThru -NoNewWindow -FilePath "python" -Args "`"$IDF_PATH/tools/idf_tools.py`" --idf-path ${IDF_PATH} install --targets=${TARGETS}"
$exit_code_tools = $proces_tools.ExitCode
if ($exit_code_tools -ne 0) { exit $exit_code_tools }  # if error

$FEATURES = (python "$IDF_PATH/tools/install_util.py" extract features "$args")

Write-Output "Setting up Python environment"
# PowerShell does not propagate variables to new process (spawned python), so we pass detected IDF_PATH as argument
# to avoid using any previously set IDF_PATH values in the terminal environment.
$proces_py_env = Start-Process -Wait -PassThru -NoNewWindow -FilePath "python" -Args "`"$IDF_PATH/tools/idf_tools.py`" --idf-path ${IDF_PATH} install-python-env --features=${FEATURES}"
$exit_code_py_env = $proces_py_env.ExitCode
if ($exit_code_py_env -ne 0) { exit $exit_code_py_env } # if error


Write-Output "
All done! You can now run:
    export.ps1
"

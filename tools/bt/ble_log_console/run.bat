@echo off
rem SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
rem SPDX-License-Identifier: Apache-2.0

rem BLE Log Console launcher for Windows CMD.
rem Works from any directory. All arguments are forwarded to console.py.

setlocal

set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"

rem Derive IDF_PATH (three levels up from script directory)
for %%I in ("%SCRIPT_DIR%\..\..\..") do set "IDF_PATH=%%~fI"

echo Activating ESP-IDF environment ...
call "%IDF_PATH%\export.bat" > nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Failed to activate ESP-IDF environment.
    exit /b 1
)

echo Installing extra dependencies ...
python -m pip install --quiet textual textual-fspicker
if %errorlevel% neq 0 (
    echo ERROR: Failed to install dependencies.
    exit /b 1
)

python "%SCRIPT_DIR%\console.py" %*
exit /b %errorlevel%

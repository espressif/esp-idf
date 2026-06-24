@echo off
rem SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
rem SPDX-License-Identifier: Apache-2.0

rem Build a single-file BLE Log Console executable via PyInstaller.
rem The executable is placed in the caller's working directory.
rem All intermediate build artifacts are cleaned up automatically.

setlocal

set "SCRIPT_DIR=%~dp0"
set "SCRIPT_DIR=%SCRIPT_DIR:~0,-1%"
set "CALLER_DIR=%cd%"

rem Derive IDF_PATH (three levels up from script directory)
for %%I in ("%SCRIPT_DIR%\..\..\..") do set "IDF_PATH=%%~fI"

echo Activating ESP-IDF environment ...
call "%IDF_PATH%\export.bat" > nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: Failed to activate ESP-IDF environment.
    exit /b 1
)

echo Installing build dependencies ...
python -m pip install --quiet textual textual-fspicker pyinstaller
if %errorlevel% neq 0 (
    echo ERROR: Failed to install dependencies.
    exit /b 1
)

echo Building executable ...
cd /d "%SCRIPT_DIR%"
python build_exe.py
if %errorlevel% neq 0 (
    echo ERROR: Build failed.
    cd /d "%CALLER_DIR%"
    exit /b 1
)

set "EXE_NAME=ble_log_console.exe"
if exist "dist\%EXE_NAME%" (
    move /y "dist\%EXE_NAME%" "%CALLER_DIR%\%EXE_NAME%" > nul
    echo.
    echo Executable ready: %CALLER_DIR%\%EXE_NAME%
) else (
    echo ERROR: Build produced no executable.
    cd /d "%CALLER_DIR%"
    exit /b 1
)

rem Remove intermediate artifacts
if exist "%SCRIPT_DIR%\build" rd /s /q "%SCRIPT_DIR%\build"
if exist "%SCRIPT_DIR%\dist" rd /s /q "%SCRIPT_DIR%\dist"
del /q "%SCRIPT_DIR%\*.spec" 2>nul

cd /d "%CALLER_DIR%"

@REM Copyright (c) Microsoft. All rights reserved.
@REM Licensed under the MIT license. See LICENSE file in the project root for full license information.

setlocal

set build-root=%~dp0..
rem // resolve to fully qualified path
for %%i in ("%build-root%") do set build-root=%%~fi

echo %build-root%

REM -- C --
cd %build-root%\sdk\c-utility\build_all\windows

call build.cmd %* --build-root %build-root% --solution-name azure_external_unit_tests
if errorlevel 1 goto :eof
cd %build-root%
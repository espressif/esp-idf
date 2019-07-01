@echo off

:: This script is called from a shortcut (cmd.exe /k export_fallback.bat), with
:: the working directory set to an ESP-IDF directory.
:: Its purpose is to support using the "IDF Tools Directory" method of
:: installation for ESP-IDF versions older than IDF v4.0.
:: It does the same thing as "export.bat" in IDF v4.0.

set IDF_PATH=%CD%
if not exist "%IDF_PATH%\tools\idf.py" (
    echo This script must be invoked from ESP-IDF directory.
    goto :end
)

if "%~2"=="" (
    echo Usage: idf_cmd_init.bat ^<Python directory^> ^<Git directory^>
    echo This script must be invoked from ESP-IDF directory.
    goto :end
)

set IDF_PYTHON_DIR=%1
set IDF_GIT_DIR=%2

:: Strip quoutes
set "IDF_PYTHON_DIR=%IDF_PYTHON_DIR:"=%"
set "IDF_GIT_DIR=%IDF_GIT_DIR:"=%"

:: Clear PYTHONPATH as it may contain libraries of other Python versions
if not "%PYTHONPATH%"=="" (
    echo Clearing PYTHONPATH, was set to %PYTHONPATH%
    set PYTHONPATH=
)

:: Add Python and Git paths to PATH
set "PATH=%IDF_PYTHON_DIR%;%IDF_GIT_DIR%;%PATH%"
echo Using Python in %IDF_PYTHON_DIR%
python.exe --version
echo Using Git in %IDF_GIT_DIR%
git.exe --version

:: Check if this is a recent enough copy of ESP-IDF.
:: If so, use export.bat provided there.
:: Note: no "call", will not return into this batch file.
if exist "%IDF_PATH%\export.bat" %IDF_PATH%\export.bat

echo IDF version does not include export.bat. Using the fallback version.

if exist "%IDF_PATH%\tools\tools.json" (
    set "IDF_TOOLS_JSON_PATH=%IDF_PATH%\tools\tools.json"
) else (
    echo IDF version does not include tools\tools.json. Using the fallback version.
    set "IDF_TOOLS_JSON_PATH=%~dp0%tools_fallback.json"
)

if exist "%IDF_PATH%\tools\idf_tools.py" (
    set "IDF_TOOLS_PY_PATH=%IDF_PATH%\tools\idf_tools.py"
) else (
    echo IDF version does not include tools\idf_tools.py. Using the fallback version.
    set "IDF_TOOLS_PY_PATH=%~dp0%idf_tools_fallback.py"
)

echo.
echo Setting IDF_PATH: %IDF_PATH%
echo.

set "OLD_PATH=%PATH%"
echo Adding ESP-IDF tools to PATH...
:: Export tool paths and environment variables.
:: It is possible to do this without a temporary file (running idf_tools.py from for /r command),
:: but that way it is impossible to get the exit code of idf_tools.py.
set "IDF_TOOLS_EXPORTS_FILE=%TEMP%\idf_export_vars.tmp"
python.exe %IDF_TOOLS_PY_PATH% --tools-json %IDF_TOOLS_JSON_PATH% export --format key-value >"%IDF_TOOLS_EXPORTS_FILE%"
if %errorlevel% neq 0 goto :end

for /f "usebackq tokens=1,2 eol=# delims==" %%a in ("%IDF_TOOLS_EXPORTS_FILE%") do (
      call set "%%a=%%b"
    )

:: This removes OLD_PATH substring from PATH, leaving only the paths which have been added,
:: and prints semicolon-delimited components of the path on separate lines
call set PATH_ADDITIONS=%%PATH:%OLD_PATH%=%%
if "%PATH_ADDITIONS%"=="" call :print_nothing_added
if not "%PATH_ADDITIONS%"=="" echo     %PATH_ADDITIONS:;=&echo.    %

echo Checking if Python packages are up to date...
python.exe %IDF_PATH%\tools\check_python_dependencies.py
if %errorlevel% neq 0 goto :end

echo.
echo Done! You can now compile ESP-IDF projects.
echo Go to the project directory and run:
echo.
echo   idf.py build
echo.

goto :end

:print_nothing_added
    echo No directories added to PATH:
    echo.
    echo %PATH%
    echo.
    goto :eof

:end

:: Clean up
if not "%IDF_TOOLS_EXPORTS_FILE%"=="" (
    del "%IDF_TOOLS_EXPORTS_FILE%" 1>nul 2>nul
)
set IDF_TOOLS_EXPORTS_FILE=
set IDF_PYTHON_DIR=
set IDF_GIT_DIR=
set IDF_TOOLS_PY_PATH=
set IDF_TOOLS_JSON_PATH=
set OLD_PATH=
set PATH_ADDITIONS=

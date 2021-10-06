@echo off
if defined MSYSTEM (
    echo This .bat file is for Windows CMD.EXE shell only. When using MSYS, run:
    echo   . ./export.sh.
    goto :eof
)

:: Missing requirements check
set MISSING_REQUIREMENTS=
python.exe --version >NUL 2>NUL
if %errorlevel% neq 0 set "MISSING_REQUIREMENTS=  python &echo\"
git.exe --version >NUL 2>NUL
if %errorlevel% neq 0 set "MISSING_REQUIREMENTS=%MISSING_REQUIREMENTS%  git"

if not "%MISSING_REQUIREMENTS%" == "" goto :error_missing_requirements

:: Infer IDF_PATH from script location
set IDF_PATH=%~dp0
set IDF_PATH=%IDF_PATH:~0,-1%

set "IDF_TOOLS_PY_PATH=%IDF_PATH%\tools\idf_tools.py"
set "IDF_TOOLS_JSON_PATH=%IDF_PATH%\tools\tools.json"
set "IDF_TOOLS_EXPORT_CMD=%IDF_PATH%\export.bat"
set "IDF_TOOLS_INSTALL_CMD=%IDF_PATH%\install.bat"
echo Setting IDF_PATH: %IDF_PATH%
echo.

set "OLD_PATH=%PATH%"
echo Adding ESP-IDF tools to PATH...
:: Export tool paths and environment variables.
:: It is possible to do this without a temporary file (running idf_tools.py from for /r command),
:: but that way it is impossible to get the exit code of idf_tools.py.
set "IDF_TOOLS_EXPORTS_FILE=%TEMP%\idf_export_vars.tmp"
python.exe "%IDF_PATH%\tools\idf_tools.py" export --format key-value >"%IDF_TOOLS_EXPORTS_FILE%"
if %errorlevel% neq 0 goto :end

for /f "usebackq tokens=1,2 eol=# delims==" %%a in ("%IDF_TOOLS_EXPORTS_FILE%") do (
      call set "%%a=%%b"
    )

:: This removes OLD_PATH substring from PATH, leaving only the paths which have been added,
:: and prints semicolon-delimited components of the path on separate lines
call set PATH_ADDITIONS=%%PATH:%OLD_PATH%=%%
if "%PATH_ADDITIONS%"=="" call :print_nothing_added
if not "%PATH_ADDITIONS%"=="" echo     %PATH_ADDITIONS:;=&echo.    %

DOSKEY idf.py=python.exe "%IDF_PATH%\tools\idf.py" $*
DOSKEY esptool.py=python.exe "%IDF_PATH%\components\esptool_py\esptool\esptool.py" $*
DOSKEY espefuse.py=python.exe "%IDF_PATH%\components\esptool_py\esptool\espefuse.py" $*
DOSKEY otatool.py=python.exe "%IDF_PATH%\components\app_update\otatool.py" $*
DOSKEY parttool.py=python.exe "%IDF_PATH%\components\partition_table\parttool.py" $*

echo Checking if Python packages are up to date...
python.exe "%IDF_PATH%\tools\check_python_dependencies.py"
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

:error_missing_requirements
    echo.
    echo Error^: The following tools are not installed in your environment.
    echo.
    echo %MISSING_REQUIREMENTS%
    echo.
    echo Please use the Windows Tool installer for setting up your environment.
    echo Download link: https://dl.espressif.com/dl/esp-idf/
    echo For more details please visit our website: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html
    goto :end

:end

:: Clean up
if not "%IDF_TOOLS_EXPORTS_FILE%"=="" (
    del "%IDF_TOOLS_EXPORTS_FILE%" 1>nul 2>nul
)
set IDF_TOOLS_EXPORTS_FILE=
set IDF_TOOLS_EXPORT_CMD=
set IDF_TOOLS_INSTALL_CMD=
set IDF_TOOLS_PY_PATH=
set IDF_TOOLS_JSON_PATH=
set OLD_PATH=
set PATH_ADDITIONS=
set MISSING_REQUIREMENTS=

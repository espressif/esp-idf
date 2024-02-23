@echo off
if defined MSYSTEM (
    echo This .bat file is for Windows CMD.EXE shell only.
    goto :eof
)

set SCRIPT_EXIT_CODE=0

:: Missing requirements check
set MISSING_REQUIREMENTS=
python.exe --version >NUL 2>NUL
if %errorlevel% neq 0 (
    set SCRIPT_EXIT_CODE=%errorlevel%
    set "MISSING_REQUIREMENTS=  python &echo\"
)
git.exe --version >NUL 2>NUL
if %errorlevel% neq 0 (
    set SCRIPT_EXIT_CODE=%errorlevel%
    set "MISSING_REQUIREMENTS=%MISSING_REQUIREMENTS%  git"
)

if not "%MISSING_REQUIREMENTS%" == "" goto :__error_missing_requirements

:: Infer IDF_PATH from script location
set IDF_PATH=%~dp0
set IDF_PATH=%IDF_PATH:~0,-1%

echo Checking Python compatibility
python.exe "%IDF_PATH%\tools\python_version_checker.py"

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
if %errorlevel% neq 0 (
    set SCRIPT_EXIT_CODE=%errorlevel%
    goto :__end
)

for /f "usebackq tokens=1,2 eol=# delims==" %%a in ("%IDF_TOOLS_EXPORTS_FILE%") do (
      call set "%%a=%%b"
    )

:: This removes OLD_PATH substring from PATH, leaving only the paths which have been added,
:: and prints semicolon-delimited components of the path on separate lines
call set PATH_ADDITIONS=%%PATH:%OLD_PATH%=%%
if "%PATH_ADDITIONS%"=="" call :__print_nothing_added
if not "%PATH_ADDITIONS%"=="" echo     %PATH_ADDITIONS:;=&echo.    %

DOSKEY idf.py=python.exe "%IDF_PATH%\tools\idf.py" $*
DOSKEY esptool.py=python.exe "%IDF_PATH%\components\esptool_py\esptool\esptool.py" $*
DOSKEY espefuse.py=python.exe "%IDF_PATH%\components\esptool_py\esptool\espefuse.py" $*
DOSKEY espsecure.py=python.exe "%IDF_PATH%\components\esptool_py\esptool\espsecure.py" $*
DOSKEY otatool.py=python.exe "%IDF_PATH%\components\app_update\otatool.py" $*
DOSKEY parttool.py=python.exe "%IDF_PATH%\components\partition_table\parttool.py" $*

echo Checking if Python packages are up to date...
python.exe "%IDF_PATH%\tools\idf_tools.py" check-python-dependencies
if %errorlevel% neq 0 (
    set SCRIPT_EXIT_CODE=%errorlevel%
    goto :__end
)

python.exe "%IDF_PATH%\tools\idf_tools.py" uninstall --dry-run > UNINSTALL_OUTPUT
SET /p UNINSTALL=<UNINSTALL_OUTPUT
DEL UNINSTALL_OUTPUT
if NOT "%UNINSTALL%"=="" call :__uninstall_message

echo.
echo Done! You can now compile ESP-IDF projects.
echo Go to the project directory and run:
echo.
echo   idf.py build
echo.

goto :__end

:__print_nothing_added
    echo No directories added to PATH:
    echo.
    echo %PATH%
    echo.
    goto :eof

:__error_missing_requirements
    echo.
    echo Error^: The following tools are not installed in your environment.
    echo.
    echo %MISSING_REQUIREMENTS%
    echo.
    echo Please use the Windows Tool installer for setting up your environment.
    echo Download link: https://dl.espressif.com/dl/esp-idf/
    echo For more details please visit our website: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/windows-setup.html
    goto :__end

:__uninstall_message
    echo.
    echo Detected installed tools that are not currently used by active ESP-IDF version.
    echo %UNINSTALL%
    echo For free up even more space, remove installation packages of those tools. Use option 'python.exe %IDF_PATH%\tools\idf_tools.py uninstall --remove-archives'.
    echo.

:__end
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
set UNINSTALL=
exit /b %SCRIPT_EXIT_CODE%

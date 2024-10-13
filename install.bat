@echo off
if defined MSYSTEM (
	echo This .bat file is for Windows CMD.EXE shell only.
	goto :__end
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

:: Print help if requested
if /I "%1" == "/?" goto :__help
if /I "%1" == "-h" goto :__help
if /I "%1" == "--help" goto :__help

for /f "delims=" %%i in ('python.exe "%IDF_PATH%\tools\install_util.py" extract targets "%*"') do set TARGETS=%%i

echo Installing ESP-IDF tools
python.exe "%IDF_PATH%\tools\idf_tools.py" install --targets=%TARGETS%
if %errorlevel% neq 0 (
    set SCRIPT_EXIT_CODE=%errorlevel%
    goto :__end
)

for /f "delims=" %%i in ('python.exe "%IDF_PATH%\tools\install_util.py" extract features "%*"') do set FEATURES=%%i

echo Setting up Python environment
python.exe "%IDF_PATH%\tools\idf_tools.py" install-python-env --features=%FEATURES%
if %errorlevel% neq 0 (
    set SCRIPT_EXIT_CODE=%errorlevel%
    goto :__end
)

echo All done! You can now run:
echo    export.bat
goto :__end

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

:__help
    python.exe "%IDF_PATH%\tools\install_util.py" print_help bat
    goto :__end

:__end
exit /b %SCRIPT_EXIT_CODE%

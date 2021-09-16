@echo off
if defined MSYSTEM (
	echo This .bat file is for Windows CMD.EXE shell only.
	goto end
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

for /f "delims=" %%i in ('python.exe "%IDF_PATH%\tools\install_util.py" extract targets "%*"') do set TARGETS=%%i

echo Installing ESP-IDF tools
python.exe "%IDF_PATH%\tools\idf_tools.py" install --targets=%TARGETS%
if %errorlevel% neq 0 goto :end

for /f "delims=" %%i in ('python.exe "%IDF_PATH%\tools\install_util.py" extract features "%*"') do set FEATURES=%%i

echo Setting up Python environment
python.exe "%IDF_PATH%\tools\idf_tools.py" install-python-env --features=%FEATURES%
if %errorlevel% neq 0 goto :end

echo All done! You can now run:
echo    export.bat
goto :end

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

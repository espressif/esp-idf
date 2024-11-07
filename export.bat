@echo off
if defined MSYSTEM (
    echo This .bat file is for Windows CMD.EXE shell only.
    goto :eof
)

set SCRIPT_EXIT_CODE=0

:: Emergency backup option to use previous export.bat (export_legacy.bat) if the new export approach fails.
:: To use it, set environmental variable like: set ESP_IDF_LEGACY_EXPORT=1
if not "%ESP_IDF_LEGACY_EXPORT%"=="" (
    tools\legacy_exports\export_legacy.bat
    set SCRIPT_EXIT_CODE=%errorlevel%
    goto :eof
)

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

if not exist "%IDF_PATH%\tools\idf.py" (
    set SCRIPT_EXIT_CODE=1
    goto :__missing_file
)
if not exist "%IDF_PATH%\tools\idf_tools.py" (
    set SCRIPT_EXIT_CODE=1
    goto :__missing_file
)
if not exist "%IDF_PATH%\tools\activate.py" (
    set SCRIPT_EXIT_CODE=1
    goto :__missing_file
)


for /f "delims=" %%i in ('python "%IDF_PATH%\tools\activate.py" --export') do set activate=%%i
%activate%

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

:__missing_file
    echo Could not detect IDF_PATH. Please navigate to your ESP-IDF directory and run:
    echo export.bat
    goto :__end

:__end
:: Clean up
set MISSING_REQUIREMENTS=
set activate=
exit /b %SCRIPT_EXIT_CODE%

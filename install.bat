@echo off
if defined MSYSTEM (
	echo This .bat file is for Windows CMD.EXE shell only. When using MSYS, run:
	echo   ./install.sh.
	goto end
)
:: Infer IDF_PATH from script location
set IDF_PATH=%~dp0
set IDF_PATH=%IDF_PATH:~0,-1%

echo Installing ESP-IDF tools
python.exe %IDF_PATH%\tools\idf_tools.py install
if %errorlevel% neq 0 goto :end

echo Setting up Python environment
python.exe %IDF_PATH%\tools\idf_tools.py install-python-env
if %errorlevel% neq 0 goto :end

echo All done! You can now run:
echo    export.bat

:end

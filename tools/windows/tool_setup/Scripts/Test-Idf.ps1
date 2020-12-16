param (
    [string]$PythonPath = "C:\Python38\",
    [string]$IdfPath = "C:\Users\ContainerAdministrator\Desktop\esp-idf"
)

$env:PATH+=";${PythonPath}"
Set-Location "${IdfPath}"
#.\export.ps1
$env:PYTHONPATH="C:\Users\ContainerAdministrator\Desktop\esp-idf\tools\"
# Append build script and launch via link
#Add-Content -Path ${IdfPath}\export.bat -Value ${BuildCommands}
# timeout is necessary to fix the problem when installer is writing some final files
# it seems that installer exits, but locks were not released yet
Start-Sleep -s 5

$WSShell = New-Object -comObject WScript.Shell
$Shortcut = $WSShell.CreateShortcut('C:\Users\ContainerAdministrator\Desktop\ESP-IDF Command Prompt (cmd.exe).lnk')
$Arguments = $Shortcut.Arguments -replace "/k ", "/c '"
$Command = $Shortcut.TargetPath + ' ' + $Arguments -replace '""', '"'
$Command += " && cd examples\get-started\blink\ && idf.py build'"
Invoke-Expression -Command $Command
#powershell.exe
#C:\Windows\system32\cmd.exe /c '"C:\Users\ContainerAdministrator\.espressif\idf_cmd_init.bat" "C:\Users\ContainerAdministrator\.espressif\python_env\idf4.1_py3.7_env\Scripts" "C:\Program Files\Git\cmd\" && cd examples\get-started\blink\ && idf.py build'
#cmd /c "ping -n 4 127.0.0.1 && .\export.bat && cd examples\get-started\blink\ && idf.py build"
#cmd /c "ping -n 4 127.0.0.1 && .\export.bat && cd examples\get-started\blink\ && idf.py build"
#cmd /c "timeout 4 && C:\Users\ContainerAdministrator\.espressif\idf_cmd_init.bat 'C:\' && cd examples\get-started\blink\ && idf.py build"

#& "C:\Users\ContainerAdministrator\Desktop\esp-idf\Run ESP-IDF Command Prompt (cmd.exe).lnk"

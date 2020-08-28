param (
    [string]$PythonPath = "C:\Python38\",
    [string]$IdfPath = "C:\Users\ContainerAdministrator\Desktop\esp-idf"
)

$env:PATH+=";${PythonPath}"
Set-Location "${IdfPath}"
$env:PYTHONPATH="C:\Users\ContainerAdministrator\Desktop\esp-idf\tools\"

# Timeout is necessary to fix the problem when installer is writing some final files
# it seems that installer exits, but locks were not released yet
Start-Sleep -s 5

$WSShell = New-Object -comObject WScript.Shell
$Shortcut = $WSShell.CreateShortcut('C:\Users\ContainerAdministrator\Desktop\ESP-IDF Command Prompt (cmd.exe).lnk')
$Arguments = $Shortcut.Arguments -replace "/k ", "/c '"
$Command = $Shortcut.TargetPath + ' ' + $Arguments -replace '""', '"'
$Command += " && cd examples\get-started\blink\ && idf.py build'"
Invoke-Expression -Command $Command

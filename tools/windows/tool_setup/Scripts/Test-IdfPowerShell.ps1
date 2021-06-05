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
$Shortcut = $WSShell.CreateShortcut('C:\Users\ContainerAdministrator\Desktop\ESP-IDF PowerShell.lnk')
$Command =  '. ' + $Shortcut.Arguments -replace '""', '"'
$Command = $Command -replace " -ExecutionPolicy Bypass -NoExit -File", ""
$Command
Invoke-Expression -Command $Command

cd examples\get-started\blink\
idf.py build

# Check whether the repository is clean
$GitChanges=(git status -s).Lenght
if ($GitChanges -gt 0) {
    "* Warning! Git repository dirty."
    $GitChanges
} else {
    "Git repository clean."
}

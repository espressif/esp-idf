param (
    [string]$Installer="C:\Output\esp-idf-tools-setup-unsigned.exe",
    [string]$IdfPath = "C:\Users\ContainerAdministrator\Desktop\esp-idf",
    [string]$IdfVersion = "v4.1"
)

"Configuration:"
"* Installer = $Installer"
"* IdfPath = $IdfPath"
"* IdfVersion = $IdfVersion"

$ProcessName = (Get-Item $Installer).Basename
"Waiting for process: $ProcessName"

# Set PYTHONHOME and PYTHONPATH to some directory which is not on the system to test process of creating venv
# The Installer and IDF shell wrappers contains clearing of variables
$env:PYTHONPATH="C:\Hvannadalshnúkur"
$env:PYTHONHOME="C:\Hvannadalshnúkur"

mkdir C:\Temp
&$Installer /VERYSILENT /LOG=C:\Temp\install.txt /SUPPRESSMSGBOXES /SP- /NOCANCEL /NORESTART /IDFVERSION=${IdfVersion}
$InstallerProcess = Get-Process $ProcessName
Wait-Process -Id $InstallerProcess.id
Get-Content -Tail 80 C:\Temp\install.txt

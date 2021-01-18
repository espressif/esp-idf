param (
    [string]$Installer="C:\Output\esp-idf-tools-setup-unsigned.exe",
    [string]$IdfPath = "C:\Users\ContainerAdministrator\Desktop\esp-idf",
    [string]$IdfVersion = "v4.1"
)

$Installer
$IdfPath
$IdfVersion

mkdir C:\Temp
C:\Output\esp-idf-tools-setup-unsigned.exe /VERYSILENT /LOG=C:\Temp\install.txt /SUPPRESSMSGBOXES /SP- /NOCANCEL /NORESTART /IDFVERSION=${IdfVersion}
$InstallerProcess = Get-Process esp-idf-tools-setup-unsigned
Wait-Process -Id $InstallerProcess.id
Get-Content C:\Temp\install.txt

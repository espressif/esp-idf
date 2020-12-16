param (
    [string]$IdfVersion = "v4.1"
)
$ErrorActionPreference = "Stop"
New-Item -Path C:\Users\ContainerAdministrator\ -Name .espressif -ItemType "directory"
New-Item -Path C:\Users\ContainerAdministrator\.espressif -Name releases -ItemType "directory"
Copy-Item -Recurse -Verbose -Path C:\Cache\dist -Destination C:\Users\ContainerAdministrator\.espressif\dist
Copy-Item -Verbose -Path C:\Cache\releases\esp-idf-${IdfVersion}.zip -Destination C:\Users\ContainerAdministrator\.espressif\releases

; Copyright 2019-2021 Espressif Systems (Shanghai) CO LTD
; SPDX-License-Identifier: Apache-2.0

#pragma include __INCLUDE__ + ";" + ReadReg(HKLM, "Software\Mitrich Software\Inno Download Plugin", "InstallDir")
#include <idp.iss>

#define MyAppName "ESP-IDF Tools"
#define MyAppVersion "2.4"
#define MyAppPublisher "Espressif Systems (Shanghai) Co. Ltd."
#define MyAppURL "https://github.com/espressif/esp-idf"

#define PythonVersion "3.9.1"
#define PythonInstallerName "idf-python-3.9.1-embed-win64.zip"
#define PythonInstallerDownloadURL "https://dl.espressif.com/dl/idf-python/idf-python-3.9.1-embed-win64.zip"

#define GitVersion "2.28.0"
#define GitInstallerName "Git-2.28.0-64-bit.exe"
#define GitInstallerDownloadURL "https://github.com/git-for-windows/git/releases/download/v2.28.0.windows.1/Git-2.28.0-64-bit.exe"

#define IDFVersionsURL "https://dl.espressif.com/dl/esp-idf/idf_versions.txt"

#define IDFCmdExeShortcutDescription "Open ESP-IDF Command Prompt (cmd.exe) Environment"
#define IDFCmdExeShortcutFile "ESP-IDF CMD.lnk"

#define IDFPsShortcutDescription "Open ESP-IDF PowerShell Environment"
#define IDFPsShortcutFile "ESP-IDF PowerShell.lnk"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{9E068D99-5C4B-4E5F-96A3-B17CF291E6BD}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={%USERPROFILE}\.espressif
UsePreviousAppDir=no
DirExistsWarning=no
DefaultGroupName=ESP-IDF
DisableProgramGroupPage=yes
OutputBaseFilename=esp-idf-tools-setup-unsigned
Compression=lzma
SolidCompression=yes
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
LicenseFile=license.txt
PrivilegesRequired=lowest
SetupLogging=yes
ChangesEnvironment=yes
WizardStyle=modern

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl,Languages/idf_tool_en-US.islu"

[Dirs]
Name: "{app}\dist"

[Files]
Source: "cmdlinerunner\build\cmdlinerunner.dll"; Flags: dontcopy
Source: "unzip\7za.exe"; Flags: dontcopy
Source: "idf_versions.txt"; Flags: dontcopy
Source: "..\..\idf_tools.py"; DestDir: "{app}"; DestName: "idf_tools_fallback.py"
; Note: this tools.json matches the requirements of IDF v3.x versions.
Source: "tools_fallback.json"; DestDir: "{app}"; DestName: "tools_fallback.json"
Source: "idf_cmd_init.bat"; DestDir: "{app}"
Source: "idf_cmd_init.ps1"; DestDir: "{app}"
Source: "dist\*"; DestDir: "{app}\dist"
; Helper Python files for sanity check of Python environment - used by system_check_page
Source: "system_check\system_check_download.py"; Flags: dontcopy
Source: "system_check\system_check_subprocess.py"; Flags: dontcopy
Source: "system_check\system_check_virtualenv.py"; Flags: dontcopy
; Helper PowerShell scripts for managing exceptions in Windows Defender
Source: "tools_WD_excl.ps1"; DestDir: "{app}\dist"
Source: "tools_WD_clean.ps1"; DestDir: "{app}\dist"

[UninstallDelete]
Type: filesandordirs; Name: "{app}\dist"
Type: filesandordirs; Name: "{app}\releases"
Type: filesandordirs; Name: "{app}\tools"
Type: filesandordirs; Name: "{app}\python_env"
Type: files; Name: "{group}\{#IDFCmdExeShortcutFile}"
Type: files; Name: "{group}\{#IDFPsShortcutFile}"
Type: files; Name: "{autodesktop}\{#IDFCmdExeShortcutFile}"
Type: files; Name: "{autodesktop}\{#IDFPsShortcutFile}"

[Tasks]
Name: CreateLnkStartCmd; Description: "Create Start Menu shortcut for the ESP-IDF Tools Command Prompt Environment";
Name: CreateLnkStartPs; Description: "Create Start Menu shortcut for the ESP-IDF Tools Powershell Environment";
Name: CreateLnkDeskCmd; Description: "Create Desktop shortcut for the ESP-IDF Tools Command Prompt Environment";
Name: CreateLnkDeskPs; Description: "Create Desktop shortcut for the ESP-IDF Tools Powershell Environment";
; WD registration checkbox is identified by 'Windows Defender' substring anywhere in its caption, not by the position index in WizardForm.TasksList.Items
; Please, keep this in mind when making changes to the item's description - WD checkbox is to be disabled on systems without the Windows Defender installed
Name: wdexcl; Description: "Register the ESP-IDF Tools executables as Windows Defender exclusions (improves compilation speed, requires elevation)";
Name: idf_tools_use_mirror; Description: "Use Espressif download server instead of downloading tool packages from Github"; Flags: unchecked;

[Run]
Filename: "{app}\dist\{#GitInstallerName}"; Parameters: "/silent /tasks="""" /norestart"; Description: "Installing Git"; Check: GitInstallRequired
Filename: "{group}\{#IDFPsShortcutFile}"; Flags: postinstall shellexec unchecked; Description: "Run ESP-IDF PowerShell Environment"; Check: IsPowerShellInstalled
Filename: "{group}\{#IDFCmdExeShortcutFile}"; Flags: postinstall shellexec unchecked; Description: "Run ESP-IDF Command Prompt Environment"; Check: IsCmdInstalled


[UninstallRun]
Filename: "powershell.exe"; \
  Parameters: "-ExecutionPolicy Bypass -File ""{app}\dist\tools_WD_clean.ps1"" -RmExclPath ""{app}"""; \
  WorkingDir: {app}; Flags: runhidden

[Registry]
Root: HKCU; Subkey: "Environment"; ValueType: string; ValueName: "IDF_TOOLS_PATH"; \
    ValueData: "{app}"; Flags: preservestringtype createvalueifdoesntexist uninsdeletevalue deletevalue;

[Code]
#include "utils.iss.inc"
#include "choice_page.iss.inc"
#include "cmdline_page.iss.inc"
#include "idf_page.iss.inc"
#include "git_page.iss.inc"
#include "python_page.iss.inc"
#include "system_check_page.iss.inc"
#include "idf_download_page.iss.inc"
#include "idf_setup.iss.inc"
#include "summary.iss.inc"
#include "main.iss.inc"

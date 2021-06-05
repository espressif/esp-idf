; Copyright 2019-2021 Espressif Systems (Shanghai) CO LTD
; SPDX-License-Identifier: Apache-2.0

#pragma include __INCLUDE__ + ";" + ReadReg(HKLM, "Software\Mitrich Software\Inno Download Plugin", "InstallDir")
#include <idp.iss>

#define MyAppName "ESP-IDF Tools"
#define MyAppVersion "2.5"
#define MyAppPublisher "Espressif Systems (Shanghai) Co. Ltd."
#define MyAppURL "https://github.com/espressif/esp-idf"

#ifndef PYTHONVERSION
  #define PYTHONVERSION "3.8.7"
#endif
#define PythonInstallerName "idf-python-" + PYTHONVERSION + "-embed-win64.zip"
#define PythonInstallerDownloadURL "https://dl.espressif.com/dl/idf-python/idf-python-" + PYTHONVERSION + "-embed-win64.zip"

#ifndef GITVERSION
  #define GITVERSION "2.30.0.2"
#endif
; The URL where git is stored is not equal to it's version. Minor build has prefixes with windows
#ifndef GITVERSIONDIR
  #define GITVERSIONDIR "v2.30.0.windows.2"
#endif
#define GitInstallerName "Git-" + GITVERSION + "-64-bit.exe"
#define GitInstallerDownloadURL "https://github.com/git-for-windows/git/releases/download/" + GITVERSIONDIR + "/Git-" + GITVERSION + "-64-bit.exe"

#define IDFVersionsURL "https://dl.espressif.com/dl/esp-idf/idf_versions.txt"

#define IDFCmdExeShortcutDescription "Open ESP-IDF Command Prompt (cmd.exe) Environment"
#define IDFCmdExeShortcutFile "ESP-IDF Command Prompt (cmd.exe).lnk"

#define IDFPsShortcutDescription "Open ESP-IDF PowerShell Environment"
#define IDFPsShortcutFile "ESP-IDF PowerShell.lnk"

; List of default values
;  Default values can be set by command-line option when startig installer
;  or it can be stored in .INI file which can be passed to installer by /CONFIG=[PATH].
;  Code for evaluating configuration is in the file configuration.inc.iss.
#ifndef COMPRESSION
  #define COMPRESSION = 'lzma';
#endif
; In case of large installer set it to 'no' to avoid problem delay during starting installer
; In case of 1 GB installer it could be 30+ seconds just to start installer.
#ifndef SOLIDCOMPRESSION
  #define SOLIDCOMPRESSION = 'yes';
#endif

; Offline installation specific options
#ifndef OFFLINE
  #define OFFLINE = 'no';
#endif
#ifndef PYTHONWHEELSVERSION
  #define PYTHONWHEELSVERSION = '3.8-2021-01-21'
#endif

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
Compression={#COMPRESSION}
SolidCompression={#SOLIDCOMPRESSION}
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64
LicenseFile=license.txt
PrivilegesRequired=lowest
SetupLogging=yes
ChangesEnvironment=yes
WizardStyle=modern

; https://jrsoftware.org/ishelp/index.php?topic=setup_touchdate
; Default values are set to 'no' which might result in files that are installed on the machine
; in the 'future'. This creates a problem for Ninja/CMake which may end up in a neverending loop.
; Setting this flag to 'yes' should avoid the problem.
TimeStampsInUTC=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl,Languages/idf_tool_en-US.islu"

[Dirs]
Name: "{app}\dist"

[Files]
Source: "configuration.ini"; Flags: dontcopy noencryption
Source: "cmdlinerunner\build\cmdlinerunner.dll"; Flags: dontcopy
Source: "unzip\7za.exe"; Flags: dontcopy
Source: "idf_versions.txt"; Flags: dontcopy
Source: "..\..\idf_tools.py"; DestDir: "{app}"; DestName: "idf_tools_fallback.py"
; Note: this tools.json matches the requirements of IDF v3.x versions.
Source: "tools_fallback.json"; DestDir: "{app}"; DestName: "tools_fallback.json"
Source: "idf_cmd_init.bat"; DestDir: "{app}"
Source: "idf_cmd_init.ps1"; DestDir: "{app}"
Source: "dist\*"; DestDir: "{app}\dist"; Flags: skipifsourcedoesntexist;

; esp-idf-bundle - bundle only in case it exists, it's used only in offline installer
Source: "releases\esp-idf-bundle\*"; DestDir: "{code:GetIDFPath}"; Flags: recursesubdirs skipifsourcedoesntexist;

Source: "tools\idf-python\*"; DestDir: "{app}\tools\idf-python\"; Flags: recursesubdirs;
Source: "tools\idf-python-wheels\*"; DestDir: "{app}\tools\idf-python-wheels\"; Flags: recursesubdirs skipifsourcedoesntexist;
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
Name: CreateLinkStartPowerShell; GroupDescription: "{cm:CreateShortcutPowerShell}"; Description: "{cm:CreateShortcutStartMenu}";
Name: CreateLinkDeskPowerShell; GroupDescription: "{cm:CreateShortcutPowerShell}"; Description: "{cm:CreateShortcutDesktop}";

Name: CreateLinkStartCmd; GroupDescription: "{cm:CreateShortcutCMD}"; Description: "{cm:CreateShortcutStartMenu}";
Name: CreateLinkDeskCmd; GroupDescription: "{cm:CreateShortcutCMD}"; Description: "{cm:CreateShortcutDesktop}";

; Optimization for Online mode
Name: UseMirror;  GroupDescription:"{cm:OptimizationTitle}"; Description: "{cm:OptimizationDownloadMirror}"; Flags: unchecked; Check: IsOnlineMode

[Run]
Filename: "{app}\dist\{#GitInstallerName}"; Parameters: "/silent /tasks="""" /norestart"; Description: "Installing Git"; Check: GitInstallRequired
Filename: "{group}\{#IDFPsShortcutFile}"; Flags: postinstall shellexec unchecked; Description: "Run ESP-IDF PowerShell Environment"; Check: IsPowerShellInstalled
Filename: "{group}\{#IDFCmdExeShortcutFile}"; Flags: postinstall shellexec unchecked; Description: "Run ESP-IDF Command Prompt Environment"; Check: IsCmdInstalled
; WD registration checkbox is identified by 'Windows Defender' substring anywhere in its caption, not by the position index in WizardForm.TasksList.Items
; Please, keep this in mind when making changes to the item's description - WD checkbox is to be disabled on systems without the Windows Defender installed
Filename: "powershell"; Parameters: "-ExecutionPolicy ByPass -File ""{app}\dist\tools_WD_excl.ps1"" -AddExclPath ""{app}\*.exe"""; Flags: postinstall shellexec runhidden; Description: "{cm:OptimizationWindowsDefender}"; Check: GetIsWindowsDefenderEnabled


[UninstallRun]
Filename: "powershell.exe"; \
  Parameters: "-ExecutionPolicy Bypass -File ""{app}\dist\tools_WD_clean.ps1"" -RmExclPath ""{app}"""; \
  WorkingDir: {app}; Flags: runhidden

[Registry]
Root: HKCU; Subkey: "Environment"; ValueType: string; ValueName: "IDF_TOOLS_PATH"; \
    ValueData: "{app}"; Flags: preservestringtype createvalueifdoesntexist uninsdeletevalue deletevalue;

[Code]
#include "configuration.iss.inc"
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

#include <idp.iss>

[Setup]
AppName=ESP-IDF Tools
OutputBaseFilename=esp-idf-tools-setup-1.1
AppVersion=1.2

DefaultDirName={pf}\Espressif\ESP-IDF Tools
DefaultGroupName=ESP-IDF Tools
Compression=lzma2
SolidCompression=yes
ChangesEnvironment=yes
LicenseFile=license.txt
; Note: the rest of the installer setup is written to work cleanly on win32 also, *however*
; Ninja doesn't ship a 32-bit binary so there's no way yet to install on win32 :(
; See https://github.com/ninja-build/ninja/issues/1339
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

[Types]
Name: "full"; Description: "Default installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: xtensa_esp32; Description: ESP32 Xtensa GCC Cross-Toolchain; Types: full custom;
Name: mconf_idf; Description: ESP-IDF console menuconfig tool; Types: full custom;
Name: openocd_esp32; Description: openocd debug interface for ESP32; Types: full custom;
Name: esp32ulp_elf_binutils; Description: ULP binutils toolchain for ESP32; Types: full custom;
Name: ninja; Description: Install Ninja build v1.8.2; Types: full custom

[Tasks]
; Should installer prepend to Path (does this by default)
Name: addpath; Description: "Add tools to Path"; GroupDescription: "Add to Path:";
Name: addpath\allusers; Description: "For all users"; GroupDescription: "Add to Path:"; Flags: exclusive
Name: addpath\user; Description: "For the current user only"; GroupDescription: "Add to Path:"; Flags: exclusive unchecked

; External installation tasks
;
; Note: The Check conditions here auto-select 32-bit or 64-bit installers, as needed
; The tasks won't appear if CMake/Python27 already appear to be installed on this system
Name: cmake32; Description: Download and Run CMake 3.11.1 Installer; GroupDescription: "Other Required Tools:"; Check: not IsWin64 and not CMakeInstalled
Name: cmake64; Description: Download and Run CMake 3.11.1 Installer; GroupDescription: "Other Required Tools:"; Check: IsWin64 and not CMakeInstalled
Name: python32; Description: Download and Run Python 2.7.14 Installer and install Python dependencies; GroupDescription: "Other Required Tools:"; Check: not IsWin64 and not Python27Installed
Name: python64; Description: Download and Run Python 2.7.14 Installer and install Python dependencies; GroupDescription: "Other Required Tools:"; Check: IsWin64 and not Python27Installed
Name: python_requirements; Description: Install any missing Python dependencies; GroupDescription: "Other Required Tools:"; Check: Python27Installed

[Files]
Components: xtensa_esp32; Source: "input\xtensa-esp32-elf\*"; DestDir: "{app}\tools\"; Flags: recursesubdirs;
Components: mconf_idf; Source: "input\mconf-v4.6.0.0-idf-20180525-win32\*"; DestDir: "{app}\mconf-idf\";
Components: esp32ulp_elf_binutils; Source: "input\esp32ulp-elf-binutils\*"; DestDir: "{app}\tools\"; Flags: recursesubdirs;
; Excludes for openocd are because some config files contain Cyrillic characters and inno can't encode them
Components: openocd_esp32; Source: "input\openocd-esp32\*"; DestDir: "{app}\tools\"; Flags: recursesubdirs; Excludes: "target\1986*.cfg,target\*1879*.cfg"
Components: ninja; Source: "input\ninja.exe"; DestDir: "{app}\tools\bin\";
Tasks: python32 python64 python_requirements; Source: "..\..\..\requirements.txt"; DestDir: "{tmp}"; Flags: deleteafterinstall;

[Run]
Tasks: cmake32 cmake64; Filename: "msiexec.exe"; Parameters: "/i ""{tmp}\cmake.msi"" /qb! {code:GetCMakeInstallerArgs}"; StatusMsg: Running CMake installer...;
Tasks: python32 python64; Filename: "msiexec.exe"; Parameters: "/i ""{tmp}\python.msi"" /qb! {code:GetPythonInstallerArgs} REBOOT=Supress"; StatusMsg: Running Python installer...;
Tasks: python32 python64; Filename: "C:\Python27\Scripts\pip.exe"; Parameters: "install -r ""{tmp}\requirements.txt"""; StatusMsg: Installing Python modules...;
Tasks: python_requirements; Filename: "{code:Python27InstallPathInclude}\Scripts\pip.exe"; Parameters: "install -r ""{tmp}\requirements.txt"""; StatusMsg: Installing Python modules...;

[Registry]
; Prepend various entries to Path in the registry. Can either be HKLM (all users) or HKCU (single user only)

; "tools" bin dir (ninja, xtensa & ULP toolchains, openocd all in this dir)
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; \
    ValueType: expandsz; ValueName: "Path"; ValueData: "{app}\tools\bin;{olddata}"; Check: not IsInPath('{app}'); \
    Components: xtensa_esp32 esp32ulp_elf_binutils openocd_esp32 ninja; Tasks: addpath\allusers
Root: HKCU; Subkey: "Environment"; \
    ValueType: expandsz; ValueName: "Path"; ValueData: "{app}\tools\bin;{olddata}"; Check: not IsInPath('{app}'); \
    Components: xtensa_esp32 esp32ulp_elf_binutils openocd_esp32 ninja; Tasks: addpath\user

; mconf-idf path
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; \
    ValueType: expandsz; ValueName: "Path"; ValueData: "{app}\mconf-idf;{olddata}"; Check: not IsInPath('{app}\mconf-idf'); \
    Components: mconf_idf; Tasks: addpath\allusers
Root: HKCU; Subkey: "Environment"; \
    ValueType: expandsz; ValueName: "Path"; ValueData: "{app}\mconf-idf;{olddata}"; Check: not IsInPath('{app}\mconf-idf'); \
    Components: mconf_idf; Tasks: addpath\user

; set OPENOCD_SCRIPTS environment variable
[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; \
    ValueType:string; ValueName: "OPENOCD_SCRIPTS"; \
    ValueData: "{app}\tools\share\openocd\scripts"; Flags: preservestringtype createvalueifdoesntexist; \
    Components: openocd_esp32; Tasks: addpath\allusers
Root: HKCU; Subkey: "Environment"; ValueType:string; ValueName: "OPENOCD_SCRIPTS"; \
    ValueData: "{app}\tools\share\openocd\scripts"; Flags: preservestringtype createvalueifdoesntexist; \
    Components: openocd_esp32; Tasks: addpath\user


[Code]
procedure InitializeWizard;
begin
      idpDownloadAfter(wpReady);
end;

procedure CurPageChanged(CurPageID: Integer);
begin
    { When the Ready page is being displayed, initialise downloads based on which Tasks are selected }
    if CurPageID=wpReady then
    begin
      if IsTaskSelected('python32') then
      begin
          idpAddFile('https://www.python.org/ftp/python/2.7.14/python-2.7.14.msi',  ExpandConstant('{tmp}\python.msi'));
      end;
      if IsTaskSelected('python64') then
      begin
          idpAddFile('https://www.python.org/ftp/python/2.7.14/python-2.7.14.amd64.msi', ExpandConstant('{tmp}\python.msi'));
      end;
      if IsTaskSelected('cmake32') then
      begin
          idpAddFile('https://cmake.org/files/v3.11/cmake-3.11.1-win32-x86.msi', ExpandConstant('{tmp}\cmake.msi'));
      end;
      if IsTaskSelected('cmake64') then
      begin
          idpAddFile('https://cmake.org/files/v3.11/cmake-3.11.1-win64-x64.msi', ExpandConstant('{tmp}\cmake.msi'));
      end;
    end;
end;

{ Utility to search in HKLM for an installation path. Looks in both 64-bit & 32-bit registry. }
function GetInstallPath(key, valuename : String) : Variant;
var
  value : string;
begin
   Result := Null;
   if RegQueryStringValue(HKEY_LOCAL_MACHINE, key, valuename, value) then
   begin
      Result := value;
   end
   else
   begin
      { This is 32-bit setup running on 64-bit Windows, but ESP-IDF can use 64-bit tools also }
      if IsWin64 and RegQueryStringValue(HKLM64, key, valuename, value) then
      begin
        Result := value;
      end;
   end;
end;

{ Return the path of the CMake install, if there is one }
function CMakeInstallPath() : Variant;
begin
  Result := GetInstallPath('SOFTWARE\Kitware\CMake', 'InstallDir');
end;

{ Return 'True' if CMake is installed }
function CMakeInstalled() : Boolean;
begin
   Result := not VarIsNull(CMakeInstallPath());
end;

{ Return the path where Python 2.7 is installed, if there is one }
function Python27InstallPath() : Variant;
begin
  Result := GetInstallPath('SOFTWARE\Python\PythonCore\2.7\InstallPath', '');
end;

{ Return the path where Python 2.7 is installed, suitable for including in code: tag }
function Python27InstallPathInclude(Ignored : String) : String;
begin
  Result := Python27InstallPath();
end;

{ Return True if Python 2.7 is installed }
function Python27Installed() : Boolean;
begin
   Result := not VarIsNull(Python27InstallPath());
end;

{ Return arguments to pass to CMake installer, ie should it add CMake to the Path }
function GetCMakeInstallerArgs(Param : String) : String;
begin
   if IsTaskSelected('addpath\allusers') then
   begin
     Result := 'ADD_CMAKE_TO_PATH=System';
   end
   else if IsTaskSelected('addpath\user') then
   begin
     Result := 'ADD_CMAKE_TO_PATH=User';
   end
   else begin
     Result := 'ADD_CMAKE_TO_PATH=None';
   end;
end;

{ Return arguments to pass to the Python installer,
  ie should it install for all users and should it prepend to the Path }
function GetPythonInstallerArgs(Param : String) : String;
begin
   { Note: The Python 2.7 installer appears to always add PATH to
     system environment variables, regardless of ALLUSERS setting.

     This appears to be fixed in the Python 3.x installers (which use WiX) }
   if IsTaskSelected('addpath') then
   begin
     Result := 'ADDLOCAL=ALL ';
   end
   else begin
     Result := ''
   end;
   if IsTaskSelected('addpath\allusers') then
   begin
     Result := Result + 'ALLUSERS=1';
   end
   else begin
     Result := Result + 'ALLUSERS=';
   end;
end;


{ Return True if the param is already set in the Path
  (user or system, depending on which Task is chosen)

  Adapted from https://stackoverflow.com/a/3431379
}
function IsInPath(Param: string): boolean;
var
  OrigPath: string;
  RootKey : Integer;
  SubKey : String;
begin
  if IsTaskSelected('addpath\allusers') then
  begin
    RootKey := HKEY_LOCAL_MACHINE;
    SubKey := 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment';
  end
  else begin
    RootKey := HKEY_CURRENT_USER;
    SubKey := 'Environment';
  end;

  if not RegQueryStringValue(RootKey, SubKey, 'Path', OrigPath)
  then begin
    Result := False;
  end
  else begin
    { look for the path with leading and trailing semicolon }
    Result := Pos(';' + Param + ';', ';' + OrigPath + ';') > 0;
  end;
end;

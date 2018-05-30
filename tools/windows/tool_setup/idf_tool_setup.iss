#include <idp.iss>

[Setup]
AppName=ESP-IDF Tools
AppVersion=1.1
OutputBaseFilename=esp-idf-tools-setup-1.1

DefaultDirName={pf}\Espressif\ESP-IDF Tools
DefaultGroupName=ESP-IDF Tools
Compression=lzma2
SolidCompression=yes
ChangesEnvironment=yes
; Note: the rest of the installer setup is written to work cleanly on win32 also, *however*
; Ninja doesn't ship a 32-bit binary so there's no way yet to install on win32 :(
; See https://github.com/ninja-build/ninja/issues/1339
ArchitecturesAllowed=x64
ArchitecturesInstallIn64BitMode=x64

[Types]
Name: "full"; Description: "Default installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Components]
Name: toolchain; Description: ESP32 Xtensa GCC Cross-Toolchain; Types: full custom;
Name: mconf_idf; Description: ESP-IDF console menuconfig tool; Types: full custom;
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
Name: python32; Description: Download and Run Python 2.7.14 Installer and install pyserial; GroupDescription: "Other Required Tools:"; Check: not IsWin64 and not Python27Installed
Name: python64; Description: Download and Run Python 2.7.14 Installer and install pyserial; GroupDescription: "Other Required Tools:"; Check: IsWin64 and not Python27Installed

[Files]
Components: toolchain; Source: "input\xtensa-esp32-elf\*"; DestDir: "{app}\toolchain\"; Flags: recursesubdirs;
Components: mconf_idf; Source: "input\mconf-v4.6.0.0-idf-20180525-win32\*"; DestDir: "{app}\mconf-idf\";
Components: ninja; Source: "input\ninja.exe"; DestDir: "{app}";

[Run]
Tasks: cmake32 cmake64; Filename: "msiexec.exe"; Parameters: "/i ""{tmp}\cmake.msi"" /qb! {code:GetCMakeInstallerArgs}"; StatusMsg: Running CMake installer...;
Tasks: python32 python64; Filename: "msiexec.exe"; Parameters: "/i ""{tmp}\python.msi"" /qb! {code:GetPythonInstallerArgs} REBOOT=Supress"; StatusMsg: Running Python installer...;
Tasks: python32 python64; Filename: "C:\Python27\Scripts\pip.exe"; Parameters: "install pyserial"; StatusMsg: Installing pyserial...;

[Registry]
; Prepend various entries to Path in the registry. Can either be HKLM (all users) or HKCU (single user only)

; ninja path (in root app directory)
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; \
    ValueType: expandsz; ValueName: "Path"; ValueData: "{app};{olddata}"; Check: not IsInPath('{app}'); \
    Components: ninja; Tasks: addpath\allusers
Root: HKCU; Subkey: "Environment"; \
    ValueType: expandsz; ValueName: "Path"; ValueData: "{app};{olddata}"; Check: not IsInPath('{app}'); \
    Components: ninja; Tasks: addpath\user

; mconf-idf path
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; \
    ValueType: expandsz; ValueName: "Path"; ValueData: "{app}\mconf-idf;{olddata}"; Check: not IsInPath('{app}\mconf-idf'); \
    Components: mconf_idf; Tasks: addpath\allusers
Root: HKCU; Subkey: "Environment"; \
    ValueType: expandsz; ValueName: "Path"; ValueData: "{app}\mconf-idf;{olddata}"; Check: not IsInPath('{app}\mconf-idf'); \
    Components: mconf_idf; Tasks: addpath\user

; toolchain path
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; \
    ValueType: expandsz; ValueName: "Path"; ValueData: "{app}\toolchain\bin;{olddata}"; Check: not IsInPath('{app}\toolchain\bin'); \
    Components: toolchain; Tasks: addpath\allusers
Root: HKCU; Subkey: "Environment"; \
    ValueType: expandsz; ValueName: "Path"; ValueData: "{app}\toolchain\bin;{olddata}"; Check: not IsInPath('{app}\toolchain\bin'); \
    Components: toolchain; Tasks: addpath\user


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

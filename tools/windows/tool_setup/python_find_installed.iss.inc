{ Copyright 2019 Espressif Systems (Shanghai) PTE LTD
  SPDX-License-Identifier: Apache-2.0 }

{ ------------------------------ Find installed Python interpreters in Windows Registry (see PEP 514) ------------------------------ }

var
  InstalledPythonVersions: TStringList;
  InstalledPythonDisplayNames: TStringList;
  InstalledPythonExecutables: TStringList;

procedure PythonVersionAdd(Version, DisplayName, Executable: String);
begin
  Log('Adding Python version=' + Version + ' name='+DisplayName+' executable='+Executable);
  InstalledPythonVersions.Append(Version);
  InstalledPythonDisplayNames.Append(DisplayName);
  InstalledPythonExecutables.Append(Executable);
end;

function GetPythonVersionInfoFromKey(RootKey: Integer; SubKeyName, CompanyName, TagName: String;
                                     var Version: String;
                                     var DisplayName: String;
                                     var ExecutablePath: String): Boolean;
var
  TagKey, InstallPathKey, DefaultPath: String;
begin
  TagKey := SubKeyName + '\' + CompanyName + '\' + TagName;
  InstallPathKey := TagKey + '\InstallPath';

  if not RegQueryStringValue(RootKey, InstallPathKey, '', DefaultPath) then
  begin
    Log('No (Default) key, skipping');
    Result := False;
    exit;
  end;

  if not RegQueryStringValue(RootKey, InstallPathKey, 'ExecutablePath', ExecutablePath) then
  begin
    Log('No ExecutablePath, using the default');
    ExecutablePath := DefaultPath + '\python.exe';
  end;

  if not RegQueryStringValue(RootKey, TagKey, 'SysVersion', Version) then
  begin
    if CompanyName = 'PythonCore' then
    begin
      Version := TagName;
      Delete(Version, 4, Length(Version));
    end else begin
      Log('Can not determine SysVersion');
      Result := False;
      exit;
    end;
  end;

  if not RegQueryStringValue(RootKey, TagKey, 'DisplayName', DisplayName) then
  begin
    DisplayName := 'Python ' + Version;
  end;

  Result := True;
end;

procedure FindPythonVersionsFromKey(RootKey: Integer; SubKeyName: String);
var
  CompanyNames: TArrayOfString;
  CompanyName, CompanySubKey, TagName, TagSubKey: String;
  ExecutablePath, DisplayName, Version: String;
  TagNames: TArrayOfString;
  CompanyId, TagId: Integer;
begin
  if not RegGetSubkeyNames(RootKey, SubKeyName, CompanyNames) then
  begin
    Log('Nothing found in ' + IntToStr(RootKey) + '\' + SubKeyName);
    Exit;
  end;

  for CompanyId := 0 to GetArrayLength(CompanyNames) - 1 do
  begin
    CompanyName := CompanyNames[CompanyId];

    if CompanyName = 'PyLauncher' then
      continue;

    CompanySubKey := SubKeyName + '\' + CompanyName;
    Log('In ' + IntToStr(RootKey) + '\' + CompanySubKey);

    if not RegGetSubkeyNames(RootKey, CompanySubKey, TagNames) then
      continue;
    
    for TagId := 0 to GetArrayLength(TagNames) - 1 do
    begin
      TagName := TagNames[TagId];
      TagSubKey := CompanySubKey + '\' + TagName;
      Log('In ' + IntToStr(RootKey) + '\' + TagSubKey);

      if not GetPythonVersionInfoFromKey(RootKey, SubKeyName, CompanyName, TagName, Version, DisplayName, ExecutablePath) then
        continue;

      PythonVersionAdd(Version, DisplayName, ExecutablePath);
    end;
  end;
end;

procedure FindInstalledPythonVersions();
begin
  InstalledPythonVersions := TStringList.Create();
  InstalledPythonDisplayNames := TStringList.Create();
  InstalledPythonExecutables := TStringList.Create();

  FindPythonVersionsFromKey(HKEY_CURRENT_USER, 'Software\Python');
  FindPythonVersionsFromKey(HKEY_LOCAL_MACHINE, 'Software\Python');
  FindPythonVersionsFromKey(HKEY_LOCAL_MACHINE, 'Software\Wow6432Node\Python');
end;

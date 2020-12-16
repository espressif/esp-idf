{ Copyright 2019-2021 Espressif Systems (Shanghai) CO LTD
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
                                     var ExecutablePath: String;
                                     var BaseDir: String): Boolean;
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

  BaseDir := DefaultPath;

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

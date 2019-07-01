{ Copyright 2019 Espressif Systems (Shanghai) PTE LTD
  SPDX-License-Identifier: Apache-2.0 }

{ ------------------------------ Find installed copies of Git ------------------------------ }

var
  InstalledGitVersions: TStringList;
  InstalledGitDisplayNames: TStringList;
  InstalledGitExecutables: TStringList;


procedure GitVersionAdd(Version, DisplayName, Executable: String);
begin
  Log('Adding Git version=' + Version + ' name='+DisplayName+' executable='+Executable);
  InstalledGitVersions.Append(Version);
  InstalledGitDisplayNames.Append(DisplayName);
  InstalledGitExecutables.Append(Executable);
end;

function GetVersionOfGitExe(Path: String; var Version: String; var ErrStr: String): Boolean;
var
  VersionOutputFile: String;
  Args: String;
  GitVersionAnsi: AnsiString;
  GitVersion: String;
  GitVersionPrefix: String;
  Err: Integer;
begin
  VersionOutputFile := ExpandConstant('{tmp}\gitver.txt');

  DeleteFile(VersionOutputFile);
  Args := '/C "' + Path + '" --version >gitver.txt';
  Log('Running ' + Args);
  if not ShellExec('', 'cmd.exe', Args,
    ExpandConstant('{tmp}'), SW_HIDE, ewWaitUntilTerminated, Err) then
  begin
    ErrStr := 'Failed to get git version, error=' + IntToStr(err);
    Log(ErrStr);
    Result := False;
    exit;
  end;

  LoadStringFromFile(VersionOutputFile, GitVersionAnsi);
  GitVersion := Trim(String(GitVersionAnsi));
  GitVersionPrefix := 'git version ';
  if Pos(GitVersionPrefix, GitVersion) <> 1 then
  begin
    ErrStr := 'Unexpected git version format: ' + GitVersion;
    Log(ErrStr);
    Result := False;
    exit;
  end;

  Delete(GitVersion, 1, Length(GitVersionPrefix));
  Version := GitVersion;
  Result := True;
end;

procedure FindGitInPath();
var
  Args: String;
  GitListFile: String;
  GitPaths: TArrayOfString;
  GitVersion: String;
  ErrStr: String;
  Err: Integer;
  i: Integer;
begin
  GitListFile := ExpandConstant('{tmp}\gitlist.txt');
  Args := '/C where git.exe >"' + GitListFile + '"';
  if not ShellExec('', 'cmd.exe', Args,
      '', SW_HIDE, ewWaitUntilTerminated, Err) then
  begin
    Log('Failed to find git using "where", error='+IntToStr(Err));
    exit;
  end;

  LoadStringsFromFile(GitListFile, GitPaths);

  for i:= 0 to GetArrayLength(GitPaths) - 1 do
  begin
    Log('Git path: ' + GitPaths[i]);
    if not GetVersionOfGitExe(GitPaths[i], GitVersion, ErrStr) then
      continue;

    Log('Git version: ' + GitVersion);
    GitVersionAdd(GitVersion, GitVersion, GitPaths[i]);
  end;
end;

procedure FindInstalledGitVersions();
begin
  InstalledGitVersions := TStringList.Create();
  InstalledGitDisplayNames := TStringList.Create();
  InstalledGitExecutables := TStringList.Create();

  FindGitInPath();
end;

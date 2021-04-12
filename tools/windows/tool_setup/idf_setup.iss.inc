{ Copyright 2019-2021 Espressif Systems (Shanghai) CO LTD
  SPDX-License-Identifier: Apache-2.0 }

{ ------------------------------ Downloading ESP-IDF ------------------------------ }

var
  IDFZIPFileVersion, IDFZIPFileName: String;

function GetIDFPath(Unused: String): String;
begin
  if IDFUseExisting then
    Result := IDFExistingPath
  else
    Result := IDFDownloadPath;
end;

function GetIDFZIPFileVersion(Version: String): String;
var
  ReleaseVerPart: String;
  i: Integer;
  Found: Boolean;
begin
  if WildCardMatch(Version, 'v*') or WildCardMatch(Version, 'v*-rc*') then
    Result := Version
  else if Version = 'master' then
    Result := ''
  else if WildCardMatch(Version, 'release/v*') then
  begin
    ReleaseVerPart := Version;
    Log('ReleaseVerPart=' + ReleaseVerPart)
    Delete(ReleaseVerPart, 1, Length('release/'));
    Log('ReleaseVerPart=' + ReleaseVerPart)
    Found := False;
    for i := 0 to GetArrayLength(IDFDownloadAvailableVersions) - 1 do
    begin
      if Pos(ReleaseVerPart, IDFDownloadAvailableVersions[i]) = 1 then
      begin
        Result := IDFDownloadAvailableVersions[i];
        Found := True;
        break;
      end;
    end;
    if not Found then
      Result := '';
  end;
  Log('GetIDFZIPFileVersion(' + Version + ')=' + Result);
end;

procedure IDFAddDownload();
var
  Url, MirrorUrl: String;
begin
  IDFZIPFileVersion := GetIDFZIPFileVersion(IDFDownloadVersion);

  Log('IDFZIPFileVersion: ' + IDFZIPFileVersion);

  if IDFZIPFileVersion <> '' then
  begin
    Url := 'https://github.com/espressif/esp-idf/releases/download/' + IDFZIPFileVersion + '/esp-idf-' + IDFZIPFileVersion + '.zip';
    MirrorUrl := 'https://dl.espressif.com/github_assets/espressif/esp-idf/releases/download/' + IDFZIPFileVersion + '/esp-idf-' + IDFZIPFileVersion + '.zip';
    IDFZIPFileName := ExpandConstant('{app}\releases\esp-idf-' + IDFZIPFileVersion + '.zip');

    if not FileExists(IDFZIPFileName) then
    begin
      Log('IDFZIPFileName: ' + IDFZIPFileName + ' exists');
      ForceDirectories(ExpandConstant('{app}\releases'))
      Log('Adding download: ' + Url + ', mirror: ' + MirrorUrl + ', destination: ' + IDFZIPFileName);
      idpAddFile(Url, IDFZIPFileName);
      idpAddMirror(Url, MirrorUrl);
    end else begin
      Log('IDFZIPFileName: ' + IDFZIPFileName + ' does not exist');
    end;
  end;
end;

procedure RemoveAlternatesFile(Path: String);
begin
  Log('Removing ' + Path);
  DeleteFile(Path);
end;

{
  Replacement of the '--dissociate' flag of 'git clone', to support older versions of Git.
  '--reference' is supported for submodules since git 2.12, but '--dissociate' only from 2.18.
}
procedure GitRepoDissociate(Path: String);
var
  CmdLine: String;
begin
  CmdLine := GitExecutablePath + ' -C ' + Path + ' repack -d -a'
  DoCmdlineInstall('Finishing ESP-IDF installation', 'Re-packing the repository', CmdLine);
  CmdLine := GitExecutablePath + ' -C ' + Path + ' submodule foreach git repack -d -a'
  DoCmdlineInstall('Finishing ESP-IDF installation', 'Re-packing the submodules', CmdLine);

  FindFileRecursive(Path + '\.git', 'alternates', @RemoveAlternatesFile);
end;

{
  Initialize submodules - required to call when switching branches in existing repo.
  E.g. created by offline installer
}
procedure GitUpdateSubmodules(Path: String);
var
  CmdLine: String;
begin
  CmdLine := GitExecutablePath + ' -C ' + Path + ' submodule update --init --recursive';
  Log('Updating submodules: ' + CmdLine);
  DoCmdlineInstall('Finishing ESP-IDF installation', 'Updating submodules', CmdLine);
end;

{
  Run git config fileMode is repairing problem when git repo was zipped on Linux and extracted on Windows.
  The repo and submodules are marked as dirty which confuses users that fresh installation already contains changes.
  More information: https://mirrors.edge.kernel.org/pub/software/scm/git/docs/git-config.html
}
procedure GitRepoFixFileMode(Path: String);
var
  CmdLine: String;
begin
  CmdLine := GitExecutablePath + ' -C ' + Path + ' config --local core.fileMode false';
  Log('Setting core.fileMode on repository: ' + CmdLine);
  DoCmdlineInstall('Finishing ESP-IDF installation', 'Updating fileMode', CmdLine);

  Log('Setting core.fileMode on repository for submodules: ' + CmdLine);
  CmdLine := GitExecutablePath + ' -C ' + Path + ' submodule foreach --recursive git config --local core.fileMode false';
  DoCmdlineInstall('Finishing ESP-IDF installation', 'Updating fileMode in submodules', CmdLine);
end;

{ Run git reset --hard in the repo and in the submodules, to fix the newlines. }
procedure GitResetHard(Path: String);
var
  CmdLine: String;
begin
  if (not IsGitResetAllowed) then begin
    Log('Git reset disabled by command line option /GITRESET=no.');
    Exit;
  end;

  CmdLine := GitExecutablePath + ' -C ' + Path + ' reset --hard';
  Log('Resetting the repository: ' + CmdLine);
  DoCmdlineInstall('Finishing ESP-IDF installation', 'Updating newlines', CmdLine);

  Log('Resetting the submodules: ' + CmdLine);
  CmdLine := GitExecutablePath + ' -C ' + Path + ' submodule foreach git reset --hard';
  DoCmdlineInstall('Finishing ESP-IDF installation', 'Updating newlines in submodules', CmdLine);
end;

{ Run git clean - clean leftovers after switching between tags }
{ The repo should be created with: git config --local clean.requireForce false}
procedure GitCleanForceDirectory(Path: String);
var
  CmdLine: String;
begin
  if (not IsGitCleanAllowed) then begin
    Log('Git clean disabled by command line option /GITCLEAN=no.');
    Exit;
  end;

  CmdLine := GitExecutablePath + ' -C ' + Path + ' clean --force -d';
  Log('Resetting the repository: ' + CmdLine);
  DoCmdlineInstall('Finishing ESP-IDF installation', 'Cleaning untracked directories', CmdLine);
end;


{
  Switch to different branch. Used in offline installation.
}
procedure GitSwitchBranch(Path: String; BranchName: String);
var
  CmdLine: String;
begin
  CmdLine := GitExecutablePath + ' -C ' + Path + ' checkout ' + BranchName;
  Log('Updating submodules: ' + CmdLine);
  DoCmdlineInstall('Switching branch', 'Switching to branch', CmdLine);

  GitUpdateSubmodules(Path);
  GitResetHard(Path);
  GitCleanForceDirectory(Path);
end;

{
  There are 3 possible ways how an ESP-IDF copy can be obtained:
  - Download the .zip archive with submodules included, extract to destination directory,
    then do 'git reset --hard' and 'git submodule foreach git reset --hard' to correct for
    possibly different newlines. This is done for release versions.
  - Do a git clone of the Github repository into the destination directory.
    This is done for the master branch.
  - Download the .zip archive of a "close enough" release version, extract into a temporary
    directory. Then do a git clone of the Github repository, using the temporary directory
    as a '--reference'. This is done for other versions (such as release branches).
}

procedure IDFOfflineInstall();
var
  IDFTempPath: String;
  IDFPath: String;
begin
  IDFPath := IDFDownloadPath;

  IDFTempPath := ExpandConstant('{app}\releases\esp-idf-bundle');
  Log('IDFTempPath - location of bundle: ' + IDFTempPath);

  GitSwitchBranch(IDFPath, IDFDownloadVersion);
end;

procedure IDFDownloadInstall();
var
  CmdLine: String;
  IDFTempPath: String;
  IDFPath: String;
  NeedToClone: Boolean;
begin
  IDFPath := IDFDownloadPath;
  { If there is a release archive to download, IDFZIPFileName and IDFZIPFileVersion will be set.
    See GetIDFZIPFileVersion function.
  }

  if IDFZIPFileName <> '' then
  begin
    if IDFZIPFileVersion <> IDFDownloadVersion then
    begin
      { The version of .zip file downloaded is not the same as the version the user has requested.
        Will use 'git clone --reference' to obtain the correct version, using the contents
        of the .zip file as reference.
      }
      NeedToClone := True;
    end;

    CmdLine := ExpandConstant('{tmp}\7za.exe x -o' + ExpandConstant('{tmp}') + ' -r -aoa "' + IDFZIPFileName + '"');
    IDFTempPath := ExpandConstant('{tmp}\esp-idf-') + IDFZIPFileVersion;
    Log('Extracting ESP-IDF reference repository: ' + CmdLine);
    Log('Reference repository path: ' + IDFTempPath);
    DoCmdlineInstall('Extracting ESP-IDF', 'Setting up reference repository', CmdLine);
  end else begin
    { IDFZIPFileName is not set, meaning that we will rely on 'git clone'. }
    NeedToClone := True;
    Log('Not .zip release archive. Will do full clone.');
  end;

  if NeedToClone then
  begin
    CmdLine := GitExecutablePath + ' clone --progress -b ' + IDFDownloadVersion;

    if (IsGitRecursive) then begin
      CmdLine := CmdLine + ' --recursive ';
    end;

    if IDFTempPath <> '' then
      CmdLine := CmdLine + ' --reference ' + IDFTempPath;

    CmdLine := CmdLine + ' ' + GitRepository +' ' + IDFPath;
    Log('Cloning IDF: ' + CmdLine);
    DoCmdlineInstall('Downloading ESP-IDF', 'Using git to clone ESP-IDF repository', CmdLine);

    if IDFTempPath <> '' then
      GitRepoDissociate(IDFPath);

  end else begin

    Log('Copying ' + IDFTempPath + ' to ' + IDFPath);
    if DirExists(IDFPath) then
    begin
      if not DirIsEmpty(IDFPath) then
      begin
        MsgBox('Destination directory exists and is not empty: ' + IDFPath, mbError, MB_OK);
        RaiseException('Failed to copy ESP-IDF')
      end;
    end;

    { If cmd.exe command argument starts with a quote, the first and last quote chars in the command
      will be removed by cmd.exe.
      Keys explanation: /s+/e includes all subdirectories, /i assumes that destination is a directory,
      /h copies hidden files, /q disables file name logging (making copying faster!)
    }

    CmdLine := ExpandConstant('cmd.exe /c ""xcopy" /s /e /i /h /q "' + IDFTempPath + '" "' + IDFPath + '""');
    DoCmdlineInstall('Extracting ESP-IDF', 'Copying ESP-IDF into the destination directory', CmdLine);

    GitRepoFixFileMode(IDFPath);
    GitResetHard(IDFPath);

    DelTree(IDFTempPath, True, True, True);
  end;
end;

{ ------------------------------ IDF Tools setup, Python environment setup ------------------------------ }

function UseBundledIDFToolsPy(Version: String) : Boolean;
begin
  Result := False;
  { Use bundled copy of idf_tools.py, as the copy shipped with these IDF versions can not work due to
    the --no-site-packages bug.
  }
  if (Version = 'v4.0') or (Version = 'v3.3.1') then
  begin
    Log('UseBundledIDFToolsPy: version=' + Version + ', using bundled idf_tools.py');
    Result := True;
  end;
end;

{ Find Major and Minor version in esp_idf_version.h file. }
function GetIDFVersionFromHeaderFile():String;
var
  HeaderFileName: String;
  HeaderLines: TArrayOfString;
  LineIndex: Integer;
  LineCount: Longint;
  Line: String;
  MajorVersion: String;
  MinorVersion: String;
begin
  HeaderFileName := GetIDFPath('') + '\components\esp_common\include\esp_idf_version.h';
  if (not FileExists(HeaderFileName)) then begin
    Result := '';
    Exit;
  end;

  LoadStringsFromFile(HeaderFileName, HeaderLines);
  LineCount := GetArrayLength(HeaderLines);
  for LineIndex := 0 to LineCount - 1 do begin
    Line := HeaderLines[LineIndex];
    if (pos('define ESP_IDF_VERSION_MAJOR', Line) > 0) then begin
      Delete(Line, 1, 29);
      MajorVersion := Trim(Line);
    end else if (pos('define ESP_IDF_VERSION_MINOR', Line) > 0) then begin
      Delete(Line, 1, 29);
      MinorVersion := Trim(Line);
      Result := MajorVersion + '.' + MinorVersion;
      Exit;
    end
  end;
end;

{ Get short version from long version e.g. 3.7.9 -> 3.7 }
function GetShortVersion(VersionString:String):String;
var
  VersionIndex: Integer;
  MajorString: String;
  MinorString: String;
  DotIndex: Integer;
begin
  { Transform version vx.y or release/vx.y to x.y }
  VersionIndex := pos('v', VersionString);
  if (VersionIndex > 0) then begin
    Delete(VersionString, 1, VersionIndex);
  end;

  { Transform version x.y.z to x.y }
  DotIndex := pos('.', VersionString);
  if (DotIndex > 0) then begin
    MajorString := Copy(VersionString, 1, DotIndex - 1);
    Delete(VersionString, 1, DotIndex);
    { Trim trailing version numbers. }
    DotIndex := pos('.', VersionString);
    if (DotIndex > 0) then begin
      MinorString := Copy(VersionString, 1, DotIndex - 1);
      VersionString := MajorString + '.' + MinorString;
    end else begin
     VersionString :=  MajorString + '.' + VersionString;
    end;
  end;

  Result := VersionString;
end;

{ Get IDF version string in combination with Python version. }
{ Result e.g.: idf4.1_py38 }
function GetIDFPythonEnvironmentVersion():String;
var
  IDFVersionString: String;
begin
  { Transform main or master to x.y }
  if (Pos('main', IDFDownloadVersion) > 0) or (Pos('master', IDFDownloadVersion) > 0) then begin
    IDFVersionString := GetIDFVersionFromHeaderFile();
  end else begin
    IDFVersionString := GetShortVersion(IDFDownloadVersion);
  end;

  Result := 'idf' + IDFVersionString + '_py' + GetShortVersion(PythonVersion);
end;

function GetPythonVirtualEnvPath(): String;
var
  PythonVirtualEnvPath: String;
begin
  { The links should contain reference to Python vitual env }
  PythonVirtualEnvPath := ExpandConstant('{app}\python_env\') + GetIDFPythonEnvironmentVersion() + '_env\Scripts';
  Log('Path to Python in virtual env: ' + PythonVirtualEnvPath);

  { Fallback in case of not existing environment. }
  if (not FileExists(PythonVirtualEnvPath + '\python.exe')) then begin
    PythonVirtualEnvPath := PythonPath;
    Log('python.exe not found, reverting to:' + PythonPath);
  end;
  Result := PythonVirtualEnvPath;
end;

procedure IDFToolsSetup();
var
  CmdLine: String;
  IDFPath: String;
  IDFToolsPyPath: String;
  IDFToolsPyCmd: String;
  BundledIDFToolsPyPath: String;
  JSONArg: String;
  PythonVirtualEnvPath: String;
begin
  IDFPath := GetIDFPath('');
  IDFToolsPyPath := IDFPath + '\tools\idf_tools.py';
  BundledIDFToolsPyPath := ExpandConstant('{app}\idf_tools_fallback.py');
  JSONArg := '';

  if FileExists(IDFToolsPyPath) then
  begin
    Log('idf_tools.py exists in IDF directory');
    if UseBundledIDFToolsPy(IDFDownloadVersion) then
    begin
      Log('Using the bundled idf_tools.py copy');
      IDFToolsPyCmd := BundledIDFToolsPyPath;
    end else begin
      IDFToolsPyCmd := IDFToolsPyPath;
    end;
  end else begin
    Log('idf_tools.py does not exist in IDF directory, using a fallback version');
    IDFToolsPyCmd := BundledIDFToolsPyPath;
    JSONArg := ExpandConstant('--tools "{app}\tools_fallback.json"');
  end;

  { IDFPath not quoted, as it can not contain spaces }
  IDFToolsPyCmd := PythonExecutablePath + ' "' + IDFToolsPyCmd + '" --idf-path ' + IDFPath + JSONArg;

  SetEnvironmentVariable('PYTHONUNBUFFERED', '1');

  if (IsOfflineMode) then begin
    SetEnvironmentVariable('PIP_NO_INDEX', 'true');
    Log('Offline installation selected. Setting environment variable PIP_NO_INDEX=1');
    SetEnvironmentVariable('PIP_FIND_LINKS', ExpandConstant('{app}\tools\idf-python-wheels\' + PythonWheelsVersion));
  end else begin
    SetEnvironmentVariable('PIP_EXTRA_INDEX_URL', PythonWheelsUrl);
    Log('Adding extra Python wheels location. Setting environment variable PIP_EXTRA_INDEX_URL=' + PythonWheelsUrl);
  end;

  Log('idf_tools.py command: ' + IDFToolsPyCmd);
  CmdLine := IDFToolsPyCmd + ' install';

  Log('Installing tools:' + CmdLine);
  DoCmdlineInstall('Installing ESP-IDF tools', '', CmdLine);

  CmdLine := PythonExecutablePath + ' -m virtualenv --version';
  Log('Checking Python virtualenv support:' + CmdLine)
  DoCmdlineInstall('Checking Python virtualenv support', '', CmdLine);

  PythonVirtualEnvPath := ExpandConstant('{app}\python_env\')  + GetIDFPythonEnvironmentVersion() + '_env';
  CmdLine := PythonExecutablePath + ' -m virtualenv "' + PythonVirtualEnvPath + '" -p ' + '"' + PythonExecutablePath + '"';
  if (DirExists(PythonVirtualEnvPath)) then begin
    Log('ESP-IDF Python Virtual environment exists, refreshing the environment: ' + CmdLine);
  end else begin
    Log('ESP-IDF Python Virtual environment does not exist, creating the environment: ' + CmdLine);
  end;
  DoCmdlineInstall('Creating Python environment', '', CmdLine);

  CmdLine := IDFToolsPyCmd + ' install-python-env';
  Log('Installing Python environment:' + CmdLine);
  DoCmdlineInstall('Installing Python environment', '', CmdLine);
end;

{ ------------------------------ Start menu shortcut ------------------------------ }

procedure CreateIDFCommandPromptShortcut(LnkString: String);
var
  Destination: String;
  Description: String;
  Command: String;
begin
  ForceDirectories(ExpandConstant(LnkString));
  Destination := ExpandConstant(LnkString + '\{#IDFCmdExeShortcutFile}');
  Description := '{#IDFCmdExeShortcutDescription}';

  { If cmd.exe command argument starts with a quote, the first and last quote chars in the command
    will be removed by cmd.exe; each argument needs to be surrounded by quotes as well. }
  Command := ExpandConstant('/k ""{app}\idf_cmd_init.bat" "') + GetPythonVirtualEnvPath() + '" "' + GitPath + '""';
  Log('CreateShellLink Destination=' + Destination + ' Description=' + Description + ' Command=' + Command)
  try
    CreateShellLink(
      Destination,
      Description,
      'cmd.exe',
      Command,
      GetIDFPath(''),
      '', 0, SW_SHOWNORMAL);
  except
    MsgBox('Failed to create the shortcut: ' + Destination, mbError, MB_OK);
    RaiseException('Failed to create the shortcut');
  end;
end;

procedure CreateIDFPowershellShortcut(LnkString: String);
var
  Destination: String;
  Description: String;
  Command: String;
  GitPathWithForwardSlashes: String;
  PythonPathWithForwardSlashes: String;
begin
  ForceDirectories(ExpandConstant(LnkString));
  Destination := ExpandConstant(LnkString + '\{#IDFPsShortcutFile}');
  Description := '{#IDFPsShortcutDescription}';
  GitPathWithForwardSlashes := GitPath;

  PythonPathWithForwardSlashes := GetPythonVirtualEnvPath();
  StringChangeEx(GitPathWithForwardSlashes, '\', '/', True);
  StringChangeEx(PythonPathWithForwardSlashes, '\', '/', True);
  Command := ExpandConstant('-ExecutionPolicy Bypass -NoExit -File ""{app}\idf_cmd_init.ps1"" ') + '"' + GitPathWithForwardSlashes + '" "' + PythonPathWithForwardSlashes + '"'
  Log('CreateShellLink Destination=' + Destination + ' Description=' + Description + ' Command=' + Command)
  try
    CreateShellLink(
      Destination,
      Description,
      'powershell.exe',
      Command,
      GetIDFPath(''),
      '', 0, SW_SHOWNORMAL);
  except
    MsgBox('Failed to create the shortcut: ' + Destination, mbError, MB_OK);
    RaiseException('Failed to create the shortcut');
  end;
end;

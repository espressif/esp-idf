{ Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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
  if IDFZIPFileVersion <> '' then
  begin
    Url := 'https://github.com/espressif/esp-idf/releases/download/' + IDFZIPFileVersion + '/esp-idf-' + IDFZIPFileVersion + '.zip';
    MirrorUrl := 'https://dl.espressif.com/dl/esp-idf/releases/esp-idf-' + IDFZIPFileVersion + '.zip';
    IDFZIPFileName := ExpandConstant('{app}\releases\esp-idf-' + IDFZIPFileVersion + '.zip')
    if not FileExists(IDFZIPFileName) then
    begin
      ForceDirectories(ExpandConstant('{app}\releases'))
      Log('Adding download: ' + Url + ', mirror: ' + MirrorUrl + ', destination: ' + IDFZIPFileName);
      idpAddFile(Url, IDFZIPFileName);
      idpAddMirror(Url, MirrorUrl);
    end else begin
      Log(IDFZIPFileName + ' already exists')
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

  FindFileRecusive(Path + '\.git', 'alternates', @RemoveAlternatesFile);
end;

{ Run git reset --hard in the repo and in the submodules, to fix the newlines. }
procedure GitRepoFixNewlines(Path: String);
var
  CmdLine: String;
begin
  CmdLine := GitExecutablePath + ' -C ' + Path + ' reset --hard';
  Log('Resetting the repository: ' + CmdLine);
  DoCmdlineInstall('Finishing ESP-IDF installation', 'Updating newlines', CmdLine);

  Log('Resetting the submodules: ' + CmdLine);
  CmdLine := GitExecutablePath + ' -C ' + Path + ' submodule foreach git reset --hard';
  DoCmdlineInstall('Finishing ESP-IDF installation', 'Updating newlines in submodules', CmdLine);
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

procedure IDFDownload();
var
  CmdLine: String;
  IDFTempPath: String;
  IDFPath: String;
  NeedToClone: Boolean;
  Res: Boolean;

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

    ExtractTemporaryFile('7za.exe')
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
    CmdLine := GitExecutablePath + ' clone --recursive --progress -b ' + IDFDownloadVersion;

    if IDFTempPath <> '' then
      CmdLine := CmdLine + ' --reference ' + IDFTempPath;

    CmdLine := CmdLine + ' https://github.com/espressif/esp-idf.git ' + IDFPath;
    Log('Cloning IDF: ' + CmdLine);
    DoCmdlineInstall('Downloading ESP-IDF', 'Using git to clone ESP-IDF repository', CmdLine);

    if IDFTempPath <> '' then
      GitRepoDissociate(IDFPath);

  end else begin
    Log('Moving ' + IDFTempPath + ' to ' + IDFPath);
    if DirExists(IDFPath) then
    begin
      if not DirIsEmpty(IDFPath) then
      begin
        MsgBox('Destination directory exists and is not empty: ' + IDFPath, mbError, MB_OK);
        RaiseException('Failed to copy ESP-IDF')
      end;

      Res := RemoveDir(IDFPath);
      if not Res then
      begin
        MsgBox('Failed to remove destination directory: ' + IDFPath, mbError, MB_OK);
        RaiseException('Failed to copy ESP-IDF')
      end;
    end;
    Res := RenameFile(IDFTempPath, IDFPath);
    if not Res then
    begin
      MsgBox('Failed to copy ESP-IDF to the destination directory: ' + IDFPath, mbError, MB_OK);
      RaiseException('Failed to copy ESP-IDF');
    end;
    GitRepoFixNewlines(IDFPath);
  end;
end;

{ ------------------------------ IDF Tools setup, Python environment setup ------------------------------ }

procedure IDFToolsSetup();
var
  CmdLine: String;
  IDFPath: String;
  IDFToolsPyPath: String;
  IDFToolsPyCmd: String;
begin
  IDFPath := GetIDFPath('');
  IDFToolsPyPath := IDFPath + '\tools\idf_tools.py';
  if FileExists(IDFToolsPyPath) then
  begin
    Log('idf_tools.py exists in IDF directory');
    IDFToolsPyCmd := PythonExecutablePath + ' ' + IDFToolsPyPath;
  end else begin
    Log('idf_tools.py does not exist in IDF directory, using a fallback version');
    IDFToolsPyCmd := ExpandConstant(PythonExecutablePath
                                    + ' "{app}\idf_tools_fallback.py"'
                                    + ' --idf-path ' + IDFPath
                                    + ' --tools "{app}\tools_fallback.json"');
  end;

  Log('idf_tools.py command: ' + IDFToolsPyCmd);
  CmdLine := IDFToolsPyCmd + ' install';
  Log('Installing tools:' + CmdLine);
  DoCmdlineInstall('Installing ESP-IDF tools', '', CmdLine);

  CmdLine := IDFToolsPyCmd + ' install-python-env';
  Log('Installing Python environment:' + CmdLine);
  DoCmdlineInstall('Installing Python environment', '', CmdLine);
end;

{ ------------------------------ Start menu shortcut ------------------------------ }

procedure CreateIDFCommandPromptShortcut();
var
  Destination: String;
  Description: String;
  Command: String;
begin
  ForceDirectories(ExpandConstant('{group}'));
  Destination := ExpandConstant('{group}\{#IDFCmdExeShortcutFile}');
  Description := '{#IDFCmdExeShortcutDescription}';
  { If cmd.exe command argument starts with a quote, the first and last quote chars in the command
    will be removed by cmd.exe; each argument needs to be surrounded by quotes as well. }
  Command := ExpandConstant('/k ""{app}\idf_cmd_init.bat" "') + PythonPath + '" "' + GitPath + '""';
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
    MsgBox('Failed to create the Start menu shortcut: ' + Destination, mbError, MB_OK);
    RaiseException('Failed to create the shortcut');
  end;
end;


{ ------------------------------ WD exclusion registration ------------------------------ }

procedure RegisterIDFToolsExecutablesInWD();
var
  CmdLine: String;
begin
  CmdLine := ExpandConstant('powershell -ExecutionPolicy ByPass -File "{app}\dist\tools_WD_excl.ps1" -AddExclPath "{app}\*.exe"');
  Log('Registering IDF Tools executables in Windows Defender: ' + CmdLine);
  DoCmdlineInstall('Finishing ESP-IDF installation', 'Registering IDF Tools executables in Windows Defender', CmdLine);
end;

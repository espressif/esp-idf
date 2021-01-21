{ Copyright 2019-2021 Espressif Systems (Shanghai) CO LTD
  SPDX-License-Identifier: Apache-2.0 }

{ ------------------------------ Custom steps before the main installation flow ------------------------------ }

var
  SetupAborted: Boolean;

function InstallationSuccessful(): Boolean;
begin
  Result := not SetupAborted;
end;

<event('InitializeWizard')>
procedure InitializeDownloader();
begin
  idpDownloadAfter(wpReady);
end;

{ If IDF_TOOLS_PATH is set in the environment,
  set the default installation directory accordingly.
  Note: here we read IDF_TOOLS_PATH using GetEnv rather than
  by getting it from registry, in case the user has set
  IDF_TOOLS_PATH as a system environment variable manually. }
<event('InitializeWizard')>
procedure UpdateInstallDir();
var
  EnvToolsPath: String;
begin
  EnvToolsPath := GetEnv('IDF_TOOLS_PATH');
  if EnvToolsPath <> '' then
  begin
    WizardForm.DirEdit.Text := EnvToolsPath;
  end;
end;

<event('NextButtonClick')>
function PreInstallSteps(CurPageID: Integer): Boolean;
var
  DestPath: String;
begin
  Result := True;
  if CurPageID <> wpReady then
    exit;

  ForceDirectories(ExpandConstant('{app}\dist'));

  if not GitUseExisting then
  begin
    DestPath := ExpandConstant('{app}\dist\{#GitInstallerName}');
    if FileExists(DestPath) then
    begin
      Log('Git installer already downloaded: ' + DestPath);
    end else begin
      idpAddFile('{#GitInstallerDownloadURL}', DestPath);
    end;
  end;

  if not IDFUseExisting then
  begin
    IDFAddDownload();
  end;
end;

{ ------------------------------ Custom steps after the main installation flow ------------------------------ }

procedure AddPythonGitToPath();
var
  EnvPath: String;
  PythonLibPath: String;
begin
  EnvPath := GetEnv('PATH');

  if not GitUseExisting then
    GitExecutablePathUpdateAfterInstall();

  EnvPath := PythonPath + ';' + GitPath + ';' + EnvPath;
  Log('Setting PATH for this process: ' + EnvPath);
  SetEnvironmentVariable('PATH', EnvPath);

  { Set IDF_TOOLS_PATH variable, in case it was set to a different value in the environment.
    The installer will set the variable to the new value in the registry, but we also need the
    new value to be visible to this process. }
  SetEnvironmentVariable('IDF_TOOLS_PATH', ExpandConstant('{app}'))

  { Log and clear PYTHONPATH variable, as it might point to libraries of another Python version}
  PythonLibPath := GetEnv('PYTHONPATH')
  Log('PYTHONPATH=' + PythonLibPath)
  SetEnvironmentVariable('PYTHONPATH', '')
end;

procedure InstallEmbeddedPython();
var
  EmbeddedPythonPath: String;
  CmdLine: String;
begin
  if (Pos('tools', PythonPath) <> 1) then begin
    Exit;
  end;

  EmbeddedPythonPath := ExpandConstant('{app}\') + PythonExecutablePath;
  UpdatePythonVariables(EmbeddedPythonPath);
  Log('Checking existence of Embedded Python: ' + EmbeddedPythonPath);
  if (FileExists(EmbeddedPythonPath)) then begin
    Exit;
  end;

  CmdLine := ExpandConstant('{tmp}\7za.exe x -o{app}\tools\python\' + PythonVersion + '\ -r -aoa "{app}\dist\idf-python-' + PythonVersion + '-embed-win64.zip"');
  DoCmdlineInstall('Extracting Python Interpreter', 'Using Embedded Python', CmdLine);
end;

<event('CurStepChanged')>
procedure PostInstallSteps(CurStep: TSetupStep);
var
  Err: Integer;
begin
  if CurStep <> ssPostInstall then
    exit;

  ExtractTemporaryFile('7za.exe')
  InstallEmbeddedPython();

  try
    AddPythonGitToPath();

    if not IDFUseExisting then
      IDFDownload();

    if WizardIsTaskSelected('idf_tools_use_mirror') then
    begin
      SetEnvironmentVariable('IDF_GITHUB_ASSETS', 'dl.espressif.com/github_assets')
    end;

    IDFToolsSetup();


  if WizardIsTaskSelected('CreateLnkStartCmd') then
  begin
    CreateIDFCommandPromptShortcut('{autostartmenu}\Programs\ESP-IDF');
  end;

  if WizardIsTaskSelected('CreateLnkStartPs') then
  begin
    CreateIDFPowershellShortcut('{autostartmenu}\Programs\ESP-IDF' );
  end;

  if WizardIsTaskSelected('CreateLnkDeskCmd') then
  begin
    CreateIDFCommandPromptShortcut('{autodesktop}');
  end;

  if WizardIsTaskSelected('CreateLnkDeskPs') then
  begin
    CreateIDFPowershellShortcut('{autodesktop}');
  end;

  if WizardIsTaskSelected('wdexcl') then
  begin
    RegisterIDFToolsExecutablesInWD();
  end;

  except
    SetupAborted := True;
    if MsgBox('Installation log has been created, it may contain more information about the problem.' + #13#10
              + 'Display the installation log now?', mbConfirmation, MB_YESNO or MB_DEFBUTTON1) = IDYES then
    begin
      ShellExec('', 'notepad.exe', ExpandConstant('{log}'), ExpandConstant('{tmp}'), SW_SHOW, ewNoWait, Err);
    end;
    Abort();
  end;
end;

<event('ShouldSkipPage')>
function SkipFinishedPage(PageID: Integer): Boolean;
begin
  Result := False;

  if PageID = wpFinished then
  begin
    Result := SetupAborted;
  end;
end;


function IsPowerShellInstalled(): Boolean;
begin
  Result := ((not SetupAborted) and (WizardIsTaskSelected('CreateLnkDeskPs') or WizardIsTaskSelected('CreateLnkStartPs')));
end;

function IsCmdInstalled(): Boolean;
begin
  Result := ((not SetupAborted) and (WizardIsTaskSelected('CreateLnkDeskCmd') or WizardIsTaskSelected('CreateLnkStartCmd')));
end;

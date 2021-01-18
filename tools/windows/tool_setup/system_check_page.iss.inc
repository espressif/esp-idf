{ Copyright 2019-2021 Espressif Systems (Shanghai) CO LTD
  SPDX-License-Identifier: Apache-2.0 }

{ SystemCheck states }
const
  SYSTEM_CHECK_STATE_INIT = 0;      { No check was executed yet. }
  SYSTEM_CHECK_STATE_RUNNING = 1;   { Check is in progress and can be cancelled. }
  SYSTEM_CHECK_STATE_COMPLETE = 2;  { Check is complete. }
  SYSTEM_CHECK_STATE_STOPPED = 3;   { User stopped the check. }

var
  { RTF View to display content of system check. }
  SystemCheckViewer: TNewMemo;
  { Indicate state of System Check. }
  SystemCheckState:Integer;
  { Text representation of log messages which are then converte to RTF. }
  SystemLogText: TStringList;
  { Message for user which gives a hint how to correct the problem. }
  SystemCheckHint: String;
  { Setup Page which displays progress/result of system check. }
  SystemCheckPage: TOutputMsgWizardPage;
  { TimeCounter for Spinner animation invoked during command execution. }
  TimeCounter:Integer;
  { Spinner is TStringList, because characters like backslash must be escaped and stored on two bytes. }
  Spinner: TStringList;
  { Button to request display of full log of system check/installation. }
  FullLogButton: TNewButton;
  { Button to request application of available fixtures. }
  ApplyFixesButton: TNewButton;
  { Commands which should be executed to fix problems discovered during system check. }
  Fixes: TStringList;
  { Button to request Stop of System Checks manually. }
  StopSystemCheckButton: TNewButton;
  { Count number of createde virtualenv to avoid collision with previous runs. }
  VirtualEnvCounter: Integer;

{ Indicates whether system check was able to find running Windows Defender. }
var IsWindowsDefenderEnabled: Boolean;

{ Const values for user32.dll which allows scrolling of the text view. }
const
  WM_VSCROLL = $0115;
  SB_BOTTOM = 7;

type
  TMsg = record
    hwnd: HWND;
    message: UINT;
    wParam: Longint;
    lParam: Longint;
    time: DWORD;
    pt: TPoint;
  end;

const
  PM_REMOVE = 1;

{ Functions to communicate via Windows API. }
function PeekMessage(var lpMsg: TMsg; hWnd: HWND; wMsgFilterMin, wMsgFilterMax, wRemoveMsg: UINT): BOOL; external 'PeekMessageW@user32.dll stdcall';
function TranslateMessage(const lpMsg: TMsg): BOOL; external 'TranslateMessage@user32.dll stdcall';
function DispatchMessage(const lpMsg: TMsg): Longint; external 'DispatchMessageW@user32.dll stdcall';

procedure AppProcessMessage;
var
  Msg: TMsg;
begin
  while PeekMessage(Msg, WizardForm.Handle, 0, 0, PM_REMOVE) do begin
    TranslateMessage(Msg);
    DispatchMessage(Msg);
  end;
end;

{ Render text message for view, add spinner if necessary and scroll the window. }
procedure SystemLogRefresh();
begin
  SystemCheckViewer.Lines := SystemLogText;

  { Add Spinner to message. }
  if ((TimeCounter > 0) and (TimeCounter < 6)) then begin
    SystemCheckViewer.Lines[SystemCheckViewer.Lines.Count - 1] := SystemCheckViewer.Lines[SystemCheckViewer.Lines.Count - 1] + ' [' + Spinner[TimeCounter - 1] + ']';
  end;

  { Scroll window to the bottom of the log - https://stackoverflow.com/questions/64587596/is-it-possible-to-display-the-install-actions-in-a-list-in-inno-setup }
  SendMessage(SystemCheckViewer.Handle, WM_VSCROLL, SB_BOTTOM, 0);
end;

{ Log message to file and display just a '.' to user so that user is not overloaded by details. }
procedure SystemLogProgress(message:String);
begin
  Log(message);
  if (SystemLogText.Count = 0) then begin
    SystemLogText.Append('');
  end;

  SystemLogText[SystemLogText.Count - 1] := SystemLogText[SystemLogText.Count - 1] + '.';
  SystemLogRefresh();
end;

{ Log message to file and display it to user as title message with asterisk prefix. }
procedure SystemLogTitle(message:String);
begin
  message := '* ' + message;
  Log(message);
  SystemLogText.Append(message);
  SystemLogRefresh();
end;

{ Log message to file and display it to user. }
procedure SystemLog(message:String);
begin
  Log(message);
    if (SystemLogText.Count = 0) then begin
    SystemLogText.Append('');
  end;

  SystemLogText[SystemLogText.Count - 1] := SystemLogText[SystemLogText.Count - 1] + message;
  SystemLogRefresh();
end;

{ Process timer tick during command execution so that the app keeps communicating with user. }
procedure TimerTick();
begin
  { TimeCounter for animating Spinner. }
  TimeCounter:=TimeCounter+1;
  if (TimeCounter = 5) then begin
    TimeCounter := 1;
  end;

  { Redraw Log with Spinner animation. }
  SystemLogRefresh();

  { Give control back to UI so that it can be updated. https://gist.github.com/jakoch/33ac13800c17eddb2dd4 }
  AppProcessMessage;
end;

{ --- Command line nonblocking exec --- }
function NonBlockingExec(command, workdir: String): Integer;
var
  Res: Integer;
  Handle: Longword;
  ExitCode: Integer;
  LogTextAnsi: AnsiString;
  LogText, LeftOver: String;

begin
  if (SystemCheckState = SYSTEM_CHECK_STATE_STOPPED) then begin
    ExitCode := -3;
    Exit;
  end;
  try
    ExitCode := -1;
    { SystemLog('Workdir: ' + workdir); }
    SystemLogProgress(' $ ' + command);
    Handle := ProcStart(command, workdir)
    if Handle = 0 then
    begin
      SystemLog('[' + CustomMessage('SystemCheckResultError') + ']');
      Result := -2;
      Exit;
    end;
    while (ExitCode = -1) and (SystemCheckState <> SYSTEM_CHECK_STATE_STOPPED) do
    begin
      ExitCode := ProcGetExitCode(Handle);
      SetLength(LogTextAnsi, 4096);
      Res := ProcGetOutput(Handle, LogTextAnsi, 4096)
      if Res > 0 then
      begin
        SetLength(LogTextAnsi, Res);
        LogText := LeftOver + String(LogTextAnsi);
        SystemLogProgress(LogText);
      end;
      TimerTick();
      Sleep(200);
    end;
    ProcEnd(Handle);
  finally
    if (SystemCheckState = SYSTEM_CHECK_STATE_STOPPED) then
    begin
      Result := -1;
    end else begin
      Result := ExitCode;
    end;
  end;
end;

{ Execute command for SystemCheck and reset timer so that Spinner will disappear after end of execution. }
function SystemCheckExec(command, workdir: String): Integer;
begin
  TimeCounter := 0;
  Result := NonBlockingExec(command, workdir);
  TimeCounter := 0;
end;

{ Get formated line from SystemCheck for user. }
function GetSystemCheckHint(Command: String; CustomCheckMessageKey:String):String;
begin
  Result := CustomMessage('SystemCheckUnableToExecute') + ' ' + Command + #13#10 + CustomMessage(CustomCheckMessageKey);
end;

{ Add command to list of fixes which can be executed by installer. }
procedure AddFix(Command:String);
begin
  { Do not add possible fix command when check command was stopped by user. }
  if (SystemCheckState = SYSTEM_CHECK_STATE_STOPPED) then begin
    Exit;
  end;
  Fixes.Append(Command);
end;

{ Execute checks to determine whether Python installation is valid so thet user can choose it to install IDF. }
function IsPythonInstallationValid(displayName: String; pythonPath:String): Boolean;
var
  ResultCode: Integer;
  ScriptFile: String;
  TempDownloadFile: String;
  Command: String;
  VirtualEvnPath: String;
  VirtualEnvPython: String;
  RemedyCommand: String;
begin
  SystemLogTitle(CustomMessage('SystemCheckForComponent') + ' ' + displayName + ' ');
  SystemCheckHint := '';

  pythonPath := pythonPath + ' ';

  Command := pythonPath + '-m pip --version';
  ResultCode := SystemCheckExec(Command, ExpandConstant('{tmp}'));
  if (ResultCode <> 0) then begin
    SystemCheckHint := GetSystemCheckHint(Command, 'SystemCheckRemedyMissingPip');
    Result := False;
    Exit;
  end;

  Command := pythonPath + '-m virtualenv --version';
  ResultCode := SystemCheckExec(Command, ExpandConstant('{tmp}'));
  if (ResultCode <> 0) then begin
    SystemCheckHint := GetSystemCheckHint(Command, 'SystemCheckRemedyMissingVirtualenv') + #13#10 + pythonPath + '-m pip install --upgrade pip' + #13#10 + pythonPath + '-m pip install virtualenv';
    AddFix(pythonPath + '-m pip install --upgrade pip');
    AddFix(pythonPath + '-m pip install virtualenv');
    Result := False;
    Exit;
  end;

  VirtualEnvCounter := VirtualEnvCounter + 1;
  VirtualEvnPath := ExpandConstant('{tmp}\') + IntToStr(VirtualEnvCounter) + '-idf-test-venv\';
  VirtualEnvPython := VirtualEvnPath + 'Scripts\python.exe ';
  Command := pythonPath + '-m virtualenv ' + VirtualEvnPath;
  ResultCode := SystemCheckExec(Command, ExpandConstant('{tmp}'));
  if (ResultCode <> 0) then begin
    SystemCheckHint := GetSystemCheckHint(Command, 'SystemCheckRemedyCreateVirtualenv');
    Result := False;
    Exit;
  end;

  ScriptFile := ExpandConstant('{tmp}\system_check_virtualenv.py')
  Command := VirtualEnvPython + ScriptFile + ' ' + VirtualEnvPython;
  ResultCode := SystemCheckExec(Command, ExpandConstant('{tmp}'));
  if (ResultCode <> 0) then begin
    SystemCheckHint := GetSystemCheckHint(Command, 'SystemCheckRemedyPythonInVirtualenv');
    Result := False;
    Exit;
  end;

  Command := VirtualEnvPython + '-m pip install --only-binary ":all:" "cryptography>=2.1.4" --no-binary future';
  ResultCode := SystemCheckExec(Command, ExpandConstant('{tmp}'));
  if (ResultCode <> 0) then begin
    SystemCheckHint := GetSystemCheckHint(Command, 'SystemCheckRemedyBinaryPythonWheel');
    Result := False;
    Exit;
  end;

  TempDownloadFile := IntToStr(VirtualEnvCounter) + '-idf-exe-v1.0.1.zip';
  ScriptFile := ExpandConstant('{tmp}\system_check_download.py');
  Command := VirtualEnvPython + ScriptFile + ExpandConstant(' https://dl.espressif.com/dl/idf-exe-v1.0.1.zip ' + TempDownloadFile);
  ResultCode := SystemCheckExec(Command , ExpandConstant('{tmp}'));
  if (ResultCode <> 0) then begin
    SystemCheckHint := GetSystemCheckHint(Command, 'SystemCheckRemedyFailedHttpsDownload');
    Result := False;
    Exit;
  end;

  if (not FileExists(ExpandConstant('{tmp}\') + TempDownloadFile)) then begin
    SystemLog(' [' + CustomMessage('SystemCheckResultFail') + '] - ' + CustomMessage('SystemCheckUnableToFindFile') + ' ' + ExpandConstant('{tmp}\') + TempDownloadFile);
    Result := False;
    Exit;
  end;

  ScriptFile := ExpandConstant('{tmp}\system_check_subprocess.py');
  Command := pythonPath + ScriptFile;
  ResultCode := SystemCheckExec(Command, ExpandConstant('{tmp}'));
  if (ResultCode <> 0) then begin
    RemedyCommand := pythonPath + '-m pip uninstall subprocess.run';
    SystemCheckHint := GetSystemCheckHint(Command, 'SystemCheckRemedyFailedSubmoduleRun') + #13#10 + RemedyCommand;
    AddFix(RemedyCommand);
    Result := False;
    Exit;
  end;

  SystemLog(' [' + CustomMessage('SystemCheckResultOk') + ']');
  Result := True;
end;

procedure FindPythonVersionsFromKey(RootKey: Integer; SubKeyName: String);
var
  CompanyNames: TArrayOfString;
  CompanyName, CompanySubKey, TagName, TagSubKey: String;
  ExecutablePath, DisplayName, Version: String;
  TagNames: TArrayOfString;
  CompanyId, TagId: Integer;
  BaseDir: String;
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

      if not GetPythonVersionInfoFromKey(RootKey, SubKeyName, CompanyName, TagName, Version, DisplayName, ExecutablePath, BaseDir) then
        continue;

      if (SystemCheckState = SYSTEM_CHECK_STATE_STOPPED) then begin
        Exit;
      end;

      { Verify Python installation and display hint in case of invalid version or env. }
      if not IsPythonInstallationValid(DisplayName, ExecutablePath) then begin
        if ((Length(SystemCheckHint) > 0) and (SystemCheckState <> SYSTEM_CHECK_STATE_STOPPED)) then begin
          SystemLogTitle(CustomMessage('SystemCheckHint') + ': ' + SystemCheckHint);
        end;
        continue;
      end;

      PythonVersionAdd(Version, DisplayName, ExecutablePath);
    end;
  end;
end;

procedure FindInstalledPythonVersions();
begin
  FindPythonVersionsFromKey(HKEY_CURRENT_USER, 'Software\Python');
  FindPythonVersionsFromKey(HKEY_LOCAL_MACHINE, 'Software\Python');
  FindPythonVersionsFromKey(HKEY_LOCAL_MACHINE, 'Software\Wow6432Node\Python');
end;


{ Get Boolean for UI to determine whether it make sense to register exceptions to Defender. }
function GetWindowsDefenderStatus(): Boolean;
var
  bHasWD: Boolean;
  szWDPath: String;
  listPSModulePath: TStringList;
  ResultCode: Integer;
  x: Integer;
begin
  Log('Checking PSMODULEPATH for Windows Defender module');

  listPSModulePath := TStringList.Create;
  listPSModulePath.Delimiter := ';';
  listPSModulePath.StrictDelimiter := True;
  listPSModulePath.DelimitedText := GetEnv('PsModulePath');

  for x:=0 to (listPSModulePath.Count-1) do
  begin
    szWDPath := listPSModulePath[x] + '\Defender'
    bHasWD := DirExists(szWDPath);
    if bHasWD then
    begin
      break;
    end
  end;

  if not bHasWD then begin
    Result := False;
    Exit;
  end;

  Log('Checking Windows Services Defender is enabled: (Get-MpComputerStatus).AntivirusEnabled');
  ResultCode := SystemCheckExec('powershell -ExecutionPolicy Bypass "if((Get-MpComputerStatus).AntivirusEnabled) { Exit 0 } else { Exit 1 }"', ExpandConstant('{tmp}'));
  if (ResultCode <> 0)  then begin
    Log('Result code: ' + IntToStr(ResultCode));
    Result := False;
    Exit;
  end;

  Result := True;
end;

{ Process user request to stop system checks. }
function SystemCheckStopRequest():Boolean;
begin
  { In case of stopped check by user, procees to next/previous step. }
  if (SystemCheckState = SYSTEM_CHECK_STATE_STOPPED) then begin
    Result := True;
    Exit;
  end;

  if (SystemCheckState = SYSTEM_CHECK_STATE_RUNNING) then begin
    if (MsgBox(CustomMessage('SystemCheckNotCompleteConsent'), mbConfirmation, MB_YESNO) = IDYES) then begin
      SystemCheckState := SYSTEM_CHECK_STATE_STOPPED;
      Result := True;
      Exit;
    end;
  end;

  if (SystemCheckState = SYSTEM_CHECK_STATE_COMPLETE) then begin
    Result := True;
  end else begin
    Result := False;
  end;
end;

{ Process request to proceed to next page. If the scan is running ask user for confirmation. }
function OnSystemCheckValidate(Sender: TWizardPage): Boolean;
begin
  Result := SystemCheckStopRequest();
end;

{ Process request to go to previous screen (license). Prompt user for confirmation when system check is running. }
function OnSystemCheckBackButton(Sender: TWizardPage): Boolean;
begin
  Result := SystemCheckStopRequest();
end;

{ Process request to stop System Check directly on the screen with System Check by Stop button. }
procedure StopSystemCheckButtonClick(Sender: TObject);
begin
  SystemCheckStopRequest();
end;

{ Check whether site is reachable and that system trust the certificate. }
procedure VerifyRootCertificates();
var
  ResultCode: Integer;
  Command: String;
  OutFile: String;
begin
  SystemLogTitle(CustomMessage('SystemCheckRootCertificates') + ' ');

  { It's necessary to invoke PowerShell *BEFORE* Python. Invoke-Request will retrieve and add Root Certificate if necessary. }
  { Without the certificate Python is failing to connect to https. }
  { Windows command to list current certificates: certlm.msc }
  OutFile := ExpandConstant('{tmp}\check');
  Command := 'powershell -ExecutionPolicy Bypass ';
  Command := Command + 'Invoke-WebRequest -Uri "https://dl.espressif.com/dl/?system_check=win' + GetWindowsVersionString + '" -OutFile "' + OutFile + '-1.txt";';
  Command := Command + 'Invoke-WebRequest -Uri "https://github.com/espressif" -OutFile "' + OutFile + '-2.txt";';
  {Command := Command + 'Invoke-WebRequest -Uri "https://www.s3.amazonaws.com/" -OutFile "' + OutFile + '-3.txt";';}
  ResultCode := SystemCheckExec(Command, ExpandConstant('{tmp}'));
  if (ResultCode <> 0) then begin
    SystemLog(' [' + CustomMessage('SystemCheckResultWarn') + ']');
    SystemLog(CustomMessage('SystemCheckRootCertificateWarning'));
  end else begin
    SystemLog(' [' + CustomMessage('SystemCheckResultOk') + ']');
  end;
end;

{ Execute system check }
procedure ExecuteSystemCheck();
var
  UseEmbeddedPythonParam: String;
begin
  { Execute system check only once. Avoid execution in case of back button. }
  if (SystemCheckState <> SYSTEM_CHECK_STATE_INIT) then begin
    Exit;
  end;

  SystemCheckState := SYSTEM_CHECK_STATE_RUNNING;
  SystemLogTitle(CustomMessage('SystemCheckStart'));
  StopSystemCheckButton.Enabled := True;

  VerifyRootCertificates();

  { Search for the installed Python version only on explicit user request. }
  UseEmbeddedPythonParam := ExpandConstant('{param:USEEMBEDDEDPYTHON|yes}');
  if (UseEmbeddedPythonParam <> 'yes') then begin
    FindInstalledPythonVersions();
  end;

  if (SystemCheckState <> SYSTEM_CHECK_STATE_STOPPED) then begin
    SystemLogTitle(CustomMessage('SystemCheckForDefender') + ' ');
    IsWindowsDefenderEnabled := GetWindowsDefenderStatus();
    if (IsWindowsDefenderEnabled) then begin
      SystemLog(' [' + CustomMessage('SystemCheckResultFound') + ']');
    end else begin
      SystemLog(' [' + CustomMessage('SystemCheckResultNotFound') + ']');
    end;
  end else begin
    { User cancelled the check, let's enable Defender script so that use can decide to disable it. }
    IsWindowsDefenderEnabled := True;
  end;

  if (SystemCheckState = SYSTEM_CHECK_STATE_STOPPED) then begin
    SystemLog('');
    SystemLogTitle(CustomMessage('SystemCheckStopped'));
  end else begin
    SystemLogTitle(CustomMessage('SystemCheckComplete'));
    SystemCheckState := SYSTEM_CHECK_STATE_COMPLETE;
  end;

  { Enable Apply Script button if some fixes are available. }
  if (Fixes.Count > 0) then begin
    ApplyFixesButton.Enabled := True;
  end;

  StopSystemCheckButton.Enabled := False;
end;

{ Invoke scan of system environment. }
procedure OnSystemCheckActivate(Sender: TWizardPage);
var SystemCheckParam:String;
begin
  { Display special controls. For some reason the first call of the page does not invoke SystemCheckOnCurPageChanged. }
  FullLogButton.Visible := True;
  ApplyFixesButton.Visible := True;
  StopSystemCheckButton.Visible := True;
  SystemCheckViewer.Visible := True;

  SystemCheckParam := ExpandConstant('{param:SKIPSYSTEMCHECK|no}');
  if (SystemCheckParam = 'yes') then begin
    SystemCheckState := SYSTEM_CHECK_STATE_STOPPED;
    SystemLog('System Check disabled by command line option /SKIPSYSTEMCHECK.');
  end;

  ExecuteSystemCheck();
end;

{ Handle request to display full log from the installation. Open the log in notepad. }
procedure FullLogButtonClick(Sender: TObject);
var
  ResultCode: Integer;
begin
  Exec(ExpandConstant('{win}\notepad.exe'), ExpandConstant('{log}'), '', SW_SHOW, ewNoWait, ResultCode);
end;

{ Handle request to apply available fixes. }
procedure ApplyFixesButtonClick(Sender: TObject);
var
  ResultCode: Integer;
  FixIndex: Integer;
  AreFixesApplied: Boolean;
begin
  if (MsgBox(CustomMessage('SystemCheckApplyFixesConsent'), mbConfirmation, MB_YESNO) = IDNO) then begin
    Exit;
  end;

  ApplyFixesButton.Enabled := false;
  SystemCheckState := SYSTEM_CHECK_STATE_INIT;
  SystemLog('');
  SystemLogTitle('Starting application of fixes');

  AreFixesApplied := True;
  for FixIndex := 0 to Fixes.Count - 1 do
  begin
    ResultCode := SystemCheckExec(Fixes[FixIndex], ExpandConstant('{tmp}'));
    if (ResultCode <> 0) then begin
      AreFixesApplied := False;
      break;
    end;
  end;

  SystemLog('');
  if (AreFixesApplied) then begin
    SystemLogTitle(CustomMessage('SystemCheckFixesSuccessful'));
  end else begin
    SystemLogTitle(CustomMessage('SystemCheckFixesFailed'));
  end;

  SystemLog('');
  Fixes.Clear();

  { Restart system check. }
  ExecuteSystemCheck();
end;

{ Add Page for System Check so that user is informed about readiness of the system. }
<event('InitializeWizard')>
procedure CreateSystemCheckPage();
begin
  { Initialize data structure for Python }
  InstalledPythonVersions := TStringList.Create();
  InstalledPythonDisplayNames := TStringList.Create();
  InstalledPythonExecutables := TStringList.Create();
  PythonVersionAdd('{#PythonVersion}', 'Use Python {#PythonVersion} Embedded (Recommended)', 'tools\python\{#PythonVersion}\python.exe');

  { Create Spinner animation. }
  Spinner := TStringList.Create();
  Spinner.Append('-');
  Spinner.Append('\');
  Spinner.Append('|');
  Spinner.Append('/');

  VirtualEnvCounter := 0;
  Fixes := TStringList.Create();
  SystemCheckState := SYSTEM_CHECK_STATE_INIT;
  SystemCheckPage := CreateOutputMsgPage(wpLicense, CustomMessage('PreInstallationCheckTitle'), CustomMessage('PreInstallationCheckSubtitle'), '');

  with SystemCheckPage do
  begin
    OnActivate := @OnSystemCheckActivate;
    OnBackButtonClick := @OnSystemCheckBackButton;
    OnNextButtonClick := @OnSystemCheckValidate;
  end;

  SystemCheckViewer := TNewMemo.Create(WizardForm);
  with SystemCheckViewer do
  begin
    Parent := WizardForm;
    Left := ScaleX(10);
    Top := ScaleY(60);
    ReadOnly := True;
    Font.Name := 'Courier New';
    Height := WizardForm.CancelButton.Top - ScaleY(40);
    Width := WizardForm.ClientWidth + ScaleX(80);
    WordWrap := True;
    Visible := False;
  end;

  SystemLogText := TStringList.Create;

  FullLogButton := TNewButton.Create(WizardForm);
  with FullLogButton do
  begin
    Parent := WizardForm;
    Left := WizardForm.ClientWidth;
    Top := SystemCheckViewer.Top + SystemCheckViewer.Height + ScaleY(5);
    Width := WizardForm.CancelButton.Width;
    Height := WizardForm.CancelButton.Height;
    Caption := CustomMessage('SystemCheckFullLogButtonCaption');
    OnClick := @FullLogButtonClick;
    Visible := False;
  end;

  ApplyFixesButton := TNewButton.Create(WizardForm);
  with ApplyFixesButton do
  begin
    Parent := WizardForm;
    Left := WizardForm.ClientWidth - FullLogButton.Width;
    Top := FullLogButton.Top;
    Width := WizardForm.CancelButton.Width;
    Height := WizardForm.CancelButton.Height;
    Caption := CustomMessage('SystemCheckApplyFixesButtonCaption');
    OnClick := @ApplyFixesButtonClick;
    Visible := False;
    Enabled := False;
  end;

  StopSystemCheckButton := TNewButton.Create(WizardForm);
  with StopSystemCheckButton do
  begin
    Parent := WizardForm;
    Left := ApplyFixesButton.Left - ApplyFixesButton.Width;
    Top := FullLogButton.Top;
    Width := WizardForm.CancelButton.Width;
    Height := WizardForm.CancelButton.Height;
    Caption := CustomMessage('SystemCheckStopButtonCaption');
    OnClick := @StopSystemCheckButtonClick;
    Visible := False;
    Enabled := False;
  end;

  { Extract helper files for sanity check of Python environment. }
  ExtractTemporaryFile('system_check_download.py')
  ExtractTemporaryFile('system_check_subprocess.py')
  ExtractTemporaryFile('system_check_virtualenv.py')
end;

{ Process Cancel Button Click event. Prompt user to confirm Cancellation of System check. }
{ Then continue with normal cancel window. }
procedure CancelButtonClick(CurPageID: Integer; var Cancel, Confirm: Boolean);
begin
  if ((CurPageId = SystemCheckPage.ID) and (SystemCheckState = SYSTEM_CHECK_STATE_RUNNING)) then begin
    SystemCheckStopRequest();
  end;
end;

{ Display control specific for System Check page. }
<event('CurPageChanged')>
procedure SystemCheckOnCurPageChanged(CurPageID: Integer);
begin
  FullLogButton.Visible := CurPageID = SystemCheckPage.ID;
  ApplyFixesButton.Visible := CurPageID = SystemCheckPage.ID;
  StopSystemCheckButton.Visible := CurPageID = SystemCheckPage.ID;
  SystemCheckViewer.Visible := CurPageID = SystemCheckPage.ID;
end;

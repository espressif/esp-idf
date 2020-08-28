{ Copyright 2019-2021 Espressif Systems (Shanghai) CO LTD
  SPDX-License-Identifier: Apache-2.0 }

{ ------------------------------ Page to select the version of ESP-IDF to download ------------------------------ }

var
  IDFDownloadPage: TInputOptionWizardPage;
  IDFDownloadAvailableVersions: TArrayOfString;
  IDFDownloadPath, IDFDownloadVersion: String;

function GetSuggestedIDFDirectory(): String;
var
BaseName: String;
RepeatIndex: Integer;
begin
  if (IDFDirectory <> '') then begin
    Result := IDFDirectory
    Exit;
  end;

  { Start with Desktop\esp-idf name and if it already exists,
    keep trying with Desktop\esp-idf-N for N=2 and above. }
  BaseName := ExpandConstant('{userdesktop}\esp-idf');
  Result := BaseName;
  RepeatIndex := 1;
  while DirExists(Result) do
  begin
    RepeatIndex := RepeatIndex + 1;
    Result := BaseName + '-' + IntToStr(RepeatIndex);
  end;
end;

function GetIDFVersionDescription(Version: String): String;
begin
  if WildCardMatch(Version, 'v*-beta*') then
    Result := 'beta version'
  else if WildCardMatch(Version, 'v*-rc*') then
    Result := 'pre-release version'
  else if WildCardMatch(Version, 'v*') then
    Result := 'release version'
  else if WildCardMatch(Version, 'release/v*') then
    Result := 'release branch'
  else if WildCardMatch(Version, 'master') then
    Result := 'development branch'
  else
    Result := '';
end;

procedure ExtractIDFVersionList();
begin
  ExtractTemporaryFile('idf_versions.txt');
end;

procedure DownloadIDFVersionsList();
var
  VersionFile: String;
begin
  VersionFile := ExpandConstant('{tmp}\idf_versions.txt');
  if idpDownloadFile(IDFVersionUrl, VersionFile) then
  begin
    Log('Downloaded ' + IDFVersionUrl + ' to ' + VersionFile);
  end else begin
    Log('Download of ' + IDFVersionUrl + ' failed, using a fallback versions list');
    ExtractIDFVersionList();
  end;
end;

procedure OnIDFDownloadPagePrepare(Sender: TObject);
var
  Page: TInputOptionWizardPage;
  VersionFile: String;
  i: Integer;
begin
  Page := TInputOptionWizardPage(Sender);
  Log('OnIDFDownloadPagePrepare');
  if Page.CheckListBox.Items.Count > 0 then
    exit;

  if (IsOfflineMode) then begin
    Log('Offline Mode: using embedded idf_versions.txt')
    ExtractIDFVersionList();
  end else begin
    DownloadIDFVersionsList();
  end;

  VersionFile := ExpandConstant('{tmp}\idf_versions.txt');
  if not LoadStringsFromFile(VersionFile, IDFDownloadAvailableVersions) then
  begin
    Log('Failed to load versions from ' + VersionFile);
    exit;
  end;

  Log('Versions count: ' + IntToStr(GetArrayLength(IDFDownloadAvailableVersions)))
  for i := 0 to GetArrayLength(IDFDownloadAvailableVersions) - 1 do
  begin
    Log('Version ' + IntToStr(i) + ': ' + IDFDownloadAvailableVersions[i]);
    Page.Add(IDFDownloadAvailableVersions[i] + ' ('
             + GetIDFVersionDescription(IDFDownloadAvailableVersions[i]) + ')');
  end;
  Page.SelectedValueIndex := 0;

  ChoicePageSetInputText(Page, GetSuggestedIDFDirectory());
end;

{ Validation of PATH for IDF releases which does not support special characters. }
{ Source: https://stackoverflow.com/questions/21623515/is-it-possible-to-filter-require-installation-path-to-be-ascii-in-innosetup }
function IsCharValid(Value: Char): Boolean;
begin
  Result := Ord(Value) <= $007F;
end;

function IsDirNameValid(const Value: string): Boolean;
var
  I: Integer;
begin
  Result := False;
  for I := 1 to Length(Value) do
    if not IsCharValid(Value[I]) then
      Exit;
  Result := True;
end;

procedure OnIDFDownloadSelectionChange(Sender: TObject);
var
  Page: TInputOptionWizardPage;
begin
  Page := TInputOptionWizardPage(Sender);
  Log('OnIDFDownloadSelectionChange index=' + IntToStr(Page.SelectedValueIndex));
end;

function OnIDFDownloadPageValidate(Sender: TWizardPage): Boolean;
var
  Page: TInputOptionWizardPage;
  IDFPath: String;
begin
  Result := False;
  Page := TInputOptionWizardPage(Sender);
  Log('OnIDFDownloadPageValidate index=' + IntToStr(Page.SelectedValueIndex));

  IDFPath := ChoicePageGetInputText(Page);
  if DirExists(IDFPath) and not DirIsEmpty(IDFPath) then
  begin
    MsgBox('Directory already exists and is not empty:' + #13#10 +
           IDFPath + #13#10 + 'Please choose a different directory.', mbError, MB_OK);
    exit;
  end;

  if Pos(' ', IDFPath) <> 0 then
  begin
    MsgBox('ESP-IDF build system does not support spaces in paths.' + #13#10
           'Please choose a different directory.', mbError, MB_OK);
    exit;
  end;

  IDFDownloadPath := IDFPath;

  { Use parameter /IDFVERSION=x to override selection in the box. }
  IDFDownloadVersion := IDFVersion;
  if (IDFDownloadVersion = '') then begin
    IDFDownloadVersion := IDFDownloadAvailableVersions[Page.SelectedValueIndex];
  end;

  { Following ZIP versions of IDF does not support installation on path with special characters. }
  { Issue: https://github.com/espressif/esp-idf/issues/5996 }
  if ((IDFDownloadVersion = 'v4.2') or (IDFDownloadVersion = 'v4.0.2') or
    (IDFDownloadVersion = 'v3.3.4')) then begin
    if (not IsDirNameValid(IDFPath)) then begin
      MsgBox('The installation of selected version of IDF is not supported on path with special characters.' + #13#10
            'Please choose a different directory.', mbError, MB_OK);
      exit;
    end;
  end;

  Result := True;
end;

<event('ShouldSkipPage')>
function ShouldSkipIDFDownloadPage(PageID: Integer): Boolean;
begin
  if (PageID = IDFDownloadPage.ID) and not IDFDownloadRequired() then
    Result := True;
end;

<event('InitializeWizard')>
procedure CreateIDFDownloadPage();
begin
  IDFDownloadPage := ChoicePageCreate(
    IDFPage.ID,
    'Version of ESP-IDF', 'Please choose ESP-IDF version to install',
    'For more information about ESP-IDF versions, see' + #13#10 +
      'https://docs.espressif.com/projects/esp-idf/en/latest/versions.html',
    'Choose a directory to install ESP-IDF to',
    True,
    @OnIDFDownloadPagePrepare,
    @OnIDFDownloadSelectionChange,
    @OnIDFDownloadPageValidate);
end;

{ Copyright 2019 Espressif Systems (Shanghai) PTE LTD
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

procedure DownloadIDFVersionsList();
var
  Url: String;
  VersionFile: String;
begin
  Url := '{#IDFVersionsURL}';
  VersionFile := ExpandConstant('{tmp}\idf_versions.txt');
  if idpDownloadFile(Url, VersionFile) then
  begin
    Log('Downloaded ' + Url + ' to ' + VersionFile);
  end else begin
    Log('Download of ' + Url + ' failed, using a fallback versions list');
    ExtractTemporaryFile('idf_versions.txt');
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

  DownloadIDFVersionsList();

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
  Page := TInputOptionWizardPage(Sender);
  Log('OnIDFDownloadPageValidate index=' + IntToStr(Page.SelectedValueIndex));

  IDFPath := ChoicePageGetInputText(Page);
  if DirExists(IDFPath) and not DirIsEmpty(IDFPath) then
  begin
    MsgBox('Directory already exists and is not empty:' + #13#10 +
           IDFPath + #13#10 + 'Please choose a different directory.', mbError, MB_OK);
    Result := False;
    exit;
  end;

  IDFDownloadPath := IDFPath;
  IDFDownloadVersion := IDFDownloadAvailableVersions[Page.SelectedValueIndex];
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
    'Download ESP-IDF', 'Please choose ESP-IDF version to download',
    'For more information about ESP-IDF versions, see' + #13#10 +
      'https://docs.espressif.com/projects/esp-idf/en/latest/versions.html',
    'Choose a directory to download ESP-IDF to',
    True,
    @OnIDFDownloadPagePrepare,
    @OnIDFDownloadSelectionChange,
    @OnIDFDownloadPageValidate);
end;

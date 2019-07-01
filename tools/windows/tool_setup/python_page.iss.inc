{ Copyright 2019 Espressif Systems (Shanghai) PTE LTD
  SPDX-License-Identifier: Apache-2.0 }

{ ------------------------------ Page to select Python interpreter ------------------------------ }

#include "python_find_installed.iss.inc"

var
  PythonPage: TInputOptionWizardPage;
  PythonVersion, PythonPath, PythonExecutablePath: String;
  PythonUseExisting: Boolean;


function GetPythonPath(Unused: String): String;
begin
  Result := PythonPath;
end;

function PythonInstallRequired(): Boolean;
begin
  Result := not PythonUseExisting;
end;

function PythonVersionSupported(Version: String): Boolean;
var
  Major, Minor: Integer;
begin
  Result := False;
  if not VersionExtractMajorMinor(Version, Major, Minor) then
  begin
    Log('PythonVersionSupported: Could not parse version=' + Version);
    exit;
  end;

  if (Major = 2) and (Minor = 7) then Result := True;
  if (Major = 3) and (Minor >= 5) then Result := True;
end;

procedure OnPythonPagePrepare(Sender: TObject);
var
  Page: TInputOptionWizardPage;
  FullName: String;
  i, Index, FirstEnabledIndex: Integer;
  OfferToInstall: Boolean;
  VersionToInstall: String;
  VersionSupported: Boolean;
begin
  Page := TInputOptionWizardPage(Sender);
  Log('OnPythonPagePrepare');
  if Page.CheckListBox.Items.Count > 0 then
    exit;

  FindInstalledPythonVersions();

  VersionToInstall := '{#PythonVersion}';
  OfferToInstall := True;
  FirstEnabledIndex := -1;

  for i := 0 to InstalledPythonVersions.Count - 1 do
  begin
    VersionSupported := PythonVersionSupported(InstalledPythonVersions[i]);
    FullName := InstalledPythonDisplayNames.Strings[i];
    if not VersionSupported then
    begin
      FullName := FullName + ' (unsupported)';
    end;
    FullName := FullName + #13#10 + InstalledPythonExecutables.Strings[i];
    Index := Page.Add(FullName);
    if not VersionSupported then
    begin
      Page.CheckListBox.ItemEnabled[Index] := False;
    end else begin
      if FirstEnabledIndex < 0 then FirstEnabledIndex := Index;
    end;
    if InstalledPythonVersions[i] = VersionToInstall then
    begin
      OfferToInstall := False;
    end;
  end;

  if OfferToInstall then
  begin
    Index := Page.Add('Install Python ' + VersionToInstall);
    if FirstEnabledIndex < 0 then FirstEnabledIndex := Index;
  end;

  Page.SelectedValueIndex := FirstEnabledIndex;
end;

procedure OnPythonSelectionChange(Sender: TObject);
var
  Page: TInputOptionWizardPage;
begin
  Page := TInputOptionWizardPage(Sender);
  Log('OnPythonSelectionChange index=' + IntToStr(Page.SelectedValueIndex));
end;

function OnPythonPageValidate(Sender: TWizardPage): Boolean;
var
  Page: TInputOptionWizardPage;
begin
  Page := TInputOptionWizardPage(Sender);
  Log('OnPythonPageValidate index=' + IntToStr(Page.SelectedValueIndex));
  if Page.SelectedValueIndex < InstalledPythonExecutables.Count then
  begin
    PythonUseExisting := True;
    PythonExecutablePath := InstalledPythonExecutables[Page.SelectedValueIndex];
    PythonPath := ExtractFilePath(PythonExecutablePath);
    PythonVersion := InstalledPythonVersions[Page.SelectedValueIndex];
  end else begin
    PythonUseExisting := False;
    PythonExecutablePath := '';
    PythonPath := '';
    PythonVersion := '{#PythonVersion}';
  end;
  Log('OnPythonPageValidate: PythonPath='+PythonPath+' PythonExecutablePath='+PythonExecutablePath);
  Result := True;
end;

procedure PythonExecutablePathUpdateAfterInstall();
var
  Version, DisplayName, ExecutablePath: String;
begin
  if not GetPythonVersionInfoFromKey(
      HKEY_CURRENT_USER, 'Software\Python', 'PythonCore', '{#PythonVersion}',
      Version, DisplayName, ExecutablePath) then
  begin
    Log('Failed to find ExecutablePath for the installed copy of Python');
    exit;
  end;
  Log('Found ExecutablePath for ' + DisplayName + ': ' + ExecutablePath);
  PythonExecutablePath := ExecutablePath;
  PythonPath := ExtractFilePath(PythonExecutablePath);
  Log('PythonExecutablePathUpdateAfterInstall: PythonPath='+PythonPath+' PythonExecutablePath='+PythonExecutablePath);
end;

<event('InitializeWizard')>
procedure CreatePythonPage();
begin
  PythonPage := ChoicePageCreate(
    wpLicense,
    'Python choice', 'Please choose Python version',
    'Available Python versions',
    '',
    False,
    @OnPythonPagePrepare,
    @OnPythonSelectionChange,
    @OnPythonPageValidate);
end;

{ Copyright 2019 Espressif Systems (Shanghai) PTE LTD
  SPDX-License-Identifier: Apache-2.0 }

{ ------------------------------ Page to select whether to download ESP-IDF, or use an existing copy ------------------------------ }

var
  IDFPage: TInputOptionWizardPage;
  IDFSelectionDownloadIndex: Integer;
  IDFSelectionCustomPathIndex: Integer;
  IDFUseExisting: Boolean;
  IDFExistingPath: String;

function IDFDownloadRequired(): Boolean;
begin
  Result := not IDFUseExisting;
end;

procedure IDFPageUpdateInput();
var
  Enable: Boolean;
begin
  if IDFPage.SelectedValueIndex = IDFSelectionCustomPathIndex then
    Enable := True;

  ChoicePageSetInputEnabled(IDFPage, Enable);
end;

procedure OnIDFPagePrepare(Sender: TObject);
var
  Page: TInputOptionWizardPage;
begin
  Page := TInputOptionWizardPage(Sender);
  Log('OnIDFPagePrepare');
  if Page.CheckListBox.Items.Count > 0 then
    exit;

  IDFSelectionDownloadIndex := Page.Add('Download ESP-IDF')
  IDFSelectionCustomPathIndex := Page.Add('Use an existing ESP-IDF directory');

  Page.SelectedValueIndex := 0;
  IDFPageUpdateInput();
end;

procedure OnIDFSelectionChange(Sender: TObject);
var
  Page: TInputOptionWizardPage;
begin
  Page := TInputOptionWizardPage(Sender);
  Log('OnIDFSelectionChange index=' + IntToStr(Page.SelectedValueIndex));
  IDFPageUpdateInput();
end;

function OnIDFPageValidate(Sender: TWizardPage): Boolean;
var
  Page: TInputOptionWizardPage;
  NotSupportedMsg, IDFPath, IDFPyPath, RequirementsPath: String;
begin
  Page := TInputOptionWizardPage(Sender);
  Log('OnIDFPageValidate index=' + IntToStr(Page.SelectedValueIndex));

  if Page.SelectedValueIndex = IDFSelectionDownloadIndex then
  begin
    IDFUseExisting := False;
    Result := True;
  end else begin
    IDFUseExisting := True;
    Result := False;
    NotSupportedMsg := 'The selected version of ESP-IDF is not supported:' + #13#10;
    IDFPath := ChoicePageGetInputText(Page);

    if not DirExists(IDFPath) then
    begin
      MsgBox('Directory doesn''t exist: ' + IDFPath + #13#10 +
             'Please choose an existing ESP-IDF directory', mbError, MB_OK);
      exit;
    end;

    IDFPyPath := IDFPath + '\tools\idf.py';
    if not FileExists(IDFPyPath) then
    begin
      MsgBox(NotSupportedMsg +
             'Can not find idf.py in ' + IDFPath + '\tools', mbError, MB_OK);
      exit;
    end;

    RequirementsPath := IDFPath + '\requirements.txt';
    if not FileExists(RequirementsPath) then
    begin
      MsgBox(NotSupportedMsg +
             'Can not find requirements.txt in ' + IDFPath, mbError, MB_OK);
      exit;
    end;

    IDFExistingPath := IDFPath;
    Result := True;
  end;
end;

<event('InitializeWizard')>
procedure CreateIDFPage();
begin
  IDFPage := ChoicePageCreate(
    wpLicense,
    'Download or use ESP-IDF', 'Please choose ESP-IDF version to download, or use an existing ESP-IDF copy',
    'Available ESP-IDF versions',
    'Choose existing ESP-IDF directory',
    True,
    @OnIDFPagePrepare,
    @OnIDFSelectionChange,
    @OnIDFPageValidate);
end;

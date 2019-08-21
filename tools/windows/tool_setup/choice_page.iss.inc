var
  ChoicePagePrepare: array of TNotifyEvent;
  ChoicePageSelectionChange: array of TNotifyEvent;
  ChoicePageValidate: array of TWizardPageButtonEvent;
  ChoicePageMaxTag: Integer;
  ChoicePages: array of TInputOptionWizardPage;

procedure ChoicePageOnClickCheck(Sender: TObject);
var
  ListBox: TNewCheckListBox;
  Id: Integer;
begin
  ListBox := TNewCheckListBox(Sender);
  Id := Integer(ListBox.Tag);
  ChoicePageSelectionChange[Id](ChoicePages[Id]);
end;

function ChoicePageGetInput(Page: TInputOptionWizardPage): TNewEdit;
begin
  Result := TNewEdit(Page.FindComponent('ChoicePageInput'));
end;

function ChoicePageGetLabel(Page: TInputOptionWizardPage): TNewStaticText;
begin
  Result := TNewStaticText(Page.FindComponent('ChoicePageLabel'));
end;

function ChoicePageGetButton(Page: TInputOptionWizardPage): TNewButton;
begin
  Result := TNewButton(Page.FindComponent('ChoicePageBrowseButton'));
end;

procedure ChoicePageSetEditLabel(Page: TInputOptionWizardPage; Caption: String);
var
  InputLabel: TNewStaticText;
begin
  InputLabel := ChoicePageGetLabel(Page);
  InputLabel.Caption := Caption;
end;

function ChoicePageGetInputText(Page: TInputOptionWizardPage): String;
begin
  Result := ChoicePageGetInput(Page).Text;
end;

procedure ChoicePageSetInputText(Page: TInputOptionWizardPage; Text: String);
begin
  ChoicePageGetInput(Page).Text := Text;
end;

procedure ChoicePageSetInputEnabled(Page: TInputOptionWizardPage; Enabled: Boolean);
begin
  ChoicePageGetLabel(Page).Enabled := Enabled;
  ChoicePageGetInput(Page).Enabled := Enabled;
  ChoicePageGetButton(Page).Enabled := Enabled;
end;


procedure ChoicePageOnBrowseButtonClick(Sender: TObject);
var
  Button: TNewButton;
  Page: TInputOptionWizardPage;
  InputLabel: TNewStaticText;
  Input: TNewEdit;
  Dir: String;
begin
  Button := TNewButton(Sender);
  Page := TInputOptionWizardPage(Button.Owner);
  Input := ChoicePageGetInput(Page);
  InputLabel := ChoicePageGetLabel(Page);
  Dir := Input.Text;
  if BrowseForFolder(InputLabel.Caption, Dir, True) then
  begin
    Input.Text := Dir;
  end;
end;

<event('CurPageChanged')>
procedure ChoicePageOnCurPageChanged(CurPageID: Integer);
var
  i: Integer;
begin
  for i := 1 to ChoicePageMaxTag do
  begin
    if ChoicePages[i].ID = CurPageID then
    begin
      ChoicePagePrepare[i](ChoicePages[i]);
      break;
    end;
  end;
end;

<event('NextButtonClick')>
function ChoicePageOnNextButtonClick(CurPageID: Integer): Boolean;
var
  i: Integer;
begin
  Result := True;
  for i := 1 to ChoicePageMaxTag do
  begin
    if ChoicePages[i].ID = CurPageID then
    begin
      Result := ChoicePageValidate[i](ChoicePages[i]);
      break;
    end;
  end;
end;

<event('InitializeWizard')>
procedure InitChoicePages();
begin
  ChoicePages := [ ];
  ChoicePagePrepare := [ ];
  ChoicePageSelectionChange := [ ];
  ChoicePageValidate := [ ];
end;

function FindLinkInText(Text: String): String;
var
  Tmp: String;
  LinkStartPos, LinkEndPos: Integer;
begin
  Result := '';
  Tmp := Text;
  LinkStartPos := Pos('https://', Tmp);
  if LinkStartPos = 0 then exit;
  Delete(Tmp, 1, LinkStartPos - 1);

  { Try to find the end of the link }
  LinkEndPos := 0
  if LinkEndPos = 0 then LinkEndPos := Pos(' ', Tmp);
  if LinkEndPos = 0 then LinkEndPos := Pos(',', Tmp);
  if LinkEndPos = 0 then LinkEndPos := Pos('.', Tmp);
  if LinkEndPos = 0 then LinkEndPos := Length(Tmp);
  Delete(Text, LinkEndPos, Length(Tmp));

  Log('Found link in "' + Text + '": "' + Tmp + '"');
  Result := Tmp;
end;

procedure OnStaticTextClick(Sender: TObject);
var
  StaticText: TNewStaticText;
  Link: String;
  Err: Integer;
begin
  StaticText := TNewStaticText(Sender);
  Link := FindLinkInText(StaticText.Caption);
  if Link = '' then
    exit;

  ShellExec('open', Link, '', '', SW_SHOWNORMAL, ewNoWait, Err);
end;

procedure MakeStaticTextClickable(StaticText: TNewStaticText);
begin
  if FindLinkInText(StaticText.Caption) = '' then
    exit;

  StaticText.OnClick := @OnStaticTextClick;
  StaticText.Cursor := crHand;
end;

function ChoicePageCreate(
          const AfterID: Integer;
          const Caption, Description, SubCaption, EditCaption: String;
          HasDirectoryChooser: Boolean;
          Prepare: TNotifyEvent;
          SelectionChange: TNotifyEvent;
          Validate: TWizardPageButtonEvent): TInputOptionWizardPage;
var
  VSpace, Y : Integer;
  ChoicePage: TInputOptionWizardPage;
  InputLabel: TNewStaticText;
  Input: TNewEdit;
  Button: TNewButton;

begin
  ChoicePageMaxTag := ChoicePageMaxTag + 1;
  VSpace := ScaleY(8);
  ChoicePage := CreateInputOptionPage(AfterID, Caption,
                  Description, SubCaption, True, True);

  MakeStaticTextClickable(ChoicePage.SubCaptionLabel);

  ChoicePage.Tag := ChoicePageMaxTag;
  ChoicePage.CheckListBox.OnClickCheck := @ChoicePageOnClickCheck;
  ChoicePage.CheckListBox.Tag := ChoicePageMaxTag;

  if HasDirectoryChooser then
  begin
    ChoicePage.CheckListBox.Anchors := [ akLeft, akTop, akRight ];
    ChoicePage.CheckListBox.Height := ChoicePage.CheckListBox.Height - ScaleY(60);
    Y := ChoicePage.CheckListBox.Top + ChoicePage.CheckListBox.Height + VSpace;

    InputLabel := TNewStaticText.Create(ChoicePage);
    with InputLabel do
    begin
      Top := Y;
      Anchors := [akTop, akLeft, akRight];
      Caption := EditCaption;
      AutoSize := True;
      Parent := ChoicePage.Surface;
      Name := 'ChoicePageLabel';
    end;
    MakeStaticTextClickable(InputLabel);
    Y := Y + InputLabel.Height + VSpace;

    Input := TNewEdit.Create(ChoicePage);
    with Input do
    begin
      Top := Y;
      Anchors := [akTop, akLeft, akRight];
      Parent := ChoicePage.Surface;
      Name := 'ChoicePageInput';
      Text := '';
    end;

    Button := TNewButton.Create(ChoicePage);
    with Button do
    begin
      Anchors := [akTop, akRight];
      Parent := ChoicePage.Surface;
      Width := WizardForm.NextButton.Width;
      Height := WizardForm.NextButton.Height;
      Top := Y - (Height - Input.Height) / 2;
      Left := ChoicePage.SurfaceWidth - Button.Width;
      Name := 'ChoicePageBrowseButton';
      Caption := SetupMessage(msgButtonWizardBrowse);
      OnClick := @ChoicePageOnBrowseButtonClick;
    end;

    Input.Width := Button.Left - ScaleX(8);
  end;

  SetArrayLength(ChoicePages, ChoicePageMaxTag+1);
  SetArrayLength(ChoicePagePrepare, ChoicePageMaxTag+1);
  SetArrayLength(ChoicePageSelectionChange, ChoicePageMaxTag+1);
  SetArrayLength(ChoicePageValidate, ChoicePageMaxTag+1);

  ChoicePages[ChoicePageMaxTag] := ChoicePage;
  ChoicePagePrepare[ChoicePageMaxTag] := Prepare;
  ChoicePageSelectionChange[ChoicePageMaxTag] := SelectionChange;
  ChoicePageValidate[ChoicePageMaxTag] := Validate;

  Result := ChoicePage;
end;

{ Copyright 2019 Espressif Systems (Shanghai) PTE LTD
  SPDX-License-Identifier: Apache-2.0 }

{ ------------------------------ Progress & log page for command line tools ------------------------------ }

var
  CmdlineInstallCancel: Boolean;

{ ------------------------------ Splitting strings into lines and adding them to TStrings ------------------------------ }

procedure StringsAddLine(Dest: TStrings; Line: String; var ReplaceLastLine: Boolean);
begin
  if ReplaceLastLine then
  begin
    Dest.Strings[Dest.Count - 1] := Line;
    ReplaceLastLine := False;
  end else begin
    Dest.Add(Line);
  end;
end;

procedure StrSplitAppendToList(Text: String; Dest: TStrings; var LastLine: String);
var
  pCR, pLF, Len: Integer;
  Tmp: String;
  ReplaceLastLine: Boolean;
begin
  if Length(LastLine) > 0 then
  begin
    ReplaceLastLine := True;
    Text := LastLine + Text;
  end;
  repeat
    Len := Length(Text);
    pLF := Pos(#10, Text);
    pCR := Pos(#13, Text);
    if (pLF > 0) and ((pCR = 0) or (pLF < pCR) or (pLF = pCR + 1)) then
    begin
      if pLF < pCR then
        Tmp := Copy(Text, 1, pLF - 1)
      else
        Tmp := Copy(Text, 1, pLF - 2);
      StringsAddLine(Dest, Tmp, ReplaceLastLine);
      Text := Copy(Text, pLF + 1, Len)
    end else begin
      if (pCR = Len) or (pCR = 0) then
      begin
        break;
      end;
      Text := Copy(Text, pCR + 1, Len)
    end;
  until (pLF = 0) and (pCR = 0);

  LastLine := Text;
  if pCR = Len then
  begin
    Text := Copy(Text, 1, pCR - 1);
  end;
  if Length(LastLine) > 0 then
  begin
    StringsAddLine(Dest, Text, ReplaceLastLine);
  end;

end;

{ ------------------------------ The actual command line install page ------------------------------ }

procedure OnCmdlineInstallCancel(Sender: TObject);
begin
  CmdlineInstallCancel := True;
end;

function DoCmdlineInstall(caption, description, command: String): Boolean;
var
  CmdlineInstallPage: TOutputProgressWizardPage;
  Res: Integer;
  Handle: Longword;
  ExitCode: Integer;
  LogTextAnsi: AnsiString;
  LogText, LeftOver: String;
  Memo: TNewMemo;
  PrevCancelButtonOnClick: TNotifyEvent;

begin
  CmdlineInstallPage := CreateOutputProgressPage('', '')
  CmdlineInstallPage.Caption := caption;
  CmdlineInstallPage.Description := description;

  Memo := TNewMemo.Create(CmdlineInstallPage);
  Memo.Top := CmdlineInstallPage.ProgressBar.Top + CmdlineInstallPage.ProgressBar.Height + ScaleY(8);
  Memo.Width := CmdlineInstallPage.SurfaceWidth;
  Memo.Height := ScaleY(120);
  Memo.ScrollBars := ssVertical;
  Memo.Parent := CmdlineInstallPage.Surface;
  Memo.Lines.Clear();

  CmdlineInstallPage.Show();

  try
    WizardForm.CancelButton.Visible := True;
    WizardForm.CancelButton.Enabled := True;
    PrevCancelButtonOnClick := WizardForm.CancelButton.OnClick;
    WizardForm.CancelButton.OnClick := @OnCmdlineInstallCancel;

    CmdlineInstallPage.SetProgress(0, 100);
    CmdlineInstallPage.ProgressBar.Style := npbstMarquee;

    ExitCode := -1;
    Memo.Lines.Append('Running command: ' + command);
    Handle := ProcStart(command, ExpandConstant('{tmp}'))
    if Handle = 0 then
    begin
      Log('ProcStart failed');
      ExitCode := -2;
    end;
    while (ExitCode = -1) and not CmdlineInstallCancel do
    begin
      ExitCode := ProcGetExitCode(Handle);
      SetLength(LogTextAnsi, 4096);
      Res := ProcGetOutput(Handle, LogTextAnsi, 4096)
      if Res > 0 then
      begin
        SetLength(LogTextAnsi, Res);
        LogText := LeftOver + String(LogTextAnsi);
        StrSplitAppendToList(LogText, Memo.Lines, LeftOver);
      end;
      CmdlineInstallPage.SetProgress(0, 100);
      Sleep(10);
    end;
    ProcEnd(Handle);
  finally
    Log('Done, exit code=' + IntToStr(ExitCode));
    Log('--------');
    Log(Memo.Lines.Text);
    Log('--------');
    if CmdlineInstallCancel then
    begin
      MsgBox('Installation has been cancelled.', mbError, MB_OK);
      Result := False;
    end else if ExitCode <> 0 then
    begin
      MsgBox('Installation has failed with exit code ' + IntToStr(ExitCode), mbError, MB_OK);
      Result := False;
    end else begin
      Result := True;
    end;
    CmdlineInstallPage.Hide;
    CmdlineInstallPage.Free;
    WizardForm.CancelButton.OnClick := PrevCancelButtonOnClick;
  end;
  if not Result then
    RaiseException('Installation has failed at step: ' + caption);
end;


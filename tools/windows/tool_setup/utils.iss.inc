{ Copyright 2019 Espressif Systems (Shanghai) PTE LTD
  SPDX-License-Identifier: Apache-2.0 }

{ ------------------------------ Helper functions from libcmdlinerunner.dll ------------------------------ }

function ProcStart(cmdline, workdir: string): Longword;
  external 'proc_start@files:cmdlinerunner.dll cdecl';

function ProcGetExitCode(inst: Longword): DWORD;
  external 'proc_get_exit_code@files:cmdlinerunner.dll cdecl';

function ProcGetOutput(inst: Longword; dest: PAnsiChar; sz: DWORD): DWORD;
  external 'proc_get_output@files:cmdlinerunner.dll cdecl';

procedure ProcEnd(inst: Longword);
  external 'proc_end@files:cmdlinerunner.dll cdecl';

{ ------------------------------ WinAPI functions ------------------------------ }

#ifdef UNICODE
  #define AW "W"
#else
  #define AW "A"
#endif

function SetEnvironmentVariable(lpName: string; lpValue: string): BOOL;
  external 'SetEnvironmentVariable{#AW}@kernel32.dll stdcall';

{ ------------------------------ Functions to query the registry ------------------------------ }

{ Utility to search in HKLM and HKCU for an installation path. Looks in both 64-bit & 32-bit registry. }
function GetInstallPath(key, valuename : String) : String;
var
  value: String;
begin
  Result := '';
  if RegQueryStringValue(HKEY_LOCAL_MACHINE, key, valuename, value) then
  begin
    Result := value;
    exit;
  end;

  if RegQueryStringValue(HKEY_CURRENT_USER, key, valuename, value) then
  begin
    Result := value;
    exit;
  end;

  { This is 32-bit setup running on 64-bit Windows, but ESP-IDF can use 64-bit tools also }
  if IsWin64 and RegQueryStringValue(HKLM64, key, valuename, value) then
  begin
    Result := value;
    exit;
  end;

  if IsWin64 and RegQueryStringValue(HKCU64, key, valuename, value) then
  begin
    Result := value;
    exit;
  end;
end;

{ ------------------------------ Function to exit from the installer ------------------------------ }

procedure AbortInstallation(Message: String);
begin
  MsgBox(Message, mbError, MB_OK);
  Abort();
end;

{ ------------------------------ File system related functions ------------------------------ }

function DirIsEmpty(DirName: String): Boolean;
var
  FindRec: TFindRec;
begin
  Result := True;
  if FindFirst(DirName+'\*', FindRec) then begin
    try
      repeat
        if (FindRec.Name <> '.') and (FindRec.Name <> '..') then begin
          Result := False;
          break;
        end;
      until not FindNext(FindRec);
    finally
      FindClose(FindRec);
    end;
  end;
end;

type
    TFindFileCallback = procedure(Filename: String);

procedure FindFileRecusive(Directory: string; FileName: string; Callback: TFindFileCallback);
var
  FindRec: TFindRec;
  FilePath: string;
begin
  if FindFirst(Directory + '\*', FindRec) then
  begin
    try
      repeat
        if (FindRec.Name = '.') or (FindRec.Name = '..') then
          continue;

        FilePath := Directory + '\' + FindRec.Name;
        if FindRec.Attributes and FILE_ATTRIBUTE_DIRECTORY <> 0 then
        begin
          FindFileRecusive(FilePath, FileName, Callback);
        end else if CompareText(FindRec.Name, FileName) = 0 then
        begin
          Callback(FilePath);
        end;
      until not FindNext(FindRec);
    finally
      FindClose(FindRec);
    end;
  end;
end;

{ ------------------------------ Version related functions ------------------------------ }

function VersionExtractMajorMinor(Version: String; var Major: Integer; var Minor: Integer): Boolean;
var
  Delim: Integer;
  MajorStr, MinorStr: String;
  OrigVersion, ExpectedPrefix: String;
begin
  Result := False;
  OrigVersion := Version;
  Delim := Pos('.', Version);
  if Delim = 0 then exit;

  MajorStr := Version;
  Delete(MajorStr, Delim, Length(MajorStr));
  Delete(Version, 1, Delim);
  Major := StrToInt(MajorStr);

  Delim := Pos('.', Version);
  if Delim = 0 then Delim := Length(MinorStr);

  MinorStr := Version;
  Delete(MinorStr, Delim, Length(MinorStr));
  Delete(Version, 1, Delim);
  Minor := StrToInt(MinorStr);
  
  { Sanity check }
  ExpectedPrefix := IntToStr(Major) + '.' + IntToStr(Minor);
  if Pos(ExpectedPrefix, OrigVersion) <> 1 then
  begin
    Log('VersionExtractMajorMinor: version=' + OrigVersion + ', expected=' + ExpectedPrefix);
    exit;
  end;

  Result := True;
end;

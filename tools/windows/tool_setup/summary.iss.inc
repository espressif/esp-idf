{ Copyright 2019 Espressif Systems (Shanghai) PTE LTD
  SPDX-License-Identifier: Apache-2.0 }

{ ------------------------------ Installation summary page ------------------------------ }

function UpdateReadyMemo(Space, NewLine, MemoUserInfoInfo, MemoDirInfo,
  MemoTypeInfo, MemoComponentsInfo, MemoGroupInfo, MemoTasksInfo: String): String;
begin
  Result := ''

  if PythonUseExisting then
  begin
    Result := Result + 'Using Python ' + PythonVersion + ':' + NewLine
              + Space + PythonExecutablePath + NewLine + NewLine;
  end else begin
    Result := Result + 'Will download and install Python ' + PythonVersion + NewLine + NewLine;
  end;

  if GitUseExisting then
  begin
    Result := Result + 'Using Git ' + GitVersion + ':' + NewLine
              + Space + GitExecutablePath + NewLine + NewLine;
  end else begin
    Result := Result + 'Will download and install Git for Windows ' + GitVersion + NewLine + NewLine;
  end;

  if IDFUseExisting then
  begin
    Result := Result + 'Using existing ESP-IDF copy: ' + NewLine
              + Space + IDFExistingPath + NewLine + NewLine;
  end else begin
    Result := Result + 'Will download ESP-IDF ' + IDFDownloadVersion + ' into:' + NewLine
              + Space + IDFDownloadPath + NewLine + NewLine;
  end;

  Result := Result + 'IDF tools directory (IDF_TOOLS_PATH):' + NewLine +
            Space + ExpandConstant('{app}') + NewLine + NewLine;

  Log('Summary message: ' + NewLine + Result);
end;

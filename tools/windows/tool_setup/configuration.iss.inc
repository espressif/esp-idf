{ Copyright 2019-2021 Espressif Systems (Shanghai) CO LTD
  SPDX-License-Identifier: Apache-2.0 }

{ ------------------------------ Load configuration of the installer ------------------------------ }

var
    ConfigurationFile: String;
    GitRepository: String;
    IsGitRecursive: Boolean;
    IsGitResetAllowed: Boolean;
    IsGitCleanAllowed: Boolean;
    IsPythonNoUserSite: Boolean;
    IsOfflineMode: Boolean;
    IDFDirectory: String;
    IDFVersion: String;
    IDFVersionUrl: String;
    PythonWheelsUrl: String;
    PythonWheelsVersion: String;
    SkipSystemCheck: Boolean;
    UseEmbeddedPython: Boolean;

function GetConfigurationString(Key: String; Default: String):String;
var Value: String;
begin
    Value := GetIniString('DEFAULT', Key, Default, ConfigurationFile);
    Value := ExpandConstant('{param:' + Key + '|' + Value + '}');
    Log('Configuration /' + Key + '=' + Value);
    Result := Value;
end;

function GetConfigurationBoolean(Key: String; DefaultString: String):Boolean;
begin
    Result := (GetConfigurationString(Key, DefaultString) = 'yes');
end;

{ Initialize configuration of the installer. }
{ Default configuration is encoded in installer. }
{ The configuration can be changed by configuration.ini file. }
{ The configuration can be changed by command line options which have highest priority. }
<event('InitializeWizard')>
procedure InitializeConfiguration();
begin
    ConfigurationFile := ExpandConstant('{param:CONFIG|}');

    if (ConfigurationFile <> '') then begin
        if (not FileExists(ConfigurationFile)) then begin
            Log('Configuration file does not exist, using default values.');
        end;
    end;

    Log('Configuration /CONFIG=' + ConfigurationFile);

    IsGitCleanAllowed := GetConfigurationBoolean('GITCLEAN', 'yes');
    IsGitRecursive := GetConfigurationBoolean('GITRECURSIVE', 'yes');
    IsGitResetAllowed := GetConfigurationBoolean('GITRESET', 'yes');
    GitRepository := GetConfigurationString('GITREPO', 'https://github.com/espressif/esp-idf.git');
    IDFDirectory := GetConfigurationString('IDFDIR', '');
    IDFVersion := GetConfigurationString('IDFVERSION', '');
    IDFVersionUrl := GetConfigurationString('IDFVERSIONSURL', 'https://dl.espressif.com/dl/esp-idf/idf_versions.txt');
    IsOfflineMode := GetConfigurationBoolean('OFFLINE', '{#OFFLINE}');
    IsPythonNoUserSite := GetConfigurationBoolean('PYTHONNOUSERSITE', 'yes');
    PythonWheelsUrl := GetConfigurationString('PYTHONWHEELSURL', 'https://dl.espressif.com/pypi');
    PythonWheelsVersion := GetConfigurationString('PYTHONWHEELSVERSION', '{#PYTHONWHEELSVERSION}');
    SkipSystemCheck := GetConfigurationBoolean('SKIPSYSTEMCHECK', 'no');
    UseEmbeddedPython := GetConfigurationBoolean('USEEMBEDDEDPYTHON', 'yes');
end;


{ Required to display option for installation configuration. }
function IsOnlineMode():Boolean;
begin
    Result := not IsOfflineMode;
end;

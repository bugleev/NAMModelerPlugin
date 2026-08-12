[Setup]
AppName=NAMampPlugin
AppContact=spam@me.com
AppCopyright=Copyright (C) 2026 bugleev
AppPublisher=bugleev
AppPublisherURL=https://github.com/bugleev/NAMModelerPlugin
AppSupportURL=https://github.com/bugleev/NAMModelerPlugin
AppVersion=0.10.0
VersionInfoVersion=0.10.0
DefaultDirName={pf}\bugleev\NAMampPlugin
DefaultGroupName=bugleev\NAMampPlugin
Compression=lzma2
SolidCompression=yes
OutputDir=.\..\build-win\installer
ArchitecturesInstallIn64BitMode=x64
OutputBaseFilename=NAMampPlugin Installer
LicenseFile=license.rtf
SetupLogging=yes
ShowComponentSizes=no

[Types]
Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Messages]
WelcomeLabel1=Welcome to the NAMampPlugin installer
SetupWindowTitle=NAMampPlugin installer
SelectDirLabel3=The standalone application and supporting files will be installed in the following folder.
SelectDirBrowseLabel=To continue, click Next. If you would like to select a different folder (not recommended), click Browse.

[Components]
Name: "app"; Description: "Standalone application (.exe)"; Types: full custom;
Name: "vst3_64"; Description: "64-bit VST3 Plugin (.vst3)"; Types: full custom; Check: Is64BitInstallMode;
Name: "manual"; Description: "User guide"; Types: full custom; Flags: fixed

[Dirs]
Name: "{cf64}\VST3\NAMampPlugin.vst3\"; Attribs: readonly; Check: Is64BitInstallMode; Components:vst3_64;

[Files]
Source: "..\build-win\NAMampPlugin_x64.exe"; DestDir: "{app}"; Check: Is64BitInstallMode; Components:app; Flags: ignoreversion;
Source: "ThirdPartyNotices.txt"; DestDir: "{app}"; Components:app; Flags: ignoreversion;

Source: "..\build-win\NAMampPlugin.vst3\*.*"; Excludes: "\Contents\x86-win\*,*.pdb,*.exp,*.lib,*.ilk,*.ico,*.ini"; DestDir: "{cf64}\VST3\NAMampPlugin.vst3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: ignoreversion recursesubdirs;
Source: "..\build-win\NAMampPlugin.vst3\Desktop.ini"; DestDir: "{cf64}\VST3\NAMampPlugin.vst3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
Source: "..\build-win\NAMampPlugin.vst3\PlugIn.ico"; DestDir: "{cf64}\VST3\NAMampPlugin.vst3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
Source: "ThirdPartyNotices.txt"; DestDir: "{cf64}\VST3\NAMampPlugin.vst3\Contents\Resources"; Check: Is64BitInstallMode; Components:vst3_64; Flags: ignoreversion;

Source: "..\manual\NeuralAmpModeler manual.pdf"; DestDir: "{app}"; DestName: "NAMampPlugin manual.pdf"
Source: "changelog.txt"; DestDir: "{app}"
Source: "readme-win.rtf"; DestDir: "{app}"; DestName: "readme.rtf"; Flags: isreadme

[Icons]
Name: "{group}\NAMampPlugin"; Filename: "{app}\NAMampPlugin_x64.exe"
Name: "{group}\User guide"; Filename: "{app}\NAMampPlugin manual.pdf"
Name: "{group}\Changelog"; Filename: "{app}\changelog.txt"
Name: "{group}\Uninstall NAMampPlugin"; Filename: "{app}\unins000.exe"

[Code]
var
  OkToCopyLog : Boolean;

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssDone then
    OkToCopyLog := True;
end;

procedure DeinitializeSetup();
begin
  if OkToCopyLog then
    FileCopy (ExpandConstant ('{log}'), ExpandConstant ('{app}\InstallationLogFile.log'), FALSE);
  RestartReplace (ExpandConstant ('{log}'), '');
end;

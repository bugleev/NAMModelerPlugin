[Setup]
AppName=Gateway Modded
AppContact=spam@me.com
AppCopyright=Copyright (C) 2026 NAMenjoyer
AppPublisher=NAMenjoyer
AppPublisherURL=https://github.com/bugleev/NAMModelerPlugin
AppSupportURL=https://github.com/bugleev/NAMModelerPlugin
AppVersion=0.10.1
VersionInfoVersion=0.10.1
DefaultDirName={pf}\NAMenjoyer\Gateway Modded
DefaultGroupName=NAMenjoyer\Gateway Modded
Compression=lzma2
SolidCompression=yes
OutputDir=.\..\build-win\installer
ArchitecturesInstallIn64BitMode=x64
OutputBaseFilename=Gateway Modded Installer
LicenseFile=license.rtf
SetupLogging=yes
ShowComponentSizes=no

[Types]
Name: "full"; Description: "Full installation"
Name: "custom"; Description: "Custom installation"; Flags: iscustom

[Messages]
WelcomeLabel1=Welcome to the Gateway Modded installer
SetupWindowTitle=Gateway Modded installer
SelectDirLabel3=The standalone application and supporting files will be installed in the following folder.
SelectDirBrowseLabel=To continue, click Next. If you would like to select a different folder (not recommended), click Browse.

[Components]
Name: "app"; Description: "Standalone application (.exe)"; Types: full custom;
Name: "vst3_64"; Description: "64-bit VST3 Plugin (.vst3)"; Types: full custom; Check: Is64BitInstallMode;
Name: "manual"; Description: "User guide"; Types: full custom; Flags: fixed

[Dirs]
Name: "{cf64}\VST3\GatewayModded.vst3\"; Attribs: readonly; Check: Is64BitInstallMode; Components:vst3_64;

[Files]
Source: "..\build-win\GatewayModded_x64.exe"; DestDir: "{app}"; Check: Is64BitInstallMode; Components:app; Flags: ignoreversion;
Source: "ThirdPartyNotices.txt"; DestDir: "{app}"; Components:app; Flags: ignoreversion;

Source: "..\build-win\GatewayModded.vst3\*.*"; Excludes: "\Contents\x86-win\*,*.pdb,*.exp,*.lib,*.ilk,*.ico,*.ini"; DestDir: "{cf64}\VST3\GatewayModded.vst3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: ignoreversion recursesubdirs;
Source: "..\build-win\GatewayModded.vst3\Desktop.ini"; DestDir: "{cf64}\VST3\GatewayModded.vst3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
Source: "..\build-win\GatewayModded.vst3\PlugIn.ico"; DestDir: "{cf64}\VST3\GatewayModded.vst3\"; Check: Is64BitInstallMode; Components:vst3_64; Flags: overwritereadonly ignoreversion; Attribs: hidden system;
Source: "ThirdPartyNotices.txt"; DestDir: "{cf64}\VST3\GatewayModded.vst3\Contents\Resources"; Check: Is64BitInstallMode; Components:vst3_64; Flags: ignoreversion;

Source: "..\manual\NeuralAmpModeler manual.pdf"; DestDir: "{app}"; DestName: "Gateway Modded manual.pdf"
Source: "changelog.txt"; DestDir: "{app}"
Source: "readme-win.rtf"; DestDir: "{app}"; DestName: "readme.rtf"; Flags: isreadme

[Icons]
Name: "{group}\Gateway Modded"; Filename: "{app}\GatewayModded_x64.exe"
Name: "{group}\User guide"; Filename: "{app}\Gateway Modded manual.pdf"
Name: "{group}\Changelog"; Filename: "{app}\changelog.txt"
Name: "{group}\Uninstall Gateway Modded"; Filename: "{app}\unins000.exe"

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

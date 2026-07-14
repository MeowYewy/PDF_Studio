; ProjectO — Windows installer (Inno Setup 6)
; Example:
; ISCC /DAppVersion=0.1.0 /DSourceDir=dist\ProjectO_0.1.0_win64 /DOutputDir=dist\artifacts packaging\windows\ProjectO.iss

#ifndef AppVersion
  #define AppVersion "0.1.0"
#endif
#ifndef SourceDir
  #define SourceDir "..\..\dist\ProjectO_" + AppVersion + "_win64"
#endif
#ifndef OutputDir
  #define OutputDir "..\..\dist\artifacts"
#endif

#define MyAppName "ProjectO"
#define MyAppExe "ProjectO.exe"
#define MyAppPublisher "TechG / MeowYewy"

[Setup]
AppId={{A7C2E9F1-3B4D-4E5F-9A10-PROJECTO0001}
AppName={#MyAppName}
AppVersion={#AppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL=https://github.com/MeowYewy/ProjectO
AppSupportURL=https://github.com/MeowYewy/ProjectO/issues
AppUpdatesURL=https://github.com/MeowYewy/ProjectO/releases
DefaultDirName={autopf}\ProjectO
DefaultGroupName=ProjectO
DisableProgramGroupPage=yes
OutputDir={#OutputDir}
OutputBaseFilename=ProjectO_{#AppVersion}_win64_Setup
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
ArchitecturesInstallIn64BitMode=x64compatible
UninstallDisplayIcon={app}\{#MyAppExe}
UninstallDisplayName=ProjectO {#AppVersion}
PrivilegesRequired=lowest
CloseApplications=yes
CloseApplicationsFilter=ProjectO.exe

[Languages]
Name: "chinesesimplified"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"

[Files]
Source: "{#SourceDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\ProjectO"; Filename: "{app}\{#MyAppExe}"; IconFilename: "{app}\{#MyAppExe}"
Name: "{group}\{cm:UninstallProgram,ProjectO}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\ProjectO"; Filename: "{app}\{#MyAppExe}"; IconFilename: "{app}\{#MyAppExe}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#MyAppExe}"; Description: "{cm:LaunchProgram,ProjectO}"; Flags: nowait postinstall skipifsilent

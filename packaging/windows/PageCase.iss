; PageCase — Windows installer (Inno Setup 6)
; Build: scripts\package-installer.bat
; Or: ISCC /DAppVersion=0.1.0 /DSourceDir=dist\PageCase_0.1.0_win64 /DOutputDir=dist\artifacts packaging\windows\PageCase.iss

#ifndef AppVersion
  #define AppVersion "0.2.0"
#endif

#ifndef SourceDir
  #define SourceDir "..\..\dist\PageCase_" + AppVersion + "_win64"
#endif

#ifndef OutputDir
  #define OutputDir "..\..\dist\artifacts"
#endif

#ifndef AppIconFile
  #define AppIconFile "..\..\resources\app-icon.ico"
#endif

[Setup]
AppId={{A1B2C3D4-E5F6-7890-ABCD-EF1234567890}
AppName=PageCase
AppVersion={#AppVersion}
AppPublisher=MeowYewy
AppPublisherURL=https://github.com/MeowYewy/PageCase
AppSupportURL=https://github.com/MeowYewy/PageCase/issues
AppUpdatesURL=https://github.com/MeowYewy/PageCase/releases
DefaultDirName={autopf}\PageCase
DefaultGroupName=PageCase
DisableProgramGroupPage=yes
OutputDir={#OutputDir}
OutputBaseFilename=PageCase_{#AppVersion}_win64_Setup
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
UninstallDisplayName=PageCase {#AppVersion}
SetupIconFile={#AppIconFile}
UninstallDisplayIcon={#AppIconFile}
LicenseFile={#SourceDir}\LICENSE.txt
; Force-close both new and legacy process names during upgrade from v0.1.0.
CloseApplications=force
CloseApplicationsFilter=PageCase.exe,PDFStudio.exe,ProjectP.exe
RestartApplications=yes
; New builds use PageCaseAppMutex; legacy 0.1.0 used PDFStudioAppMutex — AppId
; continuity still upgrades the same install directory.
AppMutex=PageCaseAppMutex

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "chinesesimplified"; MessagesFile: "languages\ChineseSimplified.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#SourceDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\PageCase"; Filename: "{app}\PageCase.exe"; IconFilename: "{app}\PageCase.exe"
Name: "{group}\{cm:UninstallProgram,PageCase}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\PageCase"; Filename: "{app}\PageCase.exe"; IconFilename: "{app}\PageCase.exe"; Tasks: desktopicon

[Run]
; postinstall + nowait: relaunch after /SILENT in-app update (do not use skipifsilent).
Filename: "{app}\PageCase.exe"; Description: "{cm:LaunchProgram,PageCase}"; Flags: nowait postinstall

[UninstallDelete]
Type: filesandordirs; Name: "{app}"

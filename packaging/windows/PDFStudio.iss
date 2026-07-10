; PDF Studio — Windows installer (Inno Setup 6)
; Build: scripts\package-installer.bat
; Or: ISCC /DAppVersion=0.1.0 /DSourceDir=dist\PDFStudio-0.1.0-win64 /DOutputDir=dist\artifacts packaging\windows\PDFStudio.iss

#ifndef AppVersion
  #define AppVersion "0.1.0"
#endif

#ifndef SourceDir
  #define SourceDir "..\..\dist\PDFStudio-" + AppVersion + "-win64"
#endif

#ifndef OutputDir
  #define OutputDir "..\..\dist\artifacts"
#endif

#ifndef AppIconFile
  #define AppIconFile "..\..\resources\app-icon.ico"
#endif

[Setup]
AppId={{A1B2C3D4-E5F6-7890-ABCD-EF1234567890}
AppName=PDF Studio
AppVersion={#AppVersion}
AppPublisher=MeowYewy
AppPublisherURL=https://github.com/MeowYewy/PDF_Studio
AppSupportURL=https://github.com/MeowYewy/PDF_Studio/issues
AppUpdatesURL=https://github.com/MeowYewy/PDF_Studio/releases
DefaultDirName={autopf}\PDF Studio
DefaultGroupName=PDF Studio
DisableProgramGroupPage=yes
OutputDir={#OutputDir}
OutputBaseFilename=PDFStudio-{#AppVersion}-win64-Setup
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
UninstallDisplayName=PDF Studio {#AppVersion}
SetupIconFile={#AppIconFile}
UninstallDisplayIcon={#AppIconFile}
LicenseFile={#SourceDir}\LICENSE.txt
CloseApplications=force
CloseApplicationsFilter=ProjectP.exe
RestartApplications=yes
AppMutex=PDFStudioAppMutex

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "chinesesimplified"; MessagesFile: "languages\ChineseSimplified.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#SourceDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[Icons]
Name: "{group}\PDF Studio"; Filename: "{app}\ProjectP.exe"; IconFilename: "{app}\ProjectP.exe"
Name: "{group}\{cm:UninstallProgram,PDF Studio}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\PDF Studio"; Filename: "{app}\ProjectP.exe"; IconFilename: "{app}\ProjectP.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\ProjectP.exe"; Description: "{cm:LaunchProgram,PDF Studio}"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: "{app}"

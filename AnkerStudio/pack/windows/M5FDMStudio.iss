; Inno Setup script for the M5 FDM Studio Windows installer (x64, unsigned).
;
; Built by .github/workflows/windows.yml:
;   iscc /DAppVersion=1.5.26 /DSourceDir=<staged package dir> /O<out dir> M5FDMStudio.iss
;
; SourceDir is the portable package (the same tree that goes into the .zip):
; M5FDMStudio.exe, eufyStudio.dll, AnkerNet.dll, the runtime DLLs, resources\, mesa\.

#ifndef AppVersion
  #define AppVersion "0.0.0"
#endif
#ifndef SourceDir
  #error SourceDir must point at the staged package directory
#endif
#ifndef OutputName
  #define OutputName "M5FDMStudio-windows-x64-setup"
#endif

#define AppName "M5 FDM Studio"
#define AppExe "M5FDMStudio.exe"
#define ViewerExe "M5FDMStudio-gcodeviewer.exe"

[Setup]
; Fixed AppId so upgrades replace the previous install. Never change it.
AppId={{6F0E3C2A-9B1D-4C59-8E5B-5A0D2C7F4E11}
AppName={#AppName}
AppVersion={#AppVersion}
AppVerName={#AppName} {#AppVersion}
AppPublisher=M5 FDM Studio contributors
AppPublisherURL=https://github.com/artur9010/eufyMake-PrusaSlicer-Release-Linux
AppSupportURL=https://github.com/artur9010/eufyMake-PrusaSlicer-Release-Linux/issues
AppUpdatesURL=https://github.com/artur9010/eufyMake-PrusaSlicer-Release-Linux/releases
DefaultDirName={autopf}\M5 FDM Studio
DefaultGroupName={#AppName}
DisableProgramGroupPage=yes
LicenseFile={#SourceDir}\LICENSE.txt
OutputBaseFilename={#OutputName}
SetupIconFile={#SourceDir}\resources\icons\AnkerStudio.ico
UninstallDisplayIcon={app}\{#AppExe}
Compression=lzma2/max
SolidCompression=yes
WizardStyle=modern
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
; Per-user or per-machine install, chosen by the user.
PrivilegesRequired=lowest
PrivilegesRequiredOverridesAllowed=dialog
MinVersion=10.0

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
Source: "{#SourceDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs

[InstallDelete]
; Drop stale files (e.g. FFmpeg DLLs with an older soname) from a previous version.
Type: files; Name: "{app}\*.dll"

[Icons]
Name: "{autoprograms}\{#AppName}"; Filename: "{app}\{#AppExe}"
Name: "{autoprograms}\{#AppName} G-code Viewer"; Filename: "{app}\{#ViewerExe}"
Name: "{autodesktop}\{#AppName}"; Filename: "{app}\{#AppExe}"; Tasks: desktopicon

[Run]
Filename: "{app}\{#AppExe}"; Description: "{cm:LaunchProgram,{#AppName}}"; Flags: nowait postinstall skipifsilent

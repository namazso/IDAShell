; Copyright 2021 namazso <admin@namazso.eu>
; This file is part of IDAShell.
; 
; IDAShell is free software: you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation, either version 3 of the License, or
; (at your option) any later version.
; 
; IDAShell is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with IDAShell.  If not, see <https://www.gnu.org/licenses/>.
#ifndef CI_VERSION
#define CI_VERSION "(unknown)"
#define CI_VERSION_NUMERIC "0.0.0"
#endif

#define MyAppName       "IDAShell"
#define MyAppVersion    CI_VERSION
#define MyAppPublisher  "namazso"
#define MyAppURL        "https://github.com/namazso/IDAShell"
#define MyCopyright     "(c) namazso. Licensed under GNU GPLv3 or (at your option) later."
#define DLLName         "IDAShell.dll"
#define DLLCLSID        "{{19AEAA33-1BA9-4D92-BFE7-066322481069}"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{19AEAA33-1BA9-4D92-BFE7-066322481069}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
AppCopyright={#MyCopyright}
CreateAppDir=yes
DefaultDirName={autopf}\IDAShell
LicenseFile=LICENSE
PrivilegesRequiredOverridesAllowed=commandline dialog
OutputBaseFilename=IDAShell_setup
OutputDir=.
Compression=lzma
SolidCompression=yes
WizardStyle=modern
VersionInfoProductTextVersion={#CI_VERSION}
VersionInfoVersion={#CI_VERSION_NUMERIC}
ChangesAssociations = yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "armenian"; MessagesFile: "compiler:Languages\Armenian.isl"
Name: "brazilianportuguese"; MessagesFile: "compiler:Languages\BrazilianPortuguese.isl"
Name: "catalan"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "corsican"; MessagesFile: "compiler:Languages\Corsican.isl"
Name: "czech"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "danish"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "finnish"; MessagesFile: "compiler:Languages\Finnish.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
Name: "hebrew"; MessagesFile: "compiler:Languages\Hebrew.isl"
Name: "icelandic"; MessagesFile: "compiler:Languages\Icelandic.isl"
Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"
Name: "norwegian"; MessagesFile: "compiler:Languages\Norwegian.isl"
Name: "polish"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "slovak"; MessagesFile: "compiler:Languages\Slovak.isl"
Name: "slovenian"; MessagesFile: "compiler:Languages\Slovenian.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"
Name: "turkish"; MessagesFile: "compiler:Languages\Turkish.isl"
Name: "ukrainian"; MessagesFile: "compiler:Languages\Ukrainian.isl"

[Files]
Source: "bin\Release\Win32\*.dll"; DestDir: "{app}"; Flags: ignoreversion solidbreak restartreplace 32bit; Check: InstallArch('x86')
Source: "bin\Release\x64\*.dll";   DestDir: "{app}"; Flags: ignoreversion solidbreak restartreplace 64bit; Check: InstallArch('x64')
Source: "bin\Release\ARM64\*.dll"; DestDir: "{app}"; Flags: ignoreversion solidbreak restartreplace 64bit; Check: InstallArch('arm64') 

[Tasks]
[Registry]
#ifdef SYSTEM
Root: HKLM32; Subkey: "Software\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved";  ValueName: "{#DLLCLSID}"; ValueData: "{#MyAppName} Shell Extension";  ValueType: string; Check: InstallArch('x86')
Root: HKLM64; Subkey: "Software\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved";  ValueName: "{#DLLCLSID}"; ValueData: "{#MyAppName} Shell Extension";  ValueType: string; Check: InstallArch('x64')
Root: HKLM64; Subkey: "Software\Microsoft\Windows\CurrentVersion\Shell Extensions\Approved";  ValueName: "{#DLLCLSID}"; ValueData: "{#MyAppName} Shell Extension";  ValueType: string; Check: InstallArch('arm64')
#endif

Root: HKA32; Subkey: "Software\Classes\CLSID\{#DLLCLSID}";  ValueName: ""; ValueData: "{#MyAppName} Shell Extension";  ValueType: string; Flags: uninsdeletekey; Check: InstallArch('x86')
Root: HKA32; Subkey: "Software\Classes\CLSID\{#DLLCLSID}\InprocServer32";  ValueName: ""; ValueData: "{app}\{#DLLName}"; ValueType: string; Check: InstallArch('x86')
Root: HKA32; Subkey: "Software\Classes\CLSID\{#DLLCLSID}\InprocServer32";  ValueName: "ThreadingModel"; ValueData: "Apartment"; ValueType: string; Check: InstallArch('x86')

Root: HKA64; Subkey: "Software\Classes\CLSID\{#DLLCLSID}";  ValueName: ""; ValueData: "{#MyAppName} Shell Extension";  ValueType: string; Flags: uninsdeletekey; Check: InstallArch('x64')
Root: HKA64; Subkey: "Software\Classes\CLSID\{#DLLCLSID}\InprocServer32";  ValueName: ""; ValueData: "{app}\{#DLLName}"; ValueType: string; Check: InstallArch('x64')
Root: HKA64; Subkey: "Software\Classes\CLSID\{#DLLCLSID}\InprocServer32";  ValueName: "ThreadingModel"; ValueData: "Apartment"; ValueType: string; Check: InstallArch('x64')

Root: HKA64; Subkey: "Software\Classes\CLSID\{#DLLCLSID}";  ValueName: ""; ValueData: "{#MyAppName} Shell Extension";  ValueType: string; Flags: uninsdeletekey; Check: InstallArch('arm64')
Root: HKA64; Subkey: "Software\Classes\CLSID\{#DLLCLSID}\InprocServer32";  ValueName: ""; ValueData: "{app}\{#DLLName}"; ValueType: string; Check: InstallArch('arm64')
Root: HKA64; Subkey: "Software\Classes\CLSID\{#DLLCLSID}\InprocServer32";  ValueName: "ThreadingModel"; ValueData: "Apartment"; ValueType: string; Check: InstallArch('arm64')

Root: HKA; Subkey: "Software\Classes\AllFilesystemObjects\shellex\ContextMenuHandlers\{#DLLCLSID}";    ValueName: ""; Flags: uninsdeletekey; ValueType: none;

[Code]
function InstallArch(Arch: String): Boolean;
begin
    Result := False;
    case ProcessorArchitecture of
        paX86:    Result := Arch = 'x86';
        paX64:    Result := (Arch = 'x64') or (Arch = 'wow');
        paIA64:   Result := (Arch = 'ia64') or (Arch = 'wow');
        paARM64:  Result := (Arch = 'arm64') or (Arch = 'wow');
    end;
end; 
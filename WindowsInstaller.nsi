; Nightbird Installer Script
; Based on Start Menu Folder Selection Example Written by Joost Verburg

;--------------------------------
; Include Modern UI

  !include "MUI2.nsh"
  !include "LogicLib.nsh"
  !include "WinMessages.nsh"

;--------------------------------
; General

  ; Name and file
  Name "Nightbird Engine"
  OutFile "NightbirdInstaller.exe"
  Unicode True

  ; Default installation folder
  InstallDir "C:\Nightbird"

  ; Request application privileges for Windows Vista and later
  RequestExecutionLevel admin

;--------------------------------
; Variables

  Var StartMenuFolder

;--------------------------------
; Interface Settings

  !define MUI_ABORTWARNING
  !define MUI_ICON "Editor\Assets\Icon.ico"
  !define MUI_UNICON "Editor\Assets\Icon.ico"

;--------------------------------
; Pages

  !insertmacro MUI_PAGE_LICENSE "LICENSE"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY

  ; Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM"
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "SOFTWARE\Nightbird Labs\Nightbird Engine"
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder

  !insertmacro MUI_PAGE_INSTFILES

  !insertmacro MUI_PAGE_FINISH

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;--------------------------------
; Languages

  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
; Installer .onInit
; Prefill $INSTDIR from the environment variable if this is a reinstall/upgrade

Function .onInit
  ReadEnvStr $0 "NIGHTBIRD_PATH"
  ${If} $0 != ""
    StrCpy $INSTDIR $0
  ${EndIf}
FunctionEnd

;--------------------------------
; Installer Sections

Section "Nightbird Engine" SecNightbird
  SetRegView 64
  SetShellVarContext all

  ; Copy the pre-staged, pre-filtered project tree
  SetOutPath "$INSTDIR"
  File /r "Staging\*.*"

  ; Set NIGHTBIRD_PATH as a persistent, system-wide environment variable
  WriteRegExpandStr HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "NIGHTBIRD_PATH" "$INSTDIR"

  ; Notify running processes (Explorer, etc.) that the environment changed
  SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000

  ; Remember settings for reinstall/repair (Start Menu folder, etc.)
  WriteRegStr HKLM "SOFTWARE\Nightbird Labs\Nightbird Engine" "InstallDir" "$INSTDIR"

  ; Create uninstaller
  WriteUninstaller "$INSTDIR\UninstallNightbird.exe"

  ; Add to Add or Remove Programs
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Nightbird" "DisplayName" "Nightbird Engine"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Nightbird" "UninstallString" "$INSTDIR\UninstallNightbird.exe"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Nightbird" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Nightbird" "Publisher" "Nightbird Labs"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Nightbird" "DisplayVersion" "0.1.0"
  WriteRegStr HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Nightbird" "DisplayIcon" "$INSTDIR\Binaries\windows-x86_64\EditorRelease\Editor.exe"
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Nightbird" "NoModify" 1
  WriteRegDWORD HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Nightbird" "NoRepair" 1

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"

    ; Create shortcuts
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Nightbird Editor.lnk" "$INSTDIR\Binaries\windows-x86_64\EditorRelease\Editor.exe"

  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
; Descriptions

  ; Language strings
  LangString DESC_SecNightbird ${LANG_ENGLISH} "Installs Nightbird Engine."

  ; Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecNightbird} $(DESC_SecNightbird)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
; Uninstaller Section

Section "Uninstall"
  SetRegView 64
  SetShellVarContext all

  RMDir /r "$INSTDIR"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
  Delete "$SMPROGRAMS\$StartMenuFolder\Nightbird Editor.lnk"
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir /r "$SMPROGRAMS\$StartMenuFolder"

  ; Remove the environment variable and notify running processes
  DeleteRegValue HKLM "SYSTEM\CurrentControlSet\Control\Session Manager\Environment" "NIGHTBIRD_PATH"
  SendMessage ${HWND_BROADCAST} ${WM_SETTINGCHANGE} 0 "STR:Environment" /TIMEOUT=5000

  DeleteRegKey HKLM "SOFTWARE\Nightbird Labs\Nightbird Engine"
  DeleteRegKey HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\Nightbird"

SectionEnd

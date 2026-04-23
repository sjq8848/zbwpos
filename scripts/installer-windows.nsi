; ZBW POS Windows Installer Script
; Requires NSIS (Nullsoft Scriptable Install System)

!define APP_NAME "ZBW POS"
!define APP_EXE "zbwpos.exe"
!define APP_REGKEY "Software\ZBW\ZBWPOS"
!define APP_UNINSTKEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\ZBW POS"

; Version (override with /DVERSION=x.x.x)
!ifndef VERSION
  !define VERSION "1.0.0"
!endif

Name "${APP_NAME}"
OutFile "..\package\zbwpos-${VERSION}-windows-x64.exe"
InstallDir "$PROGRAMFILES64\ZBW POS"
InstallDirRegKey HKLM "${APP_REGKEY}" "InstallDir"
RequestExecutionLevel admin

; Modern UI
!include "MUI2.nsh"

; UI Settings
!define MUI_ICON "..\resources\icons\app.ico"
!define MUI_UNICON "..\resources\icons\app.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "..\resources\installer\welcome.bmp"

; Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\LICENSE"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; Language
!insertmacro MUI_LANGUAGE "SimpChinese"
!insertmacro MUI_LANGUAGE "English"

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"

  ; Copy all files from install directory
  File /r "..\install\bin\*.*"

  ; Create directories
  CreateDirectory "$INSTDIR\data"
  CreateDirectory "$INSTDIR\logs"

  ; Create shortcuts
  CreateDirectory "$SMPROGRAMS\${APP_NAME}"
  CreateShortCut "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}"
  CreateShortCut "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk" "$INSTDIR\uninstall.exe"
  CreateShortCut "$DESKTOP\${APP_NAME}.lnk" "$INSTDIR\${APP_EXE}"

  ; Register application
  WriteRegStr HKLM "${APP_REGKEY}" "InstallDir" "$INSTDIR"
  WriteRegStr HKLM "${APP_REGKEY}" "Version" "${VERSION}"

  ; Add to uninstall list
  WriteRegStr HKLM "${APP_UNINSTKEY}" "DisplayName" "${APP_NAME}"
  WriteRegStr HKLM "${APP_UNINSTKEY}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
  WriteRegStr HKLM "${APP_UNINSTKEY}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "${APP_UNINSTKEY}" "Publisher" "ZBW Team"
  WriteRegDWORD HKLM "${APP_UNINSTKEY}" "NoModify" 1
  WriteRegDWORD HKLM "${APP_UNINSTKEY}" "NoRepair" 1

  ; Write uninstaller
  WriteUninstaller "$INSTDIR\uninstall.exe"

  ; Calculate installed size
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD HKLM "${APP_UNINSTKEY}" "EstimatedSize" "$0"
SectionEnd

Section "Uninstall"
  ; Delete shortcuts
  Delete "$SMPROGRAMS\${APP_NAME}\${APP_NAME}.lnk"
  Delete "$SMPROGRAMS\${APP_NAME}\Uninstall.lnk"
  RMDir "$SMPROGRAMS\${APP_NAME}"
  Delete "$DESKTOP\${APP_NAME}.lnk"

  ; Delete files
  RMDir /r "$INSTDIR"

  ; Delete registry keys
  DeleteRegKey HKLM "${APP_REGKEY}"
  DeleteRegKey HKLM "${APP_UNINSTKEY}"
SectionEnd

Function .onInit
  ; Check for existing installation
  ReadRegStr $0 HKLM "${APP_REGKEY}" "InstallDir"
  ${If} $0 != ""
    MessageBox MB_YESNO|MB_ICONQUESTION \
      "检测到已安装的 ${APP_NAME}，是否覆盖安装？$\n$\nDetected existing installation. Overwrite?" \
      IDYES +2
    Abort
  ${EndIf}
FunctionEnd

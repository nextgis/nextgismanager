; wxGis
;
; wxgis.nsi - NSIS Script for NSIS 2.46
; Requires the ExecDos plugin (http://nsis.sourceforge.net/wiki/ExecDos)
;
; Copyright (c) 2008-2011 Bishop
;
; This program is free software; you can redistribute it and/or modify
; it under the terms of the GNU General Public License as published by
; the Free Software Foundation; either version 3 of the License, or
; (at your option) any later version.
;
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
;
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

; Use better compression
SetCompressor lzma

; Sections
!include "Sections.nsh"

; Modern User Interface
!include "MUI2.nsh"

;Enclude MUI_EXTRAPAGES header
!include "MUI_EXTRAPAGES.nsh"

;!include "InstallOptions.nsh"
;!include "WordFunc.nsh"

;!insertmacro WordFind

;ReserveFile "${NSISDIR}\Plugins\InstallOptions.dll"

!define GEOS_VERSION "3.2.2"
!define PROJ_VERSION "4.7.1"

!define VERSION "0.3.0"
!define PRODUCT "wxGIS ${VERSION}"

; Use the new WIN32DIST build directory
!define WXGIS_DIR "D:\work\Projects\wxGIS"

; Place all temporary files used by the installer in their own subdirectory
; under $TEMP (makes the files easier to find)
;!define TEMPDIR "$TEMP\wxgis_installer"

; --------------------------------
; Configuration

	; General
	Name "${PRODUCT}"
	OutFile "wxGIS-${VERSION}-setup.exe"

	;Default installation folder
	InstallDir "$PROGRAMFILES\wxgis"
	;Get installation folder from registry if available
	InstallDirRegKey HKLM "Software\wxgis" ""
	
	;Request application privileges for Windows Vista
	RequestExecutionLevel highest
	  
	SetDateSave on
	SetDatablockOptimize on
	CRCCheck on
	XPStyle on
	BrandingText "${PRODUCT}"
	SetOverwrite ifnewer
; --------------------------------
	AutoCloseWindow false
	ShowInstDetails show

; --------------------------------
; Interface Settings

	!define MUI_ABORTWARNING
	!define MUI_COMPONENTSPAGE_CHECKBITMAP "${NSISDIR}\Contrib\Graphics\Checks\colorful.bmp"
	!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\box-install.ico"
	!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\box-uninstall.ico"
	!define MUI_HEADERIMAGE
	!define MUI_HEADERIMAGE_BITMAP "${WXGIS_DIR}\art\logo.bmp"
	!define MUI_HEADERIMAGE_UNBITMAP "${WXGIS_DIR}\art\logo.bmp"
	!define MUI_COMPONENTSPAGE_SMALLDESC
  
; --------------------------------
; Install Pages

	!insertmacro MUI_PAGE_WELCOME
	!insertmacro MUI_PAGE_LICENSE $(myLicenseData)		
	!insertmacro MUI_PAGE_README $(myVerData)	
	!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY
	!insertmacro MUI_PAGE_INSTFILES  
	!insertmacro MUI_PAGE_FINISH 
; --------------------------------
; Uninstall Pages

	!insertmacro MUI_UNPAGE_CONFIRM  
	!insertmacro MUI_UNPAGE_INSTFILES  

; --------------------------------	

; Languages
 
	!insertmacro MUI_LANGUAGE "Russian"
	!insertmacro MUI_LANGUAGE "English"
	
; --------------------------------
  ;Set up install lang strings for 1st lang
  ${ReadmeLanguage} "${LANG_ENGLISH}" \
          "Read Me" \
          "Please review the following important information." \
          "About $(^name):" \
          "$\n  Click on scrollbar arrows or press Page Down to review the entire text."
 
  ;Set up uninstall lang strings for 1st lang
  ${Un.ReadmeLanguage} "${LANG_ENGLISH}" \
          "Change log" \
          "Please review the following important Uninstall information." \
          "About $(^name) Uninstall:" \
          "$\n  Click on scrollbar arrows or press Page Down to review the entire text."
 
  ;set up install lang strings for second lang
  ${ReadmeLanguage} "${LANG_RUSSIAN}" \
          "Список изменений" \
          "Прочитайте внимательно следующую информацию." \
          "О $(^name):" \
          "$\n  Для просмотра всего текста используйте стрелку 'вниз' или клавишу 'Page Down'."
 
  ;set up uninstall lang strings for second lang
  ${Un.ReadmeLanguage} "${LANG_RUSSIAN}" \
          "Список изменений" \
          "Прочитайте внимательно следующую информацию." \
          "Oб удалении $(^name):" \
          "$\n  Для просмотра всего текста используйте стрелку 'вниз' или клавишу 'Page Down'."	

;--------------------------------
; License data
; Not exactly translated, but it shows what's needed
LicenseLangString myLicenseData ${LANG_ENGLISH} "${WXGIS_DIR}\build\release\COPYING_EN"
LicenseLangString myLicenseData ${LANG_RUSSIAN} "${WXGIS_DIR}\build\release\COPYING_RU"

LicenseLangString myVerData ${LANG_ENGLISH} "${WXGIS_DIR}\build\release\ChangeLog_en.txt"
LicenseLangString myVerData ${LANG_RUSSIAN} "${WXGIS_DIR}\build\release\ChangeLog_ru.txt"

;--------------------------------
;Reserve Files
  
  ;These files should be inserted before other files in the data block
  ;Keep these lines before any File command
  ;Only for solid compression (by default, solid compression is enabled for BZIP2 and LZMA)
  
  !insertmacro MUI_RESERVEFILE_LANGDLL

;--------------------------------
;Version Information

  VIProductVersion "${VERSION}.0"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductName" "wxGIS"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "Comments" "wxGIS"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileDescription" "wxGIS"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "FileVersion" "${VERSION}.0"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© 2009-2011 Bishop"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VERSION}.0"
 
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "ProductName" "wxGIS"
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "Comments" "wxGIS"
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "FileDescription" "wxGIS"
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "FileVersion" "${VERSION}.0" 
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "LegalCopyright" "© 2009-2011 Bishop"
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "ProductVersion" "${VERSION}.0"

;--------------------------------
;Installer Sections

Section !$(CommonName) SecCommon

  SetOutPath "$INSTDIR"
  SectionIn RO
  
  SetDetailsPrint textonly
  DetailPrint "Installing wxGIS Core Files..."
  SetDetailsPrint listonly  
  
  ;system libs
  SetOutPath "$INSTDIR\bin\Microsoft.VC80.CRT"
  File /r "${WXGIS_DIR}\build\release\Microsoft.VC80.CRT\*.*"
  SetOutPath "$INSTDIR\bin\Microsoft.VC90.CRT"
  File /r "${WXGIS_DIR}\build\release\Microsoft.VC90.CRT\*.*"  
  ;wxWidgets libs
  SetOutPath "$INSTDIR\bin"
  File "${WXGIS_DIR}\build\release\wxbase28u_net_vc_custom.dll"  
  File "${WXGIS_DIR}\build\release\wxbase28u_vc_custom.dll"  
  File "${WXGIS_DIR}\build\release\wxbase28u_xml_vc_custom.dll"  
  File "${WXGIS_DIR}\build\release\wxmsw28u_adv_vc_custom.dll"  
  File "${WXGIS_DIR}\build\release\wxmsw28u_aui_vc_custom.dll"  
  File "${WXGIS_DIR}\build\release\wxmsw28u_core_vc_custom.dll"  
  File "${WXGIS_DIR}\build\release\wxmsw28u_html_vc_custom.dll"  
  ;wxPropertyGrid
  File "${WXGIS_DIR}\build\release\wxcode_msw28u_propgrid.dll"  
  ;gdal libs
  File "${WXGIS_DIR}\build\release\wxgiscpl.dll"
  File "${WXGIS_DIR}\build\release\wxgisogr.dll"
  File "${WXGIS_DIR}\build\release\wxgisgdal.dll"  
  ;proj lib
  File "${WXGIS_DIR}\build\release\proj4.dll"  
  ;geos lib
  File "${WXGIS_DIR}\build\release\geos_c.dll"
  ;curl lib
  File "${WXGIS_DIR}\build\release\libcurl.dll"
  ;postgres libs
  File "${WXGIS_DIR}\build\release\libiconv-2.dll"
  File "${WXGIS_DIR}\build\release\libintl-8.dll"
  File "${WXGIS_DIR}\build\release\libeay32.dll"
  File "${WXGIS_DIR}\build\release\ssleay32.dll"
  File "${WXGIS_DIR}\build\release\libpq.dll"
  
  ;sys dir
  SetOutPath "$INSTDIR\sys\gdal"
  File /r "${WXGIS_DIR}\build\release\sys\gdal\*.*"
  SetOutPath "$INSTDIR\sys"
  File /r "${WXGIS_DIR}\build\release\sys\cs.zip"
  
  ;log dir
  SetOutPath "$INSTDIR\log"
  File /r "${WXGIS_DIR}\build\release\log\*.*"
  ;conf dir
  SetOutPath "$INSTDIR\bin\config"
  File /r "${WXGIS_DIR}\build\release\config\*.xml"
  
  ;locale dir
  SetOutPath "$INSTDIR\locale"
  File /r "${WXGIS_DIR}\build\release\locale\*.*"
  ;license
  SetOutPath "$INSTDIR\docs"
  File "${WXGIS_DIR}\build\release\COPYING_*"
  File "${WXGIS_DIR}\build\release\ChangeLog_*"
  
  SetOutPath "$INSTDIR"
  
  ;Store installation folder
  WriteRegStr HKLM "Software\wxgis" "" $INSTDIR
 
	; Store uninstall information
	;Delete previous uninstal info
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}"
	;Create uninstal info
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "DisplayName" "${PRODUCT}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "UninstallString" '"$INSTDIR\Uninstall.exe"'
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}" "DisplayIcon" '"$INSTDIR\Uninstall.exe"'  
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

SectionGroup /e "!wxGIS" SecwxGIS
	Section "Catalog" SecCatalog
	
	  SectionIn RO
	
		SetDetailsPrint textonly
		DetailPrint "Installing Catalog files..."
		SetDetailsPrint listonly

		CreateDirectory "$SMPROGRAMS\wxGIS"
		CreateShortCut "$SMPROGRAMS\wxGIS\$(Catalog).lnk" "$INSTDIR\bin\wxGISCatalog.exe"
		CreateShortCut "$SMPROGRAMS\wxGIS\$(Uninst).lnk" "$INSTDIR\Uninstall.exe"
		
		SetOutPath "$INSTDIR\bin"
		File "${WXGIS_DIR}\build\release\wxGISCatalog.exe"
		File "${WXGIS_DIR}\build\release\wxGISCarto.dll"
		File "${WXGIS_DIR}\build\release\wxGISCartoUI.dll"
		File "${WXGIS_DIR}\build\release\wxGISCatalog.dll"		
		File "${WXGIS_DIR}\build\release\wxGISCatalogUI.dll"		
		File "${WXGIS_DIR}\build\release\wxGISCore.dll"		
		File "${WXGIS_DIR}\build\release\wxGISDataSource.dll"		
		File "${WXGIS_DIR}\build\release\wxGISDisplay.dll"		
		File "${WXGIS_DIR}\build\release\wxGISFramework.dll"		
		File "${WXGIS_DIR}\build\release\wxGISGeometry.dll"		
		;configurator
		File "${WXGIS_DIR}\build\release\wxGISConf.exe"
		
		;write xml config
		
		;wxCatalog/catalog/rootitems  <rootitem name=$\"wxGxSpatialReferencesFolder$\" path=$\"/vsizip/$INSTDIR/sys/cs.zip/cs$\" is_enabled=$\"1$\"/>
		; -a wxCatalog/catalog/rootitems/rootitem -p /vsizip/$INSTDIR/sys/cs.zip/cs -n wxGxSpatialReferencesFolder -e 1
		DetailPrint "wxGISConf.exe -a wxCatalogUI/catalog/rootitems/rootitem#name=wxGxSpatialReferencesFolderUI -p /vsizip/$INSTDIR/sys/cs.zip/cs"
		nsExec::Exec '"$INSTDIR\bin\wxGISConf.exe" -a wxCatalogUI/catalog/rootitems/rootitem#name=wxGxSpatialReferencesFolderUI -p "/vsizip/$INSTDIR/sys/cs.zip/cs"'  
		DetailPrint "wxGISConf.exe -a wxCatalog/catalog/rootitems/rootitem#name=wxGxSpatialReferencesFolderUI -p /vsizip/$INSTDIR/sys/cs.zip/cs"
		nsExec::Exec '"$INSTDIR\bin\wxGISConf.exe" -a wxCatalog/catalog/rootitems/rootitem#name=wxGxSpatialReferencesFolder -p "/vsizip/$INSTDIR/sys/cs.zip/cs"'		
			
		;wxGISCatalog  <loc path=$\"$INSTDIR\locale$\"/>                          -p wxGISCatalog/loc $INSTDIR\locale     set path always HKLM
		DetailPrint "wxGISConf.exe -a wxGISCatalog/loc -p $INSTDIR\locale"
		nsExec::Exec '"$INSTDIR\bin\wxGISConf.exe" -a wxGISCatalog/loc -p "$INSTDIR\locale"'
		
		;wxGISCatalog  <log path=$\"$INSTDIR\log$\"/>                               -p wxGISCatalog/log $INSTDIR\log
		DetailPrint "wxGISConf.exe -a wxGISCatalog/log -p $INSTDIR\log"
		nsExec::Exec '"$INSTDIR\bin\wxGISConf.exe" -a wxGISCatalog/log -p "$INSTDIR\log"'
		
		;wxGISCatalog  <sys path=$\"$INSTDIR\sys$\"/>                              -p wxGISCatalog/sys $INSTDIR\sys
		DetailPrint "wxGISConf.exe -a wxGISCatalog/sys -p $INSTDIR\sys"
		nsExec::Exec '"$INSTDIR\bin\wxGISConf.exe" -a wxGISCatalog/sys -p "$INSTDIR\sys"'

	SectionEnd
  
	Section "Toolbox" SecToolbox
	
		SetDetailsPrint textonly
		DetailPrint "Installing Toolbox files..."
		SetDetailsPrint listonly

		SetOutPath "$INSTDIR\bin"
		File "${WXGIS_DIR}\build\release\wxGISGeoprocess.exe"
		File "${WXGIS_DIR}\build\release\wxGISGeoprocessing.dll"
		File "${WXGIS_DIR}\build\release\wxGISGeoprocessingUI.dll"
		
		;add to xml config 
		DetailPrint "wxGISConf.exe -a wxGISCatalog/libs/lib -p $INSTDIR\bin\wxGISGeoprocessingUI.dll -n wxGISGeoprocessingUI"	
		nsExec::Exec '"$INSTDIR\bin\wxGISConf.exe" -a wxGISCatalog/libs/lib -p "$INSTDIR\bin\wxGISGeoprocessingUI.dll" -n wxGISGeoprocessingUI'
		DetailPrint "wxGISConf.exe -a wxGISToolbox/tools -x gp_exec=$INSTDIR\bin\wxGISGeoprocess.exe"    
		nsExec::Exec '"$INSTDIR\bin\wxGISConf.exe" -a wxGISToolbox/tools -x "gp_exec=$INSTDIR\bin\wxGISGeoprocess.exe"'
			
	SectionEnd	
  
	Section /o "Remote Client" SecRemoteCli
	
		SetDetailsPrint textonly
		DetailPrint "Installing Remote Client files..."
		SetDetailsPrint listonly

		SetOutPath "$INSTDIR\bin"
		File "${WXGIS_DIR}\build\release\wxGISNetworking.dll"
		File "${WXGIS_DIR}\build\release\wxGISRemoteServer.dll"
		File "${WXGIS_DIR}\build\release\wxGISRemoteServerUI.dll"
		
		;add to xml config
		DetailPrint "wxGISConf.exe -a wxGISCatalog/libs/lib -p $INSTDIR\bin\wxGISRemoteServerUI.dll -n wxGISRemoteServerUI"
		nsExec::Exec '"$INSTDIR\bin\wxGISConf.exe" -a wxGISCatalog/libs/lib -p "$INSTDIR\bin\wxGISRemoteServerUI.dll" -n wxGISRemoteServerUI' 

	SectionEnd	  
  
;	Section "Remote Server" SecRemoteSrv
;	
;		SetDetailsPrint textonly
;		DetailPrint "Installing Remote Server files..."
;		SetDetailsPrint listonly
;
;		SetOutPath "$INSTDIR\bin"
;		File "${WXGIS_DIR}\build\release\wxGISServer.exe"
;		File "${WXGIS_DIR}\build\release\wxGISServerAuth.dll"
;		File "${WXGIS_DIR}\build\release\wxGISServerFramework.dll"
;		File "${WXGIS_DIR}\build\release\wxGISServerNetworking.dll"		
;   
;		;add to xml config
;		nsExec::Exec '"$INSTDIR\bin\wxGISConf.exe" -a wxGISServer/loc -p "$INSTDIR\locale"'
;		nsExec::Exec '"$INSTDIR\bin\wxGISConf.exe" -a wxGISServer/log -p "$INSTDIR\log"'
;		nsExec::Exec '"$INSTDIR\bin\wxGISConf.exe" -a wxGISServer/sys -p "$INSTDIR\sys"'   
;	SectionEnd	    
SectionGroupEnd

SectionGroup /e $(SCutName) SecMisc
	Section /o $(SCutDesctopName) SecSCutDCatalog
		SetShellVarContext current
		CreateShortCut "$DESKTOP\wxGISCatalog.lnk" "$INSTDIR\bin\wxGISCatalog.exe"
	SectionEnd
	Section /o $(SCutQlaunchName) SecSCutQCatalog
		SetShellVarContext current
		CreateShortCut "$QUICKLAUNCH\wxGISCatalog.lnk" "$INSTDIR\bin\wxGISCatalog.exe"
	SectionEnd
SectionGroupEnd

;--------------------------------
;Descriptions

  ;USE A LANGUAGE STRING IF YOU WANT YOUR DESCRIPTIONS TO BE LANGUAGE SPECIFIC
;--------------------------------
!include "LangFile.nsh"

LangString CommonName ${LANG_ENGLISH} "Common"
LangString CommonName ${LANG_RUSSIAN} "Общие"
LangString CommonDesc ${LANG_ENGLISH} "Install common library & data"
LangString CommonDesc ${LANG_RUSSIAN} "Установка общих библиотек и данных"
LangString CatalogDesc ${LANG_ENGLISH} "Install Catalog"
LangString CatalogDesc ${LANG_RUSSIAN} "Установка Каталога"
LangString ToolboxDesc ${LANG_ENGLISH} "Install Toolbox"
LangString ToolboxDesc ${LANG_RUSSIAN} "Установка Набора инструментов"
LangString RemoteCliDesc ${LANG_ENGLISH} "Install Remote Client"
LangString RemoteCliDesc ${LANG_RUSSIAN} "Установка Удаленного клиента"
LangString RemoteSrvDesc ${LANG_ENGLISH} "Install Toolbox"
LangString RemoteSrvDesc ${LANG_RUSSIAN} "Установка Набора инструментов"
LangString MiscDesc ${LANG_ENGLISH} "Install Additional options"
LangString MiscDesc ${LANG_RUSSIAN} "Установка Дополнительных опций"

LangString UninstQuiestion ${LANG_ENGLISH} "There is the previous installation in '$INSTDIR'.$\r$\nUninstal?"
LangString UninstQuiestion ${LANG_RUSSIAN} "Обнаружена предыдущая установка в папке '$INSTDIR'.$\r$\nДеинсталировать?"
LangString SCutName ${LANG_ENGLISH} "Shortcats"
LangString SCutName ${LANG_RUSSIAN} "Ярлыки"
LangString SCutDesctopName ${LANG_ENGLISH} "Catalog on desctop"
LangString SCutDesctopName ${LANG_RUSSIAN} "Каталога на рабочем столе"
LangString SCutQlaunchName ${LANG_ENGLISH} "Catalog on quick launch"
LangString SCutQlaunchName ${LANG_RUSSIAN} "Каталога на панели быстрого запуска"
LangString UninstSettings ${LANG_ENGLISH} "Delete user settings?"
LangString UninstSettings ${LANG_RUSSIAN} "Удалить пользовательские настройки?"
LangString Catalog ${LANG_ENGLISH} "Catalog"
LangString Catalog ${LANG_RUSSIAN} "Каталог"
LangString Uninst ${LANG_ENGLISH} "Uninstall"
LangString Uninst ${LANG_RUSSIAN} "Удалить"

  ;Assign descriptions to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT ${SecCommon} $(CommonDesc)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCatalog} $(CatalogDesc)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecToolbox} $(ToolboxDesc)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecRemoteCli} $(RemoteCliDesc)  
	!insertmacro MUI_DESCRIPTION_TEXT ${SecRemoteSrv} $(RemoteSrvDesc)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecMisc} $(MiscDesc)  
  
  !insertmacro MUI_FUNCTION_DESCRIPTION_END


;--------------------------------
;Uninstaller Section

Section "Uninstall"

  SetDetailsPrint textonly
  DetailPrint "Uninstalling wxGIS..."
  SetDetailsPrint listonly

  Delete "$INSTDIR\Uninstall.exe"

  RMDir /r /REBOOTOK "$INSTDIR"

  DeleteRegKey HKLM "Software\wxgis"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT}"
  
  RMDir /r "$SMPROGRAMS\wxGIS"
  Delete "$DESKTOP\wxGISCatalog.lnk"
  Delete "$QUICKLAUNCH\wxGISCatalog.lnk"
  
  SetShellVarContext all
  RMDir /r "$APPDATA\wxGIS"
  MessageBox MB_YESNO $(UninstSettings) IDNO wxgis_uninstalled
	SetShellVarContext current
	RMDir /r "$APPDATA\wxGIS" 
wxgis_uninstalled:
SectionEnd

;
; Miscellaneous functions
;

Function un.onInit

FunctionEnd

Function .onInit
	ReadRegStr $0 HKLM Software\wxgis ""
	
	IfFileExists $0\Uninstall.exe 0 wxgis_notinstalled
		MessageBox MB_YESNO $(UninstQuiestion) IDYES wxgis_installed
		Abort "Отменено пользователем"
wxgis_installed:
		ExecWait "$0\Uninstall.exe"
wxgis_notinstalled:
FunctionEnd

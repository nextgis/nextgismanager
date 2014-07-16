; wxGis
;
; wxgis.nsi - NSIS Script for NSIS 2.46
; Requires the ExecDos plugin (http://nsis.sourceforge.net/wiki/ExecDos)
;
; Copyright (c) 2008 Bishop
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

;XML support
!include "XML.nsh"

;!include "InstallOptions.nsh"
;!include "WordFunc.nsh"

;!insertmacro WordFind

;ReserveFile "${NSISDIR}\Plugins\InstallOptions.dll"

!define GEOS_VERSION "3.2.2"
!define PROJ_VERSION "4.7.1"

!define VERSION "0.2.0"
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
  VIAddVersionKey /LANG=${LANG_ENGLISH} "LegalCopyright" "© 2009-2010 Bishop"
  VIAddVersionKey /LANG=${LANG_ENGLISH} "ProductVersion" "${VERSION}.0"
 
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "ProductName" "wxGIS"
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "Comments" "wxGIS"
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "FileDescription" "wxGIS"
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "FileVersion" "${VERSION}.0" 
  VIAddVersionKey /LANG=${LANG_RUSSIAN} "LegalCopyright" "© 2009-2010 Bishop"
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
  ;wxWidgets libs
  SetOutPath "$INSTDIR\bin"
  File "${WXGIS_DIR}\build\release\wxbase28u_net_vc_custom.dll"  
  File "${WXGIS_DIR}\build\release\wxbase28u_vc_custom.dll"  
  File "${WXGIS_DIR}\build\release\wxbase28u_xml_vc_custom.dll"  
  File "${WXGIS_DIR}\build\release\wxmsw28u_adv_vc_custom.dll"  
  File "${WXGIS_DIR}\build\release\wxmsw28u_aui_vc_custom.dll"  
  File "${WXGIS_DIR}\build\release\wxmsw28u_core_vc_custom.dll"  
  File "${WXGIS_DIR}\build\release\wxmsw28u_html_vc_custom.dll"  
  ;gdal libs
  File "${WXGIS_DIR}\build\release\cpl.dll"
  File "${WXGIS_DIR}\build\release\ogr.dll"
  File "${WXGIS_DIR}\build\release\gdal.dll"  
  ;proj lib
  File "${WXGIS_DIR}\build\release\proj4.dll"  
  ;geos lib
  File "${WXGIS_DIR}\build\release\geos_c.dll"
  ;curl lib
  File "${WXGIS_DIR}\build\release\libcurl.dll"
   
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
  File /oname=wxCatalog.xml "${WXGIS_DIR}\build\release\config\wxCatalog_nsi.xml"
  File /oname=wxGISCatalog.xml "${WXGIS_DIR}\build\release\config\wxGISCatalog_nsi.xml"
  File "${WXGIS_DIR}\build\release\config\wxGISContDialog.xml"
  File "${WXGIS_DIR}\build\release\config\wxGISObjDialog.xml"  
  
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
		
		${xml::LoadFile} "$INSTDIR\bin\config\wxCatalog.xml" $0
			${xml::GotoPath} "/wxCatalog/catalog/rootitems" $0			
			${xml::CreateNode} "<rootitem name=$\"wxGxSpatialReferencesFolder$\" path=$\"/vsizip/$INSTDIR/sys/cs.zip/cs$\" is_enabled=$\"1$\"/>" $1
			${xml::InsertEndChild} "$1" $0				
		${xml::SaveFile} "" $0	
		${xml::Unload}	

		${xml::LoadFile} "$INSTDIR\bin\config\wxGISCatalog.xml" $0
			${xml::GotoPath} "/wxGISCatalog" $0			
			${xml::CreateNode} "<loc locale=$\"$(Language)$\" path=$\"$INSTDIR\locale$\"/>" $1
			${xml::InsertEndChild} "$1" $0				
			${xml::CreateNode} "<log path=$\"$INSTDIR\log$\"/>" $1
			${xml::InsertAfterNode} "$1" $0
			${xml::CreateNode} "<sys path=$\"$INSTDIR\sys$\"/>" $1
			${xml::InsertAfterNode} "$1" $0			
		${xml::SaveFile} "" $0	
		${xml::Unload}	  
		
		;Set lang if app has been installed (current user)
		SetShellVarContext all
		Delete "$APPDATA\wxGIS\wxGISCatalog\hklm_config.xml"		
		;All users should select the lang from properties
		SetShellVarContext current
		${xml::LoadFile} "$APPDATA\wxGIS\wxGISCatalog\hkcu_config.xml" $0
			${xml::GotoPath} "/wxGISCatalog/loc" $0
			${xml::SetAttribute} "locale" "$(Language)" $0
			${xml::GotoPath} "/wxGISCatalog/Accelerators" $0
			${xml::RemoveNode} $0 
		${xml::SaveFile} "" $0	
		${xml::Unload}	
	SectionEnd
	Section "Toolbox" SecToolbox
	
		SetDetailsPrint textonly
		DetailPrint "Installing Toolbox files..."
		SetDetailsPrint listonly

		SetOutPath "$INSTDIR\bin"
		File "${WXGIS_DIR}\build\release\wxGISGeoprocessing.dll"
		File "${WXGIS_DIR}\build\release\wxGISGeoprocessingUI.dll"
		
		SetOutPath "$INSTDIR\sys"
		File "${WXGIS_DIR}\build\release\sys\toolbox.xml"
		
		${xml::LoadFile} "$INSTDIR\bin\config\wxCatalog.xml" $0
			${xml::GotoPath} "/wxCatalog/catalog/rootitems" $0			
			${xml::CreateNode} "<rootitem name=$\"wxGxRootToolbox$\" path=$\"$INSTDIR\sys\toolbox.xml$\" is_enabled=$\"1$\"/>" $1
			${xml::InsertEndChild} "$1" $0				
		${xml::SaveFile} "" $0	
		${xml::Unload}

		${xml::LoadFile} "$INSTDIR\bin\config\wxGISCatalog.xml" $0
			${xml::GotoPath} "/wxGISCatalog/libs" $0			
			${xml::CreateNode} "<lib path=$\"$INSTDIR\bin\wxGISGeoprocessingUI.dll$\" name=$\"wxGISGeoprocessingUI$\"/>" $1
			${xml::InsertEndChild} "$1" $0
			${xml::GotoPath} "/wxGISCatalog/Commands" $0			
			${xml::CreateNode} "<Command name=$\"wxGISGeoprocessingCmd$\"/>	" $1
			${xml::InsertEndChild} "$1" $0
			${xml::GotoPath} "/wxGISCatalog/Frame/Views" $0	
			${xml::CreateNode} "<ToolboxView class=$\"wxGxToolboxViewFactory$\" name=$\"ToolboxView$\"/>" $1	
			${xml::InsertEndChild} "$1" $0	
			${xml::GotoPath} "/wxGISCatalog/Frame/ToolBars/ToolBar" $0
			${xml::CreateNode} "<Item type=$\"sep$\"/>" $1
			${xml::InsertEndChild} "$1" $0	
			${xml::CreateNode} "<Item type=$\"cmd$\" cmd_name=$\"wxGISGeoprocessingCmd$\" subtype=$\"1$\" name=$\"Show/Hide &amp;Toolbox pane$\"/>" $1
			${xml::InsertAfterNode} "$1" $0
			${xml::GotoPath} "/wxGISCatalog/Frame/Menues/Menu" $0
			${xml::CreateNode} "<Item type=$\"sep$\"/>" $1
			${xml::InsertEndChild} "$1" $0	
			${xml::CreateNode} "<Item type=$\"cmd$\" cmd_name=$\"wxGISGeoprocessingCmd$\" subtype=$\"1$\" name=$\"Show/Hide &amp;Toolbox pane$\"/>" $1
			${xml::InsertAfterNode} "$1" $0
		${xml::SaveFile} "" $0	
		${xml::Unload}			
	SectionEnd	
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

  ;USE A LANGUAGE STRING IF YOU WANT YOUR DESCRIPTIONS TO BE LANGAUGE SPECIFIC
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
;LangString UninstQuiestion ${LANG_ENGLISH} "There is the previous installation in '$INSTDIR'.$\r$\nUninstal?"
;LangString UninstQuiestion ${LANG_RUSSIAN} "Обнаружена предыдущая установка в папке '$INSTDIR'.$\r$\nДеинсталировать?"
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

LangString Language ${LANG_ENGLISH} "en"
LangString Language ${LANG_RUSSIAN} "ru"

  ;Assign descriptions to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecCommon} $(CommonDesc)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecCatalog} $(CatalogDesc)
	!insertmacro MUI_DESCRIPTION_TEXT ${SecToolbox} $(ToolboxDesc)
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
  
  ;RMDir /r "$LOCALAPPDATA\wxGIS"
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
	;remove HKLM settings
	  SetShellVarContext all
	  RMDir /r "$APPDATA\wxGIS"
;		MessageBox MB_YESNO $(UninstQuiestion) IDYES wxgis_installed
;		Abort "Отменено пользователем"
;wxgis_installed:
;		ExecWait "$0\Uninstall.exe"
wxgis_notinstalled:
FunctionEnd

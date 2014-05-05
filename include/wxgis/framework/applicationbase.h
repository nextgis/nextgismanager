/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISApplicationBase class. Base application functionality (commands, menues, etc.)
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012 Dmitry Baryshnikov
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#pragma once

#include "wxgis/framework/framework.h"
#include "wxgis/core/app.h"
#include "wxgis/framework/commandbar.h"
#include "wxgis/framework/menubar.h"

#include <wx/window.h>
#include <wx/dynarray.h>

/** \enum wxGISPluginIDs applicationbase.h
    \brief A plugin command or menu command ID enumerator.

    New command ID from user plugin will be set from wxID_HIGHEST + 1 to wxID_HIGHEST + 1049
    New menu item ID from user plugin will be set from wxID_HIGHEST + 1050 to wxID_HIGHEST + 2049
*/
enum wxGISPluginIDs
{
	ID_PLUGINCMD = wxID_HIGHEST + 2001,
	ID_PLUGINCMDMAX = ID_PLUGINCMD + 255,
	ID_TOOLBARCMD = ID_PLUGINCMDMAX + 1,
	ID_TOOLBARCMDMAX = ID_TOOLBARCMD + 255,
    ID_MENUCMD = ID_TOOLBARCMDMAX + 1,
    ID_MENUCMDMAX = ID_MENUCMD + 255
};	

/** \def typedef wxArrayInt WINDOWARRAY applicationbase.h
    \brief A wxWindow IDs array.
*/
typedef wxArrayInt WINDOWARRAY;

/** \class wxGISApplicationBase applicationbase.h
    \brief wxGISApplicationBase class.

    This is base class for framed applicaton.    
*/
class WXDLLIMPEXP_GIS_FRW wxGISApplicationBase : public IApplication
{
    DECLARE_CLASS(wxGISApplicationBase)
public:
    wxGISApplicationBase();
    virtual ~wxGISApplicationBase();
    virtual wxGISCommandBarPtrArray GetCommandBars(void) const;
	virtual wxCommandPtrArray GetCommands(void) const;
	virtual wxGISCommandBar* GetCommandBar(const wxString &sName) const;
	virtual void RemoveCommandBar(wxGISCommandBar* pBar);
	virtual bool AddCommandBar(wxGISCommandBar* pBar);
    virtual void Customize(void){};
	virtual wxIcon GetAppIcon(void){return wxNullIcon;};
	virtual wxGISCommand* GetCommand(long CmdID) const;
	virtual wxGISCommand* GetCommand(const wxString &sCmdName, unsigned char nCmdSubType) const;
    virtual wxGISStatusBar* GetStatusBar(void) const{return NULL;};
	virtual void ShowStatusBar(bool bShow){};
	virtual bool IsStatusBarShown(void){return false;};
	virtual void ShowToolBarMenu(void){};
    virtual wxGISMenuBar* GetGISMenuBar(void) const;
	virtual void ShowApplicationWindow(wxWindow* pWnd, bool bShow = true){};
	virtual bool IsApplicationWindowShown(wxWindow* pWnd){return true;};
	virtual WINDOWARRAY GetChildWindows(void) const{return m_anWindowsIDs;};
	virtual void RegisterChildWindow(wxWindowID nWndID);
	virtual void UnRegisterChildWindow(wxWindowID nWndID);
    virtual wxWindow* GetRegisteredWindowByType(const wxClassInfo * info) const;
	virtual void Command(wxGISCommand* pCmd);
    //IApplication
    virtual wxString GetAppName(void) const{return wxEmptyString;};
    virtual wxString GetAppVersionString(void) const{return wxEmptyString;};
    virtual void OnAppAbout(void){};
    virtual void OnAppOptions(void){};
    virtual bool CreateApp(void);
    virtual bool SetupLog(const wxString &sLogPath, const wxString &sNamePrefix = wxEmptyString){return true;};
    virtual bool SetupLoc(const wxString &sLoc, const wxString &sLocPath){return true;};
    virtual wxString GetDecimalPoint(void) const {return wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);};
    virtual bool SetupSys(const wxString &sSysPath){return true;};
    virtual void SetDebugMode(bool bDebugMode){};
    //events
	virtual void OnMouseDown(wxMouseEvent& event);
	virtual void OnMouseUp(wxMouseEvent& event);
	virtual void OnMouseDoubleClick(wxMouseEvent& event);
	virtual void OnMouseMove(wxMouseEvent& event);
protected:
    virtual void LoadCommands(wxXmlNode* pRootNode);
	virtual void LoadMenues(wxXmlNode* pRootNode);
protected:
	wxCommandPtrArray m_CommandArray;
	wxGISCommandBarPtrArray m_CommandBarArray;
	ITool* m_CurrentTool;
    wxGISMenuBar* m_pMenuBar;
    WINDOWARRAY m_anWindowsIDs;
};


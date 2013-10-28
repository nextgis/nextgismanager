/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISCommandBar class, and diferent implementation - wxGISMneu, wxGISToolBar 
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2012  Bishop
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
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

#include "wxgis/framework/statusbar.h"
#include "wxgis/framework/command.h"

#include <wx/aui/aui.h>
#include <wx/menu.h> 
#include <wx/xml/xml.h> 

#define STANDARDSTR _("Standard")
#define LOCATIONSTR _("Location")
#define GEOGRAPHYOSTR _("Geography")

/** \enum wxGISEnumCommandBars
    \brief A command bar types.

    This is predefined command bar types.
*/
enum wxGISEnumCommandBars
{
	enumGISCBNone = 0x0000,         /**< The type is undefined */
	enumGISCBMenubar = 0x0001,      /**< The type is menubar */
	enumGISCBContextmenu = 0x0002,  /**< The type is context menu */
	enumGISCBSubMenu = 0x0004,      /**< The type is submenu */
	enumGISCBToolbar = 0x0008       /**< The type is toolbar */
};

/** \class wxGISCommandBar commandbar.h
    \brief wxGISCommandBar class.

    This is base class for application command bars.    
*/

class WXDLLIMPEXP_GIS_FRW wxGISCommandBar : 
    public wxObject,
    public wxGISPointer
{
    DECLARE_CLASS(wxGISCommandBar)
public:
	wxGISCommandBar(const wxString& sName = NONAME, const wxString& sCaption = _("No Caption"), wxGISEnumCommandBars type = enumGISCBNone);
	virtual ~wxGISCommandBar(void);
	virtual void SetName(const wxString& sName);
	virtual wxString GetName(void) const;
	virtual void SetCaption(const wxString& sCaption);
	virtual wxString GetCaption(void) const;
	virtual void SetType(wxGISEnumCommandBars type);
	virtual wxGISEnumCommandBars GetType(void);
	virtual void AddCommand(wxGISCommand* pCmd);
    virtual void AddMenu(wxMenu* pMenu, wxString sName){};
	virtual void RemoveCommand(size_t nIndex);
	virtual void MoveCommandLeft(size_t nIndex);
	virtual void MoveCommandRight(size_t nIndex);
	virtual size_t GetCommandCount(void);
	virtual wxGISCommand* GetCommand(size_t nIndex) const;
	virtual void Serialize(wxGISApplicationBase* pApp, wxXmlNode* pNode, bool bStore = false);
protected:
	wxCommandPtrArray m_CommandArray;
	wxString m_sName;
	wxString m_sCaption;
	wxGISEnumCommandBars m_type;
};

/** \def vector<IGISCommandBar*> COMMANDBARARRAY
    \brief A CommandBar array.
*/
WX_DEFINE_ARRAY_PTR(wxGISCommandBar*, wxGISCommandBarPtrArray);

/** \class wxGISMenu commandbar.h
    \brief wxGISMenu class. 
*/
class WXDLLIMPEXP_GIS_FRW wxGISMenu :
	public wxMenu,
	public wxGISCommandBar
{
    DECLARE_CLASS(wxGISMenu)
public:
	wxGISMenu(const wxString& sName = NONAME, const wxString& sCaption = _("No Caption"), wxGISEnumCommandBars type = enumGISCBNone, const wxString& title = wxEmptyString, long style = 0);
	virtual ~wxGISMenu(void);
	virtual void AddCommand(wxGISCommand* pCmd);
	virtual void RemoveCommand(size_t nIndex);
	virtual void MoveCommandLeft(size_t nIndex);
	virtual void MoveCommandRight(size_t nIndex);
	virtual void AddMenu(wxMenu* pMenu, wxString sName);
protected:
	typedef struct submenudata
	{
		wxMenuItem* pItem;
		wxGISCommandBar* pBar;
	} SUBMENUDATA;
	wxVector<SUBMENUDATA> m_SubmenuArray;
};

/** \class wxGISToolbar commandbar.h
    \brief wxGISToolbar class. 
*/
class WXDLLIMPEXP_GIS_FRW wxGISToolBar :
	public wxAuiToolBar,
	public wxGISCommandBar
{
    DECLARE_CLASS(wxGISToolBar)
public:
	wxGISToolBar(wxWindow* parent, wxWindowID id = -1, const wxPoint& position = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxAUI_TB_DEFAULT_STYLE, const wxString& sName = NONAME, const wxString& sCaption = _("No Caption"), wxGISEnumCommandBars type = enumGISCBNone );
	virtual ~wxGISToolBar(void);
	virtual void SetLeftDockable(bool bLDock);
	virtual void SetRightDockable(bool bRDock);
	virtual bool GetLeftDockable(void);
	virtual bool GetRightDockable(void);
	virtual void AddCommand(wxGISCommand* pCmd);
	virtual void SetName(const wxString& sName);
	virtual wxString GetName(void);
	virtual void SetCaption(const wxString& sCaption);
	virtual wxString GetCaption(void);
	virtual void RemoveCommand(size_t nIndex);
	virtual void MoveCommandLeft(size_t nIndex);
	virtual void MoveCommandRight(size_t nIndex);
	virtual void Serialize(wxGISApplicationBase* pApp, wxXmlNode* pNode, bool bStore = false);
	virtual void AddMenu(wxMenu* pMenu, wxString sName);
	virtual void Activate(wxGISApplicationBase* pApp);
	virtual void Deactivate(void);
	virtual void UpdateControls(void);
protected: // handlers
    void OnMotion(wxMouseEvent& evt);
protected: 
	void ReAddCommand(wxGISCommand* pCmd);
protected:
	wxGISStatusBar* m_pStatusBar;
	bool m_bLDock;
	bool m_bRDock;
    bool m_bActive;

	std::map<size_t, IToolBarControl*> m_RemControlMap;
private:
    DECLARE_EVENT_TABLE()
};

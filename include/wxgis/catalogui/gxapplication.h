/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxApplication main header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012 Dmitry Baryshnikov
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

#include "wxgis/framework/applicationex.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/gxselection.h"
#include "wxgis/catalogui/newmenu.h"
#include "wxgis/catalogui/gxtabview.h"

#include "wx/aui/aui.h"
#include "wx/artprov.h"

class wxGxTreeView;

/** @class wxGxApplication gxapplication.h
    
    A catalog application framework class.

    @library{catalogui}
 */

class WXDLLIMPEXP_GIS_CLU wxGxApplication :
    public wxGxApplicationBase,
	public wxGISApplicationEx
{
    DECLARE_CLASS(wxGxApplication)
public:	
	wxGxApplication(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
	virtual ~wxGxApplication(void);
    //events 
    virtual void OnClose(wxCloseEvent& event);
    //wxGISApplicationBase
    virtual wxString GetAppName(void) const {return wxString(wxT("wxGISCatalog"));};
	virtual wxString GetAppDisplayName(void) const {return wxString(_("NextGIS Manager"));};
	virtual wxString GetAppDisplayNameShort(void) const {return wxString(_("Manager"));};
    virtual bool CreateApp(void);
	virtual wxIcon GetAppIcon(void);
    virtual void UpdateNewMenu(wxGxSelection* Selection);
    virtual void UpdateNewMenuInCommands(void);
protected:
	wxGxTreeView* m_pTreeView;
	wxGxTabView* m_pTabView;
	wxGxCatalogUI* m_pCatalog;
    wxGISNewMenu* m_pNewMenu;
    
	wxIcon m_pAppIcon;
};

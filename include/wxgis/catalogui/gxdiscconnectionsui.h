/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnectionsUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2013 Bishop
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
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalogui/gxfolderui.h"
#include "wxgis/catalog/gxdiscconnections.h"

/** \class wxGxDiscConnectionsUI gxdiscconnectionsui.h
    \brief The Disc Connections GxRootObject.
*/

class WXDLLIMPEXP_GIS_CLU wxGxDiscConnectionsUI :
    public wxGxDiscConnections,
	public IGxObjectUI,
    public IGxObjectSort,
    public wxGxAutoRenamer
{
   DECLARE_DYNAMIC_CLASS(wxGxDiscConnectionsUI)
public:
	wxGxDiscConnectionsUI(void);
	virtual ~wxGxDiscConnectionsUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxDiscConnections.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxDiscConnections.NewMenu"));};
	//IGxObjectSort
    virtual bool IsAlwaysTop(void){return true;};
	virtual bool IsSortEnabled(void){return false;};
protected:
    //wxGxJSONConnectionStorage
    virtual wxGxObject* CreateChildGxObject(const wxJSONValue& GxObjConfValue);
protected:
	wxIcon m_Conns16, m_Conns48;
    wxIcon m_Conn16, m_Conn48;
	wxIcon m_ConnDsbld16, m_ConnDsbld48;
};

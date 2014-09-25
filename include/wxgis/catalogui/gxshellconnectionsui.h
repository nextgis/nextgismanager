/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxShellConnectionsUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalog/gxshellconnections.h"
#include "wxgis/catalogui/gxfolderui.h"
#include "wxgis/catalogui/gxview.h"

/** @class wxGxShellConnectionsUI

    The database connections root item in user interface.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxShellConnectionsUI :
    public wxGxShellConnections,
	public IGxObjectUI,
    public wxGxAutoRenamer
{
    DECLARE_DYNAMIC_CLASS(wxGxShellConnectionsUI)
public:
    wxGxShellConnectionsUI(void);
    virtual ~wxGxShellConnectionsUI(void);
    //IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxShellConnectionsUI.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxShellConnectionsUI.NewMenu"));};
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};

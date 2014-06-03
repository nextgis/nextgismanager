/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  GIS Prolect UI classes
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
#include "wxgis/catalog/gxgisproj.h"
#include "wxgis/catalogui/gxview.h"

/** @class wxGxQGISProjFileUI

    The QGIS project file

    @library {catalog}
*/

class WXDLLIMPEXP_GIS_CLU wxGxQGISProjFileUI :
    public wxGxQGISProjFile,
	public IGxObjectUI,
	public IGxObjectEditUI
{
    DECLARE_CLASS(wxGxFolderUI)
public:
    wxGxQGISProjFileUI(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", const wxIcon & LargeIcon = wxNullIcon, const wxIcon & SmallIcon = wxNullIcon);
    virtual ~wxGxQGISProjFileUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxProjFile.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxProjFile.NewMenu"));};
	//IGxObjectEditUI
	virtual void EditProperties(wxWindow *parent);
protected:
    wxIcon m_oLargeIcon, m_oSmallIcon;
};

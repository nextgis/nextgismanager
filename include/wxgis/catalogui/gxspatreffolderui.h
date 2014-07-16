/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSpatialReferencesFolderUI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2014 Dmitry Baryshnikov
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

#include "wxgis/catalog/gxspatreffolder.h"
#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalogui/gxarchfolderui.h"

/** @class wxGxSpatialReferencesFolderUI

    A spatial reference root GxObject.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxSpatialReferencesFolderUI :
    public wxGxSpatialReferencesFolder,
	public IGxObjectUI
{
   DECLARE_DYNAMIC_CLASS(wxGxSpatialReferencesFolderUI)
public:
	wxGxSpatialReferencesFolderUI(void);//wxString Path, wxString Name, bool bShowHidden
	virtual ~wxGxSpatialReferencesFolderUI(void);
	//IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
	virtual wxString ContextMenu(void) const {return wxString(wxT("wxGxSpatialReferencesFolder.ContextMenu"));};
	virtual wxString NewMenu(void) const {return wxString(wxT("wxGxSpatialReferencesFolder.NewMenu"));};
protected:
    //wxGxArchiveFolder
    virtual wxGxObject* GetArchiveFolder(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};

/** @class wxGxPrjFolderUI
    
    A projections folder root GxObject.

    @library{catalogui}
*/


class WXDLLIMPEXP_GIS_CLU wxGxPrjFolderUI :
	public wxGxArchiveFolderUI
{
    DECLARE_CLASS(wxGxPrjFolderUI)
public:
    wxGxPrjFolderUI(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "", wxIcon LargeIcon = wxNullIcon, wxIcon SmallIcon = wxNullIcon);
	virtual ~wxGxPrjFolderUI(void);
	//IGxObject
	virtual wxString GetCategory(void) const {return wxString(_("Coordinate Systems Folder"));};
    //IGxObjectUI
	virtual wxIcon GetLargeImage(void);
	virtual wxIcon GetSmallImage(void);
protected:
    //wxGxArchiveFolder
    virtual wxGxObject* GetArchiveFolder(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
protected:
    wxIcon m_LargeIcon, m_SmallIcon;
};


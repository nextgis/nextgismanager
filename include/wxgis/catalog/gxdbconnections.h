/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDBConnections class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Dmitry Baryshnikov
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

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxfolder.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxevent.h"

/** @class wxGxDBConnections

    The database connections root item. This root item can held connections (*.xconn) and folders items.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxDBConnections :
	public wxGxFolder,
    public IGxRootObjectProperties
{
   DECLARE_DYNAMIC_CLASS(wxGxDBConnections)
public:
	wxGxDBConnections(void);
	virtual ~wxGxDBConnections(void);
	//wxGxObject
    virtual bool Create(wxGxObject *oParent = NULL, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual wxString GetCategory(void) const {return wxString(_("Database connections folder"));};
    virtual void Refresh(void);
	//wxGxObjectContainer
    virtual bool CanCreate(long nDataType, long DataSubtype);
    //wxGxObjectContainer
    virtual bool AreChildrenViewable(void) const {return true;};
    //IGxRootObjectProperties
    virtual void Init(wxXmlNode* const pConfigNode);
    virtual void Serialize(wxXmlNode* const pConfigNode);
	virtual bool CanDelete(void) {return false;};
    virtual bool CanRename(void) {return false;};
protected:
    virtual void StartWatcher(void);
	virtual void LoadChildren(void);
protected:
    wxString m_sInternalPath;
    wxGxCatalog* m_pCatalog;
};


/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Shell Connections classes (ssh, ftp, etc.).
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

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxfolder.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxevent.h"

/** @class wxGxShellConnections

    The shell connections root item.  This root item can held connections (*.shconn) and folder items

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxShellConnections :
	public wxGxFolder,
    public IGxRootObjectProperties
{
    DECLARE_DYNAMIC_CLASS(wxGxShellConnections)
public:
    wxGxShellConnections(void);
    virtual ~wxGxShellConnections(void);
	//wxGxObject
    virtual bool Create(wxGxObject *oParent = NULL, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual wxString GetCategory(void) const {return wxString(_("Shell connections folder"));};
    virtual void Refresh(void);
	//wxGxObjectContainer
    virtual bool CanCreate(long nDataType, long DataSubtype);
    //wxGxObjectContainer
    virtual bool AreChildrenViewable(void) const {return true;};
    //IGxRootObjectProperties
    virtual void Init(wxXmlNode* const pConfigNode);
    virtual void Serialize(wxXmlNode* const pConfigNode);
protected:
    virtual void StartWatcher(void);
	virtual void LoadChildren(void);
protected:
    wxString m_sInternalPath;
    wxGxCatalog* m_pCatalog;
};


/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnections class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2013 Dmitry Baryshnikov
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

#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxobject.h"
#include "wxgis/catalog/gxmlconnstor.h"

#define CONNCONF wxT("conn.json")

/** @class wxGxDiscConnections

    A Disc Connections GxRootObject.

    @library{catalog}
*/

class WXDLLIMPEXP_GIS_CLT wxGxDiscConnections :
	public wxGxJSONConnectionStorage,
    public IGxRootObjectProperties,
    public IGxObjectNoFilter
{
   DECLARE_DYNAMIC_CLASS(wxGxDiscConnections)
public:
	wxGxDiscConnections(void);
	virtual ~wxGxDiscConnections(void);
	//wxGxObject
    virtual bool Create(wxGxObject *oParent = NULL, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    virtual bool Destroy(void);
    virtual wxString GetBaseName(void) const {return GetName();};
    virtual wxString GetFullName(void) const {return wxEmptyString;};
	virtual wxString GetCategory(void) const {return wxString(_("Folder connections"));};
	virtual void Refresh(void);
	//wxGxObjectContainer
	virtual bool AreChildrenViewable(void) const {return true;};
    //IGxRootObjectProperties
    virtual void Init(wxXmlNode* const pConfigNode);
    virtual void Serialize(wxXmlNode* const pConfigNode);
    virtual bool ConnectFolder(const wxString &sPath);
    virtual bool DisconnectFolder(int nStoreId);
protected:
    //wxGxXMLConnectionStorage
    virtual bool IsObjectExist(wxGxObject* const pObj, const wxJSONValue& GxObjConfValue);
    virtual void CreateConnectionsStorage(void);
    virtual wxGxObject* CreateChildGxObject(const wxJSONValue& GxObjConfValue);
    virtual int GetStorageVersion(void) const {return 2;};
protected:
    wxString m_sUserConfigDir;
    wxGxCatalog* m_pCatalog;
    long m_ConnectionPointCatalogCookie;
private:
    DECLARE_EVENT_TABLE()
};

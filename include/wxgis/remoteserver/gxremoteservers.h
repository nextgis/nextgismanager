/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServers class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012 Bishop
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

#include "wxgis/catalog/catalog.h"
#include "wxgis/catalog/gxobject.h"
#include "wxgis/net/netfactory.h"
#include "wxgis/catalog/gxxmlconnstor.h"

#define RCONNCONF wxT("rconn.xml")

/** \class wxGxRemoteServers gxremoteservers.h
    \brief A Remote Servers GxRootObject.
*/
class WXDLLIMPEXP_GIS_RS wxGxRemoteServers :
	public wxGxXMLConnectionStorage,
    public IGxRootObjectProperties
{
   DECLARE_DYNAMIC_CLASS(wxGxRemoteServers)
public:
	wxGxRemoteServers(void);
	virtual ~wxGxRemoteServers(void);
    virtual bool IsUniqName(const wxString & sName);
    virtual bool StoreConnectionProperties(const wxXmlNode* pConnProp);
	//wxGxObject
    virtual bool Create(wxGxObject *oParent = NULL, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    virtual bool Destroy(void);
    virtual wxString GetBaseName(void) const {return GetName();};
    virtual wxString GetFullName(void) const {return wxEmptyString;};
	virtual wxString GetCategory(void) const {return wxString(_("Remote Servers"));};
	virtual void Refresh(void);
	//wxGxObjectContainer
	virtual bool AreChildrenViewable(void){return true;};
    //IGxRootObjectProperties
    virtual void Init(wxXmlNode* const pConfigNode);
    virtual void Serialize(wxXmlNode* const pConfigNode);
protected:
    //wxGxXMLConnectionStorage
    virtual bool IsObjectExist(wxGxObject* pObj, const wxXmlNode* pNode);
    virtual void CreateConnectionsStorage(void);
    virtual wxGxObject* CreateChildGxObject(const wxXmlNode* pNode);
    //
	virtual void LoadFactories(wxXmlNode* pConf);
protected:
	bool m_bIsChildrenLoaded;
    wxString m_sUserConfigDir;
    wxFileSystemWatcher *m_pWatcher;
    wxNetConnFactoryArray m_apNetConnFact;
private:
    DECLARE_EVENT_TABLE()
    /*
    //IGxObjectContainer
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){return m_Children.size() > 0 ? true : false;};
	//wxGxRemoteServers
protected:
	//wxGxRemoteServers
	virtual void LoadChildren();
	virtual void EmptyChildren(void);
	virtual void UnLoadFactories(void);
    virtual void StoreConnections(void);

protected:
    NETCONNFACTORYARRAY m_apNetConnFact;
	bool m_bIsChildrenLoaded;
    wxString m_sUserConfig;
    wxString m_sUserConfigDir;*/
};

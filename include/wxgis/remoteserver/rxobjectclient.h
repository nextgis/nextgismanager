/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxRxObject class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012 Bishop
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

#include "wxgis/catalog/gxobject.h"
#include "wxgis/net/netconn.h"
#include "wxgis/net/netevent.h"

class WXDLLIMPEXP_GIS_RS wxRxCatalog;

/** \class wxRxObject gxremoteserver.h
    \brief The base class for Remote GxObjects (RxObjects).
*/
class WXDLLIMPEXP_GIS_RS wxRxObject
{
public:
	wxRxObject(void);
	virtual ~wxRxObject(void);
    virtual bool Create (wxRxCatalog* const pRxCatalog, long nId, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    virtual void OnNetEvent(wxGISNetEvent& event);
    virtual void SetRemoteId(long nId);
    virtual long GetRemoteId(void) const;
protected:   
    wxRxCatalog* m_pRxCatalog;
    long m_nRemoteId;
};

/** \class wxRxObjectContainer gxremoteserver.h
    \brief The base class for Remote GxObjectContainers (RxObjectContainers).
*/
class WXDLLIMPEXP_GIS_RS wxRxObjectContainer : 
	public wxRxObject,
    public wxGxObjectContainer
{
    DECLARE_CLASS(wxRxObjectContainer)
public:
	wxRxObjectContainer(void);
    wxRxObjectContainer(wxRxCatalog* const pRxCatalog, long nId, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    virtual bool Create (wxRxCatalog* const pRxCatalog, long nId, wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	virtual ~wxRxObjectContainer(void);
    //wxGxObject
    virtual void Refresh(void);
    //wxGxObjectContainer
    virtual bool DestroyChildren();
    virtual bool Destroy(void);
protected:
    virtual void LoadRemoteChildren(const wxXmlNode* pChildrenNode);
    virtual void GetRemoteChildren(void);
protected:
	bool m_bIsChildrenLoaded;
};

/** \class wxRxCatalog gxremoteserver.h
    \brief The root of Remote GxObjects (RxObjects).
*/
class WXDLLIMPEXP_GIS_RS wxRxCatalog : public wxRxObjectContainer
{
    DECLARE_CLASS(wxRxCatalog)
public:
	wxRxCatalog(void);
    wxRxCatalog(wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
    virtual ~wxRxCatalog(void);
    virtual bool Create (wxGxObject *oParent, const wxString &soName = wxEmptyString, const CPLString &soPath = "");
	//register / unregister pointer and uniq ID
	virtual void RegisterRemoteObject(wxRxObject* pObj);
	virtual void UnRegisterRemoteObject(long nId);
	//get pointer by ID
	virtual wxRxObject* const GetRegisterRemoteObject(long nId);
    virtual void SendNetMessage(const wxNetMessage & msg);
protected:
    wxGISNetClientConnection* m_pNetConn;
    std::map<long, wxRxObject*> m_moRxObject; //map of registered IGxObject pointers
};

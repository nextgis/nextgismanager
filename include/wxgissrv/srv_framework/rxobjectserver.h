/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxRxObjectContainer class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012 Bishop
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

#include "wxgis/net/netevent.h"
#include "wxgis/catalog/gxcatalog.h"

/** \class wxRxObject rxobjectserver.h
    \brief The interface class for remote objects.
*/

class wxRxObject
{
public:
    virtual ~wxRxObject(void){};
    virtual void OnNetEvent(wxGISNetEvent& event) = 0;
    virtual wxString GetClassName(void) const = 0;
    //virtual wxXmlNode* GetXmlDescription(void) const = 0;
    virtual wxXmlNode* GetXmlDescription(void) const {return NULL;};
};

/** \class wxRxObjectContainer rxobjectserver.h
    \brief The interface class for remote objects container.
*/

class WXDLLIMPEXP_GIS_FRW wxRxObjectContainer : public wxRxObject
{
public:
    virtual ~wxRxObjectContainer(void);
    virtual void SendChildren(const wxGxObjectList &Children, long nSourceId, int nNetClientId);
};

/** \class wxRxCatalog rxobjectserver.h
    \brief The remote catalog class.
*/
class WXDLLIMPEXP_GIS_FRW wxRxCatalog : 
    public wxGxCatalog,
	public wxRxObjectContainer,
    public INetEventProcessor
{
    wxDECLARE_DYNAMIC_CLASS_NO_COPY(wxRxCatalog);
public:
    wxRxCatalog(wxGxObject *oParent = NULL, const wxString &soName = _("Catalog"), const CPLString &soPath = "");
    virtual ~wxRxCatalog(void);
    virtual wxString GetClassName(void) const {return wxString(wxT("wxRxCatalog"));};
    //INetEventProcessor
    virtual void ProcessNetEvent(wxGISNetEvent& event);
    //wxRxObjectContainer
    virtual void OnNetEvent(wxGISNetEvent& event);    
protected:
	virtual wxString GetConfigName(void) const {return wxString(wxT("wxServerCatalog"));};
};

/*
#include "wxgissrv/srv_framework/framework.h"
#include "wxgis/catalog/catalog.h"

/** \class wxRxObjectContainer rxobjectserver.h
    \brief The basic class for remote containers.
*//*
class WXDLLIMPEXP_GIS_FRW wxRxObjectContainer :
	public IGxObjectContainer,
	public IRxObjectServer,
	public INetMessageReceiver
{
public:
	wxRxObjectContainer(void);
	virtual ~wxRxObjectContainer(void);
	//IGxObject
	virtual wxString GetName(void) = 0;
    virtual wxString GetBaseName(void){return GetName();};
    virtual wxString GetPath(void){return wxEmptyString;};
	virtual wxString GetCategory(void) = 0;
	virtual void Detach(void);
	//IGxObjectContainer
	virtual bool DeleteChild(IGxObject* pChild);
	virtual bool AreChildrenViewable(void){return true;};
	virtual bool HasChildren(void){return m_Children.size() > 0 ? true : false;};
	//IRxObjectServer
	virtual wxXmlNode* GetDescription(void);
	virtual void SetApplication(IServerApplication* pApp);
	//INetMessageReceiver
    virtual void ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode);
	//wxRxObjectContainer
	virtual void EmptyChildren(void);
protected:
    IServerApplication* m_pApp;
	bool m_bIsChildrenLoaded;
};

/** \class wxRxObjectContainer rxobjectserver.h
    \brief The basic class for remote containers.
*//*
class WXDLLIMPEXP_GIS_FRW wxRxObject :
	public IGxObject,
	public IRxObjectServer,
	public INetMessageReceiver
{
public:
	wxRxObject(void);
	virtual ~wxRxObject(void);
	//IGxObject
	virtual wxString GetName(void) = 0;
    virtual wxString GetBaseName(void){return GetName();};
    virtual CPLString GetInternalName(void){return CPLString();};
	virtual wxString GetCategory(void) = 0;
	virtual void Detach(void);
	//IRxObjectServer
	virtual wxXmlNode* GetDescription(void);
	virtual void SetApplication(IServerApplication* pApp);
	//INetMessageReceiver
    virtual void ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode);
protected:
    IServerApplication* m_pApp;
};
*/
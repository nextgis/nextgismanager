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

#include "wxgis/remoteserver/rxobjectclient.h"
#include "wxgis/net/message.h"
#include "wxgis/catalog/gxcatalog.h"

//------------------------------------------------------------
// wxRxObject
//------------------------------------------------------------

wxRxObject::wxRxObject()
{
    m_nRemoteId = wxNOT_FOUND;
    m_pRxCatalog = NULL;
}

wxRxObject::~wxRxObject()
{
    if(m_pRxCatalog && m_pRxCatalog != this)//prevent self unregister of rxcatalog
        m_pRxCatalog->UnRegisterRemoteObject(GetRemoteId());
}

void wxRxObject::SetRemoteId(long nId)
{
    m_nRemoteId = nId;
}

long wxRxObject::GetRemoteId(void) const
{
    return m_nRemoteId;
}

bool wxRxObject::Create (wxRxCatalog* const pRxCatalog, long nId, wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
    wxASSERT_MSG(0, wxT("Function should never run"));
    return false;
}

void wxRxObject::OnNetEvent(wxGISNetEvent& event)
{
    wxASSERT_MSG(0, wxT("Function should never run"));
}

//------------------------------------------------------------
// wxRxObjectContainer
//------------------------------------------------------------

IMPLEMENT_CLASS(wxRxObjectContainer, wxGxObjectContainer)

wxRxObjectContainer::wxRxObjectContainer(void) : wxGxObjectContainer(), wxRxObject()
{
    m_bIsChildrenLoaded = false;
}

wxRxObjectContainer::wxRxObjectContainer(wxRxCatalog* const pRxCatalog, long nId, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath), wxRxObject()
{
    m_bIsChildrenLoaded = false;
    m_nRemoteId = nId;
    m_pRxCatalog = pRxCatalog;
    if(m_pRxCatalog)
        m_pRxCatalog->RegisterRemoteObject(this);
}

wxRxObjectContainer::~wxRxObjectContainer(void)
{
}

void wxRxObjectContainer::GetRemoteChildren()
{
	if(m_bIsChildrenLoaded)
		return;
    wxCHECK_RET(m_pRxCatalog, wxT("the m_pRxCatalog is null"));
    wxNetMessage msgout(enumGISNetCmdCmd, enumGISCmdGetChildren, enumGISPriorityHighest, m_nRemoteId);
    m_pRxCatalog->SendNetMessage(msgout);
}

void wxRxObjectContainer::LoadRemoteChildren(const wxXmlNode* pChildrenNode)
{
    wxCHECK_RET(pChildrenNode, wxT("Input wxXmlNode pointer is null"));
    wxXmlNode* pChild = pChildrenNode->GetChildren();
    while(pChild)
    {
        wxString sClassName = pChild->GetAttribute(wxT("class"), NONAME);

        wxObject *obj = wxCreateDynamicObject(sClassName);
        wxGxObject* pGxObject = wxDynamicCast(obj, wxGxObject);
        wxRxObject* pRxObject = dynamic_cast<wxRxObject*>(obj);
        if(pGxObject && pRxObject)
        {
            long nId = wxAtol(pChild->GetAttribute(wxT("id"),wxT("-1")));
            wxString sName = pChild->GetAttribute(wxT("name"), NONAME ); 
            CPLString szPath( pChild->GetAttribute(wxT("path"), NONAME ).mb_str(wxConvUTF8)); 

            if(!pRxObject->Create(m_pRxCatalog, nId, this, sName, szPath))
                wxDELETE(obj);
            else
                wxGIS_GXCATALOG_EVENT_ID(ObjectAdded, pGxObject->GetId());
        }
        else
        {
            wxDELETE(obj);            
        }
        pChild = pChild->GetNext();
    }

    wxGIS_GXCATALOG_EVENT(ObjectChanged);
    m_bIsChildrenLoaded = true;
}

void wxRxObjectContainer::Refresh(void)
{
	DestroyChildren();
    m_bIsChildrenLoaded = false;    
    wxGIS_GXCATALOG_EVENT(ObjectChanged);
	GetRemoteChildren();
}

bool wxRxObjectContainer::Create (wxRxCatalog* const pRxCatalog, long nId, wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
    m_nRemoteId = nId;
    m_pRxCatalog = pRxCatalog;
    if(m_pRxCatalog)
        m_pRxCatalog->RegisterRemoteObject(this);
    return wxGxObjectContainer::Create(oParent, soName, soPath);
}

bool wxRxObjectContainer::DestroyChildren()
{
    m_bIsChildrenLoaded = false;
    return wxGxObjectContainer::DestroyChildren();
}

bool wxRxObjectContainer::Destroy(void)
{
    wxGIS_GXCATALOG_EVENT(ObjectDeleted);
    return wxGxObjectContainer::Destroy();
}

//------------------------------------------------------------
// wxRxCatalog
//------------------------------------------------------------

IMPLEMENT_CLASS(wxRxCatalog, wxRxObjectContainer)

wxRxCatalog::wxRxCatalog(void) : wxRxObjectContainer()
{
    m_pRxCatalog = this;
}

wxRxCatalog::wxRxCatalog(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxRxObjectContainer(NULL, wxNOT_FOUND, oParent, soName, soPath)
{
    m_pRxCatalog = this;
}

wxRxCatalog::~wxRxCatalog(void)
{
}

bool wxRxCatalog::Create (wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
    return wxRxObjectContainer::Create(this, wxNOT_FOUND, oParent, soName, soPath);
}

void wxRxCatalog::RegisterRemoteObject(wxRxObject* pObj)
{
	m_moRxObject[pObj->GetRemoteId()] = pObj;
}

void wxRxCatalog::UnRegisterRemoteObject(long nId)
{
    m_moRxObject[nId] = NULL;
}

wxRxObject* const wxRxCatalog::GetRegisterRemoteObject(long nId)
{
    wxCHECK( nId != wxNOT_FOUND, NULL );
    return m_moRxObject[nId];
}

void wxRxCatalog::SendNetMessage(const wxNetMessage & msg)
{
    if(m_pNetConn)
        m_pNetConn->SendNetMessage(msg);
}
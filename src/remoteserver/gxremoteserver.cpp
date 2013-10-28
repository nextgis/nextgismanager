/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServer class.
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

#include "wxgis/remoteserver/gxremoteserver.h"
#include "wxgis/catalog/gxcatalog.h"

//------------------------------------------------------------------------------
// wxGxRemoteServer
//------------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxRemoteServer, wxRxCatalog)

BEGIN_EVENT_TABLE(wxGxRemoteServer, wxRxCatalog)
  EVT_GISNET_MSG(wxGxRemoteServer::OnGisNetEvent)
END_EVENT_TABLE()

wxGxRemoteServer::wxGxRemoteServer(void) : wxRxCatalog()
{
    m_bAuth = false;
    m_nRemoteId = 0;
}

wxGxRemoteServer::wxGxRemoteServer(wxGISNetClientConnection* const pNetConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxRxCatalog(oParent, soName, soPath)
{    
    m_bAuth = false;
    m_nRemoteId = 0;
    m_pNetConn = pNetConn;
    if(pNetConn)
        m_ConnectionPointNetCookie = m_pNetConn->Advise(this);
}

wxGxRemoteServer::~wxGxRemoteServer(void)
{
    m_pNetConn->Unadvise(m_ConnectionPointNetCookie);
    wxDELETE(m_pNetConn);
}

bool wxGxRemoteServer::Connect(void)
{
    wxCHECK_MSG(m_pNetConn, false, wxT("connection pointer is null"));
 	if(!m_pNetConn->IsConnected())
		return m_pNetConn->Connect(); //start connection - notifications by events
    return false;
}

bool wxGxRemoteServer::Disconnect(void)
{
    wxCHECK_MSG(m_pNetConn, false, wxT("connection pointer is null"));    
 	if(m_pNetConn->IsConnected())
    {
        bool bRes = m_pNetConn->Disconnect();
        bool bRes2 = DestroyChildren();
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
		return bRes & bRes2;
    }
    return false;
}

bool wxGxRemoteServer::IsConnected(void)
{
    wxCHECK_MSG(m_pNetConn, false, wxT("connection pointer is null"));
    return m_pNetConn->IsConnected();
}

void wxGxRemoteServer::OnGisNetEvent(wxGISNetEvent& event)
{
    wxNetMessage msg = event.GetNetMessage();
    long nlId = msg.GetId();
    if(nlId == wxNOT_FOUND || nlId == GetRemoteId())
    {
        switch(msg.GetCommand())
        {
        case enumGISNetCmdBye: //server disconnected
            DestroyChildren();
            wxGIS_GXCATALOG_EVENT(ObjectChanged);
            break;
        case enumGISNetCmdHello: 
            wxGIS_GXCATALOG_EVENT(ObjectChanged); 
            if(msg.GetState() == enumGISNetCmdStAccept)//connection accepted
            {
                GetRemoteChildren();
            }
            else //connection refused
            {      
                wxLogMessage(_("Connection refused with message: %s"), msg.GetMessage().c_str());
            }
            break;
        case enumGISNetCmdNote:
            switch(msg.GetState())
            {
            case enumGISNetCmdStOk:
                wxLogMessage(msg.GetMessage());
                break;
            case enumGISNetCmdStErr:
                wxLogError(msg.GetMessage());
                break;
            default:
                wxLogVerbose(msg.GetMessage());
                break;
            }
            break;
        case enumGISNetCmdCmd: //do something usefull
            if(msg.GetState() == enumGISCmdGetChildren && msg.GetXMLRoot())
            {
                LoadRemoteChildren(msg.GetXMLRoot()->GetChildren());
            }
            break;
        default:
            break;
        }
    }
    else
    {
        wxRxObject* pObj = GetRegisterRemoteObject(nlId);
        if(pObj)
            pObj->OnNetEvent(event);
    }
}

bool wxGxRemoteServer::Destroy(void)
{
    wxGIS_GXCATALOG_EVENT(ObjectDeleted);
    return wxRxCatalog::Destroy();
}

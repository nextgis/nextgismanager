/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServerUI class.
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

#include "wxgis/remoteserverui/gxremoteserverui.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/gxselection.h"
#include "wxgis/catalogui/gxapplication.h"

//-----------------------------------------------------------------------------
// wxGxRemoteServerUI
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxRemoteServerUI, wxGxRemoteServer)

wxGxRemoteServerUI::wxGxRemoteServerUI(void) : wxGxRemoteServer()
{
    m_nGxPendingId = wxNOT_FOUND;
}
	
wxGxRemoteServerUI::wxGxRemoteServerUI(wxGISNetClientConnection* const pNetConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &SmallIco, const wxIcon &LargeIco, const wxIcon &SmalDsbIco, const wxIcon &LargeDsbIco, const wxIcon &SmallAuthIco, const wxIcon &LargeAuthIco) : wxGxRemoteServer(pNetConn, oParent, soName, soPath)
{
    m_SmallIcon = SmallIco;
    m_LargeIcon = LargeIco;
    m_SmallAuthIcon = SmallAuthIco;
    m_LargeAuthIcon = LargeAuthIco;
    m_SmallDsblIcon = SmalDsbIco;
    m_LargeDsblIcon = LargeDsbIco;
    m_nGxPendingId = wxNOT_FOUND;
}

wxGxRemoteServerUI::~wxGxRemoteServerUI(void)
{
}

wxIcon wxGxRemoteServerUI::GetLargeImage(void)
{
    if(m_pNetConn && m_pNetConn->IsConnected())
    {
        //if(!m_bAuth)
            //return m_LargeAuthIcon;
        //else
            return m_LargeIcon;
    }
    else
        return m_LargeDsblIcon;
}

wxIcon wxGxRemoteServerUI::GetSmallImage(void)
{
    if(m_pNetConn && m_pNetConn->IsConnected())
    {
        //if(!m_bAuth)
            //return m_SmallAuthIcon;
        //else
            return m_SmallIcon;
    }
    else
        return m_SmallDsblIcon;
}

bool wxGxRemoteServerUI::Invoke(wxWindow* pParentWnd)
{
	return Connect();
}

void wxGxRemoteServerUI::OnGisNetEvent(wxGISNetEvent& event)
{
    wxNetMessage msg = event.GetNetMessage();
    long nlId = msg.GetId();
    if(nlId == wxNOT_FOUND || nlId == GetRemoteId())
    {
        switch(msg.GetCommand())
        {
        case enumGISNetCmdBye: //server disconnected
            wxLogDebug(wxT("wxGxRemoteServerUI: enumGISNetCmdBye"));
            DestroyChildren();
            if(m_nGxPendingId != wxNOT_FOUND)
                m_nGxPendingId = wxNOT_FOUND;
            {
                wxGIS_GXCATALOG_EVENT(ObjectChanged);
            }
            if(!msg.GetMessage().IsEmpty())
                wxMessageBox(msg.GetMessage(), _("Error"), wxOK | wxICON_ERROR);
            break;
        case enumGISNetCmdHello: 
            wxLogDebug(wxT("wxGxRemoteServerUI: enumGISNetCmdHello"));
            if(msg.GetState() == enumGISNetCmdStAccept)//connection accepted
            {
                GetRemoteChildren();
            }
            else //connection refused
            {            
                wxString sErr = wxString::Format(_("Connection refused with message: %s"), msg.GetMessage().c_str());
                wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
                wxLogMessage(sErr);
                Disconnect();
            }

            {
                wxGIS_GXCATALOG_EVENT(ObjectChanged);     
            }
            break;
        case enumGISNetCmdNote:
            wxLogDebug(wxT("wxGxRemoteServerUI: enumGISNetCmdNote"));
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
            wxLogDebug(wxT("wxGxRemoteServerUI: enumGISNetCmdCmd"));
            if(msg.GetState() == enumGISCmdGetChildren && msg.GetXMLRoot())
            {
                if(m_nGxPendingId != wxNOT_FOUND)
                {
                    wxGxCatalogUI* pCatUI = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
                    if(pCatUI)
                        pCatUI->RemovePending(m_nGxPendingId);  
                    m_nGxPendingId = wxNOT_FOUND;
                } 

                LoadRemoteChildren(msg.GetXMLRoot()->GetChildren());
                //select first child
                wxGxObjectList::compatibility_iterator node = GetChildren().GetFirst();
                if(node)
                {
                    wxGxObject *pFirstChild = node->GetData();
                    wxGxApplication* pApp = dynamic_cast<wxGxApplication*>(GetApplication());
                    if(pFirstChild && pApp)
                    {
                        //select it
                        wxGxSelection* pSel = pApp->GetGxSelection();
                        if(pSel)
                            pSel->Select(pFirstChild->GetId());
                    }
                }
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

bool wxGxRemoteServerUI::Connect(void)
{
    wxCHECK_MSG(m_pNetConn, false, wxT("connection pointer is null"));
    if(m_pNetConn->IsConnected())
        return true;

    if(m_nGxPendingId == wxNOT_FOUND)
    {
        wxGxCatalogUI* pCatUI = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
        if(pCatUI)
        {
            m_nGxPendingId = pCatUI->AddPending(GetId());   
            pCatUI->ObjectChanged(GetId());
            wxGxApplication* pApp = dynamic_cast<wxGxApplication*>(GetApplication());
            if(pApp)
            {
                wxGxSelection* pSel = pApp->GetGxSelection();
                if(pSel)
                    pSel->Select(m_nGxPendingId);
            }
        }
    }

	if(m_pNetConn->Connect()) //start connection - notifications by events
		return true;
    wxMessageBox(wxString::Format(_("Connection error! The server is not available.\n%s"), m_pNetConn->GetLastError()), _("Error"), wxICON_ERROR | wxOK );
	return false;

	//else if(!m_pNetConn)
 //   {
	//	wxMessageBox(_("Connection object is broken!"), _("Error"), wxICON_ERROR | wxOK );
 //   }
}

bool wxGxRemoteServerUI::Disconnect(void)
{
    m_nGxPendingId = wxNOT_FOUND;
    return wxGxRemoteServer::Disconnect();
}
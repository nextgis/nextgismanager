/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  network conection classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012-2013 Bishop
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
#include "wxgis/net/netconn.h"
#include "wxgis/net/netevent.h"
#include "wxgis/core/crypt.h"

#include <wx/sckstrm.h>

//-----------------------------------------------------------------------------
// wxGISNetServerConnection
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISNetServerConnection, INetConnection)

BEGIN_EVENT_TABLE( wxGISNetServerConnection, INetConnection )
  EVT_SOCKET(SOCKET_ID,  wxGISNetServerConnection::OnSocketEvent)
  EVT_TIMER(TIMER_ID, wxGISNetServerConnection::OnTimer)
END_EVENT_TABLE()

wxGISNetServerConnection::wxGISNetServerConnection(void) : INetConnection(), m_timer(this, TIMER_ID)
{
    m_bIsConnected = true;
}

//wxGISNetServerConnection::wxGISNetServerConnection(wxSocketBase* sock) : INetConnection(), m_timer(this, TIMER_ID)
//{
//    SetSocket(sock);
//}

void wxGISNetServerConnection::SetSocket(wxSocketBase* sock)
{
    m_bIsConnected = true;
    m_pSock = sock;
    m_pSock->SetEventHandler(*this, SOCKET_ID);
    m_pSock->SetNotify( wxSOCKET_LOST_FLAG );//|wxSOCKET_INPUT
    m_pSock->Notify(true);
    m_pSock->SetFlags(wxSOCKET_WAITALL | wxSOCKET_BLOCK);

    if (!m_pSock->IsOk())
    {
		wxLogError(_("Cannot bind listening socket"));
    }
    else
    {
        CreateAndRunThreads();
    }
    m_timer.Start(10000, true); //10 sec. disconnect timer TODO: get from config
}

void wxGISNetServerConnection::SetNetworkService(INetService* pNetService)
{
    m_pNetService = pNetService;
}


wxGISNetServerConnection::~wxGISNetServerConnection(void)
{
    DestroyThreads();

    if(m_pNetService)
        m_pNetService->RemoveConnection(this);
}

bool wxGISNetServerConnection::ProcessInputNetMessage(void)
{
    if(!m_pSock || m_pSock->IsDisconnected())
    {
        return false;
    }

    if(m_pSock->WaitForRead(WAITFOR))
    {
        RtlZeroMemory(m_Buffer, sizeof(m_Buffer));
        wxUint32 nRead(0);
        nRead = m_pSock->ReadMsg(m_Buffer, BUFF_SIZE).LastCount();
        wxString sIn(m_Buffer, nRead);
#ifdef _DEBUG
        wxLogDebug(wxString::Format(wxT("rcv:%d bits, %s"), nRead, sIn));
#endif
        //wxCriticalSectionLocker lock(m_msgCS);            

        //m_pSock->SetTimeout(SLEEP);
        //m_pSock->SetFlags(wxSOCKET_WAITALL | wxSOCKET_BLOCK);
        //wxSocketInputStream in(*m_pSock);
        wxJSONValue  value;
        wxJSONReader reader;
        //int numErrors = reader.Parse( in, &value );
        int numErrors = reader.Parse( sIn, &value );
        if ( numErrors > 0 )  
        {
            wxLogDebug(_("Invalid input message"));
            return false;
        }


        wxNetMessage msg(value);
        if(!msg.IsOk())
        {
            wxLogDebug(_("Invalid input message"));
            return false;
        }

        //check connection
        if(msg.GetCommand() == enumGISNetCmdHello)
        {
            wxJSONValue val = msg.GetValue();
            if(!val.IsValid())
                return false;
            wxString sUser = val[wxT("auth")][wxT("user")].AsString();
            wxString sPass = val[wxT("auth")][wxT("pass")].AsString();

            IPaddress addr;
            m_pSock->GetPeer(addr);
            
			if(m_pNetService->CanConnect(sUser, sPass))
			{
                m_timer.Stop(); //stop disconnect timer

                wxLogMessage(_("wxGISNetServerConnection: New client connection accepted from %s:%d"), addr.IPAddress().c_str(), addr.Service());
        
                wxNetMessage msgout(enumGISNetCmdHello, enumGISNetCmdStAccept, enumGISPriorityHighest);
                msgout.SetMessage(_("Connection accepted"));
                SendNetMessageAsync(msgout);
			}
			else
			{
				wxLogMessage(_("wxGISNetServerConnection: To many connections! Connection to address - %s is not established"), addr.IPAddress().c_str());
                wxNetMessage msgout(enumGISNetCmdHello, enumGISNetCmdStRefuse, enumGISPriorityHighest);
                msgout.SetMessage(_("To many connections or login/password is incorrect!"));
                SendNetMessageAsync(msgout);
                //disconnect automatically by timer
 			}
        }
        else
        {
            wxGISNetEvent event(m_nUserId, wxGISNET_MSG, msg);
            PostEvent(event);
        }
        return true;
    }

    return false;
}

void wxGISNetServerConnection::OnSocketEvent(wxSocketEvent& event)
{
    switch(event.GetSocketEvent())
    {
        case wxSOCKET_INPUT:
            //send event to advisers
            wxLogDebug(wxT("wxGISNetServerConnection: INPUT"));
            break;
        case wxSOCKET_OUTPUT:            
            wxLogDebug(wxT("wxGISNetServerConnection: OUTPUT"));
            //ProcessNetMessage();
            break;
        case wxSOCKET_CONNECTION:
            wxLogDebug(wxT("wxGISNetServerConnection: CONNECTION"));
            break;
        case wxSOCKET_LOST:
            wxLogDebug(wxT("wxGISNetServerConnection: LOST"));
            {
                IPaddress addr;
                if (!m_pSock->GetPeer(addr))
                {
                    wxLogMessage(_("User #%d is disconnected"), m_nUserId);
                } 
                else 
                {
                    wxLogMessage(_("User #%d from %s:%d is disconnected"), m_nUserId, addr.IPAddress().c_str(), addr.Service());
                }

                m_bIsConnected = false;
                //as connection is lost we destroy itself
                Destroy();
            }
            break;
        default:
            break;
    }
}

bool wxGISNetServerConnection::Destroy(void)
{
    delete this;
    return true;
}


void wxGISNetServerConnection::OnTimer( wxTimerEvent & event)
{
    IPaddress addr;
    if (!m_pSock->GetPeer(addr))
    {
        wxLogMessage(_("User #%d is disconnected by timer"), m_nUserId);
    } 
    else 
    {
        wxLogMessage(_("User #%d from %s:%d is disconnected by timer"), m_nUserId, addr.IPAddress().c_str(), addr.Service());
    }

    Destroy();
}

//-----------------------------------------------------------------------------
// wxGISNetClientConnection
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxGISNetClientConnection, INetConnection)

wxGISNetClientConnection::wxGISNetClientConnection(void) : INetConnection()
{
}

wxGISNetClientConnection::~wxGISNetClientConnection(void)
{
}

wxJSONValue wxGISNetClientConnection::GetAttributes(void) const
{
    return NULL;
}

bool wxGISNetClientConnection::SetAttributes(const wxJSONValue &oProperties)
{
    return true;
}


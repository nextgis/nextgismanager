/******************************************************************************
 * Project:  wxGIS
 * Purpose:  TCP network classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012,2014 Dmitry Baryshnikov
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

#include "wxgis/net/tcp.h"
#include "wxgis/net/message.h"
#include "wxgis/net/network.h"
#include "wxgis/core/crypt.h"

#define BUFF 1024

// ----------------------------------------------------------------------------
// wxServerTCPNetworkPlugin
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxServerTCPNetworkPlugin, wxEvtHandler)
  EVT_SOCKET(UDP_SERVER_ID,  wxServerTCPNetworkPlugin::OnUDPServerEvent)
  EVT_SOCKET(TCP_SERVER_ID,  wxServerTCPNetworkPlugin::OnTCPServerEvent)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxServerTCPNetworkPlugin, wxEvtHandler)

wxServerTCPNetworkPlugin::wxServerTCPNetworkPlugin(void) : m_nPort(9976), m_nAdvPort(9977)
{
}

wxServerTCPNetworkPlugin::~wxServerTCPNetworkPlugin(void)
{
}

bool wxServerTCPNetworkPlugin::Start(INetService* pNetService, const wxXmlNode* pConfig)
{
    m_pNetService = pNetService;
    m_sAddr = pConfig->GetAttribute(wxT("addr"), wxT(""));
	m_nPort = wxAtoi(pConfig->GetAttribute(wxT("port"), wxT("9976")));
	m_nAdvPort = wxAtoi(pConfig->GetAttribute(wxT("adv_port"), wxT("9977")));

    wxLogMessage(_("%s: Plugin is started...(%s:%d)"), wxT("TCPNetworkPlugin"), m_sAddr.IsEmpty() ? wxT("0.0.0.0") : m_sAddr.c_str(), m_nPort);

    if(!CreateListenSocket())
        return false;

    return CreateUDPNotifier();
}

bool wxServerTCPNetworkPlugin::Stop(void)
{

	wxLogMessage(_("%s: Plugin is shutdown..."), wxT("TCPNetworkPlugin"));
    if(m_udp_socket)
    {
        m_udp_socket->Destroy();
        m_udp_socket = NULL;
    }

    if(m_listeningSocket)
    {
        m_listeningSocket->Destroy();
        m_listeningSocket = NULL;
    }
    return true;
}

bool wxServerTCPNetworkPlugin::CreateUDPNotifier(void)
{
	IPaddress LocalAddress; // For the listening
	LocalAddress.Service(m_nAdvPort); // port on which we listen for the answers

	bool bIsAddrSet = false;
	if(m_sAddr.IsEmpty())
		bIsAddrSet = LocalAddress.AnyAddress();
	else
		bIsAddrSet = LocalAddress.Hostname(m_sAddr);
	if(!bIsAddrSet)
	{
		wxLogError(_("Invalid address - %s"), m_sAddr.c_str());
		return false;
	}

	m_udp_socket = new wxDatagramSocket(LocalAddress, wxSOCKET_NOWAIT); //wxSOCKET_REUSEADDR | wxSOCKET_NOWAIT | wxSOCKET_BROADCAST
    m_udp_socket->SetEventHandler(*this, UDP_SERVER_ID);
    m_udp_socket->SetNotify(wxSOCKET_INPUT_FLAG);
    m_udp_socket->Notify(true);
    if (!m_udp_socket->IsOk())
    {
        wxLogError(wxString(_("Cannot bind listening socket")));
        return false;
    }

    return true;
}

void wxServerTCPNetworkPlugin::OnUDPServerEvent(wxSocketEvent& event)
{
	wxDatagramSocket * sock = wxDynamicCast(event.GetSocket(), wxDatagramSocket);

	char buf[BUFF] = {0};

	IPaddress BroadCastAddress; // For broadcast sending
	BroadCastAddress.Service(m_nAdvPort);

    switch(event.GetSocketEvent())
    {
        case wxSOCKET_INPUT:
            {
                size_t nSize = sock->RecvFrom(BroadCastAddress, buf, sizeof(buf)).LastCount();
                if ( !nSize )
                {
                    wxLogError(wxString::Format(_("Read error (%d): %s"), sock->LastError(), GetSocketErrorMsg(sock->LastError())));
                    return;
                }

                wxJSONValue val(buf, nSize);
 			    wxNetMessage msg(val);
                if(!msg.IsOk())
                    return;

                if(msg.GetCommand() == enumGISNetCmdHello)
                {
                    wxNetMessage msgout(enumGISNetCmdHello, enumGISNetCmdStOk, enumGISPriorityHigh);
                    wxJSONValue val;
                    val[wxT("info")][wxT("port")] = m_nPort;
                    val[wxT("info")][wxT("name")] = m_pNetService->GetServerName();
                    val[wxT("info")][wxT("banner")] = wxGetOsDescription();
                    msgout.SetValue(val);

				    if(msgout.IsOk())
				    {
						BroadCastAddress.Service(m_nAdvPort + 2);
                        SendUDP(BroadCastAddress, msgout, true);
				    }
                }
            }
            break;

        case wxSOCKET_OUTPUT:
            break;

        case wxSOCKET_CONNECTION:
            break;

        case wxSOCKET_LOST:
            sock->Destroy();
            break;

        default:
            break;
    }
}

void wxServerTCPNetworkPlugin::OnTCPServerEvent(wxSocketEvent& event)
{
    switch(event.GetSocketEvent())
    {
        case wxSOCKET_INPUT:
            wxLogError(_("Unexpected wxSOCKET_INPUT in wxSocketServer"));
            break;
        case wxSOCKET_OUTPUT:
            wxLogError(_("Unexpected wxSOCKET_OUTPUT in wxSocketServer"));
        break;
        case wxSOCKET_CONNECTION:
        {
            // Accept new connection if there is one in the pending
            // connections queue, else exit. We use Accept(false) for
            // non-blocking accept (although if we got here, there
            // should ALWAYS be a pending connection).

            wxSocketBase* sock = m_listeningSocket->Accept(false);
            IPaddress addr;
            if (!sock->GetPeer(addr))
            {
                wxLogError(_("Cannot get peer info"));
            }
            else
            {
                wxLogMessage(_("Got connection from %s:%d"), addr.IPAddress().c_str(), addr.Service());
            }

            wxGISNetServerConnection* pSrvConn = new wxGISNetServerConnection();
            pSrvConn->SetSocket(sock);
            m_pNetService->AddConnection(pSrvConn);
        }
        break;
        case wxSOCKET_LOST:
            wxLogError(_("Unexpected wxSOCKET_LOST in wxSocketServer"));
        break;
    }
}

bool wxServerTCPNetworkPlugin::CreateListenSocket(void)
{
	IPaddress LocalAddress; // For the listening
	LocalAddress.Service(m_nPort); // port on which we listen for clients

	bool bIsAddrSet = false;
	if(m_sAddr.IsEmpty())
		bIsAddrSet = LocalAddress.AnyAddress();
	else
		bIsAddrSet = LocalAddress.Hostname(m_sAddr);
	if(!bIsAddrSet)
	{
		wxLogError(_("Invalid address - %s"), m_sAddr.c_str());
		return false;
	}

	m_listeningSocket = new wxSocketServer(LocalAddress, wxSOCKET_WAITALL);//wxSOCKET_NOWAIT|wxSOCKET_REUSEADDR
    m_listeningSocket->SetEventHandler(*this, TCP_SERVER_ID);
    m_listeningSocket->SetNotify(wxSOCKET_CONNECTION_FLAG);
    m_listeningSocket->Notify(true);
    if (!m_listeningSocket->IsOk())
    {
		wxLogError(_("Could not listen at the specified port! Port number - %d"), m_nPort);
        return false;
    }

    wxLogMessage(_("Server listening at port %d, waiting for connections"), m_nPort);

    return true;
}

//-----------------------------------------------------------------------------
// wxClientTCPNetFactory
//-----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxClientTCPNetFactory, INetConnFactory)

BEGIN_EVENT_TABLE( wxClientTCPNetFactory, INetConnFactory )
  EVT_SOCKET(BROADCAST_ID,  wxClientTCPNetFactory::OnBroadcastEvent)
END_EVENT_TABLE()

wxClientTCPNetFactory::wxClientTCPNetFactory(void) : m_udp_socket(NULL)
{
	m_nPort = 9976;
	m_nAdvPort = 9977;
}

wxClientTCPNetFactory::~wxClientTCPNetFactory(void)
{
	StopServerSearch();
}

bool wxClientTCPNetFactory::StartServerSearch()
{
    //send broadcast message
    IPaddress addrPeer;
#ifdef wxUSE_IPV6

#else
    addrPeer.BroadcastAddress();
#endif // wxUSE_IPV6
    addrPeer.Service(m_nAdvPort);
    wxNetMessage msg(enumGISNetCmdHello, enumGISNetCmdStUnk, enumGISPriorityHigh);

    if(!m_udp_socket)
    {
        //create listen broadcast socketif not exist
	    IPaddress LocalAddress;
	    LocalAddress.Service(m_nAdvPort + 2); // port on which we listen for the answers = adv port + 2

	    bool bIsAddrSet = false;
	    if(m_sAddr.IsEmpty())
		    bIsAddrSet = LocalAddress.AnyAddress();
	    else
		    bIsAddrSet = LocalAddress.Hostname(m_sAddr); //special interface to listen
	    if(!bIsAddrSet)
	    {
		    wxLogError(_("Invalid address - %s"), m_sAddr.c_str());
		    return false;
	    }

	    m_udp_socket = new wxDatagramSocket(LocalAddress, wxSOCKET_NOWAIT | wxSOCKET_REUSEADDR);

        m_udp_socket->SetEventHandler(*this, BROADCAST_ID);
        m_udp_socket->SetNotify(wxSOCKET_INPUT_FLAG);
        m_udp_socket->Notify(true);
        if (!m_udp_socket->IsOk())
        {
            wxLogError(wxString(_("Cannot bind listening socket")));
            return false;
        }
    }

    return SendUDP(addrPeer, msg, true);
}

bool wxClientTCPNetFactory::StopServerSearch()
{
    //if the broadcast listen socket is exist destroy it
    if(m_udp_socket)
    {
        m_udp_socket->Destroy();
        m_udp_socket = NULL;
    }
	return true;
};

void wxClientTCPNetFactory::OnBroadcastEvent(wxSocketEvent& event)
{
	wxDatagramSocket * sock = wxDynamicCast(event.GetSocket(), wxDatagramSocket);
	IPaddress addr;

	char buf[BUFF] = {0};

	IPaddress BroadCastAddress; // For broadcast sending
	BroadCastAddress.Service(m_nAdvPort + 2);

    switch(event.GetSocketEvent())
    {
        case wxSOCKET_INPUT:
            {
                size_t nSize = sock->RecvFrom(BroadCastAddress, buf, sizeof(buf)).LastCount();
                if ( !nSize )
                {
                    wxLogError(wxString::Format(_("Read error (%d): %s"), sock->LastError(), GetSocketErrorMsg(sock->LastError())));
                    return;
                }

                wxJSONValue val(buf, nSize);
 			    wxNetMessage msg(val);
                if(!msg.IsOk())
                    return;

                if(msg.GetState() == enumGISNetCmdStOk)
                {
                    //add factory_name to message
                    val = msg.GetValue();
                    val[wxT("details")][wxT("factory_name")] = GetName();
                    val[wxT("details")][wxT("factory")] = GetClassInfo()->GetClassName();
                    val[wxT("details")][wxT("host")] = BroadCastAddress.IPAddress();
                    //create net message event and post it
                    msg.SetValue(val);

                    //wxGISNetEvent event(0, wxGISNET_MSG, msg);
                    //PostEvent(event);
                    PostEvent(new wxGISNetEvent(0, wxGISNET_MSG, msg));
                }
            }
            break;

        case wxSOCKET_OUTPUT:
            break;

        case wxSOCKET_CONNECTION:
            break;

        case wxSOCKET_LOST:
            sock->Destroy();
            break;

        default:
            break;
    }
}

void wxClientTCPNetFactory::Serialize(wxXmlNode* pConfigNode, bool bSave)
{
	if(bSave)
	{
		pConfigNode->AddAttribute(wxT("port"), wxString::Format(wxT("%d"), m_nPort));
		pConfigNode->AddAttribute(wxT("adv_port"), wxString::Format(wxT("%d"), m_nAdvPort));
		pConfigNode->AddAttribute(wxT("addr"), m_sAddr);
	}
	else
	{
		m_nPort = wxAtoi(pConfigNode->GetAttribute(wxT("port"), wxT("9976")));
		m_nAdvPort = wxAtoi(pConfigNode->GetAttribute(wxT("adv_port"), wxT("9977")));
		m_sAddr = pConfigNode->GetAttribute(wxT("addr"), wxEmptyString);
	}
}


wxGISNetClientConnection* const wxClientTCPNetFactory::GetConnection(const wxJSONValue &oProperties)
{
    wxGISNetClientConnection* pConn = new wxClientTCPNetConnection();
    if(pConn->SetAttributes(oProperties))
    {
        return pConn;
    }
    wxDELETE(pConn);
 	return INetConnFactory::GetConnection(oProperties);
}


//------------------------------------------------------------------
// wxClientTCPNetConnection
//------------------------------------------------------------------

IMPLEMENT_CLASS(wxClientTCPNetConnection, wxGISNetClientConnection)

BEGIN_EVENT_TABLE( wxClientTCPNetConnection, wxGISNetClientConnection )
  EVT_SOCKET(SOCKET_ID,  wxClientTCPNetConnection::OnSocketEvent)
END_EVENT_TABLE()

wxClientTCPNetConnection::wxClientTCPNetConnection(void) : wxGISNetClientConnection()
{
    m_pSock = NULL;
}

wxClientTCPNetConnection::~wxClientTCPNetConnection(void)
{
    DestroyThreads();
}

wxJSONValue wxClientTCPNetConnection::GetAttributes(void) const
{
    wxJSONValue val;
    val[wxT("name")] = m_sConnName;
    val[wxT("user")] = m_sUserName;
    val[wxT("pass")] = m_sCryptPass;
    val[wxT("host")] = m_sIP;
    val[wxT("port")] = m_sPort;
    val[wxT("class")] = wxString(wxT("wxClientTCPNetFactory"));
    val[wxT("conn_timeout")] = m_nConnTimeout;
    return val;
}

bool wxClientTCPNetConnection::SetAttributes(const wxJSONValue &oProperties)
{
    wxCHECK_MSG(oProperties.IsValid(), false, wxT("The invalid JSON properties"));

    m_sConnName = oProperties.Get(wxT("name"), wxJSONValue(_("New connection"))).AsString();
	m_sUserName = oProperties[wxT("user")].AsString();
	m_sCryptPass = oProperties[wxT("pass")].AsString();
	m_sIP = oProperties.Get(wxT("host"), wxJSONValue(wxT("127.0.0.1"))).AsString();
	m_sPort = oProperties.Get(wxT("port"), wxJSONValue(wxT("9976"))).AsString();
    m_nConnTimeout = oProperties.Get(wxT("conn_timeout"), wxJSONValue(30L)).AsLong();
	return true;
}

bool wxClientTCPNetConnection::Connect(void)
{
	if(m_bIsConnected)
		return true;

	//start conn
	IPaddress addr;
	addr.Hostname(m_sIP);
	addr.Service(wxAtoi(m_sPort));

    // Create the socket
    wxSocketClient* pSock = new wxSocketClient(wxSOCKET_WAITALL | wxSOCKET_BLOCK);//wxSOCKET_NOWAITwxSOCKET_REUSEADDR
	m_pSock = pSock;
    m_pSock->SetEventHandler(*this, SOCKET_ID);
    m_pSock->Notify(true);
    m_pSock->SetNotify(wxSOCKET_CONNECTION_FLAG|wxSOCKET_LOST_FLAG);//wxSOCKET_OUTPUT_FLAG||wxSOCKET_INPUT_FLAG
	m_pSock->SetTimeout(m_nConnTimeout);
    pSock->Connect(addr, false);

    m_bIsConnecting = true;

    return CreateAndRunThreads();

    //return true;
}

bool wxClientTCPNetConnection::Disconnect(void)
{
	if(!m_bIsConnected)
		return true;

	m_bIsConnected = false;
    m_bIsConnecting = false;

    DestroyThreads();

    //if(m_pSock)
    //{
    //    bool bRet = m_pSock->Destroy();
    //    m_pSock = NULL;
    //    return bRet;
    //}
    return true;
}

wxString wxClientTCPNetConnection::GetLastError(void) const
{
    return wxString::Format(_("Error (%d): %s"), m_pSock->LastError(), GetSocketErrorMsg(m_pSock->LastError()));
}

void wxClientTCPNetConnection::OnSocketEvent(wxSocketEvent& event)
{
    event.Skip(false);
    //wxLogDebug(wxT("wxClientTCPNetConnection: event"));
    switch(event.GetSocketEvent())
    {
        case wxSOCKET_INPUT:
            //send event to advisers
            //wxLogDebug(wxT("wxClientTCPNetConnection: INPUT"));
        break;
        case wxSOCKET_OUTPUT:
            //wxLogDebug(wxT("wxClientTCPNetConnection: OUTPUT"));
            //ProcessNetMessage();
            break;
        case wxSOCKET_CONNECTION:
            //wxLogDebug(wxT("wxClientTCPNetConnection: CONNECTION"));
            m_bIsConnected = true;
            m_bIsConnecting = false;
            {
                wxNetMessage msgout(enumGISNetCmdHello, enumGISNetCmdStAuth, enumGISPriorityHigh);
                wxJSONValue val = msgout.GetValue();
                val[wxT("auth")][wxT("user")] = m_sUserName;
                wxString sOutPass;
                if(Decrypt(m_sCryptPass, sOutPass))
                    val[wxT("auth")][wxT("pass")] = sOutPass;
                msgout.SetValue(val);
                SendNetMessageAsync(msgout);
            }
        break;
        case wxSOCKET_LOST:
            //wxLogDebug(wxT("wxClientTCPNetConnection: LOST"));
            {
                wxNetMessage msgin(enumGISNetCmdBye, enumGISNetCmdStUnk, enumGISPriorityHigh);
                if(!m_bIsConnected && m_bIsConnecting)
                {
                    msgin.SetMessage(_("Could not connect to remote server"));
                    m_bIsConnecting = false;
                }
                else
                {
                    m_bIsConnected = false;
                }
                //wxGISNetEvent event(0, wxGISNET_MSG, msgin);
                //PostEvent(event);
                PostEvent(new wxGISNetEvent(0, wxGISNET_MSG, msgin));
            }
        break;
        default:
            //wxLogDebug(wxT("wxClientTCPNetConnection: default"));
            break;
    }
}

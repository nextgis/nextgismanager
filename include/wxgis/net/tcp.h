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
#pragma once

#include "wxgis/net/servernet.h"
#include "wxgis/net/netfactory.h"

/** @class wxServerTCPNetworkPlugin

    A Server side TCPNetworkPlugin.

    @library{net}
*/
class WXDLLIMPEXP_GIS_NET wxServerTCPNetworkPlugin : public INetPlugin
{
	DECLARE_DYNAMIC_CLASS(wxServerTCPNetworkPlugin)

    enum{
        // id for sockets
        UDP_SERVER_ID = 100,
        TCP_SERVER_ID 
    };
public:
	wxServerTCPNetworkPlugin(void);
	virtual ~wxServerTCPNetworkPlugin(void);
    bool CreateUDPNotifier(void);
    bool CreateListenSocket(void);
   //INetPlugin
    virtual bool Start(INetService* pNetService, const wxXmlNode* pConfig);
    virtual bool Stop(void);   
    //events
    virtual void OnUDPServerEvent(wxSocketEvent& event);
    virtual void OnTCPServerEvent(wxSocketEvent& event);
protected:
    int m_nPort, m_nAdvPort;
    wxString m_sAddr;
    INetService* m_pNetService;
    wxDatagramSocket *m_udp_socket;
    wxSocketServer* m_listeningSocket;

    DECLARE_EVENT_TABLE()
};

/** @class wxClientTCPNetConnection

    The connection to communicate with server.

    @library{net}
*/
class WXDLLIMPEXP_GIS_NET wxClientTCPNetConnection : 
	public wxGISNetClientConnection
{
    DECLARE_CLASS(wxClientTCPNetConnection)
    enum{
        // id for sockets
        SOCKET_ID = 101
    };
public:
	wxClientTCPNetConnection(void);
	virtual ~wxClientTCPNetConnection(void);
	//INetClientConnection
	virtual bool Connect(void);
	virtual bool Disconnect(void);
	virtual wxString GetName(void) const {return m_sConnName;};
	//virtual WXGISMSG GetInMessage(void) = 0;
    //virtual void PutInMessage(WXGISMSG msg);
	//wxGISNetClientConnection
    virtual bool HasAttributes(void) const {return true;};
	virtual wxJSONValue GetAttributes(void) const;
	virtual bool SetAttributes(const wxJSONValue& oProperties);
    virtual wxString GetLastError(void) const;
	//virtual void SetCallback(INetCallback* pNetCallback){m_pCallBack = pNetCallback;};
 //   virtual wxString GetUser(void){return m_sUserName;};
 //   virtual wxString GetCryptPasswd(void){return m_sCryptPass;};
protected:
    //events
    virtual void OnSocketEvent(wxSocketEvent& event);
protected:
	wxString m_sConnName;
	wxString m_sUserName;
	wxString m_sCryptPass;
	wxString m_sIP;
	wxString m_sPort;
    long m_nConnTimeout;

	//wxNetTCPReader* m_pClientTCPReader;
	//wxNetTCPWriter* m_pClientTCPWriter;
	//wxNetTCPWaitlost* m_pClientTCPWaitlost;
	//INetCallback* m_pCallBack;
private:
    DECLARE_EVENT_TABLE()
};

/** @class wxClientTCPNetFactory

    The factory to create net connections.

    @library{net}
*/
class WXDLLIMPEXP_GIS_NET wxClientTCPNetFactory : 
	public INetConnFactory
{
    DECLARE_DYNAMIC_CLASS(wxClientTCPNetFactory)
    enum{
        // id for sockets
        BROADCAST_ID = 100
    };
public:
	wxClientTCPNetFactory(void);
	~wxClientTCPNetFactory(void);
	//INetConnFactory
	virtual wxString GetName(void) const {return wxString(_("TCP/IP Network"));};
	virtual bool StartServerSearch();
	virtual bool StopServerSearch();
    virtual void Serialize(wxXmlNode* pConfigNode, bool bSave = true);
	virtual wxGISNetClientConnection* const GetConnection(const wxJSONValue &oProperties);
	//wxClientTCPNetFactory
	virtual unsigned short GetAdvPort(void){return m_nAdvPort;};
	virtual unsigned short GetPort(void){return m_nPort;};
protected:
    //events
    virtual void OnBroadcastEvent(wxSocketEvent& event);
protected:
    wxDatagramSocket *m_udp_socket;
	wxString m_sAddr;
	unsigned short m_nPort, m_nAdvPort;
private:
    DECLARE_EVENT_TABLE()
};


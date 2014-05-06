/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  TCP network server plugin class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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

#include "wxgissrv/srv_framework/framework.h"
#include "wxgissrv/srv_framework/network.h"

/** \class wxServerTCPNetworkPlugin tcpnetworkplugin.h
    \brief A Server side TCPNetworkPlugin.
*/
class WXDLLIMPEXP_GIS_NET wxServerTCPNetworkPlugin : public INetworkPlugin
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
   //INetworkPlugin
    virtual bool Start(INetworkService* pNetService, wxXmlNode* pConfig);
    virtual bool Stop(void);   
    //events
    virtual void OnUDPServerEvent(wxSocketEvent& event);
    virtual void OnTCPServerEvent(wxSocketEvent& event);
protected:
    int m_nPort, m_nAdvPort;
    wxString m_sAddr;
    INetworkService* m_pNetService;
    wxDatagramSocket *m_udp_socket;
    wxSocketServer* m_listeningSocket;

    DECLARE_EVENT_TABLE()
};

/*
#include "wxgissrv/srv_networking/networking.h"

#include "wxgis/networking/tcpnetwork.h"

/** \class wxServerUDPNotifier tcpnetworkplugin.h
    \brief The class listen for broadcast messages and answer for compatible.
*//*
class wxServerUDPNotifier : public wxThread
{
public:
	wxServerUDPNotifier(wxString sServName, wxString sAddr = wxEmptyString, int nPort = 1976, int nAdvPort = 1977);
    virtual void *Entry();
    virtual void OnExit();
private:
	wxDatagramSocket *m_socket;
	wxString m_sServName;
	wxString m_sAddr;
	int m_nPort, m_nAdvPort;
};

/** \class wxServerTCPWaitAccept tcpnetworkplugin.h
 *  \brief The tcp network wait connection thread.
 *//*
class WXDLLIMPEXP_GIS_NET wxServerTCPWaitAccept : public wxThread
{
public:
    wxServerTCPWaitAccept(wxString sListenAddr, int nListenPort, wxGISNetworkService* pGISNetworkService);
    virtual void *Entry();
    virtual void OnExit();
protected:
	wxSocketServer* m_pSock;
	wxGISNetworkService* m_pGISNetworkService;
};

class WXDLLIMPEXP_GIS_NET wxServerTCPNetConnection;
/** \class wxServerTCPWaitLost tcpnetworkplugin.h
 *  \brief The tcp network wait connection lost thread.
 *//*
class WXDLLIMPEXP_GIS_NET wxServerTCPWaitLost : public wxThread
{
public:
    wxServerTCPWaitLost(wxServerTCPNetConnection* pConnection, wxSocketBase* pSock);
    virtual void *Entry();
    virtual void OnExit();
protected:
	wxServerTCPNetConnection* m_pConnection;
	wxSocketBase* m_pSock;
};

/** \class wxServerTCPNetConnection tcpnetworkplugin.h
    \brief The connection from client.
*//*
class WXDLLIMPEXP_GIS_NET wxServerTCPNetConnection : 
	public wxObject,
	public INetServerConnection
{
public:
	wxServerTCPNetConnection(wxSocketBase* pSocket, wxGISNetworkService* pGISNetworkService);
	~wxServerTCPNetConnection(void);
	//INetConnection
	virtual bool Connect(void);
	virtual bool Disconnect(void);
    virtual void PutInMessage(WXGISMSG msg);
	//INetServerConnection
	virtual void SetAuth(AUTHRESPOND stUserInfo){m_stUserInfo = stUserInfo;};
	virtual AUTHRESPOND GetAuth(void){return m_stUserInfo;};
	virtual void SetAlive(wxDateTime dtm){m_LastAlive = dtm;};
	virtual wxDateTime GetAlive(void){return m_LastAlive;};
protected:
	wxSocketBase* m_pSock;
	wxGISNetworkService* m_pGISNetworkService;
	wxNetTCPReader* m_pServerTCPReader;
	wxNetTCPWriter* m_pServerTCPWriter;
//	wxNetTCPWaitlost* m_pServerTCPWaitlost;
	wxServerTCPWaitLost* m_pServerTCPWaitlost;
	AUTHRESPOND m_stUserInfo;
	wxDateTime m_LastAlive;
};


/** \class wxServerTCPNetworkPlugin tcpnetworkplugin.h
    \brief A Server side TCPNetworkPlugin.
*//*
class WXDLLIMPEXP_GIS_NET wxServerTCPNetworkPlugin :
	public wxObject,
    public INetworkPlugin
{
	DECLARE_DYNAMIC_CLASS(wxServerTCPNetworkPlugin)
public:
	wxServerTCPNetworkPlugin(void);
	virtual ~wxServerTCPNetworkPlugin(void);
    virtual bool Start(wxGISNetworkService* pNetService, wxXmlNode* pConfig);
    virtual bool Stop(void);
    //virtual int GetPort(void){return m_nPort;};
    //virtual wxString GetAddress(void){return m_sAddr;};
    //virtual wxString GetServerName(void){return m_pApp->GetServerName();};
    //virtual bool CanAcceptConnection(void){return m_pApp->CanAcceptConnection();};
    //virtual bool AddConnection();
    //virtual void RemoveConnection(long nID);
protected:
    int m_nPort, m_nAdvPort;
    wxString m_sAddr;
    //wxXmlNode* m_pConfig;
    wxGISNetworkService* m_pNetService;
    wxServerTCPWaitAccept* m_pWaitThread;
	wxServerUDPNotifier* m_pServerUDPNotifier;
};
*/

/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  network server conection class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012 Bishop
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

#include "wxgis/net/network.h"
#include "wxgissrv/srv_net/net.h"

class INetworkService;

/** \class INetServerConnection netconn.h
    \brief The network server connection interface class.
*/
class WXDLLIMPEXP_GIS_SRVNET wxGISNetServerConnection : public INetConnection
{
    DECLARE_CLASS(wxGISNetServerConnection)
    enum{
        // id for sockets
        SOCKET_ID = 103,
        // id for auth timer
        TIMER_ID = 1014
    };
public:
    wxGISNetServerConnection(void);
    wxGISNetServerConnection(wxSocketBase* sock);
	virtual ~wxGISNetServerConnection(void);
    virtual void SetNetworkService(INetworkService* pNetService);
    virtual void SetSocket(wxSocketBase* sock);
    virtual bool Destroy(void);
//	virtual void SetAuth(AUTHRESPOND sUserInfo) = 0;//set user authenticated state true
	//virtual AUTHRESPOND GetAuth(void) = 0; // get if user has been authenticated
	//virtual void SetAlive(wxDateTime dtm) = 0;
	//virtual wxDateTime GetAlive(void) = 0;
protected:
    //events
    virtual void OnSocketEvent(wxSocketEvent& event);
    void OnTimer( wxTimerEvent & event);
protected:    
    virtual void ProcessInputNetMessage(unsigned char * const buff);
protected:
    INetworkService* m_pNetService;
    wxTimer m_timer;
private:
    DECLARE_EVENT_TABLE()
};


/** \class INetworkService netconn.h
    \brief The network service base interface class.
*/
class INetworkService
{
public:
    virtual ~INetworkService(void){};
    virtual bool Start(void) = 0;
    virtual bool Stop(void) = 0;    
    virtual wxString GetServerName(void) const = 0;
    virtual bool CanConnect(const wxString &sName, const wxString &sPass) = 0;
    virtual void AddConnection(wxGISNetServerConnection* pConn) = 0;
    virtual bool DestroyConnection( wxGISNetServerConnection* pConn ) = 0;
    virtual void RemoveConnection( wxGISNetServerConnection* pConn ) = 0;
};


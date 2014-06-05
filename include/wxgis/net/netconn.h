/******************************************************************************
 * Project:  wxGIS
 * Purpose:  network conection classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2012-2013 Dmitry Barishnikov
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

#include "wx/socket.h"
#include "wx/xml/xml.h"

class INetService;

/** \class wxGISNetServerConnection netconn.h
    \brief The network server connection interface class.
*/
class WXDLLIMPEXP_GIS_NET wxGISNetServerConnection : public INetConnection
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
    //wxGISNetServerConnection(wxSocketBase* sock);
	virtual ~wxGISNetServerConnection(void);
    virtual void SetNetworkService(INetService* pNetService);
    virtual void SetSocket(wxSocketBase* sock);
    virtual bool Destroy(void);
//	virtual void SetAuth(AUTHRESPOND sUserInfo) = 0;//set user authenticated state true
	//virtual AUTHRESPOND GetAuth(void) = 0; // get if user has been authenticated
	//virtual void SetAlive(wxDateTime dtm) = 0;
	//virtual wxDateTime GetAlive(void) = 0;
protected:
    //events
    virtual void OnSocketEvent(wxSocketEvent& event);
    virtual void OnTimer( wxTimerEvent & event);
protected:
    virtual bool ProcessInputNetMessage(void);
protected:
    INetService* m_pNetService;
    wxTimer m_timer;
private:
    DECLARE_EVENT_TABLE()
};


/** \class INetService netconn.h
    \brief The network service base interface class.
*/
class INetService
{
public:
    virtual ~INetService(void){};
    virtual bool Start(void) = 0;
    virtual bool Stop(void) = 0;
    virtual wxString GetServerName(void) const = 0;
    virtual bool CanConnect(const wxString &sName, const wxString &sPass) = 0;
    virtual void AddConnection(wxGISNetServerConnection* pConn) = 0;
    virtual bool DestroyConnection( wxGISNetServerConnection* pConn ) = 0;
    virtual void RemoveConnection( wxGISNetServerConnection* pConn ) = 0;
};

/** \class wxGISNetClientConnection netconn.h
    \brief The network connection interface class.

	This class describes the network connection. Used in server connection list.
*/
class WXDLLIMPEXP_GIS_NET wxGISNetClientConnection : public INetConnection
{
    DECLARE_ABSTRACT_CLASS(wxGISNetClientConnection)
public:
    wxGISNetClientConnection(void);
    virtual ~wxGISNetClientConnection(void);
    virtual bool HasAttributes(void) const;
	//pure virtual
    /** \fn wxXmlNode* GetAttributes(void)
     *  \brief Get Properties of plugin.
     *  \return The properties of the plugin
	 *
	 *  It should be the new wxXmlNode (not a copy of setted properties)
     */
	virtual wxJSONValue GetAttributes(void) const = 0;
    /** \fn void SetAttributes(const wxXmlNode* pProp)
     *  \brief Set Properties of plugin.
     *  \param pProp The properties of the plugin
	 *
	 *  Executed while LoadChildren (after connection class created).
     */
	virtual bool SetAttributes(const wxJSONValue& oProperties) = 0;

    virtual wxString GetName(void) const;
    virtual wxString GetLastError(void) const;
 };


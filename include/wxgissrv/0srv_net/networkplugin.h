/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  TCP network server class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010  Bishop
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

#include "wxgissrv/srv_net/netconn.h"

#include "wx/xml/xml.h"

/** \class INetworkPlugin networkplugin.h
    \brief A Server side INetworkPlugin interface.
*/
class WXDLLIMPEXP_GIS_SRVNET INetworkPlugin : public wxEvtHandler
{
    DECLARE_ABSTRACT_CLASS(INetworkPlugin)
public:
    virtual ~INetworkPlugin(void);
    virtual bool Start(INetworkService* pNetService, wxXmlNode* pConfig) = 0;
    virtual bool Stop(void) = 0;
};

/*
#include "wxgissrv/framework/framework.h"

#include "wx/socket.h"

#ifdef __WXMSW__
#define CURROS WIN
#else
#define CURROS LIN
#endif

class wxTCPNetworkPlugin;

// ----------------------------------------------------------------------------
// wxTCPConnectThread
// ----------------------------------------------------------------------------

class wxTCPConnectThread : public wxThread
{
public:
   wxTCPConnectThread(wxTCPNetworkPlugin* pParent);

    // thread execution starts here
    virtual void *Entry();

    // called when the thread exits - whether it terminates normally or is
    // stopped with Delete() (but not when it is Kill()ed!)
    virtual void OnExit();

private:
    wxTCPNetworkPlugin* m_pParent;
	wxSocketServer *m_pServer;
};

// ----------------------------------------------------------------------------
// wxTCPNetworkPlugin
// ----------------------------------------------------------------------------

class wxTCPNetworkPlugin :
	public wxObject,
    public IService
{
	DECLARE_DYNAMIC_CLASS(wxTCPNetworkPlugin)
public:
	wxTCPNetworkPlugin(void);
	virtual ~wxTCPNetworkPlugin(void);
    virtual bool Start(IServerApplication* pApp, wxXmlNode* pConfig);
    virtual bool Stop(void);
    virtual int GetPort(void){return m_nPort;};
    virtual wxString GetAddres(void){return m_sAddr;};
    virtual wxString GetServerName(void){return m_pApp->GetServerName();};
    virtual bool CanAcceptConnection(void){return m_pApp->CanAcceptConnection();};
    virtual bool AddConnection(wxSocketBase* pNewSocket);
    virtual void RemoveConnection(long nID);
protected:
    int m_nPort;
    wxString m_sAddr;
    wxXmlNode* m_pConfig;
    IServerApplication* m_pApp;
    wxTCPConnectThread* m_pWaitThread;
};
*/

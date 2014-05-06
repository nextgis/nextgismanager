/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxGISNetworkService class. Network connection main service class. 
 *           It adds plug-ins of different types of network connections (TCP, Jabber etc.)
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2011 Bishop
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
#include "wxgissrv/srv_networking/networking.h"

class INetworkPlugin;
class wxGISNetworkService;

/** \class wxMsgOutThread server.h
    \brief A Server output message thread.
*/

class wxMsgOutThread : public wxThread
{
public:
	wxMsgOutThread(wxGISNetworkService* pParent);
    virtual void *Entry();
    virtual void OnExit();

private:
    wxGISNetworkService* m_pParent;
};



/** \class wxGISNetworkService network.h
    \brief A Server side Network Service.
*/
class WXDLLIMPEXP_GIS_FRW wxGISNetworkService : 
	public IService,
	public INetMessageReceiver
{
public:
    wxGISNetworkService(void);
    virtual ~wxGISNetworkService(void);
	//IService
    virtual bool Start(IServerApplication* pApp, wxXmlNode* pConfig);
    virtual bool Stop(void);
	//INetMessageReceiver
    virtual void ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode);
	//wxGISNetworkService
    virtual wxString GetServerName(void);
    virtual bool CanAcceptConnection(void);
	virtual bool AddConnection(INetServerConnection* pNetServerConnection);
	virtual void DelConnection(long nConnID);
    virtual void PutInMessage(WXGISMSG msg);//message to server
    virtual void PutOutMessage(WXGISMSG msg);//message to clients
    virtual WXGISMSG GetOutMessage(void);
	virtual void ProcessOutMessage(WXGISMSG msg);
	virtual void SetAuth(AUTHRESPOND sUserInfo);
	virtual AUTHRESPOND GetAuth(long nID);
protected:
	virtual void ClearMessageQueue(void);
protected:
    wxXmlNode* m_pConfig;
    IServerApplication* m_pApp;
	std::vector<INetworkPlugin*> m_NetworkPluginArray;
    short m_nMaxConnectoinCount;
    long m_nConnectionCounter, m_nConnectionID;
    wxString m_sServerName;
	std::map<long, INetServerConnection*> m_NetworkConnectionMap;
	typedef std::map<long, INetServerConnection*>::iterator ConnIT;
	wxMsgOutThread* m_pMsgOutThread;
	wxCriticalSection m_CriticalSection;
	WXGISMSGQUEUE m_MsgQueue;
};

/** \class INetworkPlugin network.h
    \brief A Server side INetworkPlugin interface.
*/
class INetworkPlugin
{
public:
    virtual ~INetworkPlugin(void){};
    virtual bool Start(wxGISNetworkService* pNetService, wxXmlNode* pConfig) = 0;
    virtual bool Stop(void) = 0;
};

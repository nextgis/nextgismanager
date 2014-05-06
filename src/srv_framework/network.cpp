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

#include "wxgissrv/srv_framework/network.h"

#include "wx/socket.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////
// wxMsgOutThread
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxMsgOutThread::wxMsgOutThread(wxGISNetworkService* pParent) : wxThread(), m_pParent(NULL)
{
	m_pParent = pParent;
}

void wxMsgOutThread::OnExit()
{
	m_pParent = NULL;
}

void *wxMsgOutThread::Entry()
{
	if(m_pParent == NULL)
		return (ExitCode)-1;

	while(!TestDestroy())
	{
		WXGISMSG msg = m_pParent->GetOutMessage();
		while(msg.pMsg)    
		{      
			m_pParent->ProcessOutMessage(msg);
			msg = m_pParent->GetOutMessage();
		}		
		wxThread::Sleep(50);
	}
    return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////
// wxGISNetworkService
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxGISNetworkService::wxGISNetworkService(void) : m_pConfig(NULL), m_pApp(NULL), m_nMaxConnectoinCount(0), m_nConnectionCounter(0), m_nConnectionID(0)
{
}

wxGISNetworkService::~wxGISNetworkService(void)
{
}

bool wxGISNetworkService::Start(IServerApplication* pApp, wxXmlNode* pConfig)
{
    m_pConfig = pConfig;
    m_pApp = pApp;

	if(pConfig == NULL || pConfig->GetName() != wxT("network"))
		return false;

	//Start out thread
	wxLogDebug(wxT("Start out thread"));
	m_pMsgOutThread = new wxMsgOutThread(this);
	if(!CreateAndRunThread(m_pMsgOutThread, wxT("wxGISNetworkService"), wxT("Out messages")))
		return false;
	else
		wxLogMessage(wxString::Format(_("wxGISNetworkService: %s Queue thread 0x%lx started"), wxT("Out messages"), m_pMsgOutThread->GetId()));

    m_nMaxConnectoinCount = wxAtoi(pConfig->GetPropVal(wxT("max_conn"), wxT("10")));
    m_sServerName = pConfig->GetPropVal(wxT("server_name"), wxT("Nemo"));

	wxXmlNode *child = pConfig->GetChildren();
	while(child) 
	{
		wxString sName = child->GetPropVal(wxT("name"), wxT(""));
		if(!sName.IsEmpty())
		{			
			wxObject *pObj = wxCreateDynamicObject(sName);
			INetworkPlugin *pPlugin = dynamic_cast<INetworkPlugin*>(pObj);
			if(pPlugin && pPlugin->Start(this, child))
			{
				m_NetworkPluginArray.push_back( pPlugin );
				wxLogMessage(_("wxGISNetworkService: Plugin %s initialize"), sName.c_str());
			}
			else
				wxLogError(_("wxGISNetworkService: Error initializing plugin %s"), sName.c_str());
		}
		child = child->GetNext();
	}	

	INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pApp);
	if(pNetMessageProcessor)
		pNetMessageProcessor->AddMessageReceiver(wxT("bye"), static_cast<INetMessageReceiver*>(this));

	wxLogMessage(_("wxGISNetworkService: Service started"));
    return true;
}

bool wxGISNetworkService::Stop(void)
{

	for(size_t i = 0; i < m_NetworkPluginArray.size(); i++)
		wgDELETE(m_NetworkPluginArray[i], Stop());

	for(ConnIT it = m_NetworkConnectionMap.begin(); it != m_NetworkConnectionMap.end(); ++it)
		if(it->second)
			it->second->Disconnect();

	//clear messages 
	ClearMessageQueue();

    //2. Stop out thread
    if(m_pMsgOutThread)
        m_pMsgOutThread->Delete();

	wxLogMessage(_("wxGISNetworkService: Service stopped"));
    return true;
}

wxString wxGISNetworkService::GetServerName(void)
{
    return m_sServerName;
}

bool wxGISNetworkService::CanAcceptConnection(void)
{
    return m_nConnectionCounter < m_nMaxConnectoinCount;
}

bool wxGISNetworkService::AddConnection(INetServerConnection* pNetServerConnection)
{
	if(!pNetServerConnection)
		return false;

	long nID = m_nConnectionID;
	m_NetworkConnectionMap[nID] = pNetServerConnection;
	pNetServerConnection->SetUserID(nID);
	m_nConnectionID++;
	m_nConnectionCounter++;
	return pNetServerConnection->Connect();
}

void wxGISNetworkService::DelConnection(long nConnID)
{
	wxDELETE(m_NetworkConnectionMap[nConnID]);
	m_nConnectionCounter--;

	wxLogMessage(_("Peer #%d disconected (%d peers is stil connected)"), nConnID, m_nConnectionCounter);

	////clean conn array
	//for(ConnIT it = m_NetworkConnectionMap.begin(); it != m_NetworkConnectionMap.end(); ++it)
	//	if(it->second != NULL && !it->second->IsConnected())
	//		wxDELETE(it->second);
}

void wxGISNetworkService::PutInMessage(WXGISMSG msg)
{
	//check permissions
    if(msg.pMsg->GetState() == enumGISMsgStCmd)
    {
	    AUTHRESPOND respond = GetAuth(msg.nUserID);
	    if(!respond.bIsValid)
        {
            return;
        }
    }
	m_pApp->PutInMessage(msg);
}

void wxGISNetworkService::PutOutMessage(WXGISMSG msg)
{
    wxCriticalSectionLocker locker(m_CriticalSection);
    m_MsgQueue.push(msg);
}

void wxGISNetworkService::ClearMessageQueue(void)
{
    wxCriticalSectionLocker locker(m_CriticalSection);
    while( m_MsgQueue.size() > 0 )
    {
	    WXGISMSG Msg = m_MsgQueue.top();
	    m_MsgQueue.pop();  
    }
}

WXGISMSG wxGISNetworkService::GetOutMessage(void)
{
    wxCriticalSectionLocker locker(m_CriticalSection);
	WXGISMSG Msg = {INetMessageSPtr(), wxNOT_FOUND};
	if(m_MsgQueue.size() > 0)
	{
		Msg = m_MsgQueue.top();
		m_MsgQueue.pop();  
	}
	return Msg;
}

void wxGISNetworkService::ProcessOutMessage(WXGISMSG msg)
{
	if(!msg.pMsg)
		return;
	if(msg.nUserID == wxNOT_FOUND)
	{
		for(ConnIT it = m_NetworkConnectionMap.begin(); it != m_NetworkConnectionMap.end(); ++it)
			if(it->second != NULL && it->second->IsConnected())
				it->second->PutOutMessage(msg);
	}
	else
	{
		if(m_NetworkConnectionMap[msg.nUserID])
			m_NetworkConnectionMap[msg.nUserID]->PutOutMessage(msg);
	}
}

void wxGISNetworkService::SetAuth(AUTHRESPOND stUserInfo)
{
	if(m_NetworkConnectionMap[stUserInfo.nUserID])
		m_NetworkConnectionMap[stUserInfo.nUserID]->SetAuth(stUserInfo);
}

AUTHRESPOND wxGISNetworkService::GetAuth(long nID)
{
	AUTHRESPOND respond;
	respond.bIsValid = false;
	if(m_NetworkConnectionMap[nID])
		respond = m_NetworkConnectionMap[nID]->GetAuth();
	return respond;
}

void wxGISNetworkService::ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode)
{
	if(!msg.pMsg)
		return;
	if(msg.pMsg->GetState() == enumGISMsgStBye)
	{
		if(m_NetworkConnectionMap[msg.nUserID])
			m_NetworkConnectionMap[msg.nUserID]->Disconnect();
	}
	if(msg.pMsg->GetState() == enumGISMsgStAlive)
	{
		if(m_NetworkConnectionMap[msg.nUserID])
			m_NetworkConnectionMap[msg.nUserID]->SetAlive(wxDateTime::Now());
	}
}
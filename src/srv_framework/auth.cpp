/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxGISAuthService class. Auth main service class. 
 *           It adds plug-ins of different types of authenticate (file, LDAP, OS etc.)
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

#include "wxgissrv/srv_framework/auth.h"
/*
/////////////////////////////////////////////////////////////////////////////////////////////////////
// wxGISAuthService
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxGISAuthService::wxGISAuthService(void) : m_pConfig(NULL), m_pApp(NULL)
{
}

wxGISAuthService::~wxGISAuthService(void)
{
}

bool wxGISAuthService::Start(IServerApplication* pApp, wxXmlNode* pConfig)
{
    m_pConfig = pConfig;
    m_pApp = pApp;

	if(pConfig == NULL || pConfig->GetName() != wxT("auth"))
		return false;

	wxXmlNode *child = pConfig->GetChildren();
	while(child) 
	{
		wxString sName = child->GetPropVal(wxT("name"), wxT(""));
		if(!sName.IsEmpty())
		{			
			short nPriority = wxAtoi(child->GetPropVal(wxT("priority"), wxT("25")));//default lowest
			wxObject *pObj = wxCreateDynamicObject(sName);
			IServerAuthPlugin *pPlugin = dynamic_cast<IServerAuthPlugin*>(pObj);
			if(pPlugin && pPlugin->Init(child))
			{
				m_AuthPluginMap[nPriority] = pPlugin;
				wxLogMessage(_("wxGISAuthService: Plugin %s initialize"), sName.c_str());
			}
			else
				wxLogError(_("wxGISAuthService: Error initializing plugin %s"), sName.c_str());
		}
		child = child->GetNext();
	}	

	INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pApp);
	if(pNetMessageProcessor)
		pNetMessageProcessor->AddMessageReceiver(wxT("auth"), static_cast<INetMessageReceiver*>(this));

	wxLogMessage(_("wxGISAuthService: Service started"));
    return true;
}

bool wxGISAuthService::Stop(void)
{
	INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pApp);
	if(pNetMessageProcessor)
		pNetMessageProcessor->DelMessageReceiver(wxT("auth"), static_cast<INetMessageReceiver*>(this));

	for(std::map<short, IServerAuthPlugin*>::iterator it = m_AuthPluginMap.begin(); it != m_AuthPluginMap.end(); ++it)
		wxDELETE(it->second);

	wxLogMessage(_("wxGISAuthService: Service stopped"));
    return true;
}

void wxGISAuthService::ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode)
{
	if(pChildNode)
	{
		wxString sLogin, sCryptPass;
		sLogin = pChildNode->GetPropVal(wxT("user"), NONAME);
		sCryptPass = pChildNode->GetPropVal(wxT("pass"), ERR);
		for(std::map<short, IServerAuthPlugin*>::iterator it = m_AuthPluginMap.begin(); it != m_AuthPluginMap.end(); ++it)
		{
			AUTHRESPOND respond = it->second->GetAuth(sLogin, sCryptPass);
			if(respond.bIsValid)
			{
				respond.nUserID = msg.nUserID;
				m_pApp->SetAuth(respond);
				wxString sMsg = wxString::Format(WXNETMESSAGE2, WXNETVER, enumGISMsgStOk, enumGISPriorityHigh, wxT("auth"));
				wxNetMessage* pMsg = new wxNetMessage(sMsg);
				WXGISMSG outmsg = {INetMessageSPtr(static_cast<INetMessage*>(pMsg)), msg.nUserID};
				m_pApp->PutOutMessage(outmsg);
				return;
			}
		}
		wxLogError(_("Login failed: user '%s'"), sLogin.c_str());
	}
	wxString sMsg = wxString::Format(WXNETMESSAGE2, WXNETVER, enumGISMsgStRefuse, enumGISPriorityHigh, wxT("auth"));
	wxNetMessage* pMsg = new wxNetMessage(sMsg);
	WXGISMSG outmsg = {INetMessageSPtr(static_cast<INetMessage*>(pMsg)), msg.nUserID};
	m_pApp->PutOutMessage(outmsg);
}
*/

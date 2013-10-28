/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  wxGxServerCatalog class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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

#include "wxgissrv/srv_framework/catalog.h"
/*
wxRxCatalog::wxRxCatalog() : wxGxCatalog()
{
}

wxRxCatalog::~wxRxCatalog()
{
}

void wxRxCatalog::Init(void)
{
	if(m_bIsChildrenLoaded)
		return;

#ifdef WXGISPORTABLE
	m_pConf = new wxGISConfig(wxString(wxT("wxServerCatalog")), CONFIG_DIR, true);
#else
	m_pConf = new wxGISConfig(wxString(wxT("wxServerCatalog")), CONFIG_DIR);
#endif

	//loads current user and when local machine items
	wxXmlNode* pObjectFactoriesNode = m_pConf->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/objectfactories")));
	LoadObjectFactories(pObjectFactoriesNode);
	pObjectFactoriesNode = m_pConf->GetConfigNode(enumGISHKLM, wxString(wxT("catalog/objectfactories")));
	LoadObjectFactories(pObjectFactoriesNode);

	//loads current user and when local machine items
	wxXmlNode* pRootItemsNode = m_pConf->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/rootitems")));
	LoadChildren(pRootItemsNode);
	pRootItemsNode = m_pConf->GetConfigNode(enumGISHKLM, wxString(wxT("catalog/rootitems")));
	LoadChildren(pRootItemsNode);

	wxXmlNode* pConfXmlNode = m_pConf->GetConfigNode(wxString(wxT("catalog")), false, true);
	if(!pConfXmlNode)
		return;
	m_bShowHidden = wxAtoi(pConfXmlNode->GetPropVal(wxT("show_hidden"), wxT("0")));
	m_bShowExt = wxAtoi(pConfXmlNode->GetPropVal(wxT("show_ext"), wxT("1")));
}

bool wxRxCatalog::Start(IServerApplication* pApp, wxXmlNode* pConfig)
{
    //m_pConfig = pConfig;???
    m_pApp = pApp;

	Init();

	INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pApp);
	if(pNetMessageProcessor)
		pNetMessageProcessor->AddMessageReceiver(GetName(), static_cast<INetMessageReceiver*>(this));

	wxLogMessage(_("wxGxServerCatalog: Service started"));
    return true;
}

bool wxRxCatalog::Stop(void)
{
	INetMessageProcessor* pNetMessageProcessor = dynamic_cast<INetMessageProcessor*>(m_pApp);
	if(pNetMessageProcessor)
		pNetMessageProcessor->DelMessageReceiver(GetName(), static_cast<INetMessageReceiver*>(this));

	Detach();
	wxLogMessage(_("wxGxServerCatalog: Service stopped"));
    return true;
}

void wxRxCatalog::ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode)
{
	switch(msg.pMsg->GetState())
	{
		case enumGISMsgStGet:
			//no restrictions yet
			if(pChildNode)
			{
				wxString sName = pChildNode->GetName();
				if(sName.CmpNoCase(wxT("children")) == 0)//return children
				{
					if(HasChildren())
					{
						//sent children count
						wxString sBeginMsg = wxString::Format(wxT("<children count=\"%d\"/>"), m_Children.size()); 
						wxNetMessage* pMsg = new wxNetMessage(wxString::Format(WXNETMESSAGE1, WXNETVER, enumGISMsgStRefuse, enumGISPriorityNormal + 10, GetName(), sBeginMsg.c_str()));
						WXGISMSG outmsg = {INetMessageSPtr(static_cast<INetMessage*>(pMsg)), msg.nUserID};
						m_pApp->PutOutMessage(outmsg);
						if(m_Children.size() == 0)
							break;
						pMsg = new wxNetMessage(enumGISMsgStOk, enumGISPriorityNormal, GetName());
						wxXmlNode* pRoot = pMsg->GetRoot();
						if(!pRoot)
						{
							wxDELETE(pMsg);
							break;
						}
						for(size_t i = 0; i < m_Children.size(); i++)
						{
							IRxObjectServer* pRxObject = dynamic_cast<IRxObjectServer*>(m_Children[i]);
							if(pRxObject)
							{
								wxXmlNode* pDesc = pRxObject->GetDescription();
								pRoot->AddChild(pDesc);
							}
						}
						//send it
						outmsg.pMsg = INetMessageSPtr(static_cast<INetMessage*>(pMsg));
						m_pApp->PutOutMessage(outmsg);
					}
					else
					{
						wxNetMessage* pMsg = new wxNetMessage(wxString::Format(WXNETMESSAGE1, WXNETVER, enumGISMsgStRefuse, enumGISPriorityNormal + 10, GetName(), wxString(wxT("<children count=\"0\"/>")).c_str()));
						WXGISMSG outmsg = {INetMessageSPtr(static_cast<INetMessage*>(pMsg)), msg.nUserID};
						m_pApp->PutOutMessage(outmsg);
					}
				}
			}
			break;
		case enumGISMsgStCmd:
			{
				wxNetMessage* pMsg = new wxNetMessage(wxString::Format(WXNETMESSAGE2, WXNETVER, enumGISMsgStRefuse, enumGISPriorityNormal + 10, GetName()));
				WXGISMSG outmsg = {INetMessageSPtr(static_cast<INetMessage*>(pMsg)), msg.nUserID};
				m_pApp->PutOutMessage(outmsg);
		}
			break;
		default:
			break;
	}
	//wxString sMsg = wxString::Format(WXNETMESSAGE2, WXNETVER, enumGISMsgStRefuse, enumGISPriorityHigh, wxT("auth"));
	//wxNetMessage* pMsg = new wxNetMessage(sMsg);
	//	WXGISMSG outmsg = {pMsg, msg.nUserID};
	//	m_pApp->PutOutMessage(outmsg);
}

void wxRxCatalog::LoadChildren(wxXmlNode* pNode)
{
	if(!pNode)
		return;

	wxXmlNode* pChildren = pNode->GetChildren();
	while(pChildren)
	{
		wxString sCatalogRootItemName = pChildren->GetPropVal(wxT("name"), NONAME);
        bool bIsEnabled = wxAtoi(pChildren->GetPropVal(wxT("is_enabled"), wxT("1")));
        //bool bShowClient = wxAtoi(pChildren->GetPropVal(wxT("show_client"), wxT("1")));
        bool bContin = false;

		for(size_t i = 0; i < m_CatalogRootItemArray.Count(); i++)
		{
			if(m_CatalogRootItemArray[i].IsSameAs(sCatalogRootItemName, false))
			{
				pChildren = pChildren->GetNext();
                bContin = true;
				break;
			}
		}

        if(bContin)
            continue;

        m_CatalogRootItemArray.Add(sCatalogRootItemName);

		//init plugin and add it
        wxObject *obj = wxCreateDynamicObject(sCatalogRootItemName);
		IGxObject *pGxObject = dynamic_cast<IGxObject*>(obj);
		if(pGxObject != NULL)
		{
            if(bIsEnabled)
            {
                if(AddChild(pGxObject))
                {
                    //m_CatalogRootItemArray.Add(sCatalogRootItemName);
                    IGxRootObjectProperties* pGxRootObjectProperties = dynamic_cast<IGxRootObjectProperties*>(pGxObject);
                    if(pGxRootObjectProperties)
                    {
                        pGxRootObjectProperties->Init(pChildren);
                        pGxRootObjectProperties->SetEnabled(bIsEnabled);
                    }
                    wxGxDiscConnections* pGxDiscConnections = dynamic_cast<wxGxDiscConnections*>(pGxObject);
                    if(pGxDiscConnections)
                        m_pGxDiscConnections = pGxDiscConnections;

					IRxObjectServer* pRxObject = dynamic_cast<IRxObjectServer*>(pGxObject);
					if(pRxObject)
						pRxObject->SetApplication(m_pApp);

				    wxLogMessage(_("wxRxCatalog: Root Object %s initialize"), sCatalogRootItemName.c_str());
                }
				else
					wxDELETE(pGxObject);
            }
            else
            {
                pGxObject->Attach(this, this);
                IGxRootObjectProperties* pGxRootObjectProperties = dynamic_cast<IGxRootObjectProperties*>(pGxObject);
                if(pGxRootObjectProperties)
                {
                    pGxRootObjectProperties->Init(pChildren);
                    pGxRootObjectProperties->SetEnabled(bIsEnabled);
                }
                m_aRootItems.push_back(pGxObject);
            }
		}
		else
			wxLogError(_("wxRxCatalog: Error initializing Root Object %s"), sCatalogRootItemName.c_str());

		pChildren = pChildren->GetNext();
	}
	m_bIsChildrenLoaded = true;
}
*/
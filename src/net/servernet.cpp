/******************************************************************************
 * Project:  wxGIS
 * Purpose:  network server class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010,2014 Dmitry Baryshnikov
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

#include "wxgis/net/servernet.h"
#include "wxgis/core/config.h"
#include "wxgis/core/app.h"

static wxGISNetworkService *g_pNetworkService( NULL );

extern WXDLLIMPEXP_GIS_NET wxGISNetworkService* const GetNetworkService(void)
{
	return g_pNetworkService;
}

extern WXDLLIMPEXP_GIS_NET void SetNetworkService(wxGISNetworkService* pService)
{
    if(g_pNetworkService != NULL)
        wxDELETE(g_pNetworkService);	
    g_pNetworkService = pService;
}

#if wxUSE_STD_CONTAINERS

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxGISNetServerConnectionList)

#else // !wxUSE_STD_CONTAINERS

void wxGISNetServerConnectionNode::DeleteData()
{
    delete (wxGISNetServerConnection *)GetData();
}

#endif // wxUSE_STD_CONTAINERS/!wxUSE_STD_CONTAINERS

//----------------------------------------------------------------------------
// INetPlugin
//----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(INetPlugin, wxEvtHandler)

INetPlugin::~INetPlugin(void)
{
}

//----------------------------------------------------------------------------
// wxGISNetworkService
//----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISNetworkService, wxEvtHandler)

BEGIN_EVENT_TABLE(wxGISNetworkService, wxEvtHandler)
  EVT_GISNET_MSG(wxGISNetworkService::OnGISNetEvent)
END_EVENT_TABLE()

wxGISNetworkService::wxGISNetworkService(INetEventProcessor* pNetEvtProc)
{
    m_nConnectionCounter = 0;
    m_nConnectionIdCounter = 0;
    m_pNetEvtProc = pNetEvtProc;
}

wxGISNetworkService::~wxGISNetworkService()
{
}

bool wxGISNetworkService::Start()
{
    wxGISAppConfig oConfig = GetConfig();
    if(!oConfig.IsOk())
    {
        wxLogError(_("wxGISNetworkService: Error get config"));
        return false;
    }

    //adwise to catalog events
    wxString sAppName(wxT("wxGISCommon"));
    if(GetApplication())
        sAppName = GetApplication()->GetAppName();
	wxString sPrepend = sAppName + wxString(wxT("/network"));
    m_nMaxConnectoinCount = oConfig.ReadInt(enumGISHKLM, sPrepend + wxString(wxT("/max_conn")), 5);
    m_sServerName = oConfig.Read(enumGISHKLM, sPrepend + wxString(wxT("/server_name")), wxT("NextGIS Server"));
    wxXmlNode *pNetworkNode = oConfig.GetConfigNode(enumGISHKLM, sPrepend);
    if(pNetworkNode)
    {
	    wxXmlNode *child = pNetworkNode->GetChildren();
	    while(child) 
	    {
            wxString sName = child->GetAttribute(wxT("name"), wxT(""));
            AddNetworkPlugin(sName, child);
		    child = child->GetNext();
	    }
    }
    wxLogVerbose(_("wxGISNetworkService: Service started"));

    return true;
}

void wxGISNetworkService::AddNetworkPlugin(const wxString &sClassName, const wxXmlNode *pConfigNode)
{
    wxCHECK_RET(!sClassName.IsEmpty(), wxT("The class name is empty"));

    //check if plugin is already loaded
    for(size_t i = 0; i < m_paNetworkPlugins.size(); ++i)
    {
        if(sClassName.IsSameAs(m_paNetworkPlugins[i]->GetClassInfo()->GetClassName()))
        {
            wxLogDebug(_("wxGISNetworkService: Plugin already loaded %s"), sClassName.c_str());
            return;
        }
    }
    		
	wxObject *pObj = wxCreateDynamicObject(sClassName);
	INetPlugin *pPlugin = dynamic_cast<INetPlugin*>(pObj);
	if(pPlugin && pPlugin->Start(this, pConfigNode))
	{
		m_paNetworkPlugins.push_back( pPlugin );
		wxLogMessage(_("Plugin %s initialize"), sClassName.c_str());
	}
	else
		wxLogError(_("Error initializing plugin %s"), sClassName.c_str());
}

bool wxGISNetworkService::Stop()
{
    DestroyConnections();

	for(size_t i = 0; i < m_paNetworkPlugins.size(); ++i)
		wgDELETE(m_paNetworkPlugins[i], Stop());

	wxLogVerbose(_("Service stopped"));
    return true;
}

wxString wxGISNetworkService::GetServerName(void) const
{
    return m_sServerName;
}

bool wxGISNetworkService::CanConnect(const wxString &sName, const wxString &sPass)
{
    return m_nConnectionCounter < m_nMaxConnectoinCount;
}

void wxGISNetworkService::AddConnection( wxGISNetServerConnection* pConn )
{
    wxCHECK_RET( pConn, wxT("can't add a NULL pConn") );

    // this should never happen and it will lead to a crash later if it does
    // because RemoveChild() will remove only one node from the children list
    // and the other(s) one(s) will be left with dangling pointers in them
    wxASSERT_MSG( !GetConnections().Find(pConn), wxT("AddConnection() called twice") );

    GetConnections().Append(pConn);
    m_nConnectionIdCounter++;
    m_nConnectionCounter++;   
    pConn->SetNetworkService(this);
    pConn->SetId(m_nConnectionIdCounter);
    pConn->Advise(this);
}

void wxGISNetworkService::RemoveConnection( wxGISNetServerConnection* pConn )
{
    wxCHECK_RET( pConn, wxT("can't remove a NULL pConn") );

    GetConnections().DeleteObject(pConn);
    pConn->SetNetworkService(NULL);
    m_nConnectionCounter--;
}

bool wxGISNetworkService::DestroyConnection( wxGISNetServerConnection* pConn )
{
    wxCHECK_MSG( pConn, false, wxT("can't remove a NULL pConn") );

    GetConnections().DeleteObject(pConn);
    pConn->Destroy();
    return true;
}

bool wxGISNetworkService::DestroyConnections()
{

    wxGISNetServerConnectionList::compatibility_iterator node;
    for ( ;; )
    {
        // we iterate until the list becomes empty
        node = GetConnections().GetFirst();
        if ( !node )
            break;

        wxGISNetServerConnection *pConn = node->GetData();
        pConn->Destroy();

        wxASSERT_MSG( !GetConnections().Find(pConn), wxT("child didn't remove itself using RemoveChild()") );
    }
    return true;
}

void wxGISNetworkService::OnGISNetEvent(wxGISNetEvent& event)
{
/*    //send message to catalog
    INetEventProcessor* pNetEvtProc = dynamic_cast<INetEventProcessor*>(GetGxCatalog());//TODO: #include "wxgis/catalog/gxobject.h"
    if(pNetEvtProc)
        pNetEvtProc->ProcessNetEvent(event);
//    wxRxCatalog* pCat = wxDynamicCast(GetGxCatalog(), wxRxCatalog);
//    pCat->ProcessNetEvent(event);*/
    if(NULL != m_pNetEvtProc)
        m_pNetEvtProc->ProcessNetEvent(event);
}

void wxGISNetworkService::SendNetMessage(const wxNetMessage & msg, int nId)
{
    if(nId == wxNOT_FOUND) // send all connections
    {
        wxGISNetServerConnectionList::const_iterator iter;
        for(iter = GetConnections().begin(); iter != GetConnections().end(); ++iter)
        {
            wxGISNetServerConnection *current = *iter;
            if(current)
            {
                current->SendNetMessageAsync(msg);
            }
        }
    }
    else //search connection by id
    {
        wxGISNetServerConnectionList::const_iterator iter;
        for(iter = GetConnections().begin(); iter != GetConnections().end(); ++iter)
        {
            wxGISNetServerConnection *current = *iter;
            if(current && current->GetId() == nId)
            {
                current->SendNetMessageAsync(msg);
                return;
            }
        }    
    }
}

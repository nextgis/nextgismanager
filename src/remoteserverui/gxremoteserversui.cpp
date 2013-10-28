/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServersUI class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012 Bishop
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

#include "wxgis/remoteserverui/gxremoteserversui.h"
#include "wxgis/remoteserverui/createnetworkconndlg.h"
#include "wxgis/remoteserverui/gxremoteserverui.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalogui/gxview.h"

#include "../../art/remoteservers_16.xpm"
#include "../../art/remoteservers_48.xpm"
#include "../../art/remoteserverauth_16.xpm"
#include "../../art/remoteserverauth_48.xpm"
#include "../../art/remoteserver_16.xpm"
#include "../../art/remoteserver_48.xpm"

IMPLEMENT_DYNAMIC_CLASS(wxGxRemoteServersUI, wxGxRemoteServers)

wxGxRemoteServersUI::wxGxRemoteServersUI(void) : wxGxRemoteServers()
{
}

wxGxRemoteServersUI::~wxGxRemoteServersUI(void)
{
}

wxIcon wxGxRemoteServersUI::GetLargeImage(void)
{
	if(!m_RemServs48.IsOk())
		m_RemServs48 = wxIcon(remoteservers_48_xpm);
	return m_RemServs48;
}

wxIcon wxGxRemoteServersUI::GetSmallImage(void)
{
	if(!m_RemServs16.IsOk())
		m_RemServs16 = wxIcon(remoteservers_16_xpm);
	return m_RemServs16;
}

wxGxObject* wxGxRemoteServersUI::CreateChildGxObject(const wxXmlNode* pNode)
{
    wxCHECK_MSG(pNode, NULL, wxT("input wxXmlNode pointer is NULL"));
    wxString soName = pNode->GetAttribute(wxT("name"), NONAME);
    //check uniq name
    if(!IsUniqName(soName))
    {
        wxString sErr(_("Connection name should be uniq"));
        //wxMessageBox(sErr, wxString(_("Error")), wxICON_ERROR | wxOK | wxCENTRE, pParent);  
        wxLogError(sErr);
        return NULL;    
    }

    wxString soClass = pNode->GetAttribute(wxT("class"), NONAME);
    for(size_t i = 0; i < m_apNetConnFact.GetCount(); ++i)
    {
        if(soClass.IsSameAs(m_apNetConnFact[i].GetClassInfo()->GetClassName()))
        {
            wxGISNetClientConnection* pConn = m_apNetConnFact[i].GetConnection(pNode);
            if(pConn)
            {
                if(!m_RemServ16.IsOk())
                    m_RemServ16 = wxIcon(remoteserver_16_xpm);
                if(!m_RemServ48.IsOk())
                    m_RemServ48 = wxIcon(remoteserver_48_xpm);
                if(!m_RemServAuth16.IsOk())
                    m_RemServAuth16 = wxIcon(remoteserverauth_16_xpm);
                if(!m_RemServAuth48.IsOk())
                    m_RemServAuth48 = wxIcon(remoteserverauth_48_xpm);
                if(!m_RemServDsbl16.IsOk())
                {
                    wxBitmap RemServDsbld16 = wxBitmap(remoteserver_16_xpm).ConvertToImage().ConvertToGreyscale();
                    m_RemServDsbl16.CopyFromBitmap(RemServDsbld16);
                }
                if(!m_RemServDsbl48.IsOk())
                {
                    wxBitmap RemServDsbld48 = wxBitmap(remoteserver_48_xpm).ConvertToImage().ConvertToGreyscale();
                    m_RemServDsbl48.CopyFromBitmap(RemServDsbld48);
                }
                return new wxGxRemoteServerUI(pConn, this, soName, "", m_RemServ16, m_RemServ48, m_RemServDsbl16, m_RemServDsbl48, m_RemServAuth16, m_RemServAuth48);
            }
        }
    }
    return NULL;
}

void wxGxRemoteServersUI::CreateConnection(wxWindow* pParent)
{
    wxCriticalSectionLocker locker(m_oCritSect); 
    
    wxGISCreateNetworkConnDlg dlg(m_apNetConnFact, pParent);
	if(dlg.ShowModal() != wxID_SAVE)
        return;

	const wxXmlNode* pConnProps = dlg.GetConnectionProperties();

    if(!pConnProps)
    {
        return;
    }

    //create gxobject for connection properties
    wxGxObject* pObj = CreateChildGxObject(pConnProps);

    if(!pObj)//name is not uniq
    {
        wxString sErr(_("Connection name should be uniq"));
        wxMessageBox(sErr, wxString(_("Error")), wxICON_ERROR | wxOK | wxCENTRE, pParent);  
        wxLogError(sErr);
        return;    
    }

    //send event to gxcatalog
    wxGxCatalog* pGxCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
    if(pGxCatalog)
    {
        pGxCatalog->ObjectAdded(pObj->GetId());
        pGxCatalog->ObjectChanged(GetId());
    }

    //store connection to xml file
    if(!StoreConnectionProperties(pConnProps))
    {
        wxString sErr(_("Connection save failed"));
        wxMessageBox(sErr, wxString(_("Error")), wxICON_ERROR | wxOK | wxCENTRE, pParent);  
        wxLogError(sErr);
        return;    
    }
    //begin rename connection
    //wxGxView* pGxView = dynamic_cast<wxGxView*>(wxWindow::FindFocus());
    //if(pGxView)
    //    pGxView->BeginRename(pObj->GetId());

}

/*
void wxGxRemoteServersUI::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->Detach();
		wxDELETE(m_Children[i]);
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

void wxGxRemoteServersUI::LoadChildren()
{
	if(m_bIsChildrenLoaded)
		return;	

    wxXmlDocument doc;
    if (doc.Load(m_sUserConfig))
    {
        wxXmlNode* pConnectionsNode = doc.GetRoot();
		wxXmlNode* pConnNode = pConnectionsNode->GetChildren();
		while(pConnNode)
		{
		    wxString sClassName = pConnNode->GetAttribute(wxT("class"), ERR);
		    if(!sClassName.IsEmpty())
		    {
			    INetClientConnection *pConn = dynamic_cast<INetClientConnection*>(wxCreateDynamicObject(sClassName));
			    if(pConn && pConn->SetAttributes(pConnNode))
			    {
				    wxGxRemoteServerUI* pServerConn = new wxGxRemoteServerUI(pConn);
				    IGxObject* pGxObj = static_cast<IGxObject*>(pServerConn);
				    if(!AddChild(pGxObj))
				    {
					    wxDELETE(pGxObj);
				    }
				    else //set callback
				    {
					    pConn->SetCallback(static_cast<INetCallback*>(pServerConn));
				    }
			    }
		    }

			pConnNode = pConnNode->GetNext();
		}
    }

	m_bIsChildrenLoaded = true;
}


*/
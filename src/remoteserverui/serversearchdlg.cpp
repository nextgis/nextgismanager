/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGISSearchServerDlg class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012 Bishop
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

#include "wxgis/remoteserverui/serversearchdlg.h"
#include "wxgis/net/network.h"

#include "../../art/remoteservers_16.xpm"
#include "../../art/remoteserver_16.xpm"

//-----------------------------------------------------------------------------
// wxGISSearchServerDlg
//-----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISSearchServerDlg, wxDialog)

BEGIN_EVENT_TABLE( wxGISSearchServerDlg, wxDialog )
	EVT_BUTTON(ID_ACCEPT, wxGISSearchServerDlg::OnAccept )
	EVT_UPDATE_UI(ID_ACCEPT, wxGISSearchServerDlg::OnAcceptUI )
    EVT_BUTTON(ID_SEARCHBT, wxGISSearchServerDlg::OnSearch )
	EVT_UPDATE_UI(ID_SEARCHBT, wxGISSearchServerDlg::OnSearchUI )
	EVT_BUTTON(ID_STOPBT, wxGISSearchServerDlg::OnStop )
	EVT_UPDATE_UI(ID_STOPBT, wxGISSearchServerDlg::OnStopUI )
    EVT_GISNET_MSG(wxGISSearchServerDlg::OnNetMsg)
    /*		EVT_CLOSE(wxGISSearchServerDlg::OnClose)*/
END_EVENT_TABLE()

wxGISSearchServerDlg::wxGISSearchServerDlg(INetConnFactory* pFactory, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	m_pFactory = pFactory;
    m_nConnectionPointCookie = m_pFactory->Advise(this);
	SetSize(size);
	CreateControls(false);
}

wxGISSearchServerDlg::~wxGISSearchServerDlg()
{
    m_pFactory->Unadvise(m_nConnectionPointCookie);

	for(size_t i = 0; i < m_listCtrl->GetItemCount(); ++i)
	{
		wxXmlNode* pNode = (wxXmlNode*)m_listCtrl->GetItemData(i);
		wxDELETE(pNode);
	}
}

void wxGISSearchServerDlg::CreateControls(bool bShowGauge)
{
	m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(remoteserver_16_xpm));
    // set frame icon

	SetIcon(remoteservers_16_xpm);

	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	fgSizer2->AddGrowableCol( 0 );
	fgSizer2->AddGrowableRow( 0 );
	
	m_listCtrl = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_NO_SORT_HEADER | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING );
	m_listCtrl->SetMinSize( wxSize( 250,150 ) );
	m_listCtrl->SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);
	m_listCtrl->InsertColumn(0, _("Server name"), wxLIST_FORMAT_LEFT, 100);
	m_listCtrl->InsertColumn(1, _("Host"), wxLIST_FORMAT_LEFT, 150);
	m_listCtrl->InsertColumn(2, _("Banner"), wxLIST_FORMAT_LEFT, 200);
	m_listCtrl->InsertColumn(3, _("Factory"), wxLIST_FORMAT_LEFT, 60);

	fgSizer2->Add( m_listCtrl, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );

	m_button_search = new wxButton( this, ID_SEARCHBT, _("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button_search, 0, wxALL, 5 );
	
	m_button_stop = new wxButton( this, ID_STOPBT, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button_stop, 0, wxALL, 5 );

	m_button_accept = new wxButton( this, ID_ACCEPT, _("Accept"), wxDefaultPosition, wxDefaultSize, 0 );//(m_pFactory == NULL ? _("Add server") : _("Accept"))
	bSizer4->Add( m_button_accept, 0, wxALL, 5 );
	
	fgSizer2->Add( bSizer4, 1, wxEXPAND, 5 );
	
	bSizer3->Add( fgSizer2, 1, wxEXPAND, 5 );
	/*
	if(bShowGauge)
	{
		m_gauge = new wxGauge( this, wxID_ANY, 254, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_SMOOTH );
		m_gauge->SetMinSize( wxSize( -1, 15 ) );
		bSizer3->Add( m_gauge, 0, wxALL|wxEXPAND, 5 );
	}
	*/
	this->SetSizer( bSizer3 );
	this->Layout();
	
	this->Centre( wxBOTH );
//	wxUpdateUIEvent::SetUpdateInterval(10);
}


void wxGISSearchServerDlg::OnAccept( wxCommandEvent& event )
{
	long item = m_listCtrl->GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	m_pConnProps = (wxXmlNode*)m_listCtrl->GetItemData(item);
	EndModal(ID_ACCEPT);
}

void wxGISSearchServerDlg::OnAcceptUI( wxUpdateUIEvent& event )
{ 
	event.Enable(m_listCtrl->GetSelectedItemCount() == 1);
}


void wxGISSearchServerDlg::OnStop( wxCommandEvent& event )
{ 
	if(m_pFactory)
	{
		if(m_pFactory->CanStopServerSearch())
			m_pFactory->StopServerSearch();
	}
	event.Skip(); 
}

void wxGISSearchServerDlg::OnStopUI( wxUpdateUIEvent& event )
{ 
	if(m_pFactory)
		event.Enable(m_pFactory->IsServerSearching());
}

void wxGISSearchServerDlg::OnSearch( wxCommandEvent& event )
{
	if(m_pFactory)
        m_pFactory->StartServerSearch();
}

void wxGISSearchServerDlg::OnSearchUI( wxUpdateUIEvent& event )
{ 
	if(m_pFactory)
		event.Enable(!m_pFactory->IsServerSearching());
}

void wxGISSearchServerDlg::AddServer(wxXmlNode* pServerData)
{
    wxCHECK_RET(pServerData, wxT("Server data wxXmlNode is NULL"));
	//check duplicates
	for(size_t i = 0; i < m_listCtrl->GetItemCount(); ++i)
	{
		wxXmlNode* pNode = (wxXmlNode*)m_listCtrl->GetItemData(i);
		bool bCmp1 = pServerData->GetAttribute(wxT("name"), NONAME) == pNode->GetAttribute(wxT("name"), NONAME);
		bool bCmp2 = pServerData->GetAttribute(wxT("factory_name"), NONAME) == pNode->GetAttribute(wxT("factory_name"), NONAME);
		bool bCmp3 = pServerData->GetAttribute(wxT("host"), NONAME) == pNode->GetAttribute(wxT("host"), NONAME);
		if(bCmp1 && bCmp2 && bCmp3)
			return;
	}

	long pos = m_listCtrl->InsertItem(-1, pServerData->GetAttribute(wxT("name"), NONAME), 0);	//Server name
	m_listCtrl->SetItem(pos, 1, pServerData->GetAttribute(wxT("host"), NONAME));				//Host
	m_listCtrl->SetItem(pos, 2, pServerData->GetAttribute(wxT("banner"), NONAME));			//Banner
	m_listCtrl->SetItem(pos, 3, pServerData->GetAttribute(wxT("factory_name"), NONAME));		//Module
	m_listCtrl->SetItemData(pos, (long)pServerData);
}

void wxGISSearchServerDlg::OnNetMsg ( wxGISNetEvent& event )
{
    wxNetMessage msg = event.GetNetMessage();
    if(msg.IsOk())
    {
        AddServer(new wxXmlNode(*msg.GetXMLRoot()->GetChildren()));
    }
}

/*
void wxGISSearchServerDlg::OnClose(wxCloseEvent& event)
{
	event.Skip();
	wxCommandEvent ev;
	OnStop(ev);
	Destroy();
}


INetClientConnection* wxGISSearchServerDlg::GetConnection()
{
	if(m_pFactory)
		return m_pFactory->GetConnection(m_pConnProps);
	return NULL;
}
*/

/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  TCP network UI classes.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012 Bishop
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

#include "wxgis/remoteserverui/netfactoryui.h"
#include "wxgis/remoteserverui/serversearchdlg.h"
#include "wxgis/core/crypt.h"

//#include "wxgis/framework/wxregexvalidator.h"

#include "wx/valgen.h"


//----------------------------------------------------------------------------
// wxNetPropertyPage
//----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxNetPropertyPage, wxPanel)

//----------------------------------------------------------------------------
// wxGISTCPClientPanel
//----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISTCPClientPanel, wxNetPropertyPage)

BEGIN_EVENT_TABLE( wxGISTCPClientPanel, wxNetPropertyPage )
	EVT_BUTTON( ID_SEARCHBT, wxGISTCPClientPanel::OnSearchBtnClick )
END_EVENT_TABLE()

wxGISTCPClientPanel::wxGISTCPClientPanel( wxClientTCPNetFactoryUI* pFactory, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size ) : wxNetPropertyPage(pFactory, parent, id, pos, size )
{
	m_pFactory = pFactory;
	if(!m_pFactory)
		return;

	m_sPort = wxString::Format(wxT("%d"), m_pFactory->GetPort());
    m_IPAddress = wxString(wxT("0.0.0.0"));
	m_sName = wxString(_("New connection"));

	SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);

    wxBoxSizer* fgSizerMain = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer1->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    fgSizer1->AddGrowableCol(1);
	
	m_staticText01 = new wxStaticText( this, wxID_ANY, _("Connection name"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText01->Wrap( -1 );
	fgSizer1->Add( m_staticText01, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_textCtr_name = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &m_sName));
	fgSizer1->Add( m_textCtr_name, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );

	m_staticText11 = new wxStaticText( this, wxID_ANY, _("Address"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText11->Wrap( -1 );
	fgSizer1->Add( m_staticText11, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
    wxTextValidator oIPAddrValidator(wxFILTER_INCLUDE_CHAR_LIST , &m_IPAddress);
    oIPAddrValidator.SetCharIncludes(wxT("0123456789."));
	m_textCtr_addr = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, oIPAddrValidator);//wxRegExValidator(wxT("[0-9]{1,3}(.[0-9]{1,3}){3,3}"), &m_IPAddress)
	fgSizer1->Add( m_textCtr_addr, 1, wxALIGN_CENTER_VERTICAL|wxEXPAND|wxALL, 5 );
	
	m_staticText12 = new wxStaticText( this, wxID_ANY, _("Port"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText12->Wrap( -1 );
	fgSizer1->Add( m_staticText12, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtr_port = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_sPort));

	fgSizer1->Add( m_textCtr_port, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5 );
	
	fgSizerMain->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	m_button_search = new wxButton( this, ID_SEARCHBT, _("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizerMain->Add( m_button_search, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	wxStaticBoxSizer* sbSizer2;
	sbSizer2 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Authentication") ), wxVERTICAL );
	
	wxFlexGridSizer* fgSubSizer;
	fgSubSizer = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSubSizer->SetFlexibleDirection( wxHORIZONTAL );
	fgSubSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
    fgSubSizer->AddGrowableCol(1);
	
	m_staticText13 = new wxStaticText( this, wxID_ANY, _("User"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText13->Wrap( -1 );
	fgSubSizer->Add( m_staticText13, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrl_user = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxEXPAND, wxGenericValidator(&m_UserName) );
	fgSubSizer->Add( m_textCtrl_user, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText14 = new wxStaticText( this, wxID_ANY, _("Password"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText14->Wrap( -1 );
	fgSubSizer->Add( m_staticText14, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_textCtrl_pass = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD | wxEXPAND, wxGenericValidator(&m_Password)  );
	fgSubSizer->Add( m_textCtrl_pass, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	sbSizer2->Add( fgSubSizer, 0, wxEXPAND, 5 );
	
	fgSizerMain->Add( sbSizer2, 0, wxEXPAND, 5 );
	
	this->SetSizer( fgSizerMain );
	this->Layout();
	TransferDataToWindow();
}

wxGISTCPClientPanel::~wxGISTCPClientPanel()
{
}

void wxGISTCPClientPanel::OnSearchBtnClick( wxCommandEvent& event )
{
	TransferDataFromWindow();
	wxGISSearchServerDlg dlg(m_pFactory, this);
	if(dlg.ShowModal() == wxGISSearchServerDlg::ID_ACCEPT)
	{
		wxXmlNode* pProps = dlg.GetConnectionProperties();
		if(pProps)
		{
			m_sPort = pProps->GetAttribute(wxT("port"), m_sPort);
			m_sName = pProps->GetAttribute(wxT("name"), m_sName);
			m_IPAddress = pProps->GetAttribute(wxT("host"), m_IPAddress);
		}
		TransferDataToWindow();
	}
	event.Skip();
}

wxXmlNode* wxGISTCPClientPanel::OnSave(void)
{
	if(!TransferDataFromWindow())
	{
		m_sErrorMsg = wxString(_("Some input values are not correct!"));
		return NULL;
	}

	wxXmlNode* pConnNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("conn_info"));
    pConnNode->AddAttribute(wxT("class"), m_pFactory->GetClassInfo()->GetClassName());
	pConnNode->AddAttribute(wxT("port"), m_sPort);
	pConnNode->AddAttribute(wxT("name"), m_sName);
	pConnNode->AddAttribute(wxT("host"), m_IPAddress);
	pConnNode->AddAttribute(wxT("user"), m_UserName);
    wxString sCryptPass;
    if(!m_Password.IsEmpty() && Crypt(m_Password, sCryptPass))
        pConnNode->AddAttribute(wxT("pass"), sCryptPass);
	
	return pConnNode;
}
/*
void wxGISTCPClientPanel::ShowSearch(bool bShow)
{
}
*/

//----------------------------------------------------------------------------
// wxClientTCPNetFactoryUI
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClientTCPNetFactoryUI, wxClientTCPNetFactory)

wxClientTCPNetFactoryUI::wxClientTCPNetFactoryUI(void) : wxClientTCPNetFactory()
{
}

wxClientTCPNetFactoryUI::~wxClientTCPNetFactoryUI(void)
{
}

wxWindow* wxClientTCPNetFactoryUI::GetPropertyPage(wxWindow* pParent)
{
	return new wxGISTCPClientPanel(this, pParent);
}

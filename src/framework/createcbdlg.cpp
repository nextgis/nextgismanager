/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  create menu or toolbar dialog.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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

#include "wxgis/framework/createcbdlg.h"
#include <wx/valgen.h>

///////////////////////////////////////////////////////////////////////////

wxGISCreateCommandBarDlg::wxGISCreateCommandBarDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	//this->SetSizeHints( wxDefaultSize, wxSize( -1,162 ) );
	this->SetSizeHints( wxSize( 246,280 ), wxSize( -1,280 ) );

	srand ( time(NULL) );
	int random_number = (rand() % 512); 
	m_sCommandbarName = wxString::Format(wxT("Custom.CommandBar#%d"), random_number);
	m_sCommandbarCaption = wxString(_("New CommandBar"));

	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 5, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Name"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText4->Wrap( -1 );
	fgSizer2->Add( m_staticText4, 0, wxALL|wxEXPAND, 5 );
	
	m_textCtrl3 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 , wxGenericValidator(&m_sCommandbarName));
	fgSizer2->Add( m_textCtrl3, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText5 = new wxStaticText( this, wxID_ANY, _("Caption"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText5->Wrap( -1 );
	fgSizer2->Add( m_staticText5, 0, wxALL|wxEXPAND, 5 );
	
	m_textCtrl4 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 , wxGenericValidator(&m_sCommandbarCaption));
	fgSizer2->Add( m_textCtrl4, 1, wxALL|wxEXPAND, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Type"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText6->Wrap( -1 );
	fgSizer2->Add( m_staticText6, 0, wxALL|wxEXPAND, 5 );
	
	wxString m_choice1Choices[] = { _("Menu"), _("Context menu"), _("ToolBar") };
	int m_choice1NChoices = sizeof( m_choice1Choices ) / sizeof( wxString );
	m_choice1 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1NChoices, m_choice1Choices, 0 , wxGenericValidator(&m_nType));
	m_choice1->SetSelection( 0 );
	fgSizer2->Add( m_choice1, 1, wxALL|wxEXPAND, 5 );

	m_staticText41 = new wxStaticText( this, wxID_ANY, _("Left dockable"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText41->Wrap( -1 );
	fgSizer2->Add( m_staticText41, 0, wxALL|wxEXPAND, 5 );
	
	m_checkLeftD = new wxCheckBox( this, wxID_ANY, wxT(" "), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bLeftDockable) );
	
	fgSizer2->Add( m_checkLeftD, 0, wxALL, 5 );
	
	m_staticText51 = new wxStaticText( this, wxID_ANY, _("Right dockable"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText51->Wrap( -1 );
	fgSizer2->Add( m_staticText51, 0, wxALL|wxEXPAND, 5 );
	
	m_checkRightD = new wxCheckBox( this, wxID_ANY, wxT(" "), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bRightDockable) );
	
	fgSizer2->Add( m_checkRightD, 0, wxALL, 5 );
	
	bSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( this, wxID_OK, _("OK") );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL, _("Cancel") );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer1->Add( m_sdbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

wxGISCreateCommandBarDlg::~wxGISCreateCommandBarDlg()
{
}

bool wxGISCreateCommandBarDlg::TransferDataFromWindow()
{
	bool bRetCode = wxDialog::TransferDataFromWindow();
	switch(m_nType)
	{		
	case 0:
		m_CommandbarType = enumGISCBMenubar;
		break;
	case 1:
		m_CommandbarType = enumGISCBContextmenu;
		break;
	case 2:
		m_CommandbarType = enumGISCBToolbar;
		break;
	default:
		//enumGISCBSubMenu
		m_CommandbarType = enumGISCBNone;
		break;
	}
	return bRetCode;
}

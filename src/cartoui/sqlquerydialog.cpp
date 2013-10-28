/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISRasterDatasetCmd main header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/cartoui/sqlquerydialog.h"
/*
wxGISSQLQueryDialog::wxGISSQLQueryDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Layer:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_SelLayerComboBox = new wxComboBox( this, ID_M_SELLAYERCOMBOBOX, _("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer1->Add( m_SelLayerComboBox, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, _("Method:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText2->Wrap( -1 );
	fgSizer1->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_MethodSelComboBox = new wxComboBox( this, ID_M_METHODSELCOMBOBOX, _("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, 0 ); 
	fgSizer1->Add( m_MethodSelComboBox, 0, wxALL|wxEXPAND, 5 );
	
	bMainSizer->Add( fgSizer1, 0, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrl8 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer12->Add( m_textCtrl8, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer( wxVERTICAL );
	
	m_button15 = new wxButton( this, wxID_ANY, _("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_button15, 0, wxALL, 5 );
	
	m_button16 = new wxButton( this, wxID_ANY, _("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_button16, 0, wxALL, 5 );
	
	m_button17 = new wxButton( this, wxID_ANY, _("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer13->Add( m_button17, 0, wxALL, 5 );
	
	bSizer12->Add( bSizer13, 0, wxEXPAND, 5 );
	
	bMainSizer->Add( bSizer12, 1, wxEXPAND, 5 );
	
	wxBoxSizer* bSizer14;
	bSizer14 = new wxBoxSizer( wxHORIZONTAL );
	
	wxGridSizer* gSizer3;
	gSizer3 = new wxGridSizer( 5, 3, 0, 0 );
	
	m_bEqBtton = new wxButton( this, ID_M_BEQBTTON, _("="), wxDefaultPosition, wxDefaultSize, 0 );
	m_bEqBtton->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_bEqBtton, 0, wxALL, 5 );
	
	m_NEqButton = new wxButton( this, wxID_ANY, _("<>"), wxDefaultPosition, wxDefaultSize, 0 );
	m_NEqButton->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_NEqButton, 0, wxALL, 5 );
	
	m_LikeButton = new wxButton( this, wxID_ANY, _("Like"), wxDefaultPosition, wxDefaultSize, 0 );
	m_LikeButton->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_LikeButton, 0, wxALL, 5 );
	
	m_GreaterButton = new wxButton( this, wxID_ANY, _(">"), wxDefaultPosition, wxDefaultSize, 0 );
	m_GreaterButton->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_GreaterButton, 0, wxALL, 5 );
	
	m_GreaterAndEqButton = new wxButton( this, ID_M_GREATERANDEQBUTTON, _(">="), wxDefaultPosition, wxDefaultSize, 0 );
	m_GreaterAndEqButton->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_GreaterAndEqButton, 0, wxALL, 5 );
	
	m_AndButton = new wxButton( this, ID_M_ANDBUTTON, _("And"), wxDefaultPosition, wxDefaultSize, 0 );
	m_AndButton->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_AndButton, 0, wxALL, 5 );
	
	m_SmallerButton = new wxButton( this, wxID_ANY, _("<"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SmallerButton->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_SmallerButton, 0, wxALL, 5 );
	
	m_SmallerAndEqButton = new wxButton( this, ID_M_SMALLERANDEQBUTTON, _("<="), wxDefaultPosition, wxDefaultSize, 0 );
	m_SmallerAndEqButton->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_SmallerAndEqButton, 0, wxALL, 5 );
	
	m_OrButton = new wxButton( this, ID_M_ORBUTTON, _("Or"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OrButton->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_OrButton, 0, wxALL, 5 );
	
	m_UnderLineButton = new wxButton( this, ID_M_UNDERLINEBUTTON, _("_"), wxDefaultPosition, wxDefaultSize, 0 );
	m_UnderLineButton->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_UnderLineButton, 0, wxALL, 5 );
	
	m_button28 = new wxButton( this, wxID_ANY, _("( )"), wxDefaultPosition, wxDefaultSize, 0 );
	m_button28->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_button28, 0, wxALL, 5 );
	
	m_NotButton = new wxButton( this, ID_M_NOTBUTTON, _("Not"), wxDefaultPosition, wxDefaultSize, 0 );
	m_NotButton->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_NotButton, 0, wxALL, 5 );
	
	m_PercentButton = new wxButton( this, ID_M_PERCENTBUTTON, _("%"), wxDefaultPosition, wxDefaultSize, 0 );
	m_PercentButton->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_PercentButton, 0, wxALL, 5 );
	
	m_IsButton = new wxButton( this, ID_M_ISBUTTON, _("Is"), wxDefaultPosition, wxDefaultSize, 0 );
	m_IsButton->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_IsButton, 0, wxALL, 5 );
	
	m_button32 = new wxButton( this, wxID_ANY, _("..."), wxDefaultPosition, wxDefaultSize, 0 );
	m_button32->SetMinSize( wxSize( 40,-1 ) );
	
	gSizer3->Add( m_button32, 0, wxALL, 5 );
	
	bSizer14->Add( gSizer3, 0, 0, 5 );
	
	wxBoxSizer* bSizer15;
	bSizer15 = new wxBoxSizer( wxVERTICAL );
	
	m_textCtrl9 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_textCtrl9, 1, wxALL|wxEXPAND, 5 );
	
	m_button33 = new wxButton( this, wxID_ANY, _("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer15->Add( m_button33, 0, wxALL, 5 );
	
	bSizer14->Add( bSizer15, 1, wxEXPAND, 5 );
	
	bMainSizer->Add( bSizer14, 0, wxEXPAND, 5 );
	
	m_SelectStaticText = new wxStaticText( this, ID_M_SELECTSTATICTEXT, _("SELECT * FROM tablename WHERE:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SelectStaticText->Wrap( -1 );
	bMainSizer->Add( m_SelectStaticText, 0, wxALL|wxEXPAND, 5 );
	
	m_textCtrl10 = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bMainSizer->Add( m_textCtrl10, 1, wxALL|wxEXPAND, 5 );
	
	bButtonsSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_ClearButton = new wxButton( this, ID_M_CLEARBUTTON, _("Clear"), wxDefaultPosition, wxDefaultSize, 0 );
	bButtonsSizer->Add( m_ClearButton, 1, wxALL|wxEXPAND, 5 );
	
	m_CheckButton = new wxButton( this, ID_M_CHECKBUTTON, _("Check"), wxDefaultPosition, wxDefaultSize, 0 );
	bButtonsSizer->Add( m_CheckButton, 1, wxALL|wxEXPAND, 5 );
	
	m_HelpButton = new wxButton( this, ID_M_HELPBUTTON, _("Help"), wxDefaultPosition, wxDefaultSize, 0 );
	m_HelpButton->Enable( false );
	
	bButtonsSizer->Add( m_HelpButton, 1, wxALL|wxEXPAND, 5 );
	
	m_LoadButton = new wxButton( this, ID_M_LOADBUTTON, _("Load..."), wxDefaultPosition, wxDefaultSize, 0 );
	bButtonsSizer->Add( m_LoadButton, 1, wxALL|wxEXPAND, 5 );
	
	m_SaveButton = new wxButton( this, ID_M_SAVEBUTTON, _("Save..."), wxDefaultPosition, wxDefaultSize, 0 );
	bButtonsSizer->Add( m_SaveButton, 1, wxALL|wxEXPAND, 5 );
	
	bMainSizer->Add( bButtonsSizer, 0, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bMainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
	
	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerApply = new wxButton( this, wxID_APPLY );
	m_sdbSizer->AddButton( m_sdbSizerApply );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();
	bMainSizer->Add( m_sdbSizer, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bMainSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

wxGISSQLQueryDialog::~wxGISSQLQueryDialog()
{
}
*/
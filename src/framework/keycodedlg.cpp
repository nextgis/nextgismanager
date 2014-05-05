/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  key code input dialog class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010  Bishop
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
#include "wxgis/framework/keycodedlg.h"

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class wxKeyCodeCtrl
///////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxKeyCodeCtrl, wxTextCtrl)
    EVT_CHAR(wxKeyCodeCtrl::OnChar)
	EVT_KEY_DOWN(wxKeyCodeCtrl::OnChar)
END_EVENT_TABLE()

wxKeyCodeCtrl::wxKeyCodeCtrl(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name) : wxTextCtrl(parent, id, value, pos, size, style, validator, name), m_Key(-1)
{
}

wxKeyCodeCtrl::~wxKeyCodeCtrl(void)
{
}

void wxKeyCodeCtrl::OnChar(wxKeyEvent& event)
{
	wxString str;
	m_Key = wxNOT_FOUND;
	if(event.GetModifiers() & wxMOD_ALT)
		str += wxT("Alt+");
	if(event.GetModifiers() & wxMOD_CONTROL)
		str += wxT("Ctrl+");
	if(event.GetModifiers() & wxMOD_SHIFT)
		str += wxT("Shift+");
	int kode = event.GetKeyCode();
	m_Flags = event.GetModifiers();
	if(kode >= WXK_F1 && kode <= WXK_F12)
	{
		str += wxString::Format(wxT("F%d"), kode - WXK_DIVIDE );
		m_Key = kode;
	}
	else if((kode >= (int)'A' && kode <= (int)'Z') || (kode >= (int)'0' && kode <= (int)'9') || (kode >= WXK_NUMPAD0 && kode <= WXK_NUMPAD9))
	{
		str += wxString(event.GetUnicodeKey());
		m_Key = kode;
	}
	else if(kode == WXK_DELETE || kode == WXK_NUMPAD_DELETE)//del
	{
		str += wxString(wxT("Del"));
		m_Key = kode;
	}
	else if(kode == WXK_ESCAPE)//esc 
	{
		str += wxString(wxT("Esc"));
		m_Key = kode;
	}
	else if(kode == WXK_SPACE || kode == WXK_NUMPAD_SPACE)//Space
	{
		str += wxString(wxT("Space"));
		m_Key = kode;
	}
	else if(kode == WXK_RETURN || kode == WXK_EXECUTE)//enter
	{
		str += wxString(wxT("Enter"));
		m_Key = kode;
	}
	else if(kode == WXK_PAUSE)//pause
	{
		str += wxString(wxT("Pause"));
		m_Key = kode;
	}
	else if(kode == WXK_END || kode == WXK_NUMPAD_END)//end
	{
		str += wxString(wxT("End"));
		m_Key = kode;
	}
	else if(kode == WXK_HOME || kode == WXK_NUMPAD_HOME)//home
	{
		str += wxString(wxT("Home"));
		m_Key = kode;
	}
	else if(kode == WXK_INSERT || kode == WXK_NUMPAD_INSERT)//ins
	{
		str += wxString(wxT("Ins"));
		m_Key = kode;
	}

	SetValue(str);
	event.Skip(false);
}


///////////////////////////////////////////////////////////////////////////////
/// Class wxKeyCodeDlg
///////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxKeyCodeDlg, wxDialog)
	EVT_UPDATE_UI(wxID_OK, wxKeyCodeDlg::OnUpdateUI)
END_EVENT_TABLE()

wxKeyCodeDlg::wxKeyCodeDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	SetIcon(wxIcon(character_map_xpm));
	//this->SetSizeHints( wxSize( 260,70 ), wxSize( -1,70 ) );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxHORIZONTAL );
	
	m_textCtrl1 = new wxKeyCodeCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NONE, &m_sKeyCode) );
	bSizer1->Add( m_textCtrl1, 1, wxALL|wxEXPAND, 5 );
	
	m_button = new wxButton( this, wxID_OK, _("OK"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer1->Add( m_button, 0, wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

wxKeyCodeDlg::~wxKeyCodeDlg()
{
}

void wxKeyCodeDlg::OnUpdateUI(wxUpdateUIEvent& event)
{
	//event.Enable(m_textCtrl1->m_Key == -1 ? false : true);
}

bool wxKeyCodeDlg::TransferDataFromWindow()
{
	m_Key = m_textCtrl1->m_Key;
	m_Flags = m_textCtrl1->m_Flags;
	//if(m_Key == -1)
	//	return false;
	return true;
}

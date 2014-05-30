/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  key code input dialog class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010,2014 Dmitry Baryshnikov
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

#pragma once

#include "wxgis/framework/framework.h"
#include "../../../art/character_map.xpm"

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/valtext.h>

/** @class wxKeyCodeCtrl

    Control to add keys for shortcuts

    @library{framework}
 */

class wxKeyCodeCtrl : public wxTextCtrl
{
public:
	wxKeyCodeCtrl(wxWindow* parent, wxWindowID id, const wxString& value = wxT(""), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxTextCtrlNameStr);
	virtual ~wxKeyCodeCtrl(void);
	//event
	void OnChar(wxKeyEvent& event);

	int m_Flags;
	int m_Key;

	DECLARE_EVENT_TABLE()
};

/** @class wxKeyCodeDlg    

    Dialog to add keys for shortcuts

    @library{framework}
 */

class WXDLLIMPEXP_GIS_FRW wxKeyCodeDlg : public wxDialog
{
private:

protected:
	wxKeyCodeCtrl* m_textCtrl1;
	wxButton* m_button;

public:
    wxKeyCodeDlg(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Enter key code"), const wxPoint& pos = wxPoint(-1, -1), const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	~wxKeyCodeDlg();
	static wxBitmap GetBitmap(){return wxBitmap(character_map_xpm);};
	//events
	void OnUpdateUI(wxUpdateUIEvent& event);
	virtual bool TransferDataFromWindow();

	int m_Flags;
	int m_Key;
	wxString m_sKeyCode;

	DECLARE_EVENT_TABLE()
};


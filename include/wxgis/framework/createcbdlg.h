/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  create menu or toolbar dialog.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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
#include "wxgis/framework/commandbar.h"

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/checkbox.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISCreateCommandBarDlg
///////////////////////////////////////////////////////////////////////////////
class WXDLLIMPEXP_GIS_FRW wxGISCreateCommandBarDlg : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText4;
		wxTextCtrl* m_textCtrl3;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrl4;
		wxStaticText* m_staticText6;
		wxChoice* m_choice1;
		wxStaticText* m_staticText41;
		wxCheckBox* m_checkLeftD;
		wxStaticText* m_staticText51;
		wxCheckBox* m_checkRightD;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Cancel;
		int m_nType;
public:
		wxString m_sCommandbarName, m_sCommandbarCaption;
		wxGISEnumCommandBars m_CommandbarType;
		bool m_bLeftDockable, m_bRightDockable;
	
	public:
		wxGISCreateCommandBarDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Create toolbar or menu"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 246,222 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~wxGISCreateCommandBarDlg();
		virtual bool TransferDataFromWindow();
};


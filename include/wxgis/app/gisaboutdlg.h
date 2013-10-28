/******************************************************************************
 * Project:  wxGIS
 * Purpose:  About Dialog class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2013 Bishop
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
#pragma once

#include "wxgis/core/app.h"

#include <wx/aui/auibook.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/statbmp.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/statline.h>
#include <wx/notebook.h>
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/generic/stattextg.h>

/** \class wxGISSimpleTextPanel gisaboutdlg.h
 *   \brief Information tab in About dialog
 */
class wxGISSimpleTextPanel : public wxPanel
{
	enum
	{
		ID_EDIT = wxID_HIGHEST + 1
	};
public:
	wxGISSimpleTextPanel( wxString soText, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 540,400 ), long style = wxTAB_TRAVERSAL );
	virtual ~wxGISSimpleTextPanel();
	virtual void edtUrlClickUrl(wxTextUrlEvent& event);
protected:
	wxTextCtrl* m_pStaticText;

    DECLARE_EVENT_TABLE()
};


/** \class wxGISAboutDialog gisaboutdlg.h
 *   \brief About dialog class
 */
class wxGISAboutDialog: public wxDialog
{
public:
    wxGISAboutDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("About"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 440,440 ), long style = wxDEFAULT_DIALOG_STYLE);
    ~wxGISAboutDialog();
protected:
	wxStaticBitmap* m_bitmap;
	wxGenericStaticText* m_title;
	wxStaticLine* m_staticline1;
	wxStdDialogButtonSizer* m_sdbSizer;
	wxButton* m_sdbSizerOK;
	wxAuiNotebook* m_AuiNotebook;
};

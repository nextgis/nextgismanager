/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  Create network connection dialog
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010,2012 Bishop
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

#include "wxgis/remoteserverui/remoteserverui.h"
#include "wxgis/net/netfactory.h"

#include <wx/wx.h>
#include <wx/xml/xml.h>
#include <wx/choicebk.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/statline.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>


/** \class wxGISCreateNetworkConnDlg createnetworkconndlg.h
    \brief The create network connection dialog.
*/

class wxGISCreateNetworkConnDlg : public wxDialog 
{
    DECLARE_CLASS(wxGISCreateNetworkConnDlg)
public:
	wxGISCreateNetworkConnDlg( wxNetConnFactoryArray& apNetConn, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Create connection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 350, 450 ), long style = wxCAPTION | wxCLOSE_BOX | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxCLIP_CHILDREN );
	~wxGISCreateNetworkConnDlg();
	wxXmlNode* const GetConnectionProperties(void);
protected:
	void OnSaveBtnClick( wxCommandEvent& event );
protected:
    wxXmlNode* m_pConnProps;
protected:
	wxChoicebook* m_choicebook;
	wxStdDialogButtonSizer* m_sdbSizer;
	wxStaticLine* m_staticline1;
	wxButton* m_sdbSizerSave;
	wxButton* m_sdbSizerCancel;
	wxBoxSizer* bSizer;
	wxImageList m_ImageList;
private:	
    DECLARE_EVENT_TABLE()	
};



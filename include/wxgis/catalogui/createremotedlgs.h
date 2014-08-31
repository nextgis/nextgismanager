/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Create Remote Database dialog.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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

#include "wxgis/catalogui/remoteconndlgs.h"

#ifdef wxGIS_USE_POSTGRES

/** @class wxGISCreateDBDlg

    The dialog to create remote database connection, test it and store the connection information in file (*.xconn)

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGISCreateDBDlg : public wxGISRemoteDBConnDlg
{
public:
    wxGISCreateDBDlg(CPLString pszConnPath, wxWindow* parent, wxWindowID id = ID_REMOTECONNDLG, const wxString& title = _("Create new Remote Database"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    virtual ~wxGISCreateDBDlg();
protected:	//events
    virtual void OnTest(wxCommandEvent& event);
	virtual void OnOK(wxCommandEvent& event);
protected:
    void CreateUI(bool bHasConnectionPath = true);
protected:
    wxStaticText *m_DBNameStaticText;
    wxStaticText *m_DBConnectionNameStaticText;
    wxStaticText *m_DBTemplateNameStaticText;
    wxTextCtrl *m_DBName;
    wxChoice *m_TempateChoice;

    wxString m_sDBName;
    wxString m_sDBTempate;
};

#endif //wxGIS_USE_POSTGRES



/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Create Remote Database connection dialog.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013,2014 Dmitry Baryshnikov
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

#include "wxgisdefs.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/catalogui/catalogui.h"

#include <wx/statline.h>

/** @class wxGISRemoteConnDlg

    The dialog to configure remote database connection, test it and store in connection file (*.xconn)

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGISRemoteConnDlg : public wxDialog
{
protected:
	enum
	{
		ID_REMOTECONNDLG = 1000,
		ID_CONNNAME,
		ID_SERVERTEXTCTRL,
		ID_PORTTEXTCTRL,
		ID_DATABASESTATICTEXT,
		ID_DATABASETEXTCTRL,
		ID_USERSTATICTEXT,
		ID_PASSSTATICTEXT,
		ID_PASSTEXTCTRL,
		ID_TESTBUTTON,
	};
public:
    wxGISRemoteConnDlg(CPLString pszConnPath, wxWindow* parent, wxWindowID id = ID_REMOTECONNDLG, const wxString& title = _("Remote Database Connection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    wxGISRemoteConnDlg(wxXmlNode* pConnectionNode, wxWindow* parent, wxWindowID id = ID_REMOTECONNDLG, const wxString& title = _("Remote Database Connection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	virtual ~wxGISRemoteConnDlg();
	virtual CPLString GetPath(void);
	virtual wxString GetName(void);
protected:	//events
    virtual void OnTest(wxCommandEvent& event);
	virtual void OnOK(wxCommandEvent& event);
protected:
    void CreateUI(bool bHasConnectionPath = true);
protected:
	wxBoxSizer* m_bMainSizer;
	wxTextCtrl* m_ConnName;
	wxStaticLine* m_staticline1;
	wxStaticText* m_SetverStaticText;
	wxTextCtrl* m_sServerTextCtrl;
	wxStaticText* m_PortStaticText;
	wxTextCtrl* m_PortTextCtrl;
	wxStaticText* m_DatabaseStaticText;
	wxTextCtrl* m_DatabaseTextCtrl;
	wxStaticText* m_UserStaticText;
	wxTextCtrl* m_UsesTextCtrl;
	wxStaticText* m_PassStaticText;
	wxTextCtrl* m_PassTextCtrl;
	wxButton* m_TestButton;
	wxStaticLine* m_staticline2;
	wxStdDialogButtonSizer* m_sdbSizer;
	wxButton* m_sdbSizerOK;
	wxButton* m_sdbSizerCancel;
    wxCheckBox* m_checkBoxBinaryCursor;

	wxString m_sConnName;
	wxString m_sServer;
	wxString m_sPort;
	wxString m_sDatabase;
	wxString m_sUser;
	wxString m_sPass;
    bool m_bIsBinaryCursor;
	bool m_bCreateNew;
    bool m_bIsFile;
    wxXmlNode* m_pConnectionNode;

	wxString m_sOutputPath;
	CPLString m_sOriginOutput;

    DECLARE_EVENT_TABLE()
};

#endif //wxGIS_USE_POSTGRES

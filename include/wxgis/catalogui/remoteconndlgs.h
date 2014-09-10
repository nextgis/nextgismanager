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

#include "wxgis/catalogui/catalogui.h"

#ifdef wxGIS_USE_POSTGRES

#include <wx/statline.h>

/** @class wxGISRemoteDBConnDlg

    The dialog to configure remote database connection, test it and store in connection file (*.xconn)

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGISRemoteDBConnDlg : public wxDialog
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
		ID_TESTBUTTON
	};
public:
    wxGISRemoteDBConnDlg(CPLString pszConnPath, wxWindow* parent, wxWindowID id = ID_REMOTECONNDLG, const wxString& title = _("Remote Database Connection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    wxGISRemoteDBConnDlg(wxXmlNode* pConnectionNode, wxWindow* parent, wxWindowID id = ID_REMOTECONNDLG, const wxString& title = _("Remote Database Connection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	virtual ~wxGISRemoteDBConnDlg();
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

private:
    DECLARE_EVENT_TABLE()
};

#endif //wxGIS_USE_POSTGRES

#ifdef wxGIS_USE_CURL

#include <wx/statline.h>

/** @class wxGISTMSConnDlg

    The dialog to configure TMS connection, test it and store in connection file (*.xconn)

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGISTMSConnDlg : public wxDialog
{
protected:
	enum
	{
		ID_REMOTECONNDLG = 1000,
		ID_TESTBUTTON,
		ID_CONNNAME,
		ID_PRESETTYPE
	};
public:
    wxGISTMSConnDlg(CPLString pszConnPath, wxWindow* parent, wxWindowID id = ID_REMOTECONNDLG, const wxString& title = _("TMS Connection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    wxGISTMSConnDlg(wxXmlNode* pConnectionNode, wxWindow* parent, wxWindowID id = ID_REMOTECONNDLG, const wxString& title = _("TMS Connection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	virtual ~wxGISTMSConnDlg();
	virtual CPLString GetPath(void);
	virtual wxString GetName(void);
protected:	//events
    virtual void OnTest(wxCommandEvent& event);
	virtual void OnOK(wxCommandEvent& event);
protected:
    void CreateUI(bool bHasConnectionPath = true);
    void FillDefaults();
protected:
	wxBoxSizer* m_bMainSizer;
	wxTextCtrl* m_ConnName;

	wxButton* m_TestButton;
	wxStaticLine* m_staticline2;
	wxStdDialogButtonSizer* m_sdbSizer;
	wxButton* m_sdbSizerOK;
	wxButton* m_sdbSizerCancel;
protected:
	wxString m_sConnName;
	wxString m_sPresetTypeChoice;
	wxString m_sURL;
	wxString m_sCachePath;
	wxString m_sCacheDepth;
	int m_nMaxConnections;
	wxString m_sTimeout;
	wxString m_sZeroBlockHttpCodes;
	wxString m_sZeroBlockOnServerException;
	wxString m_sOfflineMode;
	wxString m_sAdviseRead;
	wxString m_sVerifyAdviseRead;
	wxString m_sClampRequests;
	wxString m_sUserAgent;
	wxString m_sReferer;
	double m_dfULX;
	double m_dfULY;
	double m_dfLRX;
	double m_dfLRY;
	wxString m_sOriginTypeChoice;
	int m_nTileSizeX;
	int m_nTileSizeY;
	int m_nBandsCount;
	wxString m_sUser;
	wxString m_sPass;

	bool m_bCreateNew;
    bool m_bIsFile;
    wxXmlNode* m_pConnectionNode;

	wxString m_sOutputPath;
	CPLString m_sOriginOutput;

private:
    DECLARE_EVENT_TABLE()
};

/** @class wxGISNGWConnDlg

    The dialog to configure NextGIS Web connection, test it and store in connection file (*.xconn)

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGISNGWConnDlg : public wxDialog
{
protected:
	enum
	{
		ID_REMOTECONNDLG = 1000,
		ID_TESTBUTTON,
		ID_CONNNAME
	};
public:
    wxGISNGWConnDlg(CPLString pszConnPath, wxWindow* parent, wxWindowID id = ID_REMOTECONNDLG, const wxString& title = _("NGW Connection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    wxGISNGWConnDlg(wxXmlNode* pConnectionNode, wxWindow* parent, wxWindowID id = ID_REMOTECONNDLG, const wxString& title = _("NGW Connection"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	virtual ~wxGISNGWConnDlg();
	virtual CPLString GetPath(void);
	virtual wxString GetName(void);
protected:	//events
    virtual void OnTest(wxCommandEvent& event);
	virtual void OnOK(wxCommandEvent& event);
protected:
    void CreateUI(bool bHasConnectionPath = true);
    void FillDefaults();
protected:
	wxBoxSizer* m_bMainSizer;
	wxTextCtrl* m_ConnName;

	wxButton* m_TestButton;
	wxStaticLine* m_staticline2;
	wxStdDialogButtonSizer* m_sdbSizer;
	wxButton* m_sdbSizerOK;
	wxButton* m_sdbSizerCancel;
protected:
	wxString m_sConnName;
	wxString m_sURL;
	wxString m_sUser;
	wxString m_sPass;
	wxString m_sUserAgent;

	bool m_bCreateNew;
    bool m_bIsFile;
    wxXmlNode* m_pConnectionNode;

	wxString m_sOutputPath;
	CPLString m_sOriginOutput;

private:
    DECLARE_EVENT_TABLE()
};

#endif // wxGIS_USE_CURL

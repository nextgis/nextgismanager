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

#include "wxgis/catalogui/remoteconndlgs.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/core/crypt.h"
#include "wxgis/core/format.h"
#include "wxgis/net/curl.h"

#include <wx/valgen.h>
#include <wx/valtext.h>
#include <wx/valnum.h>

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/datasource/postgisdataset.h"

//-------------------------------------------------------------------------------
//  wxGISRemoteDBConnDlg
//-------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGISRemoteDBConnDlg, wxDialog)
    EVT_BUTTON(wxID_OK, wxGISRemoteDBConnDlg::OnOK)
    EVT_BUTTON(ID_TESTBUTTON, wxGISRemoteDBConnDlg::OnTest)
END_EVENT_TABLE()

wxGISRemoteDBConnDlg::wxGISRemoteDBConnDlg(wxXmlNode* pConnectionNode, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
    m_bIsFile = false;

    m_sServer = wxString(wxT("localhost"));
    m_sPort = wxString(wxT("5432"));
    m_sDatabase = wxString(wxT("postgres"));
    m_bIsBinaryCursor = true;

    //load values from xconn file
    if (pConnectionNode)
    {
        m_pConnectionNode = pConnectionNode;
        m_sServer = pConnectionNode->GetAttribute(wxT("server"), m_sServer);
        m_sPort = pConnectionNode->GetAttribute(wxT("port"), m_sPort);
        m_sDatabase = pConnectionNode->GetAttribute(wxT("db"), m_sDatabase);
        m_sUser = pConnectionNode->GetAttribute(wxT("user"), m_sUser);
        Decrypt(pConnectionNode->GetAttribute(wxT("pass"), wxEmptyString), m_sPass);
        m_bIsBinaryCursor = GetBoolValue(pConnectionNode, wxT("isbincursor"), m_bIsBinaryCursor);
    }

    //this->SetSizeHints( wxSize( 320,REMOTECONNDLG_MAX_HEIGHT ), wxSize( -1,REMOTECONNDLG_MAX_HEIGHT ) );

    CreateUI(false);
}


wxGISRemoteDBConnDlg::wxGISRemoteDBConnDlg( CPLString pszConnPath, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	m_bCreateNew = !CPLCheckForFile((char*)pszConnPath.c_str(), NULL);
    m_bIsFile = true;

    m_sConnName = wxString(CPLGetFilename(pszConnPath), wxConvUTF8);
	m_sOutputPath = wxString(CPLGetPath(pszConnPath), wxConvUTF8);
	//set default values
	m_sServer = wxString(wxT("localhost"));
	m_sPort = wxString(wxT("5432"));
	m_sDatabase = wxString(wxT("postgres"));
	m_bIsBinaryCursor = true;

	//load values from xconn file
	if(!m_bCreateNew)
	{
        m_sOriginOutput = pszConnPath;
		wxXmlDocument doc(wxString(pszConnPath,  wxConvUTF8));
		if(doc.IsOk())
		{
			wxXmlNode* pRootNode = doc.GetRoot();
			if(pRootNode)
			{
				m_sServer = pRootNode->GetAttribute(wxT("server"), m_sServer);
				m_sPort = pRootNode->GetAttribute(wxT("port"), m_sPort);
				m_sDatabase = pRootNode->GetAttribute(wxT("db"), m_sDatabase);
				m_sUser = pRootNode->GetAttribute(wxT("user"), m_sUser);
				Decrypt(pRootNode->GetAttribute(wxT("pass"), wxEmptyString), m_sPass);
				m_bIsBinaryCursor = GetBoolValue(pRootNode, wxT("isbincursor"), m_bIsBinaryCursor);
			}
		}
	}

    //this->SetSizeHints( wxSize( 320,REMOTECONNDLG_MAX_HEIGHT ), wxSize( -1,REMOTECONNDLG_MAX_HEIGHT ) );

    CreateUI();
}

wxGISRemoteDBConnDlg::~wxGISRemoteDBConnDlg()
{
}

void wxGISRemoteDBConnDlg::OnOK(wxCommandEvent& event)
{
	if ( Validate() && TransferDataFromWindow() )
	{
		wxString sCryptPass;
		if(!Crypt(m_sPass, sCryptPass))
		{
			wxMessageBox(wxString(_("Crypt password failed!")), wxString(_("Error")), wxICON_ERROR | wxOK );
			return;
		}

        if (m_bIsFile)
        {
		    wxXmlDocument doc;
		    wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("connection"));
		    pRootNode->AddAttribute(wxT("server"), m_sServer);
		    pRootNode->AddAttribute(wxT("port"), m_sPort);
		    pRootNode->AddAttribute(wxT("db"), m_sDatabase);
		    pRootNode->AddAttribute(wxT("user"), m_sUser);
		    pRootNode->AddAttribute(wxT("pass"), sCryptPass);

            SetBoolValue(pRootNode, wxT("isbincursor"), m_bIsBinaryCursor);

		    pRootNode->AddAttribute(wxT("type"), wxT("POSTGIS"));//store server type for future

		    doc.SetRoot(pRootNode);

		    wxString sFullPath = m_sOutputPath + wxFileName::GetPathSeparator() + GetName();
		    if(!m_bCreateNew)// && wxGISEQUAL(CPLString(sFullPath.mb_str(wxConvUTF8)), m_sOriginOutput))
		    {
                RenameFile(m_sOriginOutput, CPLString(sFullPath.mb_str(wxConvUTF8)));
		    }

            if(!doc.Save(sFullPath))
		    {
			    wxMessageBox(wxString(_("Connection create failed!")), wxString(_("Error")), wxICON_ERROR | wxOK );
			    return;
		    }

            //m_sOriginOutput = CPLString(sFullPath.mb_str(wxConvUTF8));
        }
        else
        {
            if (m_pConnectionNode->HasAttribute(wxT("server")))
                m_pConnectionNode->DeleteAttribute(wxT("server"));
            m_pConnectionNode->AddAttribute(wxT("server"), m_sServer);

            if (m_pConnectionNode->HasAttribute(wxT("port")))
                m_pConnectionNode->DeleteAttribute(wxT("port"));
            m_pConnectionNode->AddAttribute(wxT("port"), m_sPort);

            if (m_pConnectionNode->HasAttribute(wxT("db")))
                m_pConnectionNode->DeleteAttribute(wxT("db"));
            m_pConnectionNode->AddAttribute(wxT("db"), m_sDatabase);

            if (m_pConnectionNode->HasAttribute(wxT("user")))
                m_pConnectionNode->DeleteAttribute(wxT("user"));
            m_pConnectionNode->AddAttribute(wxT("user"), m_sUser);

            if (m_pConnectionNode->HasAttribute(wxT("pass")))
                m_pConnectionNode->DeleteAttribute(wxT("pass"));
            m_pConnectionNode->AddAttribute(wxT("pass"), sCryptPass);

            if (m_pConnectionNode->HasAttribute(wxT("isbincursor")))
                m_pConnectionNode->DeleteAttribute(wxT("isbincursor"));
            SetBoolValue(m_pConnectionNode, wxT("isbincursor"), m_bIsBinaryCursor);

            if (m_pConnectionNode->HasAttribute(wxT("type")))
                m_pConnectionNode->DeleteAttribute(wxT("type"));
            m_pConnectionNode->AddAttribute(wxT("type"), wxT("POSTGIS"));//store server type for future
        }
		EndModal(wxID_OK);
	}
	else
	{
		wxMessageBox(wxString(_("Some input values are incorrect!")), wxString(_("Error")), wxICON_ERROR | wxOK );
	}
}

void wxGISRemoteDBConnDlg::OnTest(wxCommandEvent& event)
{
	wxBusyCursor wait;
	if ( Validate() && TransferDataFromWindow() )
	{
		wxGISPostgresDataSource oPostgresDataSource(m_sUser, m_sPass, m_sPort, m_sServer, m_sDatabase, m_bIsBinaryCursor);
		if( oPostgresDataSource.Open(false, true ) )
		{
			wxMessageBox(wxString(_("Connected successfully!")), wxString(_("Information")), wxICON_INFORMATION | wxOK, this );
		}
		else
		{
			const char* err = CPLGetLastErrorMsg();
			wxString sErr = wxString::Format(_("Operation '%s' failed!\nHost '%s', Database name '%s', Port='%s'.\nGDAL error: %s"), wxString(_("Open")), m_sServer.c_str(), m_sDatabase.c_str(), m_sPort.c_str(), wxString(err, wxConvLocal).c_str());
			wxMessageBox(sErr, _("Error"), wxICON_ERROR | wxOK, this );
		}
	}
	else
	{
		wxMessageBox(wxString(_("Some input values are not correct!")), wxString(_("Error")), wxICON_ERROR | wxOK );
	}
}

CPLString wxGISRemoteDBConnDlg::GetPath(void)
{
	return CPLString(wxString(m_sOutputPath + wxFileName::GetPathSeparator() + GetName()).mb_str(wxConvUTF8));
}

wxString wxGISRemoteDBConnDlg::GetName(void)
{
 	if(!m_sConnName.Lower().EndsWith(wxT(".xconn")))
		m_sConnName.Append(wxT(".xconn"));
	return m_sConnName;
}

void wxGISRemoteDBConnDlg::CreateUI(bool bHasConnectionPath)
{
    m_bMainSizer = new wxBoxSizer( wxVERTICAL );

    if(bHasConnectionPath)
    {
        m_ConnName = new wxTextCtrl( this, ID_CONNNAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sConnName) );
        m_bMainSizer->Add( m_ConnName, 0, wxALL|wxEXPAND, 5 );

        m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
        m_bMainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );
    }

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_SetverStaticText = new wxStaticText( this, wxID_ANY, _("Server:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SetverStaticText->Wrap( -1 );
	fgSizer1->Add( m_SetverStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	m_sServerTextCtrl = new wxTextCtrl( this, ID_SERVERTEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sServer) );
	fgSizer1->Add( m_sServerTextCtrl, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	m_PortStaticText = new wxStaticText( this, wxID_ANY, _("Port:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_PortStaticText->Wrap( -1 );
	fgSizer1->Add( m_PortStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_PortTextCtrl = new wxTextCtrl( this, ID_PORTTEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_sPort) );
	fgSizer1->Add( m_PortTextCtrl, 0, wxALL|wxEXPAND, 5 );

	m_DatabaseStaticText = new wxStaticText( this, ID_DATABASESTATICTEXT, _("Database:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DatabaseStaticText->Wrap( -1 );
	fgSizer1->Add( m_DatabaseStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	m_DatabaseTextCtrl = new wxTextCtrl( this, ID_DATABASETEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sDatabase) );
	fgSizer1->Add( m_DatabaseTextCtrl, 0, wxALL|wxEXPAND, 5 );

	m_bMainSizer->Add( fgSizer1, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Account") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_UserStaticText = new wxStaticText( this, ID_USERSTATICTEXT, _("User:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_UserStaticText->Wrap( -1 );
	fgSizer2->Add( m_UserStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	m_UsesTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sUser) );
	fgSizer2->Add( m_UsesTextCtrl, 0, wxALL|wxEXPAND, 5 );

	m_PassStaticText = new wxStaticText( this, ID_PASSSTATICTEXT, _("Password:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_PassStaticText->Wrap( -1 );
	fgSizer2->Add( m_PassStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_PassTextCtrl = new wxTextCtrl( this, ID_PASSTEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD, wxGenericValidator(&m_sPass) );
	fgSizer2->Add( m_PassTextCtrl, 0, wxALL|wxEXPAND, 5 );

	sbSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );

	m_bMainSizer->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );

    m_checkBoxBinaryCursor = new wxCheckBox(this, wxID_ANY, _("Use binary cursor"), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bIsBinaryCursor));
	m_bMainSizer->Add( m_checkBoxBinaryCursor, 0, wxALL|wxEXPAND, 5 );

	m_TestButton = new wxButton( this, ID_TESTBUTTON, _("Test Connection"), wxDefaultPosition, wxDefaultSize, 0 );
	m_bMainSizer->Add( m_TestButton, 0, wxALL|wxEXPAND, 5 );

	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_bMainSizer->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK, _("OK") );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL, _("Cancel") );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();
	m_bMainSizer->Add( m_sdbSizer, 0, wxEXPAND|wxALL, 5 );

    this->SetSizerAndFit(m_bMainSizer);
	this->Layout();

	this->Centre( wxBOTH );
}

#endif //wxGIS_USE_POSTGRES

#ifdef wxGIS_USE_CURL

//-------------------------------------------------------------------------------
//  wxGISTMSConnDlg
//-------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGISTMSConnDlg, wxDialog)
    EVT_BUTTON(wxID_OK, wxGISTMSConnDlg::OnOK)
    EVT_BUTTON(ID_TESTBUTTON, wxGISTMSConnDlg::OnTest)
END_EVENT_TABLE()

wxGISTMSConnDlg::wxGISTMSConnDlg(wxXmlNode* pConnectionNode, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
    m_bIsFile = false;

    //set default values
    FillDefaults();

    //load values from xconn file
    if (pConnectionNode)
    {
        m_pConnectionNode = pConnectionNode;
        /*m_sServer = pConnectionNode->GetAttribute(wxT("server"), m_sServer);
        m_sPort = pConnectionNode->GetAttribute(wxT("port"), m_sPort);
        m_sDatabase = pConnectionNode->GetAttribute(wxT("db"), m_sDatabase);
        m_sUser = pConnectionNode->GetAttribute(wxT("user"), m_sUser);
        Decrypt(pConnectionNode->GetAttribute(wxT("pass"), wxEmptyString), m_sPass);
        m_bIsBinaryCursor = GetBoolValue(pConnectionNode, wxT("isbincursor"), m_bIsBinaryCursor);
        */
    }

    CreateUI(false);
}


wxGISTMSConnDlg::wxGISTMSConnDlg( CPLString pszConnPath, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	m_bCreateNew = !CPLCheckForFile((char*)pszConnPath.c_str(), NULL);
    m_bIsFile = true;

    m_sConnName = wxString(CPLGetFilename(pszConnPath), wxConvUTF8);
	m_sOutputPath = wxString(CPLGetPath(pszConnPath), wxConvUTF8);

	//set default values
	FillDefaults();

	//load values from xconn file
	if(!m_bCreateNew)
	{
        m_sOriginOutput = pszConnPath;
		wxXmlDocument doc(wxString(pszConnPath,  wxConvUTF8));
		if(doc.IsOk())
		{
			wxXmlNode* pRootNode = doc.GetRoot();
			if(pRootNode)
			{
			}
		}
	}

    CreateUI();
}

wxGISTMSConnDlg::~wxGISTMSConnDlg()
{
}

void wxGISTMSConnDlg::FillDefaults()
{
    m_sURL = wxEmptyString;
    m_sCachePath = wxEmptyString;
    m_sCacheDepth = wxString(wxT("2"));
    m_sTimeout = wxString(wxT("300"));
    m_sZeroBlockHttpCodes = wxString(wxT("204,404"));
    m_sZeroBlockOnServerException = wxString(wxT("false"));
    m_sOfflineMode = wxString(wxT("false"));
    m_sAdviseRead = wxString(wxT("false"));
    m_sVerifyAdviseRead = wxString(wxT("false"));
    m_sClampRequests = wxString(wxT("true"));

    IApplication *pApp = GetApplication();
    if(NULL != pApp)
    {
        m_sUserAgent = pApp->GetAppName() + wxT(" (") + pApp->GetAppDisplayName() + wxT(" - ") + pApp->GetAppVersionString() + wxT(")");
    }
    m_sReferer = wxString(wxT("http://nextgis.com"));

  	wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
    {
        m_sCachePath = oConfig.GetLocalConfigDirNonPortable() + wxFileName::GetPathSeparator() + wxString(wxT("cache"));
        m_sCachePath = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/cache_path")), m_sCachePath);
        m_sCacheDepth = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/cache_depth")), m_sCacheDepth);
        m_sTimeout = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/timeout")), m_sTimeout);
        m_sZeroBlockHttpCodes = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/zero_block_http_codes")), m_sZeroBlockHttpCodes);
        m_sZeroBlockOnServerException = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/zero_block_onserver_exception")), m_sZeroBlockHttpCodes);
        m_sOfflineMode = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/offline_mode")), m_sOfflineMode);
        m_sAdviseRead = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/advise_read")), m_sAdviseRead);
        m_sVerifyAdviseRead = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/verify_advise_read")), m_sVerifyAdviseRead);
        m_sClampRequests = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/clamp_requests")), m_sClampRequests);
    }
}

void wxGISTMSConnDlg::OnOK(wxCommandEvent& event)
{
	if ( Validate() && TransferDataFromWindow() )
	{
        if (m_bIsFile)
        {
		    wxXmlDocument doc;
		    wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("connection"));
/*		    pRootNode->AddAttribute(wxT("server"), m_sServer);
		    pRootNode->AddAttribute(wxT("port"), m_sPort);
		    pRootNode->AddAttribute(wxT("db"), m_sDatabase);
		    pRootNode->AddAttribute(wxT("user"), m_sUser);
		    pRootNode->AddAttribute(wxT("pass"), sCryptPass);

            SetBoolValue(pRootNode, wxT("isbincursor"), m_bIsBinaryCursor);

		    pRootNode->AddAttribute(wxT("type"), wxT("POSTGIS"));//store server type for future
		    */

		    doc.SetRoot(pRootNode);

		    wxString sFullPath = m_sOutputPath + wxFileName::GetPathSeparator() + GetName();
		    if(!m_bCreateNew)// && wxGISEQUAL(CPLString(sFullPath.mb_str(wxConvUTF8)), m_sOriginOutput))
		    {
                RenameFile(m_sOriginOutput, CPLString(sFullPath.mb_str(wxConvUTF8)));
		    }

            if(!doc.Save(sFullPath))
		    {
			    wxMessageBox(wxString(_("Connection create failed!")), wxString(_("Error")), wxICON_ERROR | wxOK );
			    return;
		    }

            //m_sOriginOutput = CPLString(sFullPath.mb_str(wxConvUTF8));
        }
        else
        {
/*            if (m_pConnectionNode->HasAttribute(wxT("server")))
                m_pConnectionNode->DeleteAttribute(wxT("server"));
            m_pConnectionNode->AddAttribute(wxT("server"), m_sServer);

            if (m_pConnectionNode->HasAttribute(wxT("port")))
                m_pConnectionNode->DeleteAttribute(wxT("port"));
            m_pConnectionNode->AddAttribute(wxT("port"), m_sPort);

            if (m_pConnectionNode->HasAttribute(wxT("db")))
                m_pConnectionNode->DeleteAttribute(wxT("db"));
            m_pConnectionNode->AddAttribute(wxT("db"), m_sDatabase);

            if (m_pConnectionNode->HasAttribute(wxT("user")))
                m_pConnectionNode->DeleteAttribute(wxT("user"));
            m_pConnectionNode->AddAttribute(wxT("user"), m_sUser);

            if (m_pConnectionNode->HasAttribute(wxT("pass")))
                m_pConnectionNode->DeleteAttribute(wxT("pass"));
            m_pConnectionNode->AddAttribute(wxT("pass"), sCryptPass);

            if (m_pConnectionNode->HasAttribute(wxT("isbincursor")))
                m_pConnectionNode->DeleteAttribute(wxT("isbincursor"));
            SetBoolValue(m_pConnectionNode, wxT("isbincursor"), m_bIsBinaryCursor);

            if (m_pConnectionNode->HasAttribute(wxT("type")))
                m_pConnectionNode->DeleteAttribute(wxT("type"));
            m_pConnectionNode->AddAttribute(wxT("type"), wxT("POSTGIS"));//store server type for future
            */
        }
		EndModal(wxID_OK);
	}
	else
	{
		wxMessageBox(wxString(_("Some input values are incorrect!")), wxString(_("Error")), wxICON_ERROR | wxOK );
	}
}

void wxGISTMSConnDlg::OnTest(wxCommandEvent& event)
{
	wxBusyCursor wait;
	if ( Validate() && TransferDataFromWindow() )
	{

	}
	else
	{
		wxMessageBox(wxString(_("Some input values are not correct!")), wxString(_("Error")), wxICON_ERROR | wxOK );
	}
}

CPLString wxGISTMSConnDlg::GetPath(void)
{
	return CPLString(wxString(m_sOutputPath + wxFileName::GetPathSeparator() + GetName()).mb_str(wxConvUTF8));
}

wxString wxGISTMSConnDlg::GetName(void)
{
 	if(!m_sConnName.Lower().EndsWith(wxT(".wconn")))
		m_sConnName.Append(wxT(".wconn"));
	return m_sConnName;
}

void wxGISTMSConnDlg::CreateUI(bool bHasConnectionPath)
{
    m_bMainSizer = new wxBoxSizer( wxVERTICAL );

    if(bHasConnectionPath)
    {
        m_ConnName = new wxTextCtrl( this, ID_CONNNAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sConnName) );
        m_bMainSizer->Add( m_ConnName, 0, wxALL|wxEXPAND, 5 );

        wxStaticLine *staticline = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
        m_bMainSizer->Add( staticline, 0, wxEXPAND | wxALL, 5 );
    }

    //add presets
    wxStaticBoxSizer* sbSizerPresets = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Presets") ), wxVERTICAL );
    wxArrayString saPresetChoices;
    saPresetChoices.Add(wxT("OpenStreetMap"));
    saPresetChoices.Add(wxT("BlueMarble Amazon S3"));
    saPresetChoices.Add(wxT("Google Maps - Map"));
    saPresetChoices.Add(wxT("Google Maps - Satellite"));
    saPresetChoices.Add(wxT("Google Maps - Hybrid"));
    saPresetChoices.Add(wxT("Google Maps - Terrain"));
    saPresetChoices.Add(wxT("Google Maps - Terrain, Streets and Water"));
    saPresetChoices.Add(wxT("ArcGIS MapServer Tiles"));

	wxChoice *PresetsType = new wxChoice( this, ID_PRESETTYPE, wxDefaultPosition, wxDefaultSize, saPresetChoices, 0, wxGenericValidator(&m_sPresetTypeChoice) );
	sbSizerPresets->Add( PresetsType, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
    m_bMainSizer->Add( sbSizerPresets, 0, wxEXPAND|wxALL, 5 );

    //add other controls

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 9, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText *URLStaticText = new wxStaticText( this, wxID_ANY, _("URL:"), wxDefaultPosition, wxDefaultSize, 0 );
	URLStaticText->Wrap( -1 );
	fgSizer1->Add( URLStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	wxTextCtrl *URLTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,  wxGenericValidator(&m_sURL) );
	fgSizer1->Add( URLTextCtrl, 0, wxALL|wxEXPAND, 5 );

	//<UpperLeftX>

	wxStaticText *ULXStaticText = new wxStaticText( this, wxID_ANY, _("Upper Left X:"), wxDefaultPosition, wxDefaultSize, 0 );
	ULXStaticText->Wrap( -1 );
	fgSizer1->Add( ULXStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    wxFloatingPointValidator<double> val_ulx(2, &m_dfULX, wxNUM_VAL_ZERO_AS_BLANK);
	wxTextCtrl *ULXTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, val_ulx );
	fgSizer1->Add( ULXTextCtrl, 0, wxALL|wxEXPAND, 5 );

	//<UpperLeftY>

	wxStaticText *ULYStaticText = new wxStaticText( this, wxID_ANY, _("Upper Left Y:"), wxDefaultPosition, wxDefaultSize, 0 );
	ULYStaticText->Wrap( -1 );
	fgSizer1->Add( ULYStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    wxFloatingPointValidator<double> val_uly(2, &m_dfULY, wxNUM_VAL_ZERO_AS_BLANK);
	wxTextCtrl *ULYTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, val_uly );
	fgSizer1->Add( ULYTextCtrl, 0, wxALL|wxEXPAND, 5 );

	//<LowerRightX>

	wxStaticText *LRXStaticText = new wxStaticText( this, wxID_ANY, _("Lower Right X:"), wxDefaultPosition, wxDefaultSize, 0 );
	LRXStaticText->Wrap( -1 );
	fgSizer1->Add( LRXStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    wxFloatingPointValidator<double> val_lrx(2, &m_dfLRX, wxNUM_VAL_ZERO_AS_BLANK);
	wxTextCtrl *LRXTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, val_lrx );
	fgSizer1->Add( LRXTextCtrl, 0, wxALL|wxEXPAND, 5 );

	//<LowerRightY>

	wxStaticText *LRYStaticText = new wxStaticText( this, wxID_ANY, _("Lower Right Y:"), wxDefaultPosition, wxDefaultSize, 0 );
	LRYStaticText->Wrap( -1 );
	fgSizer1->Add( LRYStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    wxFloatingPointValidator<double> val_lry(2, &m_dfLRY, wxNUM_VAL_ZERO_AS_BLANK);
	wxTextCtrl *LRYTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, val_lry );
	fgSizer1->Add( LRYTextCtrl, 0, wxALL|wxEXPAND, 5 );


	// <TileLevel>20</TileLevel> - optional
	// <TileCountX>1</TileCountX> - optional
	// <TileCountY>1</TileCountY> - optional

	//<YOrigin>top</YOrigin>
	wxStaticText *TMSTypeStaticText = new wxStaticText( this, wxID_ANY, _("TMS type:"), wxDefaultPosition, wxDefaultSize, 0 );
	TMSTypeStaticText->Wrap( -1 );
	fgSizer1->Add( TMSTypeStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString saOriginChoices;
    saOriginChoices.Add(wxT("Slippy map"));
    saOriginChoices.Add(wxT("Normal TMS"));

	wxChoice *TMSType = new wxChoice( this, ID_PRESETTYPE, wxDefaultPosition, wxDefaultSize, saPresetChoices, 0, wxGenericValidator(&m_sOriginTypeChoice) );
	fgSizer1->Add( TMSType, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	//<BlockSizeX>256</BlockSizeX>

	wxStaticText *BlockSizeXStaticText = new wxStaticText( this, wxID_ANY, _("Tile Size X:"), wxDefaultPosition, wxDefaultSize, 0 );
	BlockSizeXStaticText->Wrap( -1 );
	fgSizer1->Add( BlockSizeXStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    wxIntegerValidator<int> val_tilesizex(&m_nTileSizeX);
	wxTextCtrl *TileSizeXTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, val_tilesizex );
	fgSizer1->Add( TileSizeXTextCtrl, 0, wxALL|wxEXPAND, 5 );


	//<BlockSizeY>256</BlockSizeY>

	wxStaticText *BlockSizeYStaticText = new wxStaticText( this, wxID_ANY, _("Tile Size Y:"), wxDefaultPosition, wxDefaultSize, 0 );
	BlockSizeYStaticText->Wrap( -1 );
	fgSizer1->Add( BlockSizeYStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    wxIntegerValidator<int> val_tilesizey(&m_nTileSizeY);
	wxTextCtrl *TileSizeYTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, val_tilesizey );
	fgSizer1->Add( TileSizeYTextCtrl, 0, wxALL|wxEXPAND, 5 );

	//<BandsCount>3</BandsCount>
	wxStaticText *BandsCountStaticText = new wxStaticText( this, wxID_ANY, _("Bands Count:"), wxDefaultPosition, wxDefaultSize, 0 );
	BandsCountStaticText->Wrap( -1 );
	fgSizer1->Add( BandsCountStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    wxIntegerValidator<int> val_bandcount(&m_nBandsCount);
	wxTextCtrl *BandsCountStaticTextTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, val_bandcount );
	fgSizer1->Add( BandsCountStaticTextTextCtrl, 0, wxALL|wxEXPAND, 5 );

	//<MaxConnections>5</MaxConnections>
	wxStaticText *MaxConnectionsStaticText = new wxStaticText( this, wxID_ANY, _("Max Connections (1-30):"), wxDefaultPosition, wxDefaultSize, 0 );
	MaxConnectionsStaticText->Wrap( -1 );
	fgSizer1->Add( MaxConnectionsStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    wxIntegerValidator<int> val_maxconnections(&m_nMaxConnections);
    val_maxconnections.SetRange(1, 30);
	wxTextCtrl *MaxConnectionsStaticTextTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, val_maxconnections );
	fgSizer1->Add( MaxConnectionsStaticTextTextCtrl, 0, wxALL|wxEXPAND, 5 );

	//<Projection>EPSG:900913</Projection>

	m_bMainSizer->Add( fgSizer1, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Account") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText *UserStaticText = new wxStaticText( this, wxID_ANY, _("User:"), wxDefaultPosition, wxDefaultSize, 0 );
	UserStaticText->Wrap( -1 );
	fgSizer2->Add( UserStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	wxTextCtrl *UsesTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sUser) );
	fgSizer2->Add( UsesTextCtrl, 0, wxALL|wxEXPAND, 5 );

	wxStaticText *PassStaticText = new wxStaticText( this, wxID_ANY, _("Password:"), wxDefaultPosition, wxDefaultSize, 0 );
	PassStaticText->Wrap( -1 );
	fgSizer2->Add( PassStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	wxTextCtrl *PassTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD, wxGenericValidator(&m_sPass) );
	fgSizer2->Add( PassTextCtrl, 0, wxALL|wxEXPAND, 5 );

	sbSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );

	m_bMainSizer->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );

	m_TestButton = new wxButton( this, ID_TESTBUTTON, _("Test Connection"), wxDefaultPosition, wxDefaultSize, 0 );
	m_bMainSizer->Add( m_TestButton, 0, wxALL|wxEXPAND, 5 );

	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_bMainSizer->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK, _("OK") );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL, _("Cancel") );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();
	m_bMainSizer->Add( m_sdbSizer, 0, wxEXPAND|wxALL, 5 );

    this->SetSizerAndFit(m_bMainSizer);
	this->Layout();

	this->Centre( wxBOTH );
}


//-------------------------------------------------------------------------------
//  wxGISNGWConnDlg
//-------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGISNGWConnDlg, wxDialog)
    EVT_BUTTON(wxID_OK, wxGISNGWConnDlg::OnOK)
    EVT_BUTTON(ID_TESTBUTTON, wxGISNGWConnDlg::OnTest)
END_EVENT_TABLE()

wxGISNGWConnDlg::wxGISNGWConnDlg(wxXmlNode* pConnectionNode, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
    m_bIsFile = false;

    //set default values
    FillDefaults();

    //load values from wconn file
    if (pConnectionNode)
    {
        m_pConnectionNode = pConnectionNode;
        m_sURL = pConnectionNode->GetAttribute(wxT("url"), m_sURL);
        m_sUser = pConnectionNode->GetAttribute(wxT("user"), m_sUser);
        Decrypt(pConnectionNode->GetAttribute(wxT("pass"), wxEmptyString), m_sPass);
    }

    CreateUI(false);
}


wxGISNGWConnDlg::wxGISNGWConnDlg( CPLString pszConnPath, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	m_bCreateNew = !CPLCheckForFile((char*)pszConnPath.c_str(), NULL);
    m_bIsFile = true;

    m_sConnName = wxString(CPLGetFilename(pszConnPath), wxConvUTF8);
	m_sOutputPath = wxString(CPLGetPath(pszConnPath), wxConvUTF8);

	//set default values
	FillDefaults();

	//load values from wconn file
	if(!m_bCreateNew)
	{
        m_sOriginOutput = pszConnPath;
		wxXmlDocument doc(wxString(pszConnPath,  wxConvUTF8));
		if(doc.IsOk())
		{
			wxXmlNode* pRootNode = doc.GetRoot();
			if(pRootNode)
			{
			    m_sURL = pRootNode->GetAttribute(wxT("url"), m_sURL);
                m_sUser = pRootNode->GetAttribute(wxT("user"), m_sUser);
                Decrypt(pRootNode->GetAttribute(wxT("pass"), wxEmptyString), m_sPass);
			}
		}
	}

    CreateUI();
}

wxGISNGWConnDlg::~wxGISNGWConnDlg()
{
}

void wxGISNGWConnDlg::FillDefaults()
{
    m_sURL = wxString("127.0.0.1:6543");
    m_sUser = wxString("administrator");
    m_sPass = wxEmptyString;

    IApplication *pApp = GetApplication();
    if(NULL != pApp)
    {
        m_sUserAgent = pApp->GetAppName() + wxT(" (") + pApp->GetAppDisplayName() + wxT(" - ") + pApp->GetAppVersionString() + wxT(")");
    }
}

void wxGISNGWConnDlg::OnOK(wxCommandEvent& event)
{
	if ( Validate() && TransferDataFromWindow() )
	{
	    wxString sCryptPass;
		if(!Crypt(m_sPass, sCryptPass))
		{
			wxMessageBox(wxString(_("Crypt password failed!")), wxString(_("Error")), wxICON_ERROR | wxOK );
			return;
		}

        if (m_bIsFile)
        {
		    wxXmlDocument doc;
		    wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("NGW"));
		    pRootNode->AddAttribute(wxT("url"), m_sURL);
		    pRootNode->AddAttribute(wxT("user"), m_sUser);
		    pRootNode->AddAttribute(wxT("pass"), sCryptPass);

		    doc.SetRoot(pRootNode);

		    wxString sFullPath = m_sOutputPath + wxFileName::GetPathSeparator() + GetName();
		    if(!m_bCreateNew)// && wxGISEQUAL(CPLString(sFullPath.mb_str(wxConvUTF8)), m_sOriginOutput))
		    {
                RenameFile(m_sOriginOutput, CPLString(sFullPath.mb_str(wxConvUTF8)));
		    }

            if(!doc.Save(sFullPath))
		    {
			    wxMessageBox(wxString(_("Connection create failed!")), wxString(_("Error")), wxICON_ERROR | wxOK );
			    return;
		    }

            //m_sOriginOutput = CPLString(sFullPath.mb_str(wxConvUTF8));
        }
        else
        {
            if (m_pConnectionNode->HasAttribute(wxT("url")))
                m_pConnectionNode->DeleteAttribute(wxT("url"));
            m_pConnectionNode->AddAttribute(wxT("server"), m_sURL);

            if (m_pConnectionNode->HasAttribute(wxT("user")))
                m_pConnectionNode->DeleteAttribute(wxT("user"));
            m_pConnectionNode->AddAttribute(wxT("user"), m_sUser);

            if (m_pConnectionNode->HasAttribute(wxT("pass")))
                m_pConnectionNode->DeleteAttribute(wxT("pass"));
            m_pConnectionNode->AddAttribute(wxT("pass"), sCryptPass);
        }
		EndModal(wxID_OK);
	}
	else
	{
		wxMessageBox(wxString(_("Some input values are incorrect!")), wxString(_("Error")), wxICON_ERROR | wxOK );
	}
}

void wxGISNGWConnDlg::OnTest(wxCommandEvent& event)
{
	wxBusyCursor wait;
	if ( Validate() && TransferDataFromWindow() )
	{
        wxGISCurl curl;
        if (!curl.IsOk())
        {
            wxMessageBox(wxString(_("cURL initialize failed!")), wxString(_("Error")), wxICON_ERROR | wxOK );
            return;
        }

        wxString sURL = wxString::FromUTF8(m_sURL) + wxString(wxT("/login"));
        if (!sURL.StartsWith(wxT("http")))
        {
            sURL.Prepend(wxT("http://"));
        }

        wxString sPostData = wxString::Format(wxT("login=%s&password=%s"), m_sUser.c_str(), m_sPass.c_str());

        PERFORMRESULT res = curl.Post(sURL, sPostData);

        if(res.IsValid)
        {
            if(res.sHead.Find(wxT("Set-Cookie")) != wxNOT_FOUND)
            {
                wxMessageBox(wxString(_("Connected successfully!")), wxString(_("Information")), wxICON_INFORMATION | wxOK, this );
            }
            else
            {
                wxMessageBox(wxString(_("Connected successfully as guest!")), wxString(_("Information")), wxICON_INFORMATION | wxOK, this );
            }
        }
	}
	else
	{
		wxMessageBox(wxString(_("Some input values are not correct!")), wxString(_("Error")), wxICON_ERROR | wxOK );
	}
}

CPLString wxGISNGWConnDlg::GetPath(void)
{
	return CPLString(wxString(m_sOutputPath + wxFileName::GetPathSeparator() + GetName()).mb_str(wxConvUTF8));
}

wxString wxGISNGWConnDlg::GetName(void)
{
 	if(!m_sConnName.Lower().EndsWith(wxT(".wconn")))
		m_sConnName.Append(wxT(".wconn"));
	return m_sConnName;
}

void wxGISNGWConnDlg::CreateUI(bool bHasConnectionPath)
{
    m_bMainSizer = new wxBoxSizer( wxVERTICAL );

    if(bHasConnectionPath)
    {
        m_ConnName = new wxTextCtrl( this, ID_CONNNAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sConnName) );
        m_bMainSizer->Add( m_ConnName, 0, wxALL|wxEXPAND, 5 );

        wxStaticLine *staticline = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
        m_bMainSizer->Add( staticline, 0, wxEXPAND | wxALL, 5 );
    }

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText *URLStaticText = new wxStaticText( this, wxID_ANY, _("URL:"), wxDefaultPosition, wxDefaultSize, 0 );
	URLStaticText->Wrap( -1 );
	fgSizer1->Add( URLStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	wxTextCtrl *URLTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0,  wxGenericValidator(&m_sURL) );
	fgSizer1->Add( URLTextCtrl, 0, wxALL|wxEXPAND, 5 );

	m_bMainSizer->Add( fgSizer1, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Account") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText *UserStaticText = new wxStaticText( this, wxID_ANY, _("User:"), wxDefaultPosition, wxDefaultSize, 0 );
	UserStaticText->Wrap( -1 );
	fgSizer2->Add( UserStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	wxTextCtrl *UsesTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sUser) );
	fgSizer2->Add( UsesTextCtrl, 0, wxALL|wxEXPAND, 5 );

	wxStaticText *PassStaticText = new wxStaticText( this, wxID_ANY, _("Password:"), wxDefaultPosition, wxDefaultSize, 0 );
	PassStaticText->Wrap( -1 );
	fgSizer2->Add( PassStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	wxTextCtrl *PassTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD, wxGenericValidator(&m_sPass) );
	fgSizer2->Add( PassTextCtrl, 0, wxALL|wxEXPAND, 5 );

	sbSizer1->Add( fgSizer2, 1, wxEXPAND, 5 );

	m_bMainSizer->Add( sbSizer1, 0, wxEXPAND|wxALL, 5 );

	m_TestButton = new wxButton( this, ID_TESTBUTTON, _("Test Connection"), wxDefaultPosition, wxDefaultSize, 0 );
	m_bMainSizer->Add( m_TestButton, 0, wxALL|wxEXPAND, 5 );

	m_staticline2 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_bMainSizer->Add( m_staticline2, 0, wxEXPAND | wxALL, 5 );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK, _("OK") );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL, _("Cancel") );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();
	m_bMainSizer->Add( m_sdbSizer, 0, wxEXPAND|wxALL, 5 );

    this->SetSizerAndFit(m_bMainSizer);
	this->Layout();

	this->Centre( wxBOTH );
}

#endif // wxGIS_USE_CURL

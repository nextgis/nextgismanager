/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Create Remote Database connection dialog.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013,2014 Dmitry Baryshnikov
*   Copyright (C) 2014 NextGIS
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
#include "wxgis/catalogui/gxfileui.h"
#include "wxgis/catalogui/gxobjdialog.h"
#include "wxgis/catalogui/minimapdlg.h"
#include "wxgis/carto/rasterlayer.h"

#include <wx/valgen.h>
#include <wx/valnum.h>
#include <wx/valtext.h>
#include <wx/valnum.h>
#include <wx/collpane.h>
#include <wx/textfile.h>
#include <wx/mstream.h>
#include <wx/txtstrm.h>

#include "../../art/open.xpm"
#include "../../art/list_add.xpm"
#include "../../art/list_remove.xpm"
#include "../../art/export.xpm"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/datasource/postgisdataset.h"

//-------------------------------------------------------------------------------
// wxGISCreateMetadataItemDlg
//-------------------------------------------------------------------------------

wxGISCreateMetadataItemDlg::wxGISCreateMetadataItemDlg(const wxArrayString &choices, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	
	wxBoxSizer* bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer->AddGrowableCol( 1 );
	fgSizer->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	wxStaticText *staticText = new wxStaticText( this, wxID_ANY, _("Name"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	staticText->Wrap( -1 );
	fgSizer->Add( staticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxTextCtrl *textCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 , wxGenericValidator(&m_sMetadataItemName));
	fgSizer->Add( textCtrl, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	staticText = new wxStaticText( this, wxID_ANY, _("Type"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	staticText->Wrap( -1 );
	fgSizer->Add( staticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );	
	
	wxChoice *choice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices, 0 , wxGenericValidator(&m_sMetadataItemType));
	choice->SetSelection( 0 );
	fgSizer->Add( choice, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	bMainSizer->Add( fgSizer, 1, wxEXPAND, 5 );
	
	wxStaticLine *staticline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    bMainSizer->Add(staticline, 0, wxALL | wxEXPAND, 5);
	
	wxStdDialogButtonSizer *sdbSizer1 = new wxStdDialogButtonSizer();
	wxButton *sdbSizer1OK = new wxButton( this, wxID_OK, _("OK") );
	sdbSizer1->AddButton( sdbSizer1OK );
	wxButton *sdbSizer1Cancel = new wxButton( this, wxID_CANCEL, _("Cancel") );
	sdbSizer1->AddButton( sdbSizer1Cancel );
	sdbSizer1->Realize();
	bMainSizer->Add( sdbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	this->SetSizerAndFit( bMainSizer );
	this->Layout();
	
	this->Centre( wxBOTH );
}

wxGISCreateMetadataItemDlg::~wxGISCreateMetadataItemDlg()
{
}

const wxString& wxGISCreateMetadataItemDlg::GetItemName() const
{
	return m_sMetadataItemName;
}

const wxString& wxGISCreateMetadataItemDlg::GetItemType() const
{
	return m_sMetadataItemType;
}
	
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
	
	m_pConnectionNode = NULL;

    m_sServer = wxString(wxT("localhost"));
    m_sPort = wxString(wxT("5432"));
    m_sDatabase = wxString(wxT("postgres"));
    m_bIsBinaryCursor = true;
	m_sDefaultTimeOut = wxString(wxT("30"));
	m_sDefaultTestTimeOut = wxString(wxT("80"));
	
	wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
	{
        m_sServer = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/server")), m_sServer);
		m_sPort = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/port")), m_sPort);
		m_sDatabase = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/db")), m_sDatabase);
		m_bIsBinaryCursor = oConfig.ReadBool(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/is_binary_cursor")), m_bIsBinaryCursor);
		m_sDefaultTimeOut = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/connect_timeout")), m_sDefaultTimeOut);
		m_sDefaultTestTimeOut = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/test_connect_timeout")), m_sDefaultTestTimeOut);
	}

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
		m_sDefaultTimeOut = pConnectionNode->GetAttribute(wxT("connect_timeout"), m_sDefaultTimeOut);
    }

    //this->SetSizeHints( wxSize( 320,REMOTECONNDLG_MAX_HEIGHT ), wxSize( -1,REMOTECONNDLG_MAX_HEIGHT ) );
	
    CreateUI(false);
}

wxGISRemoteDBConnDlg::wxGISRemoteDBConnDlg(const wxString &sName, const wxString &sServer, const wxString &sDatabase, const wxString &sUser, const wxString &sPassword, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
    m_bIsFile = false;
	
	m_sConnName = sName;
	m_pConnectionNode = NULL;

    m_sServer = sServer;
    m_sPort = wxString(wxT("5432"));
    m_sDatabase = sDatabase;
    m_bIsBinaryCursor = true;
	m_sDefaultTimeOut = wxString(wxT("30"));
	m_sDefaultTestTimeOut = wxString(wxT("80"));
	
	wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
	{
		m_sPort = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/port")), m_sPort);
		m_bIsBinaryCursor = oConfig.ReadBool(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/is_binary_cursor")), m_bIsBinaryCursor);
		m_sDefaultTimeOut = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/connect_timeout")), m_sDefaultTimeOut);
		m_sDefaultTestTimeOut = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/test_connect_timeout")), m_sDefaultTestTimeOut);
	}

	m_sUser = sUser;
	m_sPass = sPassword;

    //this->SetSizeHints( wxSize( 320,REMOTECONNDLG_MAX_HEIGHT ), wxSize( -1,REMOTECONNDLG_MAX_HEIGHT ) );
	
    CreateUI(true);
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
	m_sDefaultTimeOut = wxString(wxT("30"));
	m_sDefaultTestTimeOut = wxString(wxT("80"));
	
	wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
	{
        m_sServer = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/server")), m_sServer);
		m_sPort = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/port")), m_sPort);
		m_sDatabase = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/db")), m_sDatabase);
		m_bIsBinaryCursor = oConfig.ReadBool(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/is_binary_cursor")), m_bIsBinaryCursor);
		m_sDefaultTimeOut = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/connect_timeout")), m_sDefaultTimeOut);
		m_sDefaultTestTimeOut = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/postgres/test_connect_timeout")), m_sDefaultTestTimeOut);
	}
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
				m_sDefaultTimeOut = pRootNode->GetAttribute(wxT("connect_timeout"), m_sDefaultTimeOut);
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
		m_sDefaultTimeOut = wxString::Format(wxT("%d"), m_nConnTimeout);
		wxString sCryptPass;
		if(!Crypt(m_sPass, sCryptPass))
		{
			wxGISErrorMessageBox(_("Crypt password failed!"));
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
			pRootNode->AddAttribute(wxT("connect_timeout"), m_sDefaultTimeOut);

		    pRootNode->AddAttribute(wxT("type"), wxT("POSTGIS"));//store server type for future

		    doc.SetRoot(pRootNode);

		    wxString sFullPath = m_sOutputPath + wxFileName::GetPathSeparator() + GetName();			
						
			if(m_bCreateNew && wxFileName::Exists(sFullPath))
			{
				wxGISErrorMessageBox(wxString(_("The connection file already exist!")));
				return;
			}
			
		    if(!m_bCreateNew)
		    {
                RenameFile(m_sOriginOutput, CPLString(sFullPath.mb_str(wxConvUTF8)));
		    }

            if(!doc.Save(sFullPath))
		    {
			    wxGISErrorMessageBox(_("Connection create failed!"));
			    return;
		    }

            //m_sOriginOutput = CPLString(sFullPath.mb_str(wxConvUTF8));
        }
        else
        {
			if(m_pConnectionNode)
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
				
				if (m_pConnectionNode->HasAttribute(wxT("connect_timeout")))
					m_pConnectionNode->DeleteAttribute(wxT("connect_timeout"));
				m_pConnectionNode->AddAttribute(wxT("connect_timeout"), m_sDefaultTimeOut);

				if (m_pConnectionNode->HasAttribute(wxT("type")))
					m_pConnectionNode->DeleteAttribute(wxT("type"));
				m_pConnectionNode->AddAttribute(wxT("type"), wxT("POSTGIS"));//store server type for future
			}
        }
		EndModal(wxID_OK);
	}
	else
	{
		wxGISErrorMessageBox(_("Some input values are incorrect!"));
	}
}

void wxGISRemoteDBConnDlg::OnTest(wxCommandEvent& event)
{
	wxBusyCursor wait;
	if ( Validate() && TransferDataFromWindow() )
	{
		m_sDefaultTimeOut = wxString::Format(wxT("%d"), m_nConnTimeout);
		wxGISPostgresDataSource oPostgresDataSource(m_sUser, m_sPass, m_sPort, m_sServer, m_sDatabase, m_sDefaultTestTimeOut, m_bIsBinaryCursor);
		if( oPostgresDataSource.Open(false, true ) )
		{
			wxMessageBox(_("Connected successfully!"), _("Information"), wxICON_INFORMATION | wxOK );
		}
		else
		{
			wxString sErr = wxString::Format(_("Operation '%s' failed!\nHost '%s', Database name '%s', Port='%s'"), _("Open"), m_sServer.c_str(), m_sDatabase.c_str(), m_sPort.c_str());
			wxGISErrorMessageBox(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()));
		}
	}
	else
	{
		wxGISErrorMessageBox(_("Some input values are not correct!"));
	}
}

CPLString wxGISRemoteDBConnDlg::GetPath(void) const
{
	return CPLString(wxString(m_sOutputPath + wxFileName::GetPathSeparator() + GetName()).ToUTF8());
}

wxString wxGISRemoteDBConnDlg::GetName(void) const
{
	wxString sConnName = m_sConnName;
	if(m_bIsFile)
	{
		if(!sConnName.Lower().EndsWith(wxT(".xconn")))
			sConnName.Append(wxT(".xconn"));
	}
	return sConnName;
}

wxString wxGISRemoteDBConnDlg::GetUser() const
{
	return m_sUser;
}

wxString wxGISRemoteDBConnDlg::GetPassword() const
{
	return m_sPass;
}

wxString wxGISRemoteDBConnDlg::GetDatabase() const
{
	return m_sDatabase;
}

wxString wxGISRemoteDBConnDlg::GetHost() const
{
	return m_sServer;
}

void wxGISRemoteDBConnDlg::CreateUI(bool bHasConnectionPath)
{
	m_nConnTimeout = wxAtoi(m_sDefaultTimeOut);
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
	
	wxStaticText* pTimeoutStaticText = new wxStaticText( this, wxID_ANY, _("Timeout (s):"), wxDefaultPosition, wxDefaultSize, 0 );
	pTimeoutStaticText->Wrap( -1 );
	fgSizer1->Add( pTimeoutStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	wxIntegerValidator<unsigned int> val(&m_nConnTimeout);
	val.SetRange(2, 3600);
	wxTextCtrl* pTimeoutTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, val );
	fgSizer1->Add( pTimeoutTextCtrl, 0, wxALL|wxEXPAND, 5 );

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
	EVT_BUTTON(ID_SELECTSRS, wxGISTMSConnDlg::OnSelectSRS)
	EVT_CHOICE(ID_PRESETTYPE, wxGISTMSConnDlg::OnSelectPreset)
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
        LoadValues(pConnectionNode);
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
				LoadValues(pRootNode);
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
	
	m_nTileSizeX = m_nTileSizeY = 256;
	m_nBandsCount = 3;
	m_nMaxConnections = 5;
	m_sSRS = wxString(wxT("..."));
	m_nTileLevel = 18;
	
	m_dfULX = -20037508.34;
	m_dfULY = 20037508.34;
	m_dfLRX = 20037508.34;
	m_dfLRY = -20037508.34;

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
        m_sZeroBlockOnServerException = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/zero_block_onserver_exception")), m_sZeroBlockOnServerException);
        m_sOfflineMode = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/offline_mode")), m_sOfflineMode);
        m_sAdviseRead = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/advise_read")), m_sAdviseRead);
        m_sVerifyAdviseRead = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/verify_advise_read")), m_sVerifyAdviseRead);
        m_sClampRequests = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/WMS/clamp_requests")), m_sClampRequests);
    }
}

wxXmlNode* wxGISTMSConnDlg::CreateContentNode(wxXmlNode* pParentNode, wxString sName, wxString sContent)
{
	wxXmlNode* pNode = new wxXmlNode(pParentNode, wxXML_ELEMENT_NODE, sName);
	wxXmlNode* pContenNode = new wxXmlNode(pNode, wxXML_TEXT_NODE, wxEmptyString);
	pContenNode->SetContent(sContent);	
	return pNode;
}

wxXmlNode* wxGISTMSConnDlg::CreateContentNode(wxXmlNode* pParentNode, wxString sName, double dfContent)
{
	wxString sVal = wxString::Format(wxT("%f"), dfContent);
	FloatStringToCLoc(sVal);
	wxXmlNode* pNode = new wxXmlNode(pParentNode, wxXML_ELEMENT_NODE, sName);
	wxXmlNode* pContenNode = new wxXmlNode(pNode, wxXML_TEXT_NODE, wxEmptyString);
	pContenNode->SetContent(sVal);	
	return pNode;
}    

void wxGISTMSConnDlg::FillValues(wxXmlNode* pRootNode)
{
	if(NULL == pRootNode)
		return;
		
	//service
	wxXmlNode* pServiceNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("Service"));
	pServiceNode->AddAttribute(wxT("name"), wxT("TMS"));
	CreateContentNode(pServiceNode, wxT("ServerUrl"), m_sURL);
	
	//datawindow
	wxXmlNode* pDataWindowNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("DataWindow"));
	CreateContentNode(pDataWindowNode, wxT("UpperLeftX"), m_dfULX);
	CreateContentNode(pDataWindowNode, wxT("UpperLeftY"), m_dfULY);
	CreateContentNode(pDataWindowNode, wxT("LowerRightX"), m_dfLRX);
	CreateContentNode(pDataWindowNode, wxT("LowerRightY"), m_dfLRY);
	CreateContentNode(pDataWindowNode, wxT("TileLevel"), wxString::Format(wxT("%d"), m_nTileLevel));

	wxString sOriginType = wxString(wxT("top"));
	if(m_sOriginTypeChoice.IsSameAs(_("Normal TMS")))
		sOriginType = wxString(wxT("bottom"));

		
	CreateContentNode(pDataWindowNode, wxT("YOrigin"), sOriginType);

	CreateContentNode(pRootNode, wxT("Projection"), m_sSRS);
	CreateContentNode(pRootNode, wxT("BandsCount"), wxString::Format(wxT("%d"), m_nBandsCount));
	CreateContentNode(pRootNode, wxT("BlockSizeX"), wxString::Format(wxT("%d"), m_nTileSizeX));
	CreateContentNode(pRootNode, wxT("BlockSizeY"), wxString::Format(wxT("%d"), m_nTileSizeY));


	wxXmlNode* pCacheNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("Cache"));
	CreateContentNode(pCacheNode, wxT("Path"), m_sCachePath);
	CreateContentNode(pCacheNode, wxT("Depth"), m_sCacheDepth);
		
	CreateContentNode(pRootNode, wxT("MaxConnections"), wxString::Format(wxT("%d"), m_nMaxConnections));
	CreateContentNode(pRootNode, wxT("Timeout"), m_sTimeout);
	CreateContentNode(pRootNode, wxT("OfflineMode"), m_sOfflineMode);
	CreateContentNode(pRootNode, wxT("AdviseRead"), m_sAdviseRead);
	CreateContentNode(pRootNode, wxT("VerifyAdviseRead"), m_sVerifyAdviseRead);
	CreateContentNode(pRootNode, wxT("ClampRequests"), m_sClampRequests);
	CreateContentNode(pRootNode, wxT("UserAgent"), m_sUserAgent);
	CreateContentNode(pRootNode, wxT("Referer"), m_sReferer);
	
	if(!m_sUser.IsEmpty() || !m_sPass.IsEmpty())
	{
		CreateContentNode(pRootNode, wxT("UserPwd"), m_sUser + wxT(":") + m_sPass);
	}
	
	CreateContentNode(pRootNode, wxT("ZeroBlockHttpCodes"), m_sZeroBlockHttpCodes);
	CreateContentNode(pRootNode, wxT("ZeroBlockOnServerException"), m_sZeroBlockOnServerException);
}


void wxGISTMSConnDlg::LoadValues(wxXmlNode* pRootNode)
{
	if(NULL == pRootNode)
		return;
	wxXmlNode* pChild = pRootNode->GetChildren();
	while(pChild)
	{
		if(pChild->GetName().IsSameAs(wxT("Service"), false))
		{
			LoadServiceValues(pChild);
		}
		else if(pChild->GetName().IsSameAs(wxT("DataWindow"), false))
		{
			LoadDataWindowValues(pChild);
		}
		else if(pChild->GetName().IsSameAs(wxT("Projection"), false))
		{
			wxGISSpatialReference SpatRef(pChild->GetNodeContent());
			if(SpatRef.IsOk())
				m_sSRS = SpatRef.GetName();
		}
		else if(pChild->GetName().IsSameAs(wxT("BandsCount"), false))
		{
			m_nBandsCount = wxAtoi(pChild->GetNodeContent());
		}
		else if(pChild->GetName().IsSameAs(wxT("BlockSizeX"), false))
		{
			m_nTileSizeX = wxAtoi(pChild->GetNodeContent());
		}
		else if(pChild->GetName().IsSameAs(wxT("BlockSizeY"), false))
		{
			m_nTileSizeY = wxAtoi(pChild->GetNodeContent());
		}
		else if(pChild->GetName().IsSameAs(wxT("MaxConnections"), false))
		{
			m_nMaxConnections = wxAtoi(pChild->GetNodeContent());
		}
		else if(pChild->GetName().IsSameAs(wxT("Timeout"), false))
		{
			m_sTimeout = pChild->GetNodeContent();
		}
		else if(pChild->GetName().IsSameAs(wxT("OfflineMode"), false))
		{
			m_sOfflineMode = pChild->GetNodeContent();
		}
		else if(pChild->GetName().IsSameAs(wxT("AdviseRead"), false))
		{
			m_sAdviseRead = pChild->GetNodeContent();
		}
		else if(pChild->GetName().IsSameAs(wxT("VerifyAdviseRead"), false))
		{
			m_sVerifyAdviseRead = pChild->GetNodeContent();
		}
		else if(pChild->GetName().IsSameAs(wxT("ClampRequests"), false))
		{
			m_sClampRequests = pChild->GetNodeContent();
		}
		else if(pChild->GetName().IsSameAs(wxT("UserPwd"), false))
		{
			wxString sUP = pChild->GetNodeContent();
			int nPos = sUP.Find(':');
			if(nPos != wxNOT_FOUND)
			{
				m_sUser = sUP.Left(nPos);
				m_sPass = sUP.Right(sUP.Len() - nPos - 1);
			}
		}
		else if(pChild->GetName().IsSameAs(wxT("Referer"), false))
		{
			m_sReferer = pChild->GetNodeContent();
		}
		else if(pChild->GetName().IsSameAs(wxT("ZeroBlockHttpCodes"), false))
		{
			m_sZeroBlockHttpCodes = pChild->GetNodeContent(); 
		}
		else if(pChild->GetName().IsSameAs(wxT("ZeroBlockOnServerException"), false))
		{
			m_sZeroBlockOnServerException = pChild->GetNodeContent();
		}

		pChild = pChild->GetNext();
	}
}

void wxGISTMSConnDlg::LoadServiceValues(wxXmlNode* pRootNode)
{
	if(NULL == pRootNode)
		return;
	wxXmlNode* pChild = pRootNode->GetChildren();
	while(pChild)
	{
		if(pChild->GetName().IsSameAs(wxT("ServerUrl"), false))
		{
			m_sURL = pChild->GetNodeContent();
		}
		else if(pChild->GetName().IsSameAs(wxT("SRS"), false))
		{
			wxGISSpatialReference SpatRef(pChild->GetNodeContent());
			if(SpatRef.IsOk())
				m_sSRS = SpatRef.GetName();
		}
		else if(pChild->GetName().IsSameAs(wxT("CRS"), false))
		{
			wxGISSpatialReference SpatRef(pChild->GetNodeContent());
			if(SpatRef.IsOk())
				m_sSRS = SpatRef.GetName();
		}
		pChild = pChild->GetNext();
	}
}

void wxGISTMSConnDlg::LoadDataWindowValues(wxXmlNode* pRootNode)
{
	if(NULL == pRootNode)
		return;
	wxXmlNode* pChild = pRootNode->GetChildren();
	while(pChild)
	{
		if(pChild->GetName().IsSameAs(wxT("UpperLeftX"), false))
		{
			m_dfULX = CPLAtofM(pChild->GetNodeContent().mb_str());
		}
		else if(pChild->GetName().IsSameAs(wxT("UpperLeftY"), false))
		{
			m_dfULY = CPLAtofM(pChild->GetNodeContent().mb_str());
		}
		else if(pChild->GetName().IsSameAs(wxT("LowerRightX"), false))
		{
			m_dfLRX = CPLAtofM(pChild->GetNodeContent().mb_str());
		}
		else if(pChild->GetName().IsSameAs(wxT("LowerRightY"), false))
		{
			m_dfLRY = CPLAtofM(pChild->GetNodeContent().mb_str());
		}
		else if(pChild->GetName().IsSameAs(wxT("YOrigin"), false))
		{
			if(pChild->GetNodeContent().IsSameAs(wxT("top")))
				m_sOriginTypeChoice = wxString(_("Slippy map"));
			else
				m_sOriginTypeChoice = wxString(_("Normal TMS"));
		}
		else if(pChild->GetName().IsSameAs(wxT("TileLevel"), false))
		{
			m_nTileLevel = wxAtoi(pChild->GetNodeContent());
		}
		pChild = pChild->GetNext();
	}
}

void wxGISTMSConnDlg::OnOK(wxCommandEvent& event)
{
	if ( Validate() && TransferDataFromWindow() )
	{
        if (m_bIsFile)
        {
		    wxXmlDocument doc;
		    wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("GDAL_WMS"));
						
			FillValues(pRootNode);

		    doc.SetRoot(pRootNode);
			

		    wxString sFullPath = m_sOutputPath + wxFileName::GetPathSeparator() + GetName();
			
			if(m_bCreateNew && wxFileName::Exists(sFullPath))
			{
				wxGISErrorMessageBox(wxString(_("The connection file already exist!")));
				return;
			}
			
		    if(!m_bCreateNew)
		    {
                RenameFile(m_sOriginOutput, CPLString(sFullPath.mb_str(wxConvUTF8)));
		    }
			
			wxMemoryOutputStream out;
            if(!doc.Save(out))
		    {
			    wxGISErrorMessageBox(_("Connection create failed!"));
			    return;
		    }
			
            wxTextFile outputFile(sFullPath);
			
			wxMemoryInputStream in(out);
			wxTextInputStream text(in);
			wxString line;
			bool bSkipProlog = true;
			while (!in.Eof()) {
				line = text.ReadLine();
				if(bSkipProlog)
				{
					bSkipProlog = false;
					continue;
				}
				
				if (!line.IsEmpty()) 
				{
					outputFile.AddLine(line);
				}
			}
			
			if(!outputFile.Write())
			{
				wxGISErrorMessageBox(_("Connection create failed!"));
			    return;
			}
            //m_sOriginOutput = CPLString(sFullPath.mb_str(wxConvUTF8));
        }
        else
        {
			wxGISConfig::DeleteNodeChildren(m_pConnectionNode);
			FillValues(m_pConnectionNode);
        }
		EndModal(wxID_OK);
	}
	else
	{
		wxGISErrorMessageBox(_("Some input values are incorrect!"));
	}
}

void wxGISTMSConnDlg::OnTest(wxCommandEvent& event)
{
	//TODO: do it via separate map window with TMS layer from input parameters to visual testing if layer shown
	if ( Validate() && TransferDataFromWindow() )
	{
		wxXmlDocument doc;
		wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("GDAL_WMS"));
						
		FillValues(pRootNode);

		doc.SetRoot(pRootNode);
			
		wxMemoryOutputStream out;
		if(!doc.Save(out))
		{
			wxGISErrorMessageBox(_("Test failed!"));
			return;
		}
			
        wxString line, connStr;
		bool bSkipProlog = true;
		wxMemoryInputStream in(out);
		wxTextInputStream text(in);
		while (!in.Eof()) {
			line = text.ReadLine();
			if(bSkipProlog)
			{
				bSkipProlog = false;
				continue;
			}
			
			if (!line.IsEmpty()) 
			{
				connStr += line;
			}
		}
			
		wxGISMiniMapDlg dlg(wxOK, this);
		wxGISRasterDataset* pDSet = new wxGISRasterDataset(CPLString(connStr.ToUTF8()), enumRasterWMSTMS);
		wxGISDataset* pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
		pwxGISDataset->Reference();
		if (!pDSet->IsOpened())
			pDSet->Open(true);
		wxGISRasterLayer* pGISRasterLayer = new wxGISRasterLayer(wxT("Test TMS"), pwxGISDataset);

		dlg.AddLayer(pGISRasterLayer);
		dlg.SetFullExtent();
				
		dlg.ShowModal();
	}
	else
	{
		wxGISErrorMessageBox(_("Some input values are not correct!"));
	}
}

CPLString wxGISTMSConnDlg::GetPath(void)
{
	return CPLString(wxString(m_sOutputPath + wxFileName::GetPathSeparator() + GetName()).ToUTF8());
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
	saPresetChoices.Add(_("No preset"));
    saPresetChoices.Add(wxT("OpenStreetMap"));
    saPresetChoices.Add(wxT("BlueMarble Amazon S3"));
    saPresetChoices.Add(wxT("Google Maps - Map"));
    saPresetChoices.Add(wxT("Google Maps - Satellite"));
    saPresetChoices.Add(wxT("Google Maps - Hybrid"));
    saPresetChoices.Add(wxT("Google Maps - Terrain"));
    saPresetChoices.Add(wxT("Google Maps - Terrain, Streets and Water"));
    saPresetChoices.Add(wxT("ArcGIS MapServer Tiles"));

	m_PresetsType = new wxChoice( this, ID_PRESETTYPE, wxDefaultPosition, wxDefaultSize, saPresetChoices, 0);
	m_PresetsType->Select(0);
	sbSizerPresets->Add( m_PresetsType, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
    m_bMainSizer->Add( sbSizerPresets, 0, wxEXPAND|wxALL, 5 );

    //add other controls

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 12, 2, 0, 0 );
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
	wxStaticText *TileLevelStaticText = new wxStaticText( this, wxID_ANY, _("Tile Level"), wxDefaultPosition, wxDefaultSize, 0 );
	TileLevelStaticText->Wrap( -1 );
	fgSizer1->Add( TileLevelStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

    wxIntegerValidator<int> val_tilelevel(&m_nTileLevel);
	wxTextCtrl *TileLevelTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, val_tilelevel );
	fgSizer1->Add( TileLevelTextCtrl, 0, wxALL|wxEXPAND, 5 );
	
	// <TileCountX>1</TileCountX> - optional
	// <TileCountY>1</TileCountY> - optional

	//<YOrigin>top</YOrigin>
	wxStaticText *TMSTypeStaticText = new wxStaticText( this, wxID_ANY, _("TMS type:"), wxDefaultPosition, wxDefaultSize, 0 );
	TMSTypeStaticText->Wrap( -1 );
	fgSizer1->Add( TMSTypeStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString saOriginChoices;
    saOriginChoices.Add(_("Slippy map"));
    saOriginChoices.Add(_("Normal TMS"));

	wxChoice *TMSType = new wxChoice( this, ID_PRESETTYPE, wxDefaultPosition, wxDefaultSize, saOriginChoices, 0, wxGenericValidator(&m_sOriginTypeChoice) );
	TMSType->SetSelection(0);
	fgSizer1->Add( TMSType, 0, wxALL|wxEXPAND, 5 );

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
	wxStaticText *ProjStaticText = new wxStaticText( this, wxID_ANY, _("Projection:"), wxDefaultPosition, wxDefaultSize, 0 );
	ProjStaticText->Wrap( -1 );
	fgSizer1->Add( ProjStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	wxBoxSizer* bPathSizer;
	bPathSizer = new wxBoxSizer( wxHORIZONTAL );

    m_pSRSTextCtrl = new wxTextCtrl( this, wxID_ANY, m_sSRS, wxDefaultPosition, wxDefaultSize, wxTE_CHARWRAP);
    m_pSRSTextCtrl->SetEditable(false);
	bPathSizer->Add(m_pSRSTextCtrl, 1, wxALL | wxEXPAND, 0);

	wxBitmapButton *bpButton = new wxBitmapButton( this, ID_SELECTSRS, wxBitmap(open_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bPathSizer->Add( bpButton, 0, wxALL, 0 );
	fgSizer1->Add( bPathSizer, 0, wxALL|wxEXPAND, 5 );

	m_bMainSizer->Add( fgSizer1, 0, wxEXPAND, 5 );

	wxCollapsiblePane *collpane = new wxCollapsiblePane(this, wxID_ANY, _("Account"));
	m_bMainSizer->Add(collpane, 0, wxGROW|wxALL, 5);
	
	wxWindow *win = collpane->GetPane();
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	wxStaticText *UserStaticText = new wxStaticText( win, wxID_ANY, _("User:"), wxDefaultPosition, wxDefaultSize, 0 );
	UserStaticText->Wrap( -1 );
	fgSizer2->Add( UserStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	wxTextCtrl *UsesTextCtrl = new wxTextCtrl( win, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sUser) );
	fgSizer2->Add( UsesTextCtrl, 0, wxALL|wxEXPAND, 5 );

	wxStaticText *PassStaticText = new wxStaticText( win, wxID_ANY, _("Password:"), wxDefaultPosition, wxDefaultSize, 0 );
	PassStaticText->Wrap( -1 );
	fgSizer2->Add( PassStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	wxTextCtrl *PassTextCtrl = new wxTextCtrl( win, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD, wxGenericValidator(&m_sPass) );
	fgSizer2->Add( PassTextCtrl, 0, wxALL|wxEXPAND, 5 );

	win->SetSizer(fgSizer2);
	fgSizer2->SetSizeHints(win);
	
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


void wxGISTMSConnDlg::OnSelectSRS(wxCommandEvent& event)
{
    wxGxObjectDialog dlg(this, wxID_ANY, _("Select Spatial Reference"));
    dlg.SetAllowMultiSelect(false);
    dlg.SetAllFilters(false);
    dlg.SetOwnsFilter(true);
    dlg.SetStartingLocation(_("Coordinate Systems"));
	dlg.AddFilter(new wxGxPrjFileFilter());
    if(dlg.ShowModalOpen() == wxID_OK)
    {
		wxString sPath = dlg.GetFullName();
		wxGxCatalogBase* pCatalog = GetGxCatalog();
		if(pCatalog)
		{
			wxGxObject* pGxObj = pCatalog->FindGxObject(sPath);
			wxGxPrjFileUI* pGxPrjFileUI = dynamic_cast<wxGxPrjFileUI*>(pGxObj);
			if(pGxPrjFileUI)
			{
				wxGISSpatialReference SpaRef = pGxPrjFileUI->GetSpatialReference();
				m_pSRSTextCtrl->ChangeValue( SpaRef.GetName() );
				m_sSRS = SpaRef.ExportAsWKT();
			}
		}  
    }
}

void wxGISTMSConnDlg::OnSelectPreset(wxCommandEvent& event)
{
	int nSel = m_PresetsType->GetCurrentSelection();
	switch(nSel)
	{
		case 0: //"No preset"
			break;
		case 1: //"OpenStreetMap"
			m_sURL = wxString(wxT("http://tile.openstreetmap.org/${z}/${x}/${y}.png"));
			m_dfULX = -20037508.34;
			m_dfULY = 20037508.34;
			m_dfLRX = 20037508.34;
			m_dfLRY = -20037508.34;
			m_nMaxConnections = 20;
			m_sOriginTypeChoice = wxString(_("Slippy map"));
			m_nTileSizeX = 256;
			m_nTileSizeY = 256;
			m_nBandsCount = 3;
			m_sSRS = wxString(wxT("EPSG:3857"));
			m_pSRSTextCtrl->ChangeValue( m_sSRS );
			m_nTileLevel = 18;
			TransferDataToWindow();
			break;
		case 2: //"BlueMarble Amazon S3"
			m_sURL = wxString(wxT("http://s3.amazonaws.com/com.modestmaps.bluemarble/${z}-r${y}-c${x}.jpg"));
			m_dfULX = -20037508.34;
			m_dfULY = 20037508.34;
			m_dfLRX = 20037508.34;
			m_dfLRY = -20037508.34;
			m_nMaxConnections = 20;
			m_sOriginTypeChoice = wxString(_("Slippy map"));
			m_nTileSizeX = 256;
			m_nTileSizeY = 256;
			m_nBandsCount = 3;
			m_sSRS = wxString(wxT("EPSG:3857"));
			m_pSRSTextCtrl->ChangeValue( m_sSRS );
			m_nTileLevel = 9;
			TransferDataToWindow();
			break;
		case 3: //"Google Maps - Map"
			m_sURL = wxString(wxT("http://mt.google.com/vt/lyrs=m&x=${x}&y=${y}&z=${z}"));
			m_dfULX = -20037508.34;
			m_dfULY = 20037508.34;
			m_dfLRX = 20037508.34;
			m_dfLRY = -20037508.34;
			m_nMaxConnections = 5;
			m_sOriginTypeChoice = wxString(_("Slippy map"));
			m_nTileSizeX = 256;
			m_nTileSizeY = 256;
			m_nBandsCount = 3;
			m_sSRS = wxString(wxT("EPSG:3857"));
			m_pSRSTextCtrl->ChangeValue( m_sSRS );
			m_nTileLevel = 20;
			TransferDataToWindow();
			break;
		case 4: //"Google Maps - Satellite"
			m_sURL = wxString(wxT("http://mt.google.com/vt/lyrs=s&x=${x}&y=${y}&z=${z}"));
			m_dfULX = -20037508.34;
			m_dfULY = 20037508.34;
			m_dfLRX = 20037508.34;
			m_dfLRY = -20037508.34;
			m_nMaxConnections = 5;
			m_sOriginTypeChoice = wxString(_("Slippy map"));
			m_nTileSizeX = 256;
			m_nTileSizeY = 256;
			m_nBandsCount = 3;
			m_sSRS = wxString(wxT("EPSG:3857"));
			m_pSRSTextCtrl->ChangeValue( m_sSRS );
			m_nTileLevel = 20;
			TransferDataToWindow();
			break;
		case 5: //"Google Maps - Hybrid"
			m_sURL = wxString(wxT("http://mt.google.com/vt/lyrs=y&x=${x}&y=${y}&z=${z}"));
			m_dfULX = -20037508.34;
			m_dfULY = 20037508.34;
			m_dfLRX = 20037508.34;
			m_dfLRY = -20037508.34;
			m_nMaxConnections = 5;
			m_sOriginTypeChoice = wxString(_("Slippy map"));
			m_nTileSizeX = 256;
			m_nTileSizeY = 256;
			m_nBandsCount = 3;
			m_sSRS = wxString(wxT("EPSG:3857"));
			m_pSRSTextCtrl->ChangeValue( m_sSRS );
			m_nTileLevel = 20;
			TransferDataToWindow();
			break;			
		case 6: //"Google Maps - Terrain"
			m_sURL = wxString(wxT("http://mt.google.com/vt/lyrs=t&x=${x}&y=${y}&z=${z}"));
			m_dfULX = -20037508.34;
			m_dfULY = 20037508.34;
			m_dfLRX = 20037508.34;
			m_dfLRY = -20037508.34;
			m_nMaxConnections = 5;
			m_sOriginTypeChoice = wxString(_("Slippy map"));
			m_nTileSizeX = 256;
			m_nTileSizeY = 256;
			m_nBandsCount = 3;
			m_sSRS = wxString(wxT("EPSG:3857"));
			m_pSRSTextCtrl->ChangeValue( m_sSRS );
			m_nTileLevel = 20;
			TransferDataToWindow();
			break;			
		case 7: //"Google Maps - Terrain, Streets and Water"
			m_sURL = wxString(wxT("http://mt.google.com/vt/lyrs=p&x=${x}&y=${y}&z=${z}"));
			m_dfULX = -20037508.34;
			m_dfULY = 20037508.34;
			m_dfLRX = 20037508.34;
			m_dfLRY = -20037508.34;
			m_nMaxConnections = 5;
			m_sOriginTypeChoice = wxString(_("Slippy map"));
			m_nTileSizeX = 256;
			m_nTileSizeY = 256;
			m_nBandsCount = 3;
			m_sSRS = wxString(wxT("EPSG:3857"));
			m_pSRSTextCtrl->ChangeValue( m_sSRS );
			m_nTileLevel = 20;
			TransferDataToWindow();
			break;			
		case 8: //"ArcGIS MapServer Tiles"
			m_sURL = wxString(wxT("http://services.arcgisonline.com/ArcGIS/rest/services/World_Street_Map/MapServer/tile/${z}/${y}/${x}"));
			m_dfULX = -20037508.34;
			m_dfULY = 20037508.34;
			m_dfLRX = 20037508.34;
			m_dfLRY = -20037508.34;
			m_nMaxConnections = 10;
			m_sOriginTypeChoice = wxString(_("Slippy map"));
			m_nTileSizeX = 256;
			m_nTileSizeY = 256;
			m_nBandsCount = 3;
			m_sSRS = wxString(wxT("EPSG:3857"));
			m_pSRSTextCtrl->ChangeValue( m_sSRS );
			m_nTileLevel = 17;
			TransferDataToWindow();
			break;
		default:
			break;
	}
}

//-------------------------------------------------------------------------------
//  wxGISNGWConnDlg
//-------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGISNGWConnDlg, wxDialog)
    EVT_BUTTON(wxID_OK, wxGISNGWConnDlg::OnOK)
    EVT_BUTTON(ID_TESTBUTTON, wxGISNGWConnDlg::OnTest)
	EVT_BUTTON(wxID_ADD, wxGISNGWConnDlg::OnAddMetadataItem)
	EVT_BUTTON(wxID_REMOVE, wxGISNGWConnDlg::OnRemoveMetadataItem)
	EVT_UPDATE_UI(wxID_REMOVE, wxGISNGWConnDlg::OnRemoveMetadataItemUI)
	EVT_BUTTON(ID_IMPORT, wxGISNGWConnDlg::OnImportMetadata)
	EVT_BUTTON(ID_EXPORT, wxGISNGWConnDlg::OnExportMetadata)
END_EVENT_TABLE()

wxGISNGWConnDlg::wxGISNGWConnDlg(wxXmlNode* pConnectionNode, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
    m_bIsFile = false;
	
	m_pg = NULL;

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
	SerializeMetadata(m_pConnectionNode, false);
}


wxGISNGWConnDlg::wxGISNGWConnDlg( CPLString pszConnPath, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	m_bCreateNew = !CPLCheckForFile((char*)pszConnPath.c_str(), NULL);
    m_bIsFile = true;

    m_sConnName = wxString(CPLGetFilename(pszConnPath), wxConvUTF8);
	m_sOutputPath = wxString(CPLGetPath(pszConnPath), wxConvUTF8);
	
	m_pg = NULL;

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

				CreateUI();
				SerializeMetadata(pRootNode, false);			
				
				return;
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

bool wxGISNGWConnDlg::SerializeMetadata(wxXmlNode* pRootNode, bool bSave)
{
	if(!pRootNode || !m_pg)
		return false;
	
	if(bSave)
	{		
		wxGISConfig::DeleteNodeChildren(pRootNode);
		wxXmlNode* pMetadataNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("metadata"));
		for (wxPropertyGridConstIterator it = m_pg->GetIterator(); !it.AtEnd(); ++it ) 
		{
			const wxPGProperty* p = *it;	
			wxXmlNode* pMetadataItemNode = new wxXmlNode(pMetadataNode, wxXML_ELEMENT_NODE, wxT("item"));
			pMetadataItemNode->AddAttribute(wxT("name"), p->GetName());
			pMetadataItemNode->AddAttribute(wxT("label"), p->GetLabel());
			pMetadataItemNode->AddAttribute(wxT("type"), p->GetClassInfo()->GetClassName());
			pMetadataItemNode->AddAttribute(wxT("value"), p->GetValueAsString(wxPG_FULL_VALUE));
		}
	}
	else
	{
		//search metadata item
		wxXmlNode* pMetadataNode = pRootNode->GetChildren();
		while(pMetadataNode)
		{
			wxString sName = pMetadataNode->GetName();
			if(sName.IsSameAs(wxT("metadata")))
				break;
			pMetadataNode = pMetadataNode->GetNext();
		}
		
		if(pMetadataNode)
		{
			wxXmlNode* pMetadataItemNode = pMetadataNode->GetChildren();
			while(pMetadataItemNode)
			{
				wxString sType = pMetadataItemNode->GetAttribute(wxT("type"));
				wxClassInfo* pClassInfo = wxClassInfo::FindClass(sType);
				if(pClassInfo)
				{
					wxPGProperty* pProp = wxDynamicCast(pClassInfo->CreateObject(), wxPGProperty);
					if(pProp)
					{
						pProp->SetName(pMetadataItemNode->GetAttribute(wxT("name")));
						pProp->SetLabel(pMetadataItemNode->GetAttribute(wxT("label")));
						pProp->SetValueFromString(pMetadataItemNode->GetAttribute(wxT("value")), wxPG_FULL_VALUE);
						m_pg->Append(pProp);
					}
				}
				pMetadataItemNode = pMetadataItemNode->GetNext();
			}
		}
	}
	return true;
}

void wxGISNGWConnDlg::OnOK(wxCommandEvent& event)
{
	if ( Validate() && TransferDataFromWindow() )
	{
	    wxString sCryptPass;
		if(!Crypt(m_sPass, sCryptPass))
		{
			wxGISErrorMessageBox(_("Crypt password failed!"));
			return;
		}

        if (m_bIsFile)
        {
		    wxXmlDocument doc;
		    wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("NGW"));
		    pRootNode->AddAttribute(wxT("url"), m_sURL);
		    pRootNode->AddAttribute(wxT("user"), m_sUser);
		    pRootNode->AddAttribute(wxT("pass"), sCryptPass);
			
			//add metadata
			SerializeMetadata(pRootNode, true);
			

		    doc.SetRoot(pRootNode);

		    wxString sFullPath = m_sOutputPath + wxFileName::GetPathSeparator() + GetName();
			
			if(m_bCreateNew && wxFileName::Exists(sFullPath))
			{
				wxGISErrorMessageBox(wxString(_("The connection file already exist!")));
				return;
			}
	
		    if(!m_bCreateNew)
		    {
                RenameFile(m_sOriginOutput, CPLString(sFullPath.mb_str(wxConvUTF8)));
		    }

            if(!doc.Save(sFullPath))
		    {
			    wxGISErrorMessageBox(_("Connection create failed!"));
			    return;
		    }

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
			
			//add metadata
			SerializeMetadata(m_pConnectionNode, true);
        }
		EndModal(wxID_OK);
	}
	else
	{
		wxGISErrorMessageBox(_("Some input values are incorrect!"));
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
            wxGISErrorMessageBox(_("cURL initialize failed!"));
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
                wxMessageBox(_("Connected successfully!"), _("Information"), wxICON_INFORMATION | wxOK, this );
            }
            else
            {
                wxMessageBox(_("Connected successfully as guest!"), _("Information"), wxICON_INFORMATION | wxOK, this );
            }
        }
		else
		{
			wxGISErrorMessageBox(_("Connect failed!"));
		}
	}
	else
	{
		wxGISErrorMessageBox(_("Some input values are not correct!"));
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
	
	wxCollapsiblePane *collpane = new wxCollapsiblePane(this, wxID_ANY, _("Metadata"));
	m_bMainSizer->Add(collpane, 0, wxGROW|wxALL, 5);
	
	wxWindow *win = collpane->GetPane();
	wxBoxSizer* pBoxSizerMeta = new wxBoxSizer( wxHORIZONTAL );
	m_pg = new wxPropertyGrid(win, ID_PPCTRL, wxDefaultPosition, wxDefaultSize, wxPG_DEFAULT_STYLE | wxPG_TOOLTIPS | wxPG_SPLITTER_AUTO_CENTER | wxPG_BOLD_MODIFIED | wxPG_AUTO_SORT);
    m_pg->SetColumnProportion(0, 30);
    m_pg->SetColumnProportion(1, 70);

    pBoxSizerMeta->Add( m_pg, 1, wxEXPAND | wxALL, 5 );
	
	wxBoxSizer* pBoxSizerMetaButtons = new wxBoxSizer( wxVERTICAL );
	
	wxBitmapButton* bpSizerAdd = new wxBitmapButton( win, wxID_ADD, wxBitmap(list_add_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	pBoxSizerMetaButtons->Add( bpSizerAdd, 0, wxALL, 5 );
	wxBitmapButton*bpSizerDel = new wxBitmapButton( win, wxID_REMOVE, wxBitmap(list_remove_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	pBoxSizerMetaButtons->Add( bpSizerDel, 0, wxALL, 5 );
	
	wxBitmap oExportOrigin(export_xpm);
	wxBitmapButton* bpSizerExport = new wxBitmapButton( win, ID_EXPORT, oExportOrigin, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	pBoxSizerMetaButtons->Add( bpSizerExport, 0, wxALL, 5 );
	
	wxImage oImport = oExportOrigin.ConvertToImage();
	oImport = oImport.Mirror(true);
				
	wxBitmapButton* bpSizerImport = new wxBitmapButton( win, ID_IMPORT, wxBitmap(oImport), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	pBoxSizerMetaButtons->Add( bpSizerImport, 0, wxALL, 5 );
	
	pBoxSizerMeta->Add( pBoxSizerMetaButtons, 0, wxALL, 0 );
	
	win->SetSizer(pBoxSizerMeta);
	pBoxSizerMeta->SetSizeHints(win);

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

void wxGISNGWConnDlg::OnAddMetadataItem(wxCommandEvent& event)
{
	if(!m_pg)
		return;

    wxArrayString choices;
    choices.Add(_("String"));
    choices.Add(_("String list"));
    choices.Add(_("Float"));
    choices.Add(_("Integer"));

	//show add new metadata item dialog
    wxGISCreateMetadataItemDlg dlg(choices, this);
	if(dlg.ShowModal() == wxID_OK)
	{		
		//check uniq name
		wxPropertyGridConstIterator it;
		for ( it = m_pg->GetIterator(); !it.AtEnd(); it++ ) 
		{
			const wxPGProperty* p = *it;	
			if(p->GetName().IsSameAs(dlg.GetItemName()))
			{
				wxMessageBox(_("The name already exist"), _("Error"), wxICON_ERROR | wxOK);
				return;
			}
		}
		
		if(dlg.GetItemType().IsSameAs(_("String")))
			m_pg->Append(new wxStringProperty( dlg.GetItemName() + wxT(" (") + _("String") + wxT(")"), dlg.GetItemName(),wxEmptyString));
		else if(dlg.GetItemType().IsSameAs(_("String list")))	
			m_pg->Append(new wxArrayStringProperty(dlg.GetItemName() + wxT(" (") + _("String list") + wxT(")"), dlg.GetItemName()));
		else if(dlg.GetItemType().IsSameAs(_("Float")))	
			m_pg->Append(new wxFloatProperty(dlg.GetItemName() + wxT(" (") + _("Float") + wxT(")"), dlg.GetItemName(), 0.0));
		else if(dlg.GetItemType().IsSameAs(_("Integer")))
			m_pg->Append(new wxIntProperty(dlg.GetItemName() + wxT(" (") + _("Integer") + wxT(")"), dlg.GetItemName(), 0));
		else
			wxMessageBox(_("Unsuported property type"), _("Error"), wxICON_ERROR | wxOK);
	}
}

void wxGISNGWConnDlg::OnRemoveMetadataItem(wxCommandEvent& event)
{
	if(m_pg)
	{
		m_pg->DeleteProperty(m_pg->GetSelection());
	}
}

void wxGISNGWConnDlg::OnRemoveMetadataItemUI(wxUpdateUIEvent& event)
{
	if(m_pg && m_pg->GetSelection())
		event.Enable(true);
	else	
		event.Enable(false);	
}

void wxGISNGWConnDlg::OnExportMetadata(wxCommandEvent& event)
{
	wxFileDialog saveFileDialog(this, _("Save metadata file"), "", "ngw.meta", "Metadata files (*.meta)|*.meta", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
	if(saveFileDialog.ShowModal() == wxID_OK)
	{
		wxString sPath = saveFileDialog.GetPath(); 
		
		wxXmlDocument doc;
		wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("NGW"));
		SerializeMetadata(pRootNode, true);
		doc.SetRoot(pRootNode);

		if(!doc.Save(sPath))
		{
			wxGISErrorMessageBox(_("Metadata file create failed!"));
		}
	}	
}

void wxGISNGWConnDlg::OnImportMetadata(wxCommandEvent& event)
{
	wxFileDialog openFileDialog(this, _("Open metadata file"), "", "", "Metadata files (*.meta)|*.meta", wxFD_OPEN|wxFD_FILE_MUST_EXIST);
	if(openFileDialog.ShowModal() == wxID_OK)
	{
		wxString sPath = openFileDialog.GetPath(); 
		wxXmlDocument doc(sPath);
		if(doc.IsOk())
		{
			wxXmlNode* pRootNode = doc.GetRoot();
			SerializeMetadata(pRootNode, false);
		}
	}
}

#endif // wxGIS_USE_CURL

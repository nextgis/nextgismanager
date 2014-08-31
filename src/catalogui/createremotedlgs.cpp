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

#include "wxgis/catalogui/createremotedlgs.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/datasource/sysop.h"
#include "wxgis/core/crypt.h"
#include "wxgis/core/format.h"
#include "wxgis/datasource/postgisdataset.h"

#include "../../art/rdb_create.xpm"

#include <wx/valgen.h>
#include <wx/valtext.h>

#define NOTEMPLATE_TEXT _("No template, use extension")

//-------------------------------------------------------------------------------
//  wxGISCreateDBDlg
//-------------------------------------------------------------------------------

wxGISCreateDBDlg::wxGISCreateDBDlg( CPLString pszConnPath, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxGISRemoteDBConnDlg( pszConnPath, parent, id, title, pos, size, style )
{
    SetIcon(wxIcon(rdb_create_xpm));

	m_bCreateNew = true;

	m_sDBName = wxString(wxT("new_geo_db"));

	CreateUI();
}

wxGISCreateDBDlg::~wxGISCreateDBDlg()
{
}

void wxGISCreateDBDlg::OnOK(wxCommandEvent& event)
{
	if ( Validate() && TransferDataFromWindow() )
	{
	    //try to create db
	    wxGISPostgresDataSource oPostgresDataSource(m_sUser, m_sPass, m_sPort, m_sServer, m_sDatabase, m_bIsBinaryCursor);
		if( oPostgresDataSource.Open(false, true ) )
		{
		    if(m_sDBTempate.IsSameAs(NOTEMPLATE_TEXT))
                m_sDBTempate.Clear();
		    if(oPostgresDataSource.CreateDatabase(m_sDBName, m_sDBTempate, m_sUser))
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
                    pRootNode->AddAttribute(wxT("db"), m_sDBName);
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

                EndModal(wxID_OK);
		    }
		    else
            {
                const char* err = CPLGetLastErrorMsg();
			wxString sErr = wxString::Format(_("Operation '%s' failed!\nHost '%s', Database name '%s', Port='%s'.\nGDAL error: %s"), wxString(_("Create DB")), m_sServer.c_str(), m_sDatabase.c_str(), m_sPort.c_str(), wxString(err, wxConvLocal).c_str());
			wxMessageBox(sErr, _("Error"), wxICON_ERROR | wxOK, this );
            }
		}

	}
	else
	{
		wxMessageBox(wxString(_("Some input values are incorrect!")), wxString(_("Error")), wxICON_ERROR | wxOK );
	}
}

void wxGISCreateDBDlg::CreateUI(bool bHasConnectionPath)
{
    if(m_bMainSizer)
    {
        m_bMainSizer->Clear(true);
    }
    else
    {
        m_bMainSizer = new wxBoxSizer( wxVERTICAL );
    }

    wxFlexGridSizer* fgSizer1;
    fgSizer1 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_DBNameStaticText = new wxStaticText( this, wxID_ANY, _("DB Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer1->Add( m_DBNameStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

    m_DBName = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sDBName) );
	fgSizer1->Add( m_DBName, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	m_DBTemplateNameStaticText = new wxStaticText( this, wxID_ANY, _("DB Template:"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer1->Add( m_DBTemplateNameStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

    wxArrayString choices;
    choices.Add(NOTEMPLATE_TEXT);
    choices.Add(wxT("postgres"));
    m_TempateChoice = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, choices, 0, wxGenericValidator(&m_sDBTempate) );
	fgSizer1->Add( m_TempateChoice, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	m_TempateChoice->SetSelection(0);

    m_DBConnectionNameStaticText = new wxStaticText( this, wxID_ANY, _("DB Connection Name:"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer1->Add( m_DBConnectionNameStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	m_ConnName = new wxTextCtrl( this, ID_CONNNAME, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sConnName) );
	fgSizer1->Add( m_ConnName, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	m_bMainSizer->Add( fgSizer1, 0, wxEXPAND, 5 );

	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	m_bMainSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer2->AddGrowableCol( 1 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_SetverStaticText = new wxStaticText( this, wxID_ANY, _("Server:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_SetverStaticText->Wrap( -1 );
	fgSizer2->Add( m_SetverStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	m_sServerTextCtrl = new wxTextCtrl( this, ID_SERVERTEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sServer) );
	fgSizer2->Add( m_sServerTextCtrl, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );

	m_PortStaticText = new wxStaticText( this, wxID_ANY, _("Port:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_PortStaticText->Wrap( -1 );
	fgSizer2->Add( m_PortStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_PortTextCtrl = new wxTextCtrl( this, ID_PORTTEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxTextValidator(wxFILTER_NUMERIC, &m_sPort) );
	fgSizer2->Add( m_PortTextCtrl, 0, wxALL|wxEXPAND, 5 );

	m_DatabaseStaticText = new wxStaticText( this, ID_DATABASESTATICTEXT, _("Database:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_DatabaseStaticText->Wrap( -1 );
	fgSizer2->Add( m_DatabaseStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	m_DatabaseTextCtrl = new wxTextCtrl( this, ID_DATABASETEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sDatabase) );
	fgSizer2->Add( m_DatabaseTextCtrl, 0, wxALL|wxEXPAND, 5 );

	m_bMainSizer->Add( fgSizer2, 0, wxEXPAND, 5 );

	wxStaticBoxSizer* sbSizer1;
	sbSizer1 = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Account") ), wxVERTICAL );

	wxFlexGridSizer* fgSizer3;
	fgSizer3 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer3->AddGrowableCol( 1 );
	fgSizer3->SetFlexibleDirection( wxBOTH );
	fgSizer3->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_UserStaticText = new wxStaticText( this, ID_USERSTATICTEXT, _("User:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_UserStaticText->Wrap( -1 );
	fgSizer3->Add( m_UserStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	m_UsesTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sUser) );
	fgSizer3->Add( m_UsesTextCtrl, 0, wxALL|wxEXPAND, 5 );

	m_PassStaticText = new wxStaticText( this, ID_PASSSTATICTEXT, _("Password:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_PassStaticText->Wrap( -1 );
	fgSizer3->Add( m_PassStaticText, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	m_PassTextCtrl = new wxTextCtrl( this, ID_PASSTEXTCTRL, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD, wxGenericValidator(&m_sPass) );
	fgSizer3->Add( m_PassTextCtrl, 0, wxALL|wxEXPAND, 5 );

	sbSizer1->Add( fgSizer3, 1, wxEXPAND, 5 );

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

void wxGISCreateDBDlg::OnTest(wxCommandEvent& event)
{
	wxBusyCursor wait;
	if ( Validate() && TransferDataFromWindow() )
	{
		wxGISPostgresDataSource oPostgresDataSource(m_sUser, m_sPass, m_sPort, m_sServer, m_sDatabase, m_bIsBinaryCursor);
		if( oPostgresDataSource.Open(false, true ) )
		{
		    wxGISTableCached* pInfoSchema = wxDynamicCast(oPostgresDataSource.ExecuteSQL2(wxT("SELECT datname FROM pg_database;"), wxT("PG")), wxGISTableCached);
		    if(pInfoSchema)
            {
                //add tables to the list
                m_TempateChoice->Clear();

                m_TempateChoice->Append(NOTEMPLATE_TEXT);
                wxFeatureCursor Cursor = pInfoSchema->Search();
                wxGISFeature Feature;
                while ((Feature = Cursor.Next()).IsOk())
                {
                    wxString sDB = Feature.GetFieldAsString(0);
                    m_TempateChoice->Append(sDB);
                }
                m_TempateChoice->SetSelection(0);

                wsDELETE(pInfoSchema);
            }
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


#endif //wxGIS_USE_POSTGRES

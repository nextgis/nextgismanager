/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Create Remote Database dialog.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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

#include "wxgis/catalogui/createremotedlgs.h"
#include "wxgis/framework/application.h"
#include "wxgis/cartoui/tableview.h"

#include "../../art/state.xpm"

#include <wx/fontmap.h>

#define COL_LABEL_SIZE 25
#define COL_FILE_NAME_SIZE 250
#define COL_ENCODING_SIZE 250

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/datasource/sysop.h"
#include "wxgis/core/crypt.h"
#include "wxgis/core/format.h"
#include "wxgis/datasource/postgisdataset.h"
#include "wxgis/framework/applicationbase.h"
#include "wxgis/framework/icon.h"

#include "../../art/rdb_conn_16.xpm"

#include <wx/valgen.h>
#include <wx/valtext.h>

#define NOTEMPLATE_TEXT _("No template, use extension")

//-------------------------------------------------------------------------------
//  wxGISCreateDBDlg
//-------------------------------------------------------------------------------

wxGISCreateDBDlg::wxGISCreateDBDlg( CPLString pszConnPath, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxGISRemoteDBConnDlg( pszConnPath, parent, id, title, pos, size, style )
{
    SetIcon(GetStateIcon(rdb_conn_16_xpm, wxGISEnumIconStateNew, false));

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
	    wxGISPostgresDataSource oPostgresDataSource(m_sUser, m_sPass, m_sPort, m_sServer, m_sDatabase, wxT("30"), m_bIsBinaryCursor);
		if( oPostgresDataSource.Open(false, true ) )
		{
		    if(m_sDBTempate.IsSameAs(NOTEMPLATE_TEXT))
                m_sDBTempate.Clear();
		    if(oPostgresDataSource.CreateDatabase(m_sDBName, m_sDBTempate, m_sUser))
		    {
                wxString sCryptPass;
                if(!Crypt(m_sPass, sCryptPass))
                {
                    wxGISErrorMessageBox(wxString(_("Crypt password failed!")));
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
                        wxGISErrorMessageBox(wxString(_("Connection create failed!")));
                        return;
                    }

                }

                EndModal(wxID_OK);
		    }
		    else
            {
				wxString sErr = wxString::Format(_("Operation '%s' failed!\nHost '%s', Database name '%s', Port='%s'"), wxString(_("Create DB")), m_sServer.c_str(), m_sDatabase.c_str(), m_sPort.c_str());
				wxGISErrorMessageBox(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()) );
            }
		}

	}
	else
	{
		wxGISErrorMessageBox(wxString(_("Some input values are incorrect!")));
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
		wxGISPostgresDataSource oPostgresDataSource(m_sUser, m_sPass, m_sPort, m_sServer, m_sDatabase, wxT("80"), m_bIsBinaryCursor);
		if( oPostgresDataSource.Open(false, true ) )
		{
            wxGISTableCached* pInfoSchema = wxDynamicCast(oPostgresDataSource.ExecuteSQL2(wxT("SELECT datname FROM pg_database;"), wxT("PG")), wxGISTableCached);
            wxGISPointerHolder holder(pInfoSchema);
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

            }
			wxMessageBox(wxString(_("Connected successfully!")), wxString(_("Information")), wxICON_INFORMATION | wxOK, this );
		}
		else
		{
			wxString sErr = wxString::Format(_("Operation '%s' failed!\nHost '%s', Database name '%s', Port='%s'"), wxString(_("Open")), m_sServer.c_str(), m_sDatabase.c_str(), m_sPort.c_str());
			wxGISErrorMessageBox(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()));
		}
	}
	else
	{
		wxGISErrorMessageBox(wxString(_("Some input values are not correct!")));
	}
}


#endif //wxGIS_USE_POSTGRES

//-------------------------------------------------------------------------------
// wxGISBaseImportPanel
//-------------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxGISBaseImportPanel, wxPanel)

BEGIN_EVENT_TABLE(wxGISBaseImportPanel, wxPanel)
	EVT_BUTTON(wxID_CLOSE, wxGISBaseImportPanel::OnClose)
END_EVENT_TABLE()

wxGISBaseImportPanel::wxGISBaseImportPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(state_xpm));
	
	
	wxBoxSizer* fgSizer0 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* fgSizer1 = new wxBoxSizer( wxHORIZONTAL );

    m_pStateBitmap = new wxStaticBitmap( this, wxID_ANY, m_ImageList.GetIcon(1) , wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_pStateBitmap, 0, wxALL, 5 );

	m_bMainSizer = new wxBoxSizer(wxVERTICAL);
	fgSizer1->Add( m_bMainSizer, 1, wxALL|wxEXPAND, 0 );

	m_pCloseBitmap = new wxBitmapButton( this, wxID_CLOSE, m_ImageList.GetIcon(8), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	fgSizer1->Add( m_pCloseBitmap, 0, wxALL, 5 );

	fgSizer0->Add( fgSizer1, 0, wxEXPAND | wxALL, 5 );
	fgSizer0->Add( new wxStaticLine(this), 0, wxEXPAND | wxALL, 5 );
	this->SetSizer( fgSizer0 );
	this->Layout();
}

wxGISBaseImportPanel::~wxGISBaseImportPanel()
{
}

void wxGISBaseImportPanel::PutMessage(const wxString &sMessage, size_t nIndex, wxGISEnumMessageType eType)
{
	m_eCurrentType = eType;
	m_sCurrentMsg = sMessage;
    switch(eType)
    {
    case enumGISMessageInformation:
        m_pStateBitmap->SetBitmap(m_ImageList.GetIcon(0));
        break;
    case enumGISMessageError:
        m_pStateBitmap->SetBitmap(m_ImageList.GetIcon(2));
        break;
    case enumGISMessageWarning:
        m_pStateBitmap->SetBitmap(m_ImageList.GetIcon(3));
        break;
    case enumGISMessageRequired:
        m_pStateBitmap->SetBitmap(m_ImageList.GetIcon(4));
        break;
    case enumGISMessageOk:
        m_pStateBitmap->SetBitmap(m_ImageList.GetIcon(1));
        break;
    case enumGISMessageNone:
        m_pStateBitmap->SetBitmap(wxNullBitmap);
        break;
    default:
    case enumGISMessageUnknown:
        m_pStateBitmap->SetBitmap(wxNullBitmap);
        break;
    }
    m_pStateBitmap->SetToolTip(sMessage);
}

wxString wxGISBaseImportPanel::GetLastMessage(void) const
{
	return m_sCurrentMsg;
}

wxGISEnumMessageType wxGISBaseImportPanel::GetLastMessageType() const
{
	return m_eCurrentType;
}

void wxGISBaseImportPanel::OnClose(wxCommandEvent& event)
{
	wxWindow* pWnd = GetParent();
	Destroy();
	if(pWnd)
		pWnd->Layout();
}

wxString wxGISBaseImportPanel::GetDatasetName() const
{
	return m_sDatasetName;
}

//-------------------------------------------------------------------------------
//  wxGISVectorImportPanel
//-------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISVectorImportPanel, wxGISBaseImportPanel)

BEGIN_EVENT_TABLE(wxGISVectorImportPanel, wxGISBaseImportPanel)
    EVT_CHOICE(wxGISVectorImportPanel::ID_ENCODING, wxGISVectorImportPanel::OnEncodingSelect)
	EVT_BUTTON(wxGISVectorImportPanel::ID_TEST, wxGISVectorImportPanel::OnTestEncoding)
END_EVENT_TABLE();

wxGISVectorImportPanel::wxGISVectorImportPanel(wxGISFeatureDataset *pSrcDs, wxGxObjectContainer *pDestDs, const wxString &sOutName, OGRwkbGeometryType eFilterGeomType, bool bToMulti, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxGISBaseImportPanel(parent, id, pos, size, style )
{
	m_pFeatureClass = pSrcDs;
	
	wxFlexGridSizer* fgSizer1;
    fgSizer1 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    wxStaticText *pInputStaticText = new wxStaticText( this, wxID_ANY, _("Input dataset:"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer1->Add( pInputStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

    wxStaticText *pInputStaticTextVal = new wxStaticText( this, wxID_ANY, pSrcDs->GetName(), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( pInputStaticTextVal, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxStaticText *pOutputStaticText = new wxStaticText( this, wxID_ANY, _("Output name:"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer1->Add( pOutputStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	wxTextCtrl* pLayerName = new wxTextCtrl( this, wxID_ANY, sOutName, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sDatasetName) );
	fgSizer1->Add( pLayerName, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxStaticText *pEncStaticText = new wxStaticText( this, wxID_ANY, _("Encoding:"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer1->Add( pEncStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	wxArrayString asEnc;
	wxString sDefault;
	for (int i = wxFONTENCODING_DEFAULT; i < wxFONTENCODING_MAX; i++)
	{
		wxString sDesc = wxFontMapper::GetEncodingDescription((wxFontEncoding)i);
		if(sDesc.StartsWith(_("Unknown")))
			continue;
#ifndef __WXMAC__
		if(sDesc.StartsWith(_("Mac")))
			continue;
#endif //MAC
			
		if (i == wxFONTENCODING_DEFAULT)
			sDefault = sDesc;
		asEnc.Add(sDesc);
		m_mnEnc[sDesc] = (wxFontEncoding)i;
	}
	
	m_pEncodingsCombo = new wxChoice(this, ID_ENCODING, wxDefaultPosition, wxDefaultSize, asEnc, wxCB_SORT);
	m_pEncodingsCombo->SetSelection(m_pEncodingsCombo->FindString (sDefault));
	
	wxBoxSizer* pFunctSizer = new wxBoxSizer(wxHORIZONTAL);
	pFunctSizer->Add( m_pEncodingsCombo, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_pTestButton = new wxButton(this, ID_TEST, _("Test"));
	pFunctSizer->Add( m_pTestButton, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	fgSizer1->Add( pFunctSizer, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 0 );
		
	
	m_bMainSizer->Add(fgSizer1, 0, wxEXPAND | wxALL, 5 );
	
	this->Layout();
	
	m_bToMulti = pDestDs->ValidateDataset(pSrcDs, eFilterGeomType, this);
	if(m_eCurrentType == enumGISMessageError)
	{
		if(m_pTestButton)
			m_pTestButton->Enable(false);
		if(m_pEncodingsCombo)	
			m_pEncodingsCombo->Enable(false);
		if(pLayerName)
			pLayerName->Enable(false);
	}
	
	m_eFilterGeometryType = eFilterGeomType;
}

wxGISVectorImportPanel::~wxGISVectorImportPanel()
{
	wsDELETE(m_pFeatureClass);
}

void wxGISVectorImportPanel::OnEncodingSelect(wxCommandEvent& event)
{
    wxString sSel = m_pEncodingsCombo->GetStringSelection();
    wxFontEncoding eEnc = m_mnEnc[sSel];
	if(m_pFeatureClass)
		m_pFeatureClass->SetEncoding(eEnc);
}

void wxGISVectorImportPanel::OnTestEncoding(wxCommandEvent& event)
{
	wxGISDatasetTestEncodingDlg dlg(m_pFeatureClass, this);
	dlg.ShowModal();
}

wxGISDataset* wxGISVectorImportPanel::GetDataset() const
{
	wxGISDataset* pDSet = wxStaticCast(m_pFeatureClass, wxGISDataset);
	wsGET(pDSet);
}

OGRwkbGeometryType wxGISVectorImportPanel::GetFilterGeometryType() const
{
	return m_eFilterGeometryType;
}

bool wxGISVectorImportPanel::GetToMulti() const
{
	return m_bToMulti;
}


//-------------------------------------------------------------------------------
//  wxGISRasterImportPanel
//-------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISRasterImportPanel, wxGISBaseImportPanel)

BEGIN_EVENT_TABLE(wxGISRasterImportPanel, wxGISBaseImportPanel)
	EVT_TOGGLEBUTTON(wxGISRasterImportPanel::ID_CROP, wxGISRasterImportPanel::OnCrop)
END_EVENT_TABLE();

wxGISRasterImportPanel::wxGISRasterImportPanel(wxGISRasterDataset *pSrcDs, wxGxObjectContainer *pDestDs, const wxString &sOutName, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxGISBaseImportPanel(parent, id, pos, size, style )
{
	wsSET(m_pRasterDataset, pSrcDs);
	
	wxFlexGridSizer* fgSizer1;
    fgSizer1 = new wxFlexGridSizer( 3, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    wxStaticText *pInputStaticText = new wxStaticText( this, wxID_ANY, _("Input dataset:"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer1->Add( pInputStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

    wxStaticText *pInputStaticTextVal = new wxStaticText( this, wxID_ANY, pSrcDs->GetName(), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( pInputStaticTextVal, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxStaticText *pOutputStaticText = new wxStaticText( this, wxID_ANY, _("Output name:"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer1->Add( pOutputStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	wxTextCtrl* pLayerName = new wxTextCtrl( this, wxID_ANY, sOutName, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sDatasetName) );
	fgSizer1->Add( pLayerName, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
		
	wxArrayString asBands;
	wxString sRed("1"), sGreen("1"), sBlue("1"), sAlpha(_("none"));
	
	GDALDataset* poGDALDataset = pSrcDs->GetMainRaster();
    if(!poGDALDataset)
        poGDALDataset = pSrcDs->GetRaster();
		
	if(!poGDALDataset)	
		return;
		
	if(poGDALDataset->GetRasterCount() > 2)
	{
		sGreen = wxString("2");
		sBlue = wxString("3");
	}
	
	for (int nBand = 1; nBand < poGDALDataset->GetRasterCount() + 1; ++nBand)
	{		
		GDALRasterBand* pBand = poGDALDataset->GetRasterBand(nBand);
		if(pBand)
		{
			wxString sBand = wxString::Format(wxT("%d"), nBand);
			GDALColorInterp nColorInt = pBand->GetColorInterpretation();
			if(nColorInt == GCI_RedBand)
				sRed = sBand;
			else if(nColorInt == GCI_GreenBand)
				sGreen = sBand; 
			else if(nColorInt == GCI_BlueBand)
				sBlue = sBand; 
			else if(nColorInt == GCI_AlphaBand)
				sAlpha = sBand; 
			asBands.Add(sBand);
		}
	}
	
	wxStaticText *pBandsStaticText = new wxStaticText( this, wxID_ANY, _("Bands:"), wxDefaultPosition, wxDefaultSize, 0 );
    fgSizer1->Add( pBandsStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	wxBoxSizer* pFunctSizer = new wxBoxSizer(wxHORIZONTAL);
	
	// red band
	
	wxStaticText *pRStaticText = new wxStaticText( this, wxID_ANY, _("Red"), wxDefaultPosition, wxDefaultSize, 0 );
    pFunctSizer->Add( pRStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_pRedBandCombo = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, asBands);
	m_pRedBandCombo->SetSelection(m_pRedBandCombo->FindString (sRed));	
	
	pFunctSizer->Add( m_pRedBandCombo, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	// green
	
	wxStaticText *pGStaticText = new wxStaticText( this, wxID_ANY, _("Green"), wxDefaultPosition, wxDefaultSize, 0 );
    pFunctSizer->Add( pGStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_pGreenBandCombo = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, asBands);
	m_pGreenBandCombo->SetSelection(m_pGreenBandCombo->FindString (sGreen));	
	
	pFunctSizer->Add( m_pGreenBandCombo, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	// blue
	
	wxStaticText *pBStaticText = new wxStaticText( this, wxID_ANY, _("Blue"), wxDefaultPosition, wxDefaultSize, 0 );
    pFunctSizer->Add( pBStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_pBlueBandCombo = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, asBands);
	m_pBlueBandCombo->SetSelection(m_pBlueBandCombo->FindString (sBlue));	
	
	pFunctSizer->Add( m_pBlueBandCombo, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	// alpha
	
	wxStaticText *pAStaticText = new wxStaticText( this, wxID_ANY, _("Alpha (opt.)"), wxDefaultPosition, wxDefaultSize, 0 );
    pFunctSizer->Add( pAStaticText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	asBands.Insert(_("none"), 0);
	m_pAlphaBandCombo = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, asBands);
	m_pAlphaBandCombo->SetSelection(m_pAlphaBandCombo->FindString (sAlpha));	
	
	pFunctSizer->Add( m_pAlphaBandCombo, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );	
	
	
	m_pCropButton = new wxToggleButton(this, ID_CROP, _("Auto crop"));
	pFunctSizer->Add( m_pCropButton, 0, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 5 );
	
	fgSizer1->Add( pFunctSizer, 1, wxALL|wxEXPAND|wxALIGN_CENTER_VERTICAL, 0 );
		
	
	m_bMainSizer->Add(fgSizer1, 0, wxEXPAND | wxALL, 5 );
	
	this->Layout();
	
	pDestDs->ValidateDataset(pSrcDs, this);
	if(m_eCurrentType == enumGISMessageError)
	{
		if(m_pCropButton)
			m_pCropButton->Enable(false);
		if(pLayerName)
			pLayerName->Enable(false);
			
		if(m_pRedBandCombo)
			m_pRedBandCombo->Enable(false);
	    if(m_pGreenBandCombo)
			m_pGreenBandCombo->Enable(false);
		if(m_pBlueBandCombo)
			m_pBlueBandCombo->Enable(false);
		if(m_pAlphaBandCombo)
			m_pAlphaBandCombo->Enable(false);
	}
}

wxGISRasterImportPanel::~wxGISRasterImportPanel()
{
	wsDELETE(m_pRasterDataset);
}

void wxGISRasterImportPanel::OnCrop(wxCommandEvent& event)
{
	if(m_pCropButton->GetValue())
	{
		m_pAlphaBandCombo->Disable();
	}
	else
	{
		m_pAlphaBandCombo->Enable();
	}
}

wxGISDataset* wxGISRasterImportPanel::GetDataset() const
{
	wxGISDataset* pDSet = wxStaticCast(m_pRasterDataset, wxGISDataset);
	wsGET(pDSet);
}

bool wxGISRasterImportPanel::GetAutoCrop() const
{
	return m_pCropButton->GetValue();
}	

wxGISDatasetImportDlg::BANDS wxGISRasterImportPanel::GetBands() const
{
	wxGISDatasetImportDlg::BANDS out;
	out.R = wxAtoi(m_pRedBandCombo->GetString(m_pRedBandCombo->GetSelection()));
	out.G = wxAtoi(m_pGreenBandCombo->GetString(m_pGreenBandCombo->GetSelection()));
	out.B = wxAtoi(m_pBlueBandCombo->GetString(m_pBlueBandCombo->GetSelection()));
	wxString sAVal = m_pAlphaBandCombo->GetString(m_pAlphaBandCombo->GetSelection());
	if(m_pCropButton->GetValue() || sAVal.IsSameAs(_("none")))
		out.A = 0;
	else
		out.A = wxAtoi(sAVal);
	return out;
}
	
//-------------------------------------------------------------------------------
//  wxGISDatasetImportDlg
//-------------------------------------------------------------------------------

wxGISDatasetImportDlg::wxGISDatasetImportDlg(wxGxObjectContainer *pDestDs, wxVector<IGxDataset*> &paDatasets, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
    this->SetLayoutAdaptationMode(wxDIALOG_ADAPTATION_MODE_ENABLED);
    this->SetSizeHints(400, 300, wxNOT_FOUND, wxNOT_FOUND);

	m_bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	if(pDestDs)
	{
        wxBusyCursor wait;
		for(size_t i = 0; i < paDatasets.size(); ++i)
		{
			wxGxDataset *pSrcDs = dynamic_cast<wxGxDataset*>(paDatasets[i]);
			if(NULL != pSrcDs)
			{			
				if(pSrcDs->GetType() == enumGISFeatureDataset)
				{		
                    wxGISDataset* pGISDs = pSrcDs->GetDataset(false);
                    wxGISPointerHolder holder(pGISDs);
					if(!pGISDs)
						continue;
					wxGISFeatureDataset* pSrcFeatureDs = dynamic_cast<wxGISFeatureDataset*>(pGISDs);		
					if(!pSrcFeatureDs)
					{
						continue;
					}
					//split for geometry bag to separate panels
					if(pDestDs->CanStoreMultipleGeometryTypes())
					{
						wxString sOutName = pDestDs->ValidateName(pSrcDs->GetBaseName());
						m_bMainSizer->Add( new wxGISVectorImportPanel(pSrcFeatureDs, pDestDs, sOutName, wkbUnknown, false, this), 0, wxEXPAND | wxALL, 0 );
					}
					else
					{
						OGRwkbGeometryType eGeomType = pSrcFeatureDs->GetGeometryType();
						bool bIsMultigeom = wkbFlatten(eGeomType) == wkbUnknown || wkbFlatten(eGeomType) == wkbGeometryCollection;
						if(bIsMultigeom)
						{
							wxArrayString saIgnoredFields = pSrcFeatureDs->GetFieldNames();
							saIgnoredFields.Add(wxT("OGR_STYLE"));
							pSrcFeatureDs->SetIgnoredFields(saIgnoredFields);
							pSrcFeatureDs->Reset();

							std::map<OGRwkbGeometryType, int> mnCounts;
							
							wxGISApplication *pApp = dynamic_cast<wxGISApplication*>(GetApplication());
							wxGISStatusBar* pStatusBar = NULL;
							if(pApp)
								pStatusBar = pApp->GetStatusBar();
							IProgressor* pProgressor(NULL);
							if (pStatusBar)
							{
								pStatusBar->SetMessage(_("Get feature count"), 0);
								pProgressor = pStatusBar->GetProgressor();
							}

							int nCounter(0);
							if (pProgressor)
							{
								pProgressor->SetRange(pSrcFeatureDs->GetFeatureCount(false, NULL));
							}

							wxGISFeature Feature;
							while ((Feature = pSrcFeatureDs->Next()).IsOk())
							{
								//check if Feature will destroy by Ref Count
								wxGISGeometry Geom = Feature.GetGeometry();
								if (Geom.IsOk())
								{
									OGRwkbGeometryType eFeatureGeomType = Geom.GetType();
									if (wkbFlatten(eFeatureGeomType) > 1 && wkbFlatten(eFeatureGeomType) < 4)
									{
										mnCounts[(OGRwkbGeometryType)(eFeatureGeomType + 3)] += 1;
									}
									else
									{
										mnCounts[eFeatureGeomType] += 1;
									}
								}
								if (pProgressor)
								{
									pProgressor->SetValue(nCounter++);
								}
							}

							for (std::map<OGRwkbGeometryType, int>::const_iterator IT = mnCounts.begin(); IT != mnCounts.end(); ++IT)
							{
								if (IT->second > 0)
								{
									wxString sType(OGRGeometryTypeToName(IT->first), wxConvUTF8);
									sType.Replace(" ", "");
									
									wxString sOutName = pDestDs->ValidateName(pSrcDs->GetBaseName() + wxT(" ") + sType.MakeLower());
									m_bMainSizer->Add( new wxGISVectorImportPanel(pSrcFeatureDs, pDestDs, sOutName, IT->first, bIsMultigeom, this), 0, wxEXPAND | wxALL, 0 );
								}
							}
						}
						else
						{
							wxString sOutName = pDestDs->ValidateName(pSrcDs->GetBaseName());
							m_bMainSizer->Add( new wxGISVectorImportPanel(pSrcFeatureDs, pDestDs, sOutName, wkbUnknown, false, this), 0, wxEXPAND | wxALL, 0 );
						}
					}
				}
				else if(pSrcDs->GetType() == enumGISRasterDataset)
				{
					wxGISDataset* pGISDs = pSrcDs->GetDataset(false);
                    wxGISPointerHolder holder(pGISDs);
					if(!pGISDs)
						continue;
					wxGISRasterDataset* pSrcRasterDs = dynamic_cast<wxGISRasterDataset*>(pGISDs);		
					if(!pSrcRasterDs)
					{
						continue;
					}
					
					wxString sOutName = pDestDs->ValidateName(pSrcDs->GetBaseName());
					m_bMainSizer->Add( new wxGISRasterImportPanel(pSrcRasterDs, pDestDs, sOutName, this), 0, wxEXPAND | wxALL, 0 );
				}
			}
		}
	}

	wxStdDialogButtonSizer *sdbSizer = new wxStdDialogButtonSizer();
	wxButton *sdbSizerOK = new wxButton( this, wxID_OK, _("OK") );
	sdbSizer->AddButton( sdbSizerOK );
	wxButton *sdbSizerCancel = new wxButton( this, wxID_CANCEL, _("Cancel") );
	sdbSizer->AddButton( sdbSizerCancel );
	sdbSizer->Realize();
	m_bMainSizer->Add( sdbSizer, 0, wxEXPAND|wxALL, 5 );


    this->SetSizerAndFit(m_bMainSizer);
	this->Layout();

	this->Centre( wxBOTH );		
}

wxGISDatasetImportDlg::~wxGISDatasetImportDlg()
{
    for (size_t i = 0; i < m_paDatasets.size(); ++i)
    {
        wsDELETE(m_paDatasets[i].pDataset);
    }
}

size_t wxGISDatasetImportDlg::GetDatasetCount()
{
	if(m_paDatasets.empty())
	{
		for ( size_t i = 0; i < m_bMainSizer->GetItemCount(); ++i ) 
		{    
			wxSizerItem * pItem = m_bMainSizer->GetItem(i);
			if(!pItem)
				continue;
			wxGISBaseImportPanel* pImportPanel = dynamic_cast<wxGISBaseImportPanel*>(pItem->GetWindow());
			if(pImportPanel && pImportPanel->GetLastMessageType() != enumGISMessageError && pImportPanel->TransferDataFromWindow())
			{
				wxGISVectorImportPanel* pVectorPanel = dynamic_cast<wxGISVectorImportPanel*>(pImportPanel);
				wxGISRasterImportPanel* pRasterPanel = dynamic_cast<wxGISRasterImportPanel*>(pImportPanel);
				wxGISDataset* pDSet = pImportPanel->GetDataset();
				if(pDSet)
				{
					if(pVectorPanel)
					{
						DATASETDESCR descr = {pDSet, pVectorPanel->GetDatasetName(), pVectorPanel->GetFilterGeometryType(), pVectorPanel->GetToMulti(), 0, 0, 0, 0};
						m_paDatasets.push_back(descr);						
					}
					else if(pRasterPanel)
					{
						DATASETDESCR descr = {pDSet, pRasterPanel->GetDatasetName(), wkbUnknown, pRasterPanel->GetAutoCrop(), pRasterPanel->GetBands()};
						m_paDatasets.push_back(descr);
					}
				}
			}
		} 
	}	
	return m_paDatasets.size();
}

wxGISDatasetImportDlg::DATASETDESCR wxGISDatasetImportDlg::GetDataset(size_t nIndex) const
{
	if(m_paDatasets.size() > nIndex)
	{
        if (m_paDatasets[nIndex].pDataset)
            m_paDatasets[nIndex].pDataset->Reference();
		return m_paDatasets[nIndex];
	}
	DATASETDESCR ret = {NULL, wxEmptyString, wkbUnknown, false, 0, 0, 0, 0};
	return ret;
}

//-------------------------------------------------------------------------------
// wxGISDatasetTestEncodingDlg
//-------------------------------------------------------------------------------

wxGISDatasetTestEncodingDlg::wxGISDatasetTestEncodingDlg(wxGISTable  *const pDSet, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	if(pDSet)
	{
		SetTitle(pDSet->GetName());
		wxBoxSizer *bMainSizer = new wxBoxSizer( wxVERTICAL );
		wxGISTableView* pView = new wxGISTableView(this);
		bMainSizer->Add( pView , 0, wxEXPAND, 5 );
		this->SetSizer( bMainSizer );
		this->Layout();
		
		if (!pDSet->IsOpened())
		{
			pDSet->Open(0, true, true, false);
		}
		if(pDSet->IsCaching())
		{
			pDSet->StopCaching();
		}
		wxGISGridTable* pTable = new wxGISGridTable(wxStaticCast(pDSet ,wxGISDataset));
		pView->SetTable(pTable, true);
	}
}

wxGISDatasetTestEncodingDlg::~wxGISDatasetTestEncodingDlg()
{
	
}
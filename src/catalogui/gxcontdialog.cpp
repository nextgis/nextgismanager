/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxContainerDialog class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2012 Bishop
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

#include "wxgis/catalogui/gxcontdialog.h"
/*
#include "wxgis/catalogui/catalogcmd.h"
#include "wxgis/core/globalfn.h"
#include "wxgis/framework/application.h"

//////////////////////////////////////////////////////////////////////////////
// wxTreeContainerView
//////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC_CLASS(wxTreeContainerView, wxGxTreeView)

BEGIN_EVENT_TABLE(wxTreeContainerView, wxGxTreeView)
END_EVENT_TABLE()

wxTreeContainerView::wxTreeContainerView(void) : wxGxTreeView()
{
}

wxTreeContainerView::wxTreeContainerView(wxWindow* parent, wxWindowID id, long style) : wxGxTreeView(parent, id, style)
{
}

wxTreeContainerView::~wxTreeContainerView(void)
{
    //RemoveAllShowFilters();
}

void wxTreeContainerView::AddTreeItem(IGxObject* pGxObject, wxTreeItemId hParent)
{
	if(NULL == pGxObject)
		return;

    if(m_ShowFilterArray.size() > 0)
    {
		for(size_t i = 0; i < m_ShowFilterArray.size(); ++i)
			if(m_ShowFilterArray[i]->CanDisplayObject(pGxObject))
                goto ADD;
        return;
	}


ADD:
    wxGxTreeViewBase::AddTreeItem(pGxObject, hParent);
}

void wxTreeContainerView::AddShowFilter(IGxObjectFilter* pFilter)
{
	m_ShowFilterArray.push_back(pFilter);
}

void wxTreeContainerView::RemoveAllShowFilters(void)
{
//	for(size_t i = 0; i < m_ShowFilterArray.size(); ++i)
//		wxDELETE(m_ShowFilterArray[i]);
	m_ShowFilterArray.clear();
}

bool wxTreeContainerView::CanChooseObject( IGxObject* pObject )
{
	if(NULL == pObject)
		return false;

    if(m_ShowFilterArray.size() > 0)
		for(size_t i = 0; i < m_ShowFilterArray.size(); ++i)
			if(m_ShowFilterArray[i]->CanChooseObject(pObject))
                return true;
    return false;
}

////////////////////////////////////////////////////////////////////////////////
//// wxGxContainerDialog
////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_CLASS2(wxGxContainerDialog, wxDialog, wxGISApplicationBase)

BEGIN_EVENT_TABLE(wxGxContainerDialog, wxDialog)
    EVT_COMBOBOX(FILTERCOMBO, wxGxContainerDialog::OnFilerSelect)
    EVT_BUTTON(wxID_OK, wxGxContainerDialog::OnOK)
    EVT_BUTTON(ID_CREATE, wxGxContainerDialog::OnCreate)
    EVT_UPDATE_UI(wxID_OK, wxGxContainerDialog::OnOKUI)
    EVT_UPDATE_UI(ID_CREATE, wxGxContainerDialog::OnCreateUI)
	EVT_MENU_RANGE(ID_PLUGINCMD, ID_PLUGINCMDMAX, wxGxContainerDialog::OnCommand)
	EVT_UPDATE_UI_RANGE(ID_PLUGINCMD, ID_PLUGINCMDMAX, wxGxContainerDialog::OnCommandUI)
END_EVENT_TABLE()

wxGxContainerDialog::wxGxContainerDialog( wxWindow* parent, IGxCatalog* pExternalCatalog, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style ), wxGISApplicationBase(), m_pCatalog(NULL), m_pTree(NULL), m_bShowCreateButton(false), m_bAllFilters(true), m_nDefaultFilter(0), m_bShowExportFormats(false), m_bOwnFilter(true), m_bOwnShowFilter(true)
{
	this->SetSizeHints( wxSize( 400,300 ), wxDefaultSize );

    m_pExternalCatalog = pExternalCatalog;
    m_pCatalog = new wxGxCatalogUI(true);
	m_pCatalog->Init(m_pExternalCatalog);

	bMainSizer = new wxBoxSizer( wxVERTICAL );

	m_staticDescriptionText = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	m_staticDescriptionText->Wrap( -1 );
	bMainSizer->Add( m_staticDescriptionText, 0, wxALL|wxEXPAND, 8 );//|wxALIGN_CENTER_VERTICAL

	wxBoxSizer* bSizer = new wxBoxSizer( wxHORIZONTAL );

	m_staticWildText = new wxStaticText( this, wxID_ANY, _("Select export format"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticWildText->Wrap( -1 );
    m_staticWildText->Show(false);
	bSizer->Add( m_staticWildText, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_WildcardCombo = new wxComboBox( this, FILTERCOMBO, _("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxCB_READONLY );
    m_WildcardCombo->Show(false);
	bSizer->Add( m_WildcardCombo, 1, wxALL|wxEXPAND, 8 );

	bMainSizer->Add( bSizer, 0, wxEXPAND, 5 );


	wxFlexGridSizer* bFooterSizer = new wxFlexGridSizer( 1, 4, 0, 0 );
	bFooterSizer->AddGrowableCol( 1 );
	bFooterSizer->SetFlexibleDirection( wxBOTH );
	bFooterSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );


	m_CreateButton = new wxButton( this, ID_CREATE , _("Create container"), wxDefaultPosition, wxDefaultSize, 0 );
    m_CreateButton->Show(false);
	bFooterSizer->Add( m_CreateButton, 0, wxALL, 8 );

    wxStaticText* m_static = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
	bFooterSizer->Add( m_static, 1, wxALL, 8 );

	m_OkButton = new wxButton( this, wxID_OK , _("Select"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OkButton->SetDefault();
	bFooterSizer->Add( m_OkButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 8 );

	m_CancelButton = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	bFooterSizer->Add( m_CancelButton, 0, wxALL, 8 );

	bMainSizer->Add( bFooterSizer, 0, wxEXPAND, 8 );

	this->SetSizer( bMainSizer );
	this->Layout();

    wxGISApplication* pApp = dynamic_cast<wxGISApplication*>(GetApplication());
    if(pApp)
    {
		wxGISCommand *pCmd(NULL);
		wxGISCatalogMainCmd* pwxGISCatalogMainCmd(NULL);

		//create folder
		pCmd = pApp->GetCommand(wxT("wxGISCatalogMainCmd"), 7);
		if(pCmd)
		{
			pwxGISCatalogMainCmd = new wxGISCatalogMainCmd();
			pwxGISCatalogMainCmd->OnCreate(this);
			pwxGISCatalogMainCmd->SetID(pCmd->GetID());
			pwxGISCatalogMainCmd->SetSubType(7);
			m_CommandArray.push_back(pwxGISCatalogMainCmd);
		}

		//rename
		pCmd = pApp->GetCommand(wxT("wxGISCatalogMainCmd"), 8);
		if(pCmd)
		{
			pwxGISCatalogMainCmd = new wxGISCatalogMainCmd();
			pwxGISCatalogMainCmd->OnCreate(this);
			pwxGISCatalogMainCmd->SetID(pCmd->GetID());
			pwxGISCatalogMainCmd->SetSubType(8);
			m_CommandArray.push_back(pwxGISCatalogMainCmd);
		}

		//refresh
		pCmd = pApp->GetCommand(wxT("wxGISCatalogMainCmd"), 9);
		if(pCmd)
		{
			pwxGISCatalogMainCmd = new wxGISCatalogMainCmd();
			pwxGISCatalogMainCmd->OnCreate(this);
			pwxGISCatalogMainCmd->SetID(pCmd->GetID());
			pwxGISCatalogMainCmd->SetSubType(9);
			m_CommandArray.push_back(pwxGISCatalogMainCmd);
		}

		//load accelerators
		if(pApp->GetGISAcceleratorTable())
			SetAcceleratorTable(pApp->GetGISAcceleratorTable()->GetAcceleratorTable());
	}
}

wxGxContainerDialog::~wxGxContainerDialog()
{
	if(m_pTree)
		m_pTree->Deactivate();

    SerializeFramePos(true);

	for(size_t i = 0; i < m_CommandArray.size(); ++i)
		wxDELETE(m_CommandArray[i]);

	RemoveAllFilters();

    wxDELETE(m_pTree);

	if(m_pCatalog)
	{
		m_pCatalog->Detach();
		delete m_pCatalog;
	}
}

void wxGxContainerDialog::OnCommand(wxCommandEvent& event)
{
	event.Skip();
	Command(GetCommand(event.GetId()));
}

void wxGxContainerDialog::Command(wxGISCommand* pCmd)
{
	pCmd->OnClick();
}

void wxGxContainerDialog::OnCommandUI(wxUpdateUIEvent& event)
{
	wxGISCommand* pCmd = GetCommand(event.GetId());
	if(pCmd)
	{
		if(pCmd->GetKind() == enumGISCommandCheck)
			event.Check(pCmd->GetChecked());
		event.Enable(pCmd->GetEnabled());
    }
}

void wxGxContainerDialog::SetButtonCaption(wxString sOkBtLabel)
{
	m_sOkBtLabel = sOkBtLabel;
}

void wxGxContainerDialog::SetStartingLocation(wxString sStartPath)
{
	m_sStartPath = sStartPath;
}

void wxGxContainerDialog::SetDescriptionText(wxString sText)
{
    m_staticDescriptionText->SetLabel(sText);
}

void wxGxContainerDialog::ShowCreateButton(bool bShow)
{
    m_bShowCreateButton = bShow;
    m_CreateButton->Show(m_bShowCreateButton);
}

void wxGxContainerDialog::AddFilter(IGxObjectFilter* pFilter, bool bDefault)
{
	m_FilterArray.push_back(pFilter);
	if(bDefault)
		m_nDefaultFilter = m_FilterArray.size() - 1;
}

void wxGxContainerDialog::RemoveAllFilters(void)
{
    if(m_bOwnFilter)
		for(size_t i = 0; i < m_FilterArray.size(); ++i)
			wxDELETE(m_FilterArray[i]);
}

void wxGxContainerDialog::AddShowFilter(IGxObjectFilter* pFilter)
{
	m_paShowFilter.push_back(pFilter);
	//m_pTree->AddShowFilter(pFilter);
}

void wxGxContainerDialog::RemoveAllShowFilters(void)
{
    if(m_bOwnShowFilter)
		for(size_t i = 0; i < m_paShowFilter.size(); ++i)
			wxDELETE(m_paShowFilter[i]);
    m_pTree->RemoveAllShowFilters();
}

int wxGxContainerDialog::ShowModal(void)
{
	OnInit();
	return wxDialog::ShowModal();
}

void wxGxContainerDialog::OnInit()
{
    m_pTree = new wxTreeContainerView( this, TREECTRLID, wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS | wxTR_NO_LINES | wxTR_SINGLE | wxTR_EDIT_LABELS );
	m_pTree->Activate(this, NULL);//TODO !!!!

    RegisterChildWindow(m_pTree->GetId());

	bMainSizer->Insert(1, m_pTree, 1, wxALL|wxEXPAND, 8 );

	//m_pTree->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( wxGxTreeView::OnSelChanged ), NULL, this );

    if(m_bShowExportFormats)
    {
        m_staticWildText->Show(true);
        m_WildcardCombo->Show(true);
    }
    else
    {
        m_staticWildText->Show(false);
        m_WildcardCombo->Show(false);
    }


	for(size_t i = 0; i < m_FilterArray.size(); ++i)
		m_WildcardCombo->AppendString(m_FilterArray[i]->GetName());
	if(m_FilterArray.size() > 1 && m_bAllFilters)
		m_WildcardCombo->AppendString(_("All listed filters"));
	if(m_FilterArray.size() == 0)
		m_WildcardCombo->AppendString(_("All items"));
	m_WildcardCombo->Select(m_nDefaultFilter);

	for(size_t i = 0; i < m_paShowFilter.size(); ++i)
		m_pTree->AddShowFilter(m_paShowFilter[i]);


	if(m_sStartPath.IsEmpty())
	{
		IGxObject* pObj = dynamic_cast<IGxObject*>(m_pCatalog);
		wxString sLastPath;
		wxGISAppConfig oConfig = GetConfig();
		if(oConfig.IsOk())
			sLastPath = oConfig.Read(enumGISHKCU, GetAppName() + wxString(wxT("/lastpath/path")), pObj->GetName());
		else
			sLastPath = pObj->GetName();
		m_pCatalog->SetLocation(sLastPath);
	}
	else
		m_pCatalog->SetLocation(m_sStartPath);

    SerializeFramePos(false);
}

void wxGxContainerDialog::SerializeFramePos(bool bSave)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
        return;

	if(bSave)
	{
		if( IsMaximized() )
		{
			oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/maxi")), true);
		}
		else
		{
			int x, y, w, h;
			GetClientSize(&w, &h);
			GetPosition(&x, &y);

			oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/maxi")), false);
			oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/width")), w);
			oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/height")), h);
			oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/xpos")), x);
			oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/ypos")), y);
		}
	}
	else
	{
		//load
		bool bMaxi = oConfig.ReadBool(enumGISHKCU, GetAppName() + wxString(wxT("/frame/maxi")), false);
		if(!bMaxi)
		{
			int x = oConfig.ReadInt(enumGISHKCU, GetAppName() + wxString(wxT("/frame/xpos")), 50);
			int y = oConfig.ReadInt(enumGISHKCU, GetAppName() + wxString(wxT("/frame/ypos")), 50);
			int w = oConfig.ReadInt(enumGISHKCU, GetAppName() + wxString(wxT("/frame/width")), 450);
			int h = oConfig.ReadInt(enumGISHKCU, GetAppName() + wxString(wxT("/frame/height")), 650);

			Move(x, y);
			SetClientSize(w, h);
		}
		else
		{
			Maximize();
		}
	}
}

void wxGxContainerDialog::SetAllFilters(bool bAllFilters)
{
	m_bAllFilters = bAllFilters;
}

void wxGxContainerDialog::ShowExportFormats(bool bShow)
{
    m_bShowExportFormats = bShow;
}

IGxObjectFilter* wxGxContainerDialog::GetCurrentFilter(void)
{
    if(m_FilterArray.size() == 0)
        return NULL;
    return m_FilterArray[m_nDefaultFilter];
}

long wxGxContainerDialog::GetLocation(void)
{
    return m_pCatalog->GetSelection()->GetLastSelectedObjectID();
}

wxString wxGxContainerDialog::GetPath(void)
{
    IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(GetLocation());
    if(pGxObject)
    {
        return pGxObject->GetFullName();
    }
    return wxEmptyString;
}

CPLString wxGxContainerDialog::GetInternalPath(void)
{
    IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(GetLocation());
    if(pGxObject)
    {
        return pGxObject->GetInternalName();
    }
    return CPLString();
}

void wxGxContainerDialog::OnOKUI(wxUpdateUIEvent& event)
{
    IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(GetLocation());
    bool bEnable = false;
    if(m_pTree)
        bEnable = m_pTree->CanChooseObject(pGxObject.get());
    event.Enable(bEnable);
}

void wxGxContainerDialog::OnFilerSelect(wxCommandEvent& event)
{
    m_nDefaultFilter = m_WildcardCombo->GetCurrentSelection();
}

void wxGxContainerDialog::OnOK(wxCommandEvent& event)
{
    m_nRetCode = wxID_OK;
    if ( Validate() && TransferDataFromWindow() )
    {
        int nPos = m_WildcardCombo->GetCurrentSelection();
        //fill out data
        IGxSelection* pSel = m_pCatalog->GetSelection();
        for(size_t i = 0; i < pSel->GetCount(); ++i)
        {
			IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pSel->GetSelectedObjectID(i));
            m_ObjectArray.push_back(pGxObject.get());
        }

		wxGISAppConfig oConfig = GetConfig();
		if(oConfig.IsOk())
			oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/lastpath/path")), GetPath());

        if ( IsModal() )
            EndModal(m_nRetCode);
        else
        {
            SetReturnCode(m_nRetCode);
            this->Show(false);
        }
    }
}

void wxGxContainerDialog::OnCreate(wxCommandEvent& event)
{
    //focus tree view
    m_pTree->SetFocus();
    if(m_CommandArray[0])
        m_CommandArray[0]->OnClick();
}

void wxGxContainerDialog::OnCreateUI(wxUpdateUIEvent& event)
{
    if(!m_CommandArray[0])
    {
        event.Enable(false);
        return;
    }
    event.Enable(m_CommandArray[0]->GetEnabled());
}
*/
/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxContainerDialog class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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

#include "wxgis/catalogui/gxcontdialog.h"

#include "wxgis/catalogui/catalogcmd.h"
#include "wxgis/framework/application.h"

//---------------------------------------------------------------------------------
// wxTreeContainerView
//---------------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxTreeContainerView, wxGxTreeView)

wxTreeContainerView::wxTreeContainerView(void) : wxGxTreeView()
{
}

wxTreeContainerView::wxTreeContainerView(wxWindow* parent, wxWindowID id, long style) : wxGxTreeView(parent, id, style)
{
}

wxTreeContainerView::~wxTreeContainerView(void)
{
}

void wxTreeContainerView::AddTreeItem(wxGxObject* pGxObject, wxTreeItemId hParent)
{
	if(NULL == pGxObject)
		return;

    if (!m_ShowFilter.IsEmpty())
    {
        for (size_t i = 0; i < m_ShowFilter.GetCount(); ++i)
        {
            if (m_ShowFilter[i]->CanDisplayObject(pGxObject))
                goto ADD;
        }
        return;
	}


ADD:
    wxGxTreeViewBase::AddTreeItem(pGxObject, hParent);
}

void wxTreeContainerView::AddShowFilter(wxGxObjectFilter* pFilter)
{
	m_ShowFilter.push_back(pFilter);
}

void wxTreeContainerView::RemoveAllShowFilters(void)
{
    m_ShowFilter.Clear();
}

bool wxTreeContainerView::CanChooseObject(wxGxObject* pGxObject)
{
    if (NULL == pGxObject)
		return false;

    if (!m_ShowFilter.IsEmpty())
    {
        for (size_t i = 0; i < m_ShowFilter.GetCount(); ++i)
        {
            if (m_ShowFilter[i]->CanChooseObject(pGxObject))
            {
                return true;
            }
        }
    }
    return false;
}

//---------------------------------------------------------------------------------
// wxGxContainerDialog
//---------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxContainerDialog, wxDialog)

BEGIN_EVENT_TABLE(wxGxContainerDialog, wxDialog)
    EVT_COMBOBOX(FILTERCOMBO, wxGxContainerDialog::OnFilterSelect)
    EVT_BUTTON(wxID_OK, wxGxContainerDialog::OnOK)
    EVT_BUTTON(ID_CREATE, wxGxContainerDialog::OnCreate)
    EVT_UPDATE_UI(wxID_OK, wxGxContainerDialog::OnOKUI)
    EVT_UPDATE_UI(ID_CREATE, wxGxContainerDialog::OnCreateUI)
	EVT_MENU_RANGE(ID_PLUGINCMD, ID_PLUGINCMDMAX, wxGxContainerDialog::OnCommand)
	EVT_UPDATE_UI_RANGE(ID_PLUGINCMD, ID_PLUGINCMDMAX, wxGxContainerDialog::OnCommandUI)
END_EVENT_TABLE()

wxGxContainerDialog::wxGxContainerDialog(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style), wxGISApplicationBase(), wxGxApplicationBase()
{
	//this->SetSizeHints( wxSize( 400,300 ), wxDefaultSize );

    m_pTree = NULL;
    m_bShowExportFormats = false;
    m_bShowCreateButton = false;
    m_nDefaultFilter = 0;
    m_bAllFilters = true;
    m_bOwnFilter = m_bOwnShowFilter = true;

    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);

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


	m_CreateButton = new wxButton( this, ID_CREATE , _("Create folder"), wxDefaultPosition, wxDefaultSize, 0 );
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

	this->SetSizerAndFit( bMainSizer );
	this->Layout();

	wxGISCatalogMainCmd* pwxGISCatalogMainCmd(NULL);
    int nCmdCounter(0);

	//create folder
	pwxGISCatalogMainCmd = new wxGISCatalogMainCmd();
	pwxGISCatalogMainCmd->OnCreate(this);
    pwxGISCatalogMainCmd->SetId(ID_PLUGINCMD + nCmdCounter);
	pwxGISCatalogMainCmd->SetSubType(7);
	m_CommandArray.push_back(pwxGISCatalogMainCmd);
    nCmdCounter++;

	//rename
	pwxGISCatalogMainCmd = new wxGISCatalogMainCmd();
	pwxGISCatalogMainCmd->OnCreate(this);
    pwxGISCatalogMainCmd->SetId(ID_PLUGINCMD + nCmdCounter);
	pwxGISCatalogMainCmd->SetSubType(8);
	m_CommandArray.push_back(pwxGISCatalogMainCmd);
    nCmdCounter++;

	//refresh
	pwxGISCatalogMainCmd = new wxGISCatalogMainCmd();
	pwxGISCatalogMainCmd->OnCreate(this);
    pwxGISCatalogMainCmd->SetId(ID_PLUGINCMD + nCmdCounter);
	pwxGISCatalogMainCmd->SetSubType(9);
	m_CommandArray.push_back(pwxGISCatalogMainCmd);

	//load accelerators
    m_pGISAcceleratorTable = new wxGISAcceleratorTable(this);
}

wxGxContainerDialog::~wxGxContainerDialog()
{
    if (NULL != m_pTree)
    {
		m_pTree->Deactivate();
    }

    SerializeFramePos(true);

	RemoveAllFilters();

    wxDELETE(m_pTree);

    wxDELETE(m_pGISAcceleratorTable);
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

void wxGxContainerDialog::SetButtonCaption(const wxString &sOkBtLabel)
{
	m_sOkBtLabel = sOkBtLabel;
}

void wxGxContainerDialog::SetStartingLocation(const wxString &sStartPath)
{
	m_sStartPath = sStartPath;
}

void wxGxContainerDialog::SetDescriptionText(const wxString &sText)
{
    m_staticDescriptionText->SetLabel(sText);
}

void wxGxContainerDialog::ShowCreateButton(bool bShow)
{
    m_bShowCreateButton = bShow;
    m_CreateButton->Show(m_bShowCreateButton);
}

void wxGxContainerDialog::AddFilter(wxGxObjectFilter* pFilter, bool bDefault)
{
	m_FilterArray.push_back(pFilter);
    if (bDefault)
    {
		m_nDefaultFilter = m_FilterArray.GetCount() - 1;
    }
}

void wxGxContainerDialog::RemoveAllFilters(void)
{
    if (m_bOwnFilter)
    {
        for (size_t i = 0; i < m_FilterArray.GetCount(); ++i)
        {
			wxDELETE(m_FilterArray[i]);
        }
    }
}

void wxGxContainerDialog::AddShowFilter(wxGxObjectFilter* pFilter)
{
	m_paShowFilter.push_back(pFilter);
	//m_pTree->AddShowFilter(pFilter);
}

void wxGxContainerDialog::RemoveAllShowFilters(void)
{
    m_pTree->RemoveAllShowFilters();
    if (m_bOwnShowFilter)
    {
        for (size_t i = 0; i < m_paShowFilter.size(); ++i)
        {
            wxDELETE(m_paShowFilter[i]);
        }
    }
}

int wxGxContainerDialog::ShowModal(void)
{
	OnInit();
	return wxDialog::ShowModal();
}

void wxGxContainerDialog::OnInit()
{
    m_pTree = new wxTreeContainerView( this, TREECTRLID, wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS | wxTR_NO_LINES | wxTR_SINGLE | wxTR_EDIT_LABELS );
    wxGISAppConfig oConfig = GetConfig();
    if (oConfig.IsOk())
    {
        wxXmlNode* pTreeViewConf = oConfig.GetConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/frame/views/treeview")));
        m_pTree->Activate(this, pTreeViewConf);
    }

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


    wxString sLastPath = m_sStartPath;
    if (sLastPath.IsEmpty())
    {
        if (oConfig.IsOk())
            sLastPath = oConfig.Read(enumGISHKCU, GetAppName() + wxString(wxT("/lastpath/path")), m_pCatalog->GetName());
        else
            sLastPath = m_pCatalog->GetName();
    }

    SetLocation(sLastPath);

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

wxGxObjectFilter* wxGxContainerDialog::GetCurrentFilter(void) const
{
    if (m_FilterArray.IsEmpty())
        return NULL;
    return m_FilterArray[m_nDefaultFilter];
}

size_t wxGxContainerDialog::GetCurrentFilterId(void) const
{
    return m_nDefaultFilter;
}

wxGxObject* const wxGxContainerDialog::GetLocation(void) const
{
    wxGxSelection* const pSel = GetGxSelection();
    if (NULL == pSel)
        return NULL;
    long nId = pSel->GetLastSelectedObjectId();
    return m_pCatalog->GetRegisterObject(nId);
}

CPLString wxGxContainerDialog::GetPath(void) const
{
    wxGxObject* pObj = GetLocation();
    if (NULL == pObj)
        return CPLString();

    return pObj->GetPath();
}

wxString wxGxContainerDialog::GetFullName(void) const
{
    wxGxObject* pObj = GetLocation();
    if (NULL == pObj)
        return wxEmptyString;

    return pObj->GetFullName();
}


wxString wxGxContainerDialog::GetName(void) const
{
    wxGxObject* pObj = GetLocation();
    if (NULL == pObj)
        return wxEmptyString;

    return pObj->GetName();
}

void wxGxContainerDialog::OnOKUI(wxUpdateUIEvent& event)
{
    wxGxObject* const pGxObject = GetLocation();
    bool bEnable = false;
    if (NULL != m_FilterArray[m_WildcardCombo->GetCurrentSelection()])
        bEnable = m_FilterArray[m_WildcardCombo->GetCurrentSelection()]->CanStoreToObject(pGxObject);
    event.Enable(bEnable);
}

void wxGxContainerDialog::OnFilterSelect(wxCommandEvent& event)
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
        wxGxSelection* const pSel = GetGxSelection();
        if (NULL != pSel)
        {
            for (size_t i = 0; i < pSel->GetCount(); ++i)
            {
                wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pSel->GetSelectedObjectId(i));
                m_ObjectList.Append(pGxObject);
            }
        }

		wxGISAppConfig oConfig = GetConfig();
        if (oConfig.IsOk())
        {
			oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/lastpath/path")), GetFullName());
        }

        if (IsModal())
        {
            EndModal(m_nRetCode);
        }
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
    if (NULL != m_CommandArray[0])
    {
        m_CommandArray[0]->OnClick();
    }
}

void wxGxContainerDialog::OnCreateUI(wxUpdateUIEvent& event)
{
    if (NULL != m_CommandArray[0])
    {
        event.Enable(m_CommandArray[0]->GetEnabled());
    }
    else
    {
        event.Enable(false);
    }
}

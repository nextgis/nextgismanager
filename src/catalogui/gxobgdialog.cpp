/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObjectDialog class.
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

#include "wxgis/catalogui/gxobjdialog.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/catalogui/catalogcmd.h"
#include "wxgis/catalogui/viewscmd.h"
#include "wxgis/catalogui/gxdiscconnectionui.h"
#include "wxgis/framework/application.h"

#include "../../art/document_16.xpm"
#include "../../art/document_48.xpm"
#include "../../art/small_arrow.xpm"

#include <wx/valgen.h>

//---------------------------------------------------------------------------------
// wxTreeViewComboPopup
//---------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxTreeViewComboPopup, wxGxTreeViewBase)

BEGIN_EVENT_TABLE(wxTreeViewComboPopup, wxGxTreeViewBase)
    //TODO: EVT_LEFT_UP(wxTreeViewComboPopup::OnMouseClick)
    EVT_LEFT_DOWN(wxTreeViewComboPopup::OnMouseClick)
    EVT_MOTION(wxTreeViewComboPopup::OnMouseMove)
    EVT_TREE_ITEM_ACTIVATED(TREECTRLID, wxTreeViewComboPopup::OnDblClick)
	EVT_GXSELECTION_CHANGED(wxTreeViewComboPopup::OnSelectionChanged)
END_EVENT_TABLE()

bool wxTreeViewComboPopup::Create(wxWindow* parent)
{
    return wxTreeViewComboPopup::Create(parent, TREECTRLID);
}

bool wxTreeViewComboPopup::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_bClicked = false;
    return wxGxTreeViewBase::Create(parent, TREECTRLID, pos, size, wxBORDER_SIMPLE | wxTR_NO_BUTTONS | wxTR_NO_LINES | wxTR_SINGLE, name);
}

void wxTreeViewComboPopup::Init()
{	
}

void wxTreeViewComboPopup::OnPopup()
{
   m_bClicked = false;
   SelectItem(m_TreeMap[m_pSelection->GetLastSelectedObjectId()]);
   //CaptureMouse();
}

void wxTreeViewComboPopup::OnDismiss()
{
    //ReleaseMouse();
}

void wxTreeViewComboPopup::SetStringValue(const wxString& s)
{
    //int n = wxListView::FindItem(-1,s);
    //if ( n >= 0 && n < wxListView::GetItemCount() )
    //    wxListView::Select(n);
}

// Get list selection as a string
wxString wxTreeViewComboPopup::GetStringValue() const
{
    if( m_bClicked == false )
    {
        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(m_pSelection->GetLastSelectedObjectId());
        if(pGxObject)
            return pGxObject->GetName();
        return wxEmptyString;
    }

    wxTreeItemId ItemId = wxTreeCtrl::GetSelection();
    if(ItemId.IsOk())
        return GetItemText(ItemId);
    return wxEmptyString;
}

void wxTreeViewComboPopup::OnMouseMove(wxMouseEvent& event)
{
    wxPoint pt = event.GetPosition();
    int flag = wxTREE_HITTEST_ONITEMINDENT;
    wxTreeItemId ItemId = wxTreeCtrl::HitTest(pt, flag);
    if(ItemId.IsOk())
    {
        SelectItem(ItemId);
    }
    //event.Skip();
}

void wxTreeViewComboPopup::OnMouseClick(wxMouseEvent& event)
{
    m_bClicked = true;
    wxTreeItemId ItemId = GetSelection();
	if(ItemId.IsOk())
    {
        if(m_PrewItemId.IsOk())
        {
            CollapseAllChildren(GetItemParent(m_PrewItemId));
        }
        if(ItemId != GetRootItem())
        {
            CollapseAllChildren(ItemId);
        }
        else
        {
            CollapseAll();
            Expand(GetRootItem());
        }

	    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(ItemId);
	    if(pData != NULL)
	    {
            SelectItem(ItemId);
		    m_pSelection->Select(pData->m_nObjectID, false, GetId());
            m_PrewItemId = ItemId;
	    }
    }
    Dismiss();

    //event.Skip();
}

void wxTreeViewComboPopup::OnSelectionChanged(wxGxSelectionEvent& event)
{
	if(event.GetInitiator() == GetId())
		return;

    long nSelID = m_pSelection->GetLastSelectedObjectId();
	wxTreeItemId ItemId = m_TreeMap[nSelID];
	if(ItemId.IsOk())
	{
		SelectItem(ItemId);
	}
	else
	{
        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(nSelID);
		wxGxObject* pParentGxObj = pGxObject->GetParent();
		while(pParentGxObj)
		{
			wxTreeItemId ItemId = m_TreeMap[pParentGxObj->GetId()];
			if(ItemId.IsOk())
			{
				Expand(ItemId);
				break;
			}
			else
				pParentGxObj = pParentGxObj->GetParent();
		}
		OnSelectionChanged(event);
	}
    //set combo text
    wxString sText = GetStringValue();
    m_combo->SetText(sText);
}

void wxTreeViewComboPopup::OnDblClick(wxTreeEvent& event)
{
    wxTreeItemId ItemId = wxTreeCtrl::GetSelection();
	if(!ItemId.IsOk())
		return;
	wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(ItemId);
	if(pData != NULL)
	{
		m_pSelection->Select(pData->m_nObjectID, false, GetId());
	}
    wxComboPopup::Dismiss();
}

void wxTreeViewComboPopup::AddTreeItem(wxGxObject* pGxObject, wxTreeItemId hParent)
{
	if(NULL == pGxObject)
		return;
	wxGxObjectContainer* pContainer = wxDynamicCast(pGxObject, wxGxObjectContainer);
	if(NULL == pContainer)
		return;

    if (m_TreeMap[pGxObject->GetId()].IsOk())
    {
        return;
    }

	IGxObjectUI* pObjUI =  dynamic_cast<IGxObjectUI*>(pGxObject);
	wxIcon icon;
	if(pObjUI != NULL)
		icon = pObjUI->GetSmallImage();

	int pos(wxNOT_FOUND);
	if(icon.IsOk())
    {
        for(size_t i = 0; i < m_IconsArray.size(); ++i)
        {
            if(m_IconsArray[i].oIcon.IsSameAs(icon))
            {
                pos = m_IconsArray[i].iImageIndex;
                break;
            }
        }
        if(pos == wxNOT_FOUND)
        {
            pos = m_TreeImageList.Add(icon);
            ICONDATA myicondata = {icon, pos};
            m_IconsArray.push_back(myicondata);
        }
    }
	else
		pos = 0;//0 col img, 1 - col img

	wxGxTreeItemData* pData = new wxGxTreeItemData(pGxObject->GetId(), pos, false);

    wxString sName;
    if(m_pCatalog->GetShowExt())
        sName = pGxObject->GetName();
    else
        sName = pGxObject->GetBaseName();


	wxTreeItemId NewTreeItem = AppendItem(hParent, sName, pos, -1, pData);
	m_TreeMap[pGxObject->GetId()] = NewTreeItem;

	if(pContainer->AreChildrenViewable())
		SetItemHasChildren(NewTreeItem);

//	SortChildren(hParent);
	wxTreeCtrl::Refresh();
}

wxSize wxTreeViewComboPopup::GetAdjustedSize(int minWidth, int prefHeight, int maxHeight)
{
    return wxSize(minWidth + 190, prefHeight);
}

wxGxSelection* const wxTreeViewComboPopup::GetSelectedObjects(void)
{
    m_pSelection->SetInitiator(TREECTRLID);
    return m_pSelection;
}

//-----------------------------------------------------------------------------
// wxGxDialogContentView
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxDialogContentView, wxGxContentView)

BEGIN_EVENT_TABLE(wxGxDialogContentView, wxGxContentView)
    EVT_LIST_BEGIN_LABEL_EDIT(OBJDLGLISTCTRLID, wxGxDialogContentView::OnBeginLabelEdit)
    EVT_LIST_END_LABEL_EDIT(OBJDLGLISTCTRLID, wxGxDialogContentView::OnEndLabelEdit)
    EVT_LIST_ITEM_SELECTED(OBJDLGLISTCTRLID, wxGxDialogContentView::OnSelected)
    EVT_LIST_ITEM_DESELECTED(OBJDLGLISTCTRLID, wxGxDialogContentView::OnDeselected)
    EVT_LIST_ITEM_ACTIVATED(OBJDLGLISTCTRLID, wxGxDialogContentView::OnActivated)

    ////EVT_LIST_BEGIN_DRAG(OBJDLGLISTCTRLID, wxGxDialogContentView::OnBeginDrag)
    //EVT_LIST_BEGIN_RDRAG(OBJDLGLISTCTRLID, wxGxDialogContentView::OnBeginDrag)

    EVT_LIST_COL_CLICK(OBJDLGLISTCTRLID, wxGxDialogContentView::OnColClick)
END_EVENT_TABLE()

wxGxDialogContentView::wxGxDialogContentView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    m_ConnectionPointSelectionCookie = wxNOT_FOUND;
    m_nFilterIndex = 0;
    Create(parent, id, pos, size, style);
}

bool wxGxDialogContentView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{  
    m_HighLightItem = wxNOT_FOUND;
    m_pCatalog = NULL;
    m_bSortAsc = true;
    m_current_style = enumGISCVUndefined;
    m_ConnectionPointCatalogCookie = wxNOT_FOUND;
    m_nParentGxObjectID = wxNOT_FOUND;
    m_currentSortCol = 0;
    m_pSelection = NULL;
    m_bDragging = false;
    m_pDeleteCmd = NULL;

    wxListCtrl::Create(parent, OBJDLGLISTCTRLID, pos, size, wxLC_LIST | wxLC_EDIT_LABELS | wxLC_SORT_ASCENDING);

	m_ImageListSmall.Create(16, 16);
	m_ImageListLarge.Create(48, 48);

	//set default icons
	//col ico & default
    m_ImageListLarge.Add(wxBitmap(48, 48));
    m_ImageListLarge.Add(wxBitmap(48, 48));
	m_ImageListLarge.Add(wxBitmap(document_48_xpm));

    wxBitmap SmallA(small_arrow_xpm);// > arrow
    wxImage SmallImg = SmallA.ConvertToImage();
    SmallImg = SmallImg.Rotate90();
    wxBitmap SmallDown(SmallImg);
    SmallImg = SmallImg.Mirror(false);
    wxBitmap SmallUp(SmallImg);

	m_ImageListSmall.Add(wxBitmap(SmallDown));
	m_ImageListSmall.Add(wxBitmap(SmallUp));
	m_ImageListSmall.Add(wxBitmap(document_16_xpm));

	SetImageList(&m_ImageListLarge, wxIMAGE_LIST_NORMAL);
	SetImageList(&m_ImageListSmall, wxIMAGE_LIST_SMALL);

    return true;
}

wxGxDialogContentView::~wxGxDialogContentView(void)
{
}

bool wxGxDialogContentView::Activate(wxGISApplicationBase* application, wxXmlNode* pConf)
{
	if(!wxGxContentView::Activate(application, pConf))
        return false;

	m_ConnectionPointSelectionCookie = m_pSelection->Advise(this);

	return true;
}

void wxGxDialogContentView::Deactivate(void)
{
	if(m_ConnectionPointSelectionCookie != wxNOT_FOUND)
		m_pSelection->Unadvise(m_ConnectionPointSelectionCookie);
	wxGxContentView::Deactivate();
}

void wxGxDialogContentView::OnActivated(wxListEvent& event)
{
	//event.Skip();
	//dbl click
	LPITEMDATA pItemData = (LPITEMDATA)event.GetData();
	if(pItemData == NULL)
		return;

    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pItemData->nObjectID);
	wxGxObjectContainer* pGxObjectContainer = wxDynamicCast(pGxObject, wxGxObjectContainer);
	if(pGxObjectContainer != NULL )
	{
	    IGxObjectWizard* pGxObjectWizard = dynamic_cast<IGxObjectWizard*>(pGxObjectContainer);
	    if(pGxObjectWizard != NULL)
		    if(!pGxObjectWizard->Invoke(this))
			    return;
        m_pSelection->Select(pItemData->nObjectID, false, wxGxSelection::INIT_ALL);//GetId()
		return;
	}

	wxCommandEvent butevent( wxEVT_COMMAND_BUTTON_CLICKED, wxID_OK );
    GetParent()->GetEventHandler()->ProcessEvent( butevent );
}

void wxGxDialogContentView::SetCurrentFilter(size_t nFilterIndex)
{
	m_nFilterIndex = nFilterIndex;
}

bool wxGxDialogContentView::AddObject(wxGxObject* const pObject)
{
    if(m_FiltersArray.IsEmpty())
		return wxGxContentView::AddObject(pObject);

    if(m_nFilterIndex != m_FiltersArray.GetCount())
	{
		if(m_FiltersArray[m_nFilterIndex]->CanDisplayObject(pObject))
			return wxGxContentView::AddObject(pObject);
	}
	else
	{
		for(size_t i = 0; i < m_FiltersArray.GetCount(); ++i)
			if(m_FiltersArray[i]->CanDisplayObject(pObject))
				return wxGxContentView::AddObject(pObject);
	}
    return false;
}

wxGxSelection* const wxGxDialogContentView::GetSelectedObjects(void)
{
    m_pSelection->SetInitiator(NOTFIRESELID);
    return m_pSelection;
}

//---------------------------------------------------------------------------------
// wxGxObjectDialog
//---------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxObjectDialog, wxDialog)

BEGIN_EVENT_TABLE(wxGxObjectDialog, wxDialog)
	//EVT_LIST_ITEM_SELECTED(LISTCTRLID, wxGxObjectDialog::OnItemSelected)
	//EVT_LIST_ITEM_DESELECTED(LISTCTRLID, wxGxObjectDialog::OnItemSelected)
	EVT_MENU_RANGE(ID_PLUGINCMD, ID_PLUGINCMDMAX, wxGxObjectDialog::OnCommand)
	EVT_MENU_RANGE(ID_MENUCMD, ID_MENUCMDMAX, wxGxObjectDialog::OnDropDownCommand)
	EVT_UPDATE_UI_RANGE(ID_PLUGINCMD, ID_PLUGINCMDMAX, wxGxObjectDialog::OnCommandUI)
    EVT_AUITOOLBAR_TOOL_DROPDOWN(wxID_ANY, wxGxObjectDialog::OnToolDropDown)
    EVT_COMBOBOX(FILTERCOMBO, wxGxObjectDialog::OnFilterSelect)
    EVT_BUTTON(wxID_OK, wxGxObjectDialog::OnOK)
    EVT_UPDATE_UI(wxID_OK, wxGxObjectDialog::OnOKUI)
END_EVENT_TABLE()

wxGxObjectDialog::wxGxObjectDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style ), wxGISApplicationBase(), wxGxApplicationBase()
{
	//this->SetSizeHints( wxSize( 400,300 ), wxDefaultSize );

    m_pDropDownCommand = NULL;
    m_bAllowMultiSelect = false;
    m_bOverwritePrompt = false;
    m_nDefaultFilter = 0;
    m_pwxGxContentView = NULL;
    m_PopupCtrl = NULL;
    m_bAllFilters = true;
    m_bOwnFilter = true;

    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);

	bMainSizer = new wxBoxSizer( wxVERTICAL );

	bHeaderSizer = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Look in:   "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText1->Wrap( -1 );
	bHeaderSizer->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_TreeCombo = new wxComboCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCB_READONLY);
#ifdef __WXMSW__
    m_TreeCombo->UseAltPopupWindow(true);
#else
    m_TreeCombo->UseAltPopupWindow(false);
#endif
    m_PopupCtrl = new wxTreeViewComboPopup();
    m_TreeCombo->SetPopupControl(m_PopupCtrl);
    m_TreeCombo->EnablePopupAnimation(true);
    //m_PopupCtrl->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( wxTreeViewComboPopup::OnMouseClick ), NULL, m_PopupCtrl );
    m_PopupCtrl->Activate(this, NULL);//TODO:

	bHeaderSizer->Add( m_TreeCombo, 1, wxALL | wxALIGN_CENTER_VERTICAL, 5 );

	m_toolBar = new wxAuiToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize,  wxAUI_TB_HORZ_LAYOUT|wxBORDER_NONE);
	m_toolBar->SetToolBitmapSize( wxSize( 16,16 ) );
	m_toolBar->SetArtProvider(new wxGxToolBarArt());

    //	0	Up One Level
    //	1	Connect Folder
    //	2	Disconnect Folder
    //	3	Location
    //  4   Delete Item
    //  5   Back
    //  6   Forward
    //  7   Create Folder

    int IDs[8] = {5,0,wxNOT_FOUND,1,2,wxNOT_FOUND,4,7};
    wxGISCatalogMainCmd* pwxGISCatalogMainCmd(NULL);
    int nCmdCounter(0);
    for(size_t i = 0; i < 8; ++i)
    {
        if(IDs[i] == wxNOT_FOUND)
        {
            m_toolBar->AddSeparator();
            continue;
        }
        pwxGISCatalogMainCmd = new wxGISCatalogMainCmd();
        pwxGISCatalogMainCmd->OnCreate(this);
        pwxGISCatalogMainCmd->SetSubType(IDs[i]);
        pwxGISCatalogMainCmd->SetId(ID_PLUGINCMD + nCmdCounter);
        nCmdCounter++;

        m_CommandArray.push_back(pwxGISCatalogMainCmd);

        wxGISEnumCommandKind kind = pwxGISCatalogMainCmd->GetKind();
        if (kind == enumGISCommandDropDown)
            kind = enumGISCommandNormal;
		m_toolBar->AddTool( pwxGISCatalogMainCmd->GetId(), pwxGISCatalogMainCmd->GetCaption(), pwxGISCatalogMainCmd->GetBitmap(), wxNullBitmap, (wxItemKind)kind, pwxGISCatalogMainCmd->GetTooltip(), pwxGISCatalogMainCmd->GetMessage(), NULL );
		if(pwxGISCatalogMainCmd->GetKind() == enumGISCommandDropDown)
			m_toolBar->SetToolDropDown(pwxGISCatalogMainCmd->GetId(), true);
    }
    
    wxGISCatalogViewsCmd* pwxGISCatalogViewsCmd = new wxGISCatalogViewsCmd();
    pwxGISCatalogViewsCmd->OnCreate(this);
    pwxGISCatalogViewsCmd->SetSubType(0);
    pwxGISCatalogViewsCmd->SetId(ID_PLUGINCMD + nCmdCounter);
    nCmdCounter++;

    //pwxGISCatalogViewsCmd->SetID(ID_PLUGINCMD + 9);
    m_CommandArray.push_back(pwxGISCatalogViewsCmd);
    wxGISEnumCommandKind kind = pwxGISCatalogViewsCmd->GetKind();
    if(pwxGISCatalogViewsCmd->GetKind() == enumGISCommandDropDown)
        kind = enumGISCommandNormal;
	m_toolBar->AddTool( pwxGISCatalogViewsCmd->GetId(), pwxGISCatalogViewsCmd->GetCaption(), pwxGISCatalogViewsCmd->GetBitmap(), wxNullBitmap, (wxItemKind)kind, pwxGISCatalogViewsCmd->GetTooltip(), pwxGISCatalogViewsCmd->GetMessage(), NULL );
	if(pwxGISCatalogViewsCmd->GetKind() == enumGISCommandDropDown)
		m_toolBar->SetToolDropDown(pwxGISCatalogViewsCmd->GetId(), true);

	m_toolBar->Realize();

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
    nCmdCounter++;

    //load accelerators
    m_pGISAcceleratorTable = new wxGISAcceleratorTable(this);    

	bHeaderSizer->Add( m_toolBar, 0, wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("  "), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText2->Wrap( -1 );
	bHeaderSizer->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	bMainSizer->Add( bHeaderSizer, 0, wxEXPAND, 5 );
//
//
	fgCeilSizer = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgCeilSizer->AddGrowableCol( 1 );
	fgCeilSizer->SetFlexibleDirection( wxBOTH );
	fgCeilSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText4->Wrap( -1 );
	fgCeilSizer->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );

	m_NameTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL, wxGenericValidator(&m_sName) );
	fgCeilSizer->Add( m_NameTextCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_OkButton = new wxButton( this, wxID_OK , _("Activate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OkButton->SetDefault();
	fgCeilSizer->Add( m_OkButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Show of type:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText6->Wrap( -1 );
	fgCeilSizer->Add( m_staticText6, 0, wxALL|wxALIGN_RIGHT, 5 );

	m_WildcardCombo = new wxComboBox( this, FILTERCOMBO, _("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxCB_READONLY );//|wxCB_SORT
	fgCeilSizer->Add( m_WildcardCombo, 0, wxALL|wxEXPAND, 5 );

	m_CancelButton = new wxButton( this, wxID_CANCEL, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	fgCeilSizer->Add( m_CancelButton, 0, wxALL, 5 );

	bMainSizer->Add( fgCeilSizer, 0, wxEXPAND, 5 );

    this->SetSizerAndFit(bMainSizer);
	this->Layout();
}

wxGxObjectDialog::~wxGxObjectDialog()
{
	if(m_pwxGxContentView)
		m_pwxGxContentView->Deactivate();
	if(m_PopupCtrl)
		m_PopupCtrl->Deactivate();

    SerializeFramePos(true);

	RemoveAllFilters();

    wxDELETE(m_pwxGxContentView);

    wxDELETE(m_pGISAcceleratorTable);
}

void wxGxObjectDialog::OnCommand(wxCommandEvent& event)
{
	event.Skip();
	Command(GetCommand(event.GetId()));
}

void wxGxObjectDialog::Command(wxGISCommand* pCmd)
{
	pCmd->OnClick();
    if(pCmd->GetSubType() == 4)
    {
        wxGxSelection* pSel = m_pwxGxContentView->GetSelectedObjects();
        if(pSel)
            pSel->Clear(NOTFIRESELID);//LISTCTRLID
    }
}

void wxGxObjectDialog::OnCommandUI(wxUpdateUIEvent& event)
{
	wxGISCommand* pCmd = GetCommand(event.GetId());
	if(pCmd)
	{
		if(pCmd->GetKind() == enumGISCommandCheck)
			event.Check(pCmd->GetChecked());
		event.Enable(pCmd->GetEnabled());
        m_toolBar->SetToolShortHelp(event.GetId(), pCmd->GetTooltip());
    }
}

void wxGxObjectDialog::OnDropDownCommand(wxCommandEvent& event)
{
	event.Skip();
	if(m_pDropDownCommand)
        m_pDropDownCommand->OnDropDownCommand(event.GetId());
}

void wxGxObjectDialog::OnToolDropDown(wxAuiToolBarEvent& event)
{
    if(event.IsDropDownClicked())
    {
        wxGISCommand* pCmd = GetCommand(event.GetToolId());
        m_pDropDownCommand = dynamic_cast<IDropDownCommand*>(pCmd);
        if(m_pDropDownCommand)
        {
            wxMenu* pMenu = m_pDropDownCommand->GetDropDownMenu();
            if(pMenu)
            {
                PushEventHandler(pMenu);
				m_toolBar->PopupMenu(pMenu, event.GetItemRect().GetBottomLeft());
                PopEventHandler();
                delete pMenu;
                return;
            }
        }
    }
	event.Skip();
}

void wxGxObjectDialog::SetButtonCaption(const wxString &sOkBtLabel)
{
	m_sOkBtLabel = sOkBtLabel;
}

void wxGxObjectDialog::SetStartingLocation(const wxString &sStartPath)
{
	m_sStartPath = sStartPath;
}

void wxGxObjectDialog::SetName(const wxString &sName)
{
	m_sName = sName;
}

void wxGxObjectDialog::SetAllowMultiSelect(bool bAllowMultiSelect)
{
	m_bAllowMultiSelect = bAllowMultiSelect;
}

void wxGxObjectDialog::SetOverwritePrompt(bool bOverwritePrompt)
{
	m_bOverwritePrompt = bOverwritePrompt;
}

void wxGxObjectDialog::SetAllFilters(bool bAllFilters)
{
	m_bAllFilters = bAllFilters;
}

int wxGxObjectDialog::ShowModalOpen()
{
    m_bIsSaveDlg = false;
	if(m_OkButton)
	{
		if(m_sOkBtLabel.IsEmpty())
			m_OkButton->SetLabel(_("Open"));
		else
			m_OkButton->SetLabel(m_sOkBtLabel);
	}
	OnInit();

	return wxDialog::ShowModal();
}

int wxGxObjectDialog::ShowModalSave()
{
    m_bIsSaveDlg = true;
	if(m_OkButton)
	{
		if(m_sOkBtLabel.IsEmpty())
			m_OkButton->SetLabel(_("Save"));
		else
			m_OkButton->SetLabel(m_sOkBtLabel);
	}
	m_bAllowMultiSelect = false;
	OnInit();

	return wxDialog::ShowModal();
}

void wxGxObjectDialog::OnInit()
{
    long nStyle = wxLC_LIST | wxLC_EDIT_LABELS | wxLC_SORT_ASCENDING | wxBORDER_THEME;
	if(!m_bAllowMultiSelect)
		nStyle |= wxLC_SINGLE_SEL;
   	m_pwxGxContentView = new wxGxDialogContentView(this, LISTCTRLID, wxDefaultPosition, wxDefaultSize, nStyle);
	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
	{
	    wxXmlNode* pContentViewConf = oConfig.GetConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/frame/views/contentsview")));
	    m_pwxGxContentView->Activate(this, pContentViewConf);
	}
    RegisterChildWindow(m_pwxGxContentView->GetId());

	bMainSizer->Insert(1, m_pwxGxContentView, 1, wxALL|wxEXPAND, 5 );
	//m_pwxGxContentView->Connect( wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler( wxGxObjectDialog::OnItemSelected ), NULL, this );
	//m_pwxGxContentView->Connect( wxEVT_COMMAND_LIST_ITEM_DESELECTED, wxListEventHandler( wxGxObjectDialog::OnItemSelected ), NULL, this );
	m_pwxGxContentView->Bind( wxEVT_COMMAND_LIST_ITEM_SELECTED, &wxGxObjectDialog::OnItemSelected, this );
	m_pwxGxContentView->Bind( wxEVT_COMMAND_LIST_ITEM_DESELECTED, &wxGxObjectDialog::OnItemSelected, this );

	for(size_t i = 0; i < m_FilterArray.size(); ++i)
		m_WildcardCombo->AppendString(m_FilterArray[i]->GetName());
	if(m_FilterArray.size() > 1 && m_bAllFilters)
		m_WildcardCombo->AppendString(_("All listed filters"));
	if(m_FilterArray.size() == 0)
		m_WildcardCombo->AppendString(_("All items"));
	m_WildcardCombo->Select(m_nDefaultFilter);

	m_pwxGxContentView->SetCurrentFilter(m_nDefaultFilter);
	m_pwxGxContentView->SetFilters(m_FilterArray);

    wxString sLastPath = m_sStartPath;
	if(sLastPath.IsEmpty())
	{		
		if(oConfig.IsOk())
			sLastPath = oConfig.Read(enumGISHKCU, GetAppName() + wxString(wxT("/lastpath/path")), m_pCatalog->GetName());
		else
			sLastPath = m_pCatalog->GetName();
	}

    SetLocation(sLastPath);

    m_NameTextCtrl->SetFocus();

    SerializeFramePos(false);
}

void wxGxObjectDialog::OnItemSelected(wxListEvent& event)
{
	event.Skip();
    //if(m_bIsSaveDlg)
    //    return;

    wxGxDialogContentView::LPITEMDATA pItemData = (wxGxDialogContentView::LPITEMDATA)event.GetData();
	if(pItemData == NULL)
		return;

	wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pItemData->nObjectID);
    bool bIsDataset = pGxObject->IsKindOf(wxCLASSINFO(wxGxDataset)) || m_FilterArray[m_WildcardCombo->GetCurrentSelection()]->CanChooseObject(pGxObject);
    bool bIsObjContainer = pGxObject->IsKindOf(wxCLASSINFO(wxGxObjectContainer));
    if(!bIsDataset && bIsObjContainer)
    {
        return;
    }

    wxString sTempName;
	long item = wxNOT_FOUND;
	while(1)
    {
        item = m_pwxGxContentView->GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( item == wxNOT_FOUND )
            break;
		if(sTempName.IsEmpty())
			sTempName += m_pwxGxContentView->GetItemText(item);
		else
			sTempName += wxT("; ") + m_pwxGxContentView->GetItemText(item);
    }
	if(!sTempName.IsEmpty())
        m_sName = sTempName;
	TransferDataToWindow();
}

void wxGxObjectDialog::AddFilter(wxGxObjectFilter* pFilter, bool bDefault)
{
	m_FilterArray.Add(pFilter);
	if(bDefault)
		m_nDefaultFilter = m_FilterArray.size() - 1;
}

void wxGxObjectDialog::RemoveAllFilters(void)
{    
    if(m_bOwnFilter)
    {
        for(size_t i = 0; i < m_FilterArray.GetCount(); ++i)
            wxDELETE(m_FilterArray[i]);
        m_FilterArray.Clear();
    }
}

void wxGxObjectDialog::OnFilterSelect(wxCommandEvent& event)
{
    m_nDefaultFilter = m_WildcardCombo->GetCurrentSelection();
    m_pwxGxContentView->SetCurrentFilter(m_nDefaultFilter);
    m_pwxGxContentView->RefreshAll();
}

void wxGxObjectDialog::OnOK(wxCommandEvent& event)
{
    m_nRetCode = wxID_OK;
    if ( Validate() && TransferDataFromWindow() )
    {
        int nPos = m_WildcardCombo->GetCurrentSelection();

        wxGxSelection* pSel = m_pwxGxContentView->GetSelectedObjects();
        if(NULL == pSel)
            return;
        long nSelID = pSel->GetFirstSelectedObjectId();
		wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(nSelID);

        //fill out data
        if(m_bIsSaveDlg)
        {
            if(pGxObject && pGxObject->IsKindOf(wxCLASSINFO(wxGxObjectContainer)))
            {
                GetGxSelection()->Select(nSelID, false, wxGxSelection::INIT_ALL);
                return;
            }

            if(nPos != m_FilterArray.GetCount())
            {
                wxGISEnumSaveObjectResults Result = m_FilterArray[nPos]->CanSaveObject(GetLocation(), GetName());
                if(!DoSaveObject(Result))
                    return;
            }
            else
            {
                for(size_t j = 0; j < m_FilterArray.GetCount(); ++j)
                {
                    wxGISEnumSaveObjectResults Result = m_FilterArray[j]->CanSaveObject(GetLocation(), GetName());
                    if(DoSaveObject(Result))
                        break;
                }
            }
        }
        else
        {
            if (pGxObject && pGxObject->IsKindOf(wxCLASSINFO(wxGxObjectContainer)) && !m_FilterArray[nPos]->CanChooseObject(pGxObject))
            {
                GetGxSelection()->Select(nSelID, false, wxGxSelection::INIT_ALL);
                return;
            }

            for (size_t i = 0; i < pSel->GetCount(); ++i)
            {
				wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pSel->GetSelectedObjectId(i));
                if(nPos == m_FilterArray.GetCount())
                {
                    for(size_t j = 0; j < m_FilterArray.size(); ++j)
                    {
                        bool bCanChooseObject = false;
                        if(m_FilterArray[j]->CanChooseObject(pGxObject))
                        {
                            m_ObjectList.Append(pGxObject);
                            break;
                        }
                        else
                        {
                            if(pGxObject->IsKindOf(wxCLASSINFO(wxGxObjectContainer)))
                            {
                                GetGxSelection()->Select(pGxObject->GetId(), false, wxGxSelection::INIT_ALL);
                                return;
                            }
                        }
                    }
                }
                else
                {
                    if(m_FilterArray[nPos]->CanChooseObject(pGxObject))
                    {
                        m_ObjectList.Append(pGxObject);
                    }
                    else
                    {
                        if(pGxObject->IsKindOf(wxCLASSINFO(wxGxObjectContainer)))
                        {
                            GetGxSelection()->Select(pGxObject->GetId(), false, wxGxSelection::INIT_ALL);
                            return;
                        }
                    }
                }
            }

            if(GetChildren().IsEmpty())
            {
                wxMessageBox(_("Cannot choose item(s)"), _("Error"), wxICON_ERROR | wxOK, this);
                return;
            }
        }

        wxTreeItemId ItemId = m_PopupCtrl->GetSelection();
        if(ItemId.IsOk())
        {
            wxGxTreeItemData* pData = (wxGxTreeItemData*)m_PopupCtrl->GetItemData(ItemId);
            if(pData != NULL)
            {
		        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
				wxGISAppConfig oConfig = GetConfig();
                if (pGxObject && oConfig.IsOk())
                {
					oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/lastpath/path")), pGxObject->GetFullName());
                }
            }
        }

        if ( IsModal() )
            EndModal(m_nRetCode);
        else
        {
            SetReturnCode(m_nRetCode);
            this->Show(false);
        }
    }
}

void wxGxObjectDialog::SerializeFramePos(bool bSave)
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

void wxGxObjectDialog::OnOKUI(wxUpdateUIEvent& event)
{
    if(m_NameTextCtrl->GetValue().IsEmpty())
    {
        event.Enable(false);
        return;
    }
    if(m_bIsSaveDlg)
    {
        wxGxObjectFilter* pFilter = GetCurrentFilter();
        if(pFilter)
        {
            wxGISEnumSaveObjectResults Result = pFilter->CanSaveObject(GetLocation(), GetName());

            if(enumGISSaveObjectDeny == Result)
            {
                event.Enable(false);
                return;
            }
        }
    }
    event.Enable(true);
}

wxString wxGxObjectDialog::GetName(void) const
{
	if(m_bIsSaveDlg)
	{
		wxString sName = m_NameTextCtrl->GetValue();
        wxFileName FileName(sName);
        FileName.SetEmptyExt();

		wxGxObjectFilter* pFilter = GetCurrentFilter();
		if(NULL != pFilter)
		{
			wxString sExt;
			sExt = pFilter->GetExt();
			if(!sExt.IsEmpty())
                FileName.SetExt(sExt);
		}
		return FileName.GetFullName();
	}
	else
	{
        if(GetChildren().GetCount() > 0)
		{
            wxGxObjectList::compatibility_iterator node = GetChildren().GetFirst();
            if(!node)
                return wxEmptyString; 
            wxGxObject *current = node->GetData();
            return current->GetName();            
		}
	}
	return wxEmptyString;
}

wxString wxGxObjectDialog::GetFullName(void) const
{
    wxGxObject* pObj = GetLocation();
    if(NULL == pObj)
        return wxEmptyString;

    wxString sPath = pObj->GetFullName();
    wxString sFPath;
    if(sPath.EndsWith(wxFileName::GetPathSeparator()))
        sFPath = sPath + GetName();
    else
        sFPath = sPath + wxFileName::GetPathSeparator() + GetName();
    return sFPath;
}

CPLString wxGxObjectDialog::GetPath(void) const
{
    wxGxObject* pObj = GetLocation();
    if (NULL == pObj)
        return CPLString();

    return pObj->GetPath();
}

wxGxObject* const wxGxObjectDialog::GetLocation(void) const
{
    return m_pwxGxContentView->GetParentGxObject();
}

bool wxGxObjectDialog::DoSaveObject(wxGISEnumSaveObjectResults Result)
{
    if(Result == enumGISSaveObjectDeny)
    {
        wxMessageBox(_("Cannot save item!"), _("Error"), wxICON_ERROR | wxOK, this);
        return false;
    }
    else if(Result == enumGISSaveObjectExists && m_bOverwritePrompt)
    {
        int nRes = wxMessageBox(_("Item exists! Overwrite?"), _("Question"), wxICON_QUESTION | wxYES_NO | wxCANCEL, this);
        if(nRes == wxCANCEL)
            return false;
        else if(nRes == wxNO)
            m_nRetCode = wxID_CANCEL;
    }
    return true;
}

wxGxObjectFilter* wxGxObjectDialog::GetCurrentFilter(void) const
{
    if(m_FilterArray.IsEmpty())
        return NULL;
    return m_FilterArray[m_nDefaultFilter];
}

size_t wxGxObjectDialog::GetCurrentFilterId(void) const
{
    return m_nDefaultFilter;
}

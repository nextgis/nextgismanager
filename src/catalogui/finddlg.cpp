/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISPropertyDlg class - dialog/dock window with the GxObject properties.
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
#include "wxgis/catalogui/finddlg.h"


#include "wxgis/catalogui/gxapplication.h"
#include "wxgis/catalog/gxcatalog.h"

#include <wx/statline.h>
#include <wx/valgen.h>
#include <wx/valtext.h>

#include "../../art/system_search.xpm"
#include "../../art/document_16.xpm"

//-------------------------------------------------------------------
// wxGISSelectSearchScopeComboPopup
//-------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISSelectSearchScopeComboPopup, wxTreeViewComboPopup)

void wxGISSelectSearchScopeComboPopup::OnSelectionChanged(wxGxSelectionEvent& event)
{
    return;
}

bool wxGISSelectSearchScopeComboPopup::Create(wxWindow* parent)
{
    return wxGISSelectSearchScopeComboPopup::Create(parent, TREECTRLID);
}

bool wxGISSelectSearchScopeComboPopup::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_nSelObject = wxNOT_FOUND;
    m_bClicked = false;
	int nOSMajorVer(0);
    wxGetOsVersion(&nOSMajorVer);    
    return wxGxTreeViewBase::Create(parent, TREECTRLID, pos, size, wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS | wxBORDER_SIMPLE | wxTR_SINGLE | wxTR_EDIT_LABELS | (nOSMajorVer > 5 ? wxTR_NO_LINES : 0), name);
}

bool wxGISSelectSearchScopeComboPopup::CanChooseObject(wxGxObject* pGxObject)
{
    if (NULL == pGxObject)
        return false;

    wxGxSearchObjectFilter fil;
    if (fil.CanChooseObject(pGxObject))
        return true;
    return false;
}


void wxGISSelectSearchScopeComboPopup::AddTreeItem(wxGxObject* pGxObject, wxTreeItemId hParent)
{
    if (NULL == pGxObject)
        return;

    wxGxSearchObjectFilter fil;
    if (fil.CanDisplayObject(pGxObject))
       wxGxTreeViewBase::AddTreeItem(pGxObject, hParent);
}

void wxGISSelectSearchScopeComboPopup::OnMouseMove(wxMouseEvent& event)
{
    event.Skip(true);
}

void wxGISSelectSearchScopeComboPopup::OnPopup()
{
	m_bClicked = false;
	wxGISAppConfig oConfig = GetConfig();	
	wxString sLastScope(_("Catalog"));
	if(oConfig.IsOk())
    {
		wxString sAppName = GetApplication()->GetAppName();
		sLastScope = oConfig.Read(enumGISHKCU, sAppName + wxString(wxT("/find/scope/last_path")), sLastScope);
    }   
	
	wxGxObject* pGxObject = m_pCatalog->FindGxObject(sLastScope);
	if(pGxObject)
	{
		wxTreeItemId ItemId = m_TreeMap[pGxObject->GetId()];
		if(ItemId.IsOk())
		{
			SelectItem(ItemId);
		}
		else
		{
			wxGxObject* pParentGxObj = pGxObject->GetParent();
			unsigned char nCounter = 0;
			while(pParentGxObj && nCounter < 255)
			{
				ItemId = m_TreeMap[pParentGxObj->GetId()];
				if(ItemId.IsOk())
				{
					Expand(ItemId);
					ItemId = m_TreeMap[pGxObject->GetId()];
					if(ItemId.IsOk())
						break;
					else
						pParentGxObj = pGxObject->GetParent();
				}
				else
					pParentGxObj = pParentGxObj->GetParent();
				nCounter++;
			}
			SelectItem(ItemId);
		}
	}
}

void wxGISSelectSearchScopeComboPopup::OnMouseClick(wxMouseEvent& event)
{
    m_bClicked = true;
	
	int flags;
    wxTreeItemId item = wxTreeCtrl::HitTest(event.GetPosition(), flags);
	//wxTreeItemId item = event.GetItem();
	if(!item.IsOk() || !(flags & wxTREE_HITTEST_ONITEM))
	{
		event.Skip(true);
		return;		
	}
		
	wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(item);
	if(pData != NULL)
	{
		SelectItem(item);
		m_PrewItemId = item;
		wxGISAppConfig oConfig = GetConfig();	
		if(oConfig.IsOk())
		{
			wxString sAppName = GetApplication()->GetAppName();
			wxGxObject* pObj = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
			if(pObj)
				oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/find/scope/last_path")), pObj->GetFullName());
		} 
		
		m_nSelObject = pData->m_nObjectID;
	}

    Dismiss();

    event.Skip(true);
}

void wxGISSelectSearchScopeComboPopup::OnDblClick(wxTreeEvent& event)
{
	
	int flags;
    wxTreeItemId item = wxTreeCtrl::HitTest(event.GetPoint(), flags);
	//wxTreeItemId item = event.GetItem();
	if(!item.IsOk() || !(flags & wxTREE_HITTEST_ONITEM))
	{
		event.Veto();
		return;		
	}
	
	wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(item);
	if(pData != NULL)
	{
		SelectItem(item);
		m_PrewItemId = item;
		wxGISAppConfig oConfig = GetConfig();	
		if(oConfig.IsOk())
		{
			wxString sAppName = GetApplication()->GetAppName();
			wxGxObject* pObj = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
			if(pObj)
				oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/find/scope/last_path")), pObj->GetFullName());
		} 
	} 

    wxComboPopup::Dismiss();
}

wxString wxGISSelectSearchScopeComboPopup::GetStringValue() const
{
    wxTreeItemId ItemId = wxTreeCtrl::GetSelection();
    if(ItemId.IsOk())
        return GetItemText(ItemId);
    return wxEmptyString;
}

void wxGISSelectSearchScopeComboPopup::SetSelectedObject(wxGxObject* const pGxObject)
{
    m_nSelObject = pGxObject->GetId();
}

bool wxGISSelectSearchScopeComboPopup::CanSearch()
{
    wxGxSearchObjectFilter fil;
	wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(m_nSelObject);
	if(pGxObject)
		return fil.CanChooseObject(pGxObject);
	else
		return false;
}

long wxGISSelectSearchScopeComboPopup::GetSelectedObjectId()
{
	return m_nSelObject;	
}

//-------------------------------------------------------------------
// wxGISFindResultItemPanel
//-------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGISFindResultItemPanel, wxPanel)
    EVT_MOTION(wxGISFindResultItemPanel::OnMouseMove)
    EVT_LEFT_UP(wxGISFindResultItemPanel::OnLeftUp)
END_EVENT_TABLE();

wxGISFindResultItemPanel::wxGISFindResultItemPanel(wxGxObject* const pObject, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxPanel(parent, id, pos, size, style)
{    
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));
    m_nSelObject = pObject->GetId();

    if (pObject)
    {
        wxBoxSizer* bParentMainSizer = new wxBoxSizer(wxVERTICAL);
        wxBoxSizer* bMainSizer = new wxBoxSizer(wxHORIZONTAL);
        IGxObjectUI* pObjUI = dynamic_cast<IGxObjectUI*>(pObject);
        wxStaticBitmap* pStateBitmap = NULL;
        if (pObjUI)
        {
            pStateBitmap = new wxStaticBitmap(this, wxID_ANY, pObjUI->GetSmallImage(), wxDefaultPosition, wxDefaultSize, 0);
        }
        else
        {
            pStateBitmap = new wxStaticBitmap(this, wxID_ANY, wxBitmap(document_16_xpm), wxDefaultPosition, wxDefaultSize, 0);
        }
        bMainSizer->Add(pStateBitmap, 0, wxALL, 5);

        wxBoxSizer* bDetailesSizer = new wxBoxSizer(wxVERTICAL);
        //add bold name and category
        wxGenericStaticText *title = new wxGenericStaticText(this, wxID_ANY, pObject->GetName() + wxT(" (") + pObject->GetCategory() + wxT(")"), wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);

        wxFont titleFont = this->GetFont();
        titleFont.SetWeight(wxFONTWEIGHT_BOLD);
        title->SetFont(titleFont);
        //title->Wrap(-1);
        title->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));
        bDetailesSizer->Add(title, 0, wxALL, 1);
        //add full name
        m_pPath = new wxGenericStaticText(this, ID_PATHTO, pObject->GetFullName(), wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
        
        wxFont Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
        Font.SetUnderlined(true);
        m_pPath->SetFont(Font);
        m_pPath->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
        m_pPath->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));
        bDetailesSizer->Add(m_pPath, 0, wxALL, 1);
        //add size, mod date, count childrent, etc.
        IGxDataset* pDataset = dynamic_cast<IGxDataset*>(pObject);
        if (pDataset)
        {
            pDataset->FillMetadata();
            pDataset->GetSize();
            wxString sDate;
            wxDateTime dt = pDataset->GetModificationDate();
            if (dt.IsValid())
                sDate = dt.Format();

            wxString sText = wxString(_("Size")) + wxT(": ") + wxFileName::GetHumanReadableSize(pDataset->GetSize());
            if (!sDate.IsEmpty())
                sText += wxT("; ") + wxString(_("Modification date")) + wxT(": ") + sDate;
            wxGenericStaticText *adds = new wxGenericStaticText(this, wxID_ANY, sText, wxDefaultPosition, wxDefaultSize, wxST_ELLIPSIZE_END);
            adds->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_LISTBOX));
            bDetailesSizer->Add(adds, 0, wxALL, 1);
        }

        bMainSizer->Add(bDetailesSizer, 1, wxALL, 5);

        bParentMainSizer->Add(bMainSizer, 0, wxALL, 5);
        wxStaticLine *staticline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
        staticline->SetBackgroundColour(GetBackgroundColour());
        //staticline->GetForegroundColour
        bParentMainSizer->Add(staticline, 0, wxEXPAND, 5);

        this->SetSizer(bParentMainSizer);
        this->Layout();

        //add event handler for m_pPath
        m_pPath->Bind(wxEVT_MOTION, &wxGISFindResultItemPanel::OnMouseMove, this);
        m_pPath->Bind(wxEVT_LEFT_UP, &wxGISFindResultItemPanel::OnLeftUp, this);
    }
}

wxGISFindResultItemPanel::~wxGISFindResultItemPanel(void)
{

}

void wxGISFindResultItemPanel::OnMouseMove(wxMouseEvent& event)
{
    //event.Skip(true);

    wxRect rect = m_pPath->GetScreenRect();
    //rect.Inflate(2, 2);
    wxPoint pt;
    if (event.GetId() == m_pPath->GetId())
        pt = m_pPath->ClientToScreen(event.GetPosition());
    else if (event.GetId() == m_pPath->GetId())
        pt = ClientToScreen(event.GetPosition());

    if (rect.Contains(pt))
    {
        SetCursor(wxCursor(wxCURSOR_HAND));
    }
    else
    {
        SetCursor(wxCursor(wxCURSOR_ARROW));
    }
}

void wxGISFindResultItemPanel::OnLeftUp(wxMouseEvent& event)
{
    //event.Skip(true);

    wxRect rect = m_pPath->GetScreenRect();
    //rect.Inflate(3, 3);
    wxPoint pt;
    if (event.GetId() == m_pPath->GetId())
        pt = m_pPath->ClientToScreen(event.GetPosition());
    else if (event.GetId() == m_pPath->GetId())
        pt = ClientToScreen(event.GetPosition());

    if (rect.Contains(pt))
    {
        wxGxApplication* pApp = dynamic_cast<wxGxApplication*>(GetApplication());
        if (pApp && pApp->GetGxSelection())
            pApp->GetGxSelection()->Select(m_nSelObject, false, wxGxSelection::INIT_ALL);
    }
}

//-------------------------------------------------------------------
// wxGISFindResultsView
//-------------------------------------------------------------------

wxGISFindResultsView::wxGISFindResultsView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxScrolledWindow(parent, id, pos, size, style)
{
    m_bMainSizer = new wxBoxSizer(wxVERTICAL);

    SetScrollbars(20, 20, 50, 50);

    this->SetSizer(m_bMainSizer);
    this->Layout();
}

wxGISFindResultsView::~wxGISFindResultsView(void)
{
}

void wxGISFindResultsView::AddPanel(wxGISFindResultItemPanel* const pResultPanel)
{
    m_bMainSizer->Add(pResultPanel, 0, wxEXPAND, 5);
    Layout();
    FitInside();
}

void wxGISFindResultsView::RemovePanel(wxGISFindResultItemPanel* const pResultPanel)
{
    m_bMainSizer->Detach(pResultPanel);
    Layout();
    FitInside();
}

void wxGISFindResultsView::InsertPanel(wxGISFindResultItemPanel* const pResultPanel, long nPos)
{
    m_bMainSizer->Insert(nPos, pResultPanel, 0, wxEXPAND, 5);
    Layout();
    FitInside();
}

//-------------------------------------------------------------------
// wxGISFindDlg
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxGISFindDlg, wxPanel)

BEGIN_EVENT_TABLE(wxGISFindDlg, wxPanel)
    EVT_BUTTON(wxID_FIND, wxGISFindDlg::OnFind)
    EVT_UPDATE_UI(wxID_FIND, wxGISFindDlg::OnFindUI)
END_EVENT_TABLE()


wxGISFindDlg::wxGISFindDlg( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : ITrackCancel()
{
	m_PopupCtrl = NULL;
	Create(parent, id, pos, size, style);
}

wxGISFindDlg::wxGISFindDlg(void) : ITrackCancel()
{
	m_PopupCtrl = NULL;
}

bool wxGISFindDlg::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if(!wxPanel::Create( parent, id, pos, size, style, name ))
		return false;
		
	wxGISAppConfig oConfig = GetConfig();
	wxString sLastScope(_("Catalog"));
	if(oConfig.IsOk())
    {
		wxString sAppName = GetApplication()->GetAppName();
		sLastScope = oConfig.Read(enumGISHKCU, sAppName + wxString(wxT("/find/scope/last_path")), sLastScope);
    }

	m_bMainSizer = new wxBoxSizer( wxVERTICAL );
    wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(2, 2, 0, 0);
    fgSizer1->AddGrowableCol(1);
    fgSizer1->SetFlexibleDirection(wxBOTH);
    fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);


	wxStaticText *staticText2 = new wxStaticText( this, wxID_ANY, _("Scope:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticText2->Wrap( -1 );
    fgSizer1->Add(staticText2, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 5);

    //add scope control
    wxComboCtrl* pTreeCombo = new wxComboCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCB_READONLY);
#ifdef __WXMSW__
    pTreeCombo->UseAltPopupWindow(true);
#else
    pTreeCombo->UseAltPopupWindow(false);
#endif
    m_PopupCtrl = new wxGISSelectSearchScopeComboPopup();
    pTreeCombo->SetPopupControl(m_PopupCtrl);
    pTreeCombo->EnablePopupAnimation(true);
    //m_PopupCtrl->Connect( wxEVT_LEFT_UP, wxMouseEventHandler( wxTreeViewComboPopup::OnMouseClick ), NULL, m_PopupCtrl );
    m_PopupCtrl->Activate(GetApplication(), NULL);//TODO:
    wxGxCatalogBase* pCat = GetGxCatalog();
    if (pCat)
    {
        wxGxObject* pObj = pCat->FindGxObject(sLastScope);
        if (pObj)
        {
            pTreeCombo->SetText(pObj->GetName());
            m_PopupCtrl->SetSelectedObject(pObj);
        }
    }
    fgSizer1->Add(pTreeCombo, 1, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 5);

    //TODO:  	wxSearchCtrl 
    //TODO: search when hit enter
	wxStaticText* staticText1 = new wxStaticText(this, wxID_ANY, _("Find:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticText1->Wrap( -1 );
    fgSizer1->Add(staticText1, 0, wxALL | wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT, 5);

    wxBoxSizer *bFindSizer = new wxBoxSizer(wxHORIZONTAL);
	m_pFindCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sFind) );
    bFindSizer->Add(m_pFindCtrl, 1, wxALL | wxEXPAND, 5);

    wxBitmapButton* bpFind = new wxBitmapButton(this, wxID_FIND, system_search_xpm, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW);
    bFindSizer->Add(bpFind, 0, wxALL, 5);

    fgSizer1->Add(bFindSizer, 1, wxALL | wxEXPAND | wxALIGN_CENTER_VERTICAL, 0);
    m_bMainSizer->Add(fgSizer1, 0, wxALL | wxEXPAND, 5);

	wxStaticLine *staticline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    m_bMainSizer->Add(staticline, 0, wxALL | wxEXPAND, 5);
	
    pFindResultsView = new wxGISFindResultsView(this);
#ifdef TEST_SEARCHPANEL
    for (size_t i = 0; i < 12; ++i)
    {
        wxGxObject* pOOBSD = GetGxCatalog()->FindGxObjectByPath("D:\\work\\testgeodata\\shp\\burned-areas_2011.shp");
        wxGISFindResultItemPanel* pTest1 = new wxGISFindResultItemPanel(pOOBSD, pFindResultsView);
        pFindResultsView->AddPanel(pTest1);
    }
#endif
	m_bMainSizer->Add( pFindResultsView, 1, wxALL | wxEXPAND, 5 );

	this->SetSizer( m_bMainSizer );
	this->Layout();

    return true;
}

wxGISFindDlg::~wxGISFindDlg()
{
    if (m_PopupCtrl)
        m_PopupCtrl->Deactivate();
}
 
void wxGISFindDlg::OnFind(wxCommandEvent& event)
{
	wxBusyCursor wait;
	wxGxCatalogBase* pCat = GetGxCatalog();
	IGxSearchObject* pSearchObject = dynamic_cast<IGxSearchObject*>(pCat->GetRegisterObject(m_PopupCtrl->GetSelectedObjectId()));
	if(pSearchObject)
	{		
		const wxGxObjectList pSearchResult = pSearchObject->SimpleSearch(m_pFindCtrl->GetValue(), this);
		wxGxObjectList::const_iterator iter;
		for (iter = pSearchResult.begin(); iter != pSearchResult.end(); ++iter)
		{
			wxGxObject *current = *iter;
			wxGISFindResultItemPanel* pResultPanel = new wxGISFindResultItemPanel(current, pFindResultsView);
			pFindResultsView->AddPanel(pResultPanel);
		}		
	}
}

void wxGISFindDlg::OnFindUI(wxUpdateUIEvent& event)
{
    if (m_PopupCtrl && m_PopupCtrl->CanSearch() && m_pFindCtrl && !m_pFindCtrl->GetValue().IsEmpty())
        event.Enable(true);
	else	
		event.Enable(false);
}

//-------------------------------------------------------------------
// wxAxFindView
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxAxFindView, wxGISFindDlg)

BEGIN_EVENT_TABLE(wxAxFindView, wxGISFindDlg)
END_EVENT_TABLE()

wxAxFindView::wxAxFindView(void)
{
}

wxAxFindView::wxAxFindView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISFindDlg(parent, id, pos, size, wxNO_BORDER | wxTAB_TRAVERSAL)
{
    Create(parent, id, pos, size);
}

wxAxFindView::~wxAxFindView(void)
{
}

bool wxAxFindView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_sViewName = wxString(_("Search"));
    return wxGISFindDlg::Create(parent, id, pos, size, style, name);
}

bool wxAxFindView::Activate(IApplication* const pApplication, wxXmlNode* const pConf)
{	
    m_pApp = dynamic_cast<wxGISApplicationBase*>(pApplication);
    if(NULL == m_pApp)
        return false;
		
	return true;
}

void wxAxFindView::Deactivate(void)
{
    if (m_PopupCtrl)
        m_PopupCtrl->Deactivate();
}

IProgressor* const wxAxFindView::GetProgressor(void)
{
	if(m_pApp)
	{
		wxGISStatusBar* pStatusBar = m_pApp->GetStatusBar();
		if(pStatusBar)
			return pStatusBar->GetProgressor();
	}
	return NULL;
}

void wxAxFindView::PutMessage(const wxString &sMessage, size_t nIndex, wxGISEnumMessageType eType)
{
	if(m_pApp)
	{
		wxGISStatusBar* pStatusBar = m_pApp->GetStatusBar();
		if(pStatusBar)
			pStatusBar->SetMessage(sMessage);
	}	
}

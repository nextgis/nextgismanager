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

//-------------------------------------------------------------------
// wxGISSelectSearchScopeComboPopup
//-------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISSelectSearchScopeComboPopup, wxTreeViewComboPopup)

void wxGISSelectSearchScopeComboPopup::OnSelectionChanged(wxGxSelectionEvent& event)
{

}

bool wxGISSelectSearchScopeComboPopup::Create(wxWindow* parent)
{
    return wxGISSelectSearchScopeComboPopup::Create(parent, TREECTRLID);
}

bool wxGISSelectSearchScopeComboPopup::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_bClicked = false;
	int nOSMajorVer(0);
    wxGetOsVersion(&nOSMajorVer);
    return wxGxTreeViewBase::Create(parent, TREECTRLID, pos, size, wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS | wxBORDER_SIMPLE | wxTR_SINGLE | wxTR_EDIT_LABELS | (nOSMajorVer > 5 ? wxTR_NO_LINES : 0), name);
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
/*    if( m_bClicked == false )
    {
        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(m_pSelection->GetLastSelectedObjectId());
        if(pGxObject)
            return pGxObject->GetName();
        return wxEmptyString;
    }
*/
    wxTreeItemId ItemId = wxTreeCtrl::GetSelection();
    if(ItemId.IsOk())
        return GetItemText(ItemId);
    return wxEmptyString;
}

//-------------------------------------------------------------------
// wxGISFindDlg
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxGISFindDlg, wxPanel)

BEGIN_EVENT_TABLE(wxGISFindDlg, wxPanel)
    EVT_BUTTON(wxID_OK, wxGISFindDlg::OnFind)
	EVT_UPDATE_UI(wxID_OK, wxGISFindDlg::OnFindUI)
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

	wxStaticText* staticText1 = new wxStaticText(this, wxID_ANY, _("Find:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticText1->Wrap( -1 );
	m_bMainSizer->Add( staticText1, 0, wxALL|wxLEFT, 5 );
	//add search text ctrl
	m_pFindCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_sFind) );
	m_bMainSizer->Add( m_pFindCtrl, 0, wxALL|wxEXPAND, 5 );
	
	wxStaticText *staticText2 = new wxStaticText( this, wxID_ANY, _("Scope:"), wxDefaultPosition, wxDefaultSize, 0 );
	staticText2->Wrap( -1 );
	m_bMainSizer->Add( staticText2, 0, wxALL|wxLEFT, 5 );
	//add scope control
	wxComboCtrl* pTreeCombo = new wxComboCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxCB_READONLY);
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
	if(pCat)
	{
		wxGxObject* pObj = pCat->FindGxObject(sLastScope);
		if(pObj)
			pTreeCombo->SetText(pObj->GetName());		
	}

	m_bMainSizer->Add( pTreeCombo, 0, wxALL | wxEXPAND, 5 );	
	
	wxStaticLine *staticline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    m_bMainSizer->Add(staticline, 0, wxALL | wxEXPAND, 5);
	
	wxStdDialogButtonSizer* sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerFind = new wxButton(this, wxID_OK, _("Find"));
	sdbSizer->AddButton(m_sdbSizerFind);
    sdbSizer->Realize();
    m_bMainSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, 5);	
	
	staticline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    m_bMainSizer->Add(staticline, 0, wxALL | wxEXPAND, 5);
	
	wxPanel* pPanel = new wxPanel(this);
	m_bMainSizer->Add( pPanel, 1, wxALL | wxEXPAND, 5 );


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
}

void wxGISFindDlg::OnFindUI(wxUpdateUIEvent& event)
{
	if(!m_pFindCtrl || m_pFindCtrl->GetValue().IsEmpty())
		event.Enable(false);
	else	
		event.Enable(true);
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

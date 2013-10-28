/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxLocationComboBox class
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
#include "wxgis/catalogui/gxlocationcombobox.h"
//#include "wxgis/catalogui/gxcatalogui.h"

#include <wx/tipwin.h>

#include "../../art/document_16.xpm"

#ifdef __WXMSW__
    #define wxGIS_LIST_STATE_DROPHILITED wxLIST_STATE_DROPHILITED
#else
    #define wxGIS_LIST_STATE_DROPHILITED wxLIST_STATE_SELECTED
#endif

//---------------------------------------------------------------------------------
// wxGxPathsListView
//---------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxPathsListView, wxListCtrl)

BEGIN_EVENT_TABLE(wxGxPathsListView, wxListCtrl)
    //TODO: EVT_LEFT_UP(wxListViewComboPopup::OnMouseClick)
    EVT_LEFT_DOWN(wxGxPathsListView::OnMouseClick)
    EVT_MOTION(wxGxPathsListView::OnMouseMove)
    EVT_CHAR(wxGxPathsListView::OnChar)
END_EVENT_TABLE()

wxGxPathsListView::wxGxPathsListView(void) : wxListCtrl()
{
    m_HighLightItem = wxNOT_FOUND;
}

wxGxPathsListView::wxGxPathsListView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxListCtrl(parent, id, pos, size, style)
{
    m_HighLightItem = wxNOT_FOUND;
    Activate();
}

wxGxPathsListView::~wxGxPathsListView(void)
{
    for(long i = 0; i < GetItemCount(); ++i)
    {
		delete (LPITEMDATA)GetItemData(i);
    }
}

bool wxGxPathsListView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    bool bRes = wxListCtrl::Create(parent, id, pos, size, style, wxDefaultValidator, name);
    Activate();
    return bRes;
}

void wxGxPathsListView::Activate()
{
    m_ImageListSmall.Create(16, 16);
    m_ImageListSmall.Add(wxBitmap(document_16_xpm));
    SetImageList(&m_ImageListSmall, wxIMAGE_LIST_SMALL);

    InsertColumn(0, _("Name"),	wxLIST_FORMAT_LEFT);
}


void wxGxPathsListView::OnMouseMove(wxMouseEvent& event)
{
    SetItemState(m_HighLightItem, 0, wxGIS_LIST_STATE_DROPHILITED);
    wxPoint pt = event.GetPosition();
	unsigned long nFlags(0);
	long nItemId = HitTest(pt, (int &)nFlags);
	if(nItemId != wxNOT_FOUND && (nFlags & wxLIST_HITTEST_ONITEM))
	{
 //       wxSize sz = GetClientSize();
 //       if(DNDSCROLL > y)//scroll up
 //           ScrollLines(-1);
 //       else if((sz.GetHeight() - DNDSCROLL) < y)//scroll down
 //           ScrollLines(1);

        m_HighLightItem = nItemId;
        SetItemState(m_HighLightItem, wxGIS_LIST_STATE_DROPHILITED, wxGIS_LIST_STATE_DROPHILITED);

    }
    event.Skip();
}

void wxGxPathsListView::OnMouseClick(wxMouseEvent& event)
{
    SetItemState(m_HighLightItem, 0, wxGIS_LIST_STATE_DROPHILITED);
    wxPoint pt = event.GetPosition();
	unsigned long nFlags(0);
	long nItemId = HitTest(pt, (int &)nFlags);
	if(nItemId != wxNOT_FOUND && (nFlags & wxLIST_HITTEST_ONITEM))
	{
        SetItemState(nItemId, wxGIS_LIST_STATE_DROPHILITED, wxGIS_LIST_STATE_DROPHILITED);
    }
    event.Skip();
}

void wxGxPathsListView::Append(const wxString& sFullName)
{
    wxGxCatalogBase* pCat = GetGxCatalog();
    if(!pCat)
        return;
    wxGxObject *pObject = pCat->FindGxObject(sFullName);
    IGxObjectUI* pObjUI =  dynamic_cast<IGxObjectUI*>(pObject);
    if(!pObjUI)
        return;

	wxIcon icon_small = pObjUI->GetSmallImage();
	int pos = GetIconPos(icon_small);

	LPITEMDATA pData = new _itemdata;
	pData->nObjectID = pObject->GetId();
	pData->iImageIndex = pos;

    wxString sName = pObject->GetFullName();

	long ListItemID = InsertItem(0, sName, pos);
	SetItemPtrData(ListItemID, (wxUIntPtr) pData);
}

int wxGxPathsListView::GetIconPos(const wxIcon &icon_small)
{
	int pos(wxNOT_FOUND);
	if(icon_small.IsOk())
	{
		for(size_t i = 0; i < m_IconsArray.size(); ++i)
		{
			if(m_IconsArray[i].oIcon.IsSameAs(icon_small))
			{
				pos = m_IconsArray[i].iImageIndex;
				break;
			}
		}
		if(pos == wxNOT_FOUND)
		{
			pos = m_ImageListSmall.Add(icon_small);
			ICONDATA myicondata = {icon_small, pos};
			m_IconsArray.push_back(myicondata);
		}
	}
	else
		pos = 0;
    return pos;
}

void wxGxPathsListView::OnChar(wxKeyEvent& event)
{
    event.Skip();
	if(event.GetModifiers() & wxMOD_ALT)
		return;
	if(event.GetModifiers() & wxMOD_CONTROL)
		return;
	if(event.GetModifiers() & wxMOD_SHIFT)
		return;
    switch(event.GetKeyCode())
    {
    case WXK_UP:
        {
            long nSelItem = GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL, wxGIS_LIST_STATE_DROPHILITED);
            if(nSelItem == wxNOT_FOUND)
            {
                m_HighLightItem = 0;
                SetItemState(m_HighLightItem, wxGIS_LIST_STATE_DROPHILITED, wxGIS_LIST_STATE_DROPHILITED);
            }
            else
            {
                SetItemState(nSelItem, 0, wxGIS_LIST_STATE_DROPHILITED);
                if(nSelItem == 0)
                {
                    m_HighLightItem = GetItemCount() - 1;
                    SetItemState(m_HighLightItem, wxGIS_LIST_STATE_DROPHILITED, wxGIS_LIST_STATE_DROPHILITED);
                }
                else
                {
                    m_HighLightItem = nSelItem - 1;
                    SetItemState(m_HighLightItem, wxGIS_LIST_STATE_DROPHILITED, wxGIS_LIST_STATE_DROPHILITED);
                }
            }
        }
        event.Skip(false);
        break;
    case WXK_DOWN:
        {
            long nSelItem = GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL, wxGIS_LIST_STATE_DROPHILITED);
            if(nSelItem == wxNOT_FOUND)
            {
                m_HighLightItem = 0;
                SetItemState(m_HighLightItem, wxGIS_LIST_STATE_DROPHILITED, wxGIS_LIST_STATE_DROPHILITED);
            }
            else
            {
                SetItemState(nSelItem, 0, wxGIS_LIST_STATE_DROPHILITED);
                if(nSelItem == GetItemCount() - 1)
                {
                    m_HighLightItem = 0;
                    SetItemState(m_HighLightItem, wxGIS_LIST_STATE_DROPHILITED, wxGIS_LIST_STATE_DROPHILITED);
                }
                else
                {
                    m_HighLightItem = nSelItem + 1;
                    SetItemState(m_HighLightItem, wxGIS_LIST_STATE_DROPHILITED, wxGIS_LIST_STATE_DROPHILITED);
                }
            }
        }
        event.Skip(false);
        break;
    default:
        event.Skip(true);
        break;
    }
}

//---------------------------------------------------------------------------------
// wxListViewComboPopup
//---------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxListViewComboPopup, wxGxPathsListView)

bool wxListViewComboPopup::Create(wxWindow* parent)
{
    return wxListViewComboPopup::Create(parent, LISTVIEWPOPUPID);
}

bool wxListViewComboPopup::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    return wxGxPathsListView::Create(parent, LISTVIEWPOPUPID, pos, size, LISTPOPUSTYLE, name);
}

void wxListViewComboPopup::Init()
{
}

void wxListViewComboPopup::OnPopup()
{
    m_sVal = GetComboCtrl()->GetValue();
    long nItem = wxNOT_FOUND;
    for ( ;; )
    {
        nItem = GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( nItem == wxNOT_FOUND )
            break;
        SetItemState(nItem, 0, wxLIST_STATE_SELECTED);
    }
}

void wxListViewComboPopup::OnDismiss()
{
}

wxSize wxListViewComboPopup::GetAdjustedSize(int minWidth, int prefHeight, int maxHeight)
{
    SetColumnWidth(0, minWidth - 2);
    return wxSize(minWidth, -1);
}

void wxListViewComboPopup::SetStringValue(const wxString& s)
{
    //int n = wxListView::FindItem(-1,s);
    //if ( n >= 0 && n < wxListView::GetItemCount() )
    //    wxListView::Select(n);
}

// Get list selection as a string
wxString wxListViewComboPopup::GetStringValue() const
{
    return m_sVal;
    //if(!m_sVal.IsEmpty())
    //    return m_sVal;

    //wxGxCatalogBase* pCat = GetGxCatalog();
    //if(!pCat)
    //    return wxEmptyString;

    //long nItem = wxNOT_FOUND;
    //for ( ;; )
    //{
    //    nItem = GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_DROPHILITED);
    //    if ( nItem == wxNOT_FOUND )
    //        break;
    //    LPITEMDATA pItemData = (LPITEMDATA)GetItemData(nItem);
	   // if(pItemData == NULL)
    //        continue;

    //    return GetItemText(nItem);
    //}
    //return wxEmptyString;
}

void wxListViewComboPopup::OnMouseClick(wxMouseEvent& event)
{
    //SetItemState(m_HighLightItem, 0, wxLIST_STATE_DROPHILITED);
    wxPoint pt = event.GetPosition();
	unsigned long nFlags(0);
	long nItemId = HitTest(pt, (int &)nFlags);
	if(nItemId != wxNOT_FOUND && (nFlags & wxLIST_HITTEST_ONITEM))
	{
        //SetItemState(nItemId, wxLIST_STATE_DROPHILITED, wxLIST_STATE_DROPHILITED);
        m_sVal = GetItemText(nItemId);
    }
    wxComboPopup::Dismiss();
}


void wxListViewComboPopup::OnChar(wxKeyEvent& event)
{
    event.Skip();
	if(event.GetModifiers() & wxMOD_ALT)
		return;
	if(event.GetModifiers() & wxMOD_CONTROL)
		return;
	if(event.GetModifiers() & wxMOD_SHIFT)
		return;
    switch(event.GetKeyCode())
    {
    case WXK_UP:
        {
            long nSelItem = GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if(nSelItem == wxNOT_FOUND)
            {
                m_HighLightItem = 0;
                SetItemState(m_HighLightItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            }
            else
            {
                SetItemState(nSelItem, 0, wxLIST_STATE_SELECTED);
                if(nSelItem == 0)
                {
                    m_HighLightItem = GetItemCount() - 1;
                    SetItemState(m_HighLightItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                }
                else
                {
                    m_HighLightItem = nSelItem - 1;
                    SetItemState(m_HighLightItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                }
            }
        }
        if(m_HighLightItem != wxNOT_FOUND)
        {
            m_sVal = GetItemText(m_HighLightItem);
            GetComboCtrl()->SetText(m_sVal);
        }
        break;
    case WXK_DOWN:
        {
            long nSelItem = GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if(nSelItem == wxNOT_FOUND)
            {
                m_HighLightItem = 0;
                SetItemState(m_HighLightItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            }
            else
            {
                SetItemState(nSelItem, 0, wxLIST_STATE_SELECTED);
                if(nSelItem == GetItemCount() - 1)
                {
                    m_HighLightItem = 0;
                    SetItemState(m_HighLightItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                }
                else
                {
                    m_HighLightItem = nSelItem + 1;
                    SetItemState(m_HighLightItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                }
            }
        }
        if(m_HighLightItem != wxNOT_FOUND)
        {
            m_sVal = GetItemText(m_HighLightItem);
            GetComboCtrl()->SetText(m_sVal);
        }
        break;
    case WXK_RETURN:
    case WXK_EXECUTE:
    case WXK_NUMPAD_ENTER:
    case WXK_SELECT:
        //SetItemState(m_HighLightItem, wxLIST_STATE_DROPHILITED, wxLIST_STATE_DROPHILITED);
        wxComboPopup::Dismiss();
        break;
    default:
        break;
    }
}

//---------------------------------------------------------------------------------
// wxListViewComboPopup
//---------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxPathsListViewPopup, wxPopupWindow)

BEGIN_EVENT_TABLE(wxGxPathsListViewPopup, wxPopupWindow)
    EVT_SIZE(wxGxPathsListViewPopup::OnSize)
    EVT_CHAR(wxGxPathsListViewPopup::OnChar)
END_EVENT_TABLE()

wxGxPathsListViewPopup::wxGxPathsListViewPopup(wxWindow* parent, int nMaxHeight) : wxPopupWindow(parent, wxBORDER_SIMPLE)
{
    m_nMaxHeight = nMaxHeight;
    m_pParent = dynamic_cast<wxGxPathsListViewPopupParent*>(parent);
    m_pGxPathsListView = new wxGxPathsListView(this, LISTVIEWPOPUPID, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxLC_SORT_ASCENDING | wxLC_AUTOARRANGE | wxLC_REPORT | wxLC_NO_HEADER | wxLC_SINGLE_SEL | wxTAB_TRAVERSAL | wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN);
    m_pGxPathsListView->Bind(wxEVT_LEFT_DOWN, &wxGxPathsListViewPopup::OnMouseClick, this);
    m_nItemHeight = wxNOT_FOUND;
    m_bSelected = false;
}

wxGxPathsListViewPopup::~wxGxPathsListViewPopup(void)
{
    wxDELETE(m_pGxPathsListView);
}

void wxGxPathsListViewPopup::OnSize(wxSizeEvent& WXUNUSED(event))
{
    m_pGxPathsListView->SetSize(GetSize());
    m_pGxPathsListView->SetColumnWidth(0, GetSize().GetWidth());
}

void wxGxPathsListViewPopup::Append(const wxString& sFullName)
{
    m_pGxPathsListView->Append(sFullName);
    if(m_nItemHeight == wxNOT_FOUND)
    {
        wxRect rect;
        if( m_pGxPathsListView->GetItemRect(0, rect, wxLIST_RECT_BOUNDS) )
        {
            m_nItemHeight = rect.GetHeight();
        }
    }
}

wxRect wxGxPathsListViewPopup::GetViewRect() const
{
    return m_pGxPathsListView->GetViewRect();
}

void wxGxPathsListViewPopup::Show(const wxString &sPath)
{
    Update(sPath);

    //m_pGxPathsListView->CaptureMouse();
    wxPopupWindow::Show();
}

void wxGxPathsListViewPopup::Update(const wxString &sPath)
{
    m_pGxPathsListView->DeleteAllItems();

    wxFileName name(sPath);

    wxString sVal = name.GetPath(wxPATH_GET_VOLUME | wxPATH_NO_SEPARATOR);
    wxGxCatalogBase* pCatalog = GetGxCatalog();
    wxGxObjectContainer* pGxObjectCont = wxDynamicCast(pCatalog->FindGxObject(sVal), wxGxObjectContainer);
    if(pGxObjectCont && pGxObjectCont->HasChildren())
    {
        wxGxObjectList::const_iterator iter;
        for(iter = pGxObjectCont->GetChildren().begin(); iter != pGxObjectCont->GetChildren().end(); ++iter)
        {
            wxGxObject *current = *iter;

            wxString sName = current->GetName();
            if(sName.MakeLower().StartsWith(name.GetName().MakeLower()))
                Append(current->GetFullName());
        }
    }

    wxRect rect = m_pParent->GetControlRect();
    wxPoint pt = rect.GetBottomLeft();
    wxSize size = m_pParent->GetControlSize();

    int nH = m_nItemHeight * m_pGxPathsListView->GetItemCount();
    SetSize(pt.x, pt.y, size.GetWidth(), nH < m_nMaxHeight ? nH : m_nMaxHeight);
}

void wxGxPathsListViewPopup::OnMouseWheel(wxMouseEvent& event)
{
    //m_pGxPathsListView->ScrollLines(event.GetWheelRotation());
    int nAdd = event.GetWheelRotation() / -6;
    m_pGxPathsListView->ScrollList(0, nAdd);
}


void wxGxPathsListViewPopup::OnMouseClick(wxMouseEvent& event)
{
    long nSelItem = m_pGxPathsListView->GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL, wxGIS_LIST_STATE_DROPHILITED);
    if(nSelItem != wxNOT_FOUND)
    {
        m_pParent->SetControlText(m_pGxPathsListView->GetItemText(nSelItem), true);
    }
    m_pParent->DestroyPathsPopup();
}

void wxGxPathsListViewPopup::OnChar(wxKeyEvent& event)
{
    switch(event.GetKeyCode())
    {
    case WXK_UP:
    case WXK_DOWN:
        m_bSelected = true;
        m_pGxPathsListView->GetEventHandler()->ProcessEvent(event);
        {
            long nSelItem = m_pGxPathsListView->GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL, wxGIS_LIST_STATE_DROPHILITED);
            if(nSelItem != wxNOT_FOUND)
            {
                m_pParent->SetControlText(m_pGxPathsListView->GetItemText(nSelItem), false);

                m_pGxPathsListView->EnsureVisible(nSelItem);
            }
        }
        break;
    case WXK_RETURN:
    case WXK_EXECUTE:
    case WXK_NUMPAD_ENTER:
    case WXK_SELECT:
        {
            long nSelItem = m_pGxPathsListView->GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL, wxGIS_LIST_STATE_DROPHILITED);
            if(nSelItem != wxNOT_FOUND && IsSelected())
            {
                m_pParent->SetControlText(m_pGxPathsListView->GetItemText(nSelItem), true);
            }
            else
            {
                m_pParent->SetControlText(m_pParent->GetControlText(), true);
            }
        }
        //m_pParent->DestroyPathsPopup();
        break;
    default:
        m_bSelected = false;
        break;
    }
    event.Skip();
}
//---------------------------------------------------------------------------------
// wxGxPathsListViewPopupParent
//---------------------------------------------------------------------------------
wxGxPathsListViewPopupParent::wxGxPathsListViewPopupParent(void) : m_pProbablePathsPopup(NULL)
{
}

wxGxPathsListViewPopupParent::~wxGxPathsListViewPopupParent(void)
{
}

void wxGxPathsListViewPopupParent::DestroyPathsPopup(void)
{
    if(m_pProbablePathsPopup)
    {
        m_pProbablePathsPopup->Destroy();
        m_pProbablePathsPopup = NULL;
    }
}

void wxGxPathsListViewPopupParent::OnKillFocus(wxFocusEvent& event)
{
    DestroyPathsPopup();
}

void wxGxPathsListViewPopupParent::OnChar(wxKeyEvent& event)
{
    if(m_pProbablePathsPopup)
    {
        m_pProbablePathsPopup->GetEventHandler()->ProcessEvent(event);
    }
    event.Skip();
}

void wxGxPathsListViewPopupParent::OnMouseWheel(wxMouseEvent& event)
{
    if(m_pProbablePathsPopup)
    {
        m_pProbablePathsPopup->OnMouseWheel(event);
    }
}

//---------------------------------------------------------------------------------
// wxGxLocationComboBox
//---------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGxLocationComboBox, wxComboCtrl)
    EVT_TEXT_ENTER(wxID_ANY, wxGxLocationComboBox::OnTextEnter)
	EVT_COMBOBOX(wxID_ANY, wxGxLocationComboBox::OnTextEnter)
	EVT_GXSELECTION_CHANGED(wxGxLocationComboBox::OnSelectionChanged)
    EVT_COMBOBOX_CLOSEUP(wxID_ANY, wxGxLocationComboBox::OnTextEnter)
    EVT_TEXT(wxID_ANY, wxGxLocationComboBox::OnText)
    EVT_KILL_FOCUS(wxGxLocationComboBox::OnKillFocus)
    EVT_CHAR(wxGxLocationComboBox::OnChar)
    EVT_MOUSEWHEEL(wxGxLocationComboBox::OnMouseWheel)
END_EVENT_TABLE()

wxGxLocationComboBox::wxGxLocationComboBox(wxWindow* parent, wxWindowID id, const wxSize& size) : wxComboCtrl(parent, id, wxEmptyString, wxDefaultPosition, size, wxTE_PROCESS_ENTER), wxGxPathsListViewPopupParent()
{
    m_pApp = NULL;
    m_pSelection = NULL;
    m_pCatalog = NULL;
    m_ConnectionPointSelectionCookie = wxNOT_FOUND;
    m_pListViewComboPopup = NULL;
    m_pWnd = parent;
}

wxGxLocationComboBox::~wxGxLocationComboBox(void)
{
    if(m_pProbablePathsPopup)
    {
        m_pProbablePathsPopup->Destroy();
        m_pProbablePathsPopup = NULL;
    }
}

void wxGxLocationComboBox::OnTextEnter(wxCommandEvent& event)
{
	if(m_pCatalog)
	{
        wxString sVal = GetValue();
        if(sVal.IsEmpty())
        {
            wxTipWindow* pTipWnd = new wxTipWindow(this, _("The entered path is unrecheable!"));
            return;
        }

        if(sVal.Len() > 2 && sVal[sVal.Len() - 1] == wxFileName::GetPathSeparator() && sVal[sVal.Len() - 2] != ':')
            sVal = sVal.Left(sVal.Len() - 1);
        wxGxObject* pGxObject = m_pCatalog->FindGxObject(sVal);
        if(!pGxObject && sVal[sVal.Len() - 1] == wxFileName::GetPathSeparator())
        {
            sVal = sVal.Left(sVal.Len() - 1);
            pGxObject = m_pCatalog->FindGxObject(sVal);
        }

        if(pGxObject)
        {
            m_pSelection->Select(pGxObject->GetId());
            if(m_ValuesArr.Index(sVal, false) == wxNOT_FOUND)
            {
                if(m_pListViewComboPopup)
                    m_pListViewComboPopup->Append(sVal);
                m_ValuesArr.Add(sVal);
            }
        }
        else
        {
            wxTipWindow* pTipWnd = new wxTipWindow(this, _("The entered path is unrecheable!"));
        }
	}
}

void wxGxLocationComboBox::OnText(wxCommandEvent& event)
{
	if(m_pCatalog && IsShown() && HasFocus() )
	{
        wxString sFullPath = GetValue();
        if(sFullPath.IsEmpty())
            return;

        if(!m_pProbablePathsPopup)
        {
            m_pProbablePathsPopup = new wxGxPathsListViewPopup(this);
            m_pProbablePathsPopup->Show(sFullPath);
        }
        else
        {
            m_pProbablePathsPopup->Update(sFullPath);
        }
	}
}

void wxGxLocationComboBox::OnSelectionChanged(wxGxSelectionEvent& event)
{
    if(event.GetInitiator() != TREECTRLID || !event.GetSelection())
		return;
    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(event.GetSelection()->GetLastSelectedObjectId());
    if(!pGxObject)
        return;
	wxString sPath = pGxObject->GetFullName();
	if(sPath.IsEmpty())
		sPath = pGxObject->GetName();
	SetControlText(sPath);
    if(m_pApp)
    {
        m_pApp->UpdateTitle(sPath);
    }
}

void wxGxLocationComboBox::Activate(wxGISApplicationBase* pApp)
{
    m_pApp = dynamic_cast<wxGxApplication*>(pApp);
    if(!m_pApp)
        return;
    if(!GetGxCatalog())
		return;

    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);

//#ifdef __WXMSW__
    UseAltPopupWindow(true);
//#else
//    UseAltPopupWindow(false);
//#endif

    m_pListViewComboPopup = new wxListViewComboPopup();
    SetPopupControl(m_pListViewComboPopup);
    EnablePopupAnimation(true);

	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
	{
        int nMaxCount = oConfig.ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/location_combo/paths_count")), 10);
        wxXmlNode *pNode = oConfig.GetConfigNode(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/location_combo")));
        if(pNode)
        {
            wxXmlNode *pPathNode = pNode->GetChildren();
            while(pPathNode)
            {
                wxString sFullName = pPathNode->GetAttribute(wxT("fullname"));
                if(m_ValuesArr.Index(sFullName, false) == wxNOT_FOUND)
                {
                    if(m_pListViewComboPopup)
                        m_pListViewComboPopup->Append(sFullName);
                    m_ValuesArr.Add(sFullName);
                }
                pPathNode = pPathNode->GetNext();
            }
        }
	}

    m_pSelection = m_pApp->GetGxSelection();
	if(m_pSelection)
        m_ConnectionPointSelectionCookie = m_pSelection->Advise(this);
}

void wxGxLocationComboBox::Deactivate(void)
{
	if(m_ConnectionPointSelectionCookie != wxNOT_FOUND && m_pSelection)
		m_pSelection->Unadvise(m_ConnectionPointSelectionCookie);

	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk() && m_pApp)
	{
        int nMaxCount = oConfig.ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/location_combo/paths_count")), 10);
        wxXmlNode *pNode = oConfig.GetConfigNode(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/location_combo")));
        if(!pNode)
            pNode = oConfig.CreateConfigNode(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/location_combo")));
        else
            oConfig.DeleteNodeChildren(pNode);

        if(m_ValuesArr.GetCount() > nMaxCount)
        {
            for(size_t i = m_ValuesArr.GetCount() - nMaxCount; i < m_ValuesArr.GetCount(); ++i)
            {
                wxXmlNode *pPathNode = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, wxT("location"));
                pPathNode->AddAttribute(wxT("fullname"), m_ValuesArr[i]);
            }
        }
        else
        {
            for(size_t i = 0; i < m_ValuesArr.GetCount(); ++i)
            {
                wxXmlNode *pPathNode = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, wxT("location"));
                pPathNode->AddAttribute(wxT("fullname"), m_ValuesArr[i]);
            }
        }
	}
}

wxString wxGxLocationComboBox::GetControlText(void) const
{
    return GetValue();
}

void wxGxLocationComboBox::SetControlText(const wxString &str, bool bFireEvents)
{
    if(bFireEvents)
    {
        wxComboCtrl::SetValue(str);
        wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER);
        GetEventHandler()->ProcessEvent( event );
    }
    else
    {
        SetText(str);
        SetInsertionPointEnd();
    }
}

wxRect wxGxLocationComboBox::GetControlRect() const
{
    return GetScreenRect();
}

wxSize wxGxLocationComboBox::GetControlSize() const
{
    return GetSize();
}


void wxGxLocationComboBox::OnKillFocus(wxFocusEvent& event)
{
    wxGxPathsListViewPopupParent::OnKillFocus(event);
}

void wxGxLocationComboBox::OnChar(wxKeyEvent& event)
{
    wxGxPathsListViewPopupParent::OnChar(event);
}

void wxGxLocationComboBox::OnMouseWheel(wxMouseEvent& event)
{
    wxGxPathsListViewPopupParent::OnMouseWheel(event);
}


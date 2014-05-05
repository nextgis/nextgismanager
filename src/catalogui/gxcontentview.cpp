/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxContentView class.
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
#include "wxgis/catalogui/gxcontentview.h"
#include "wxgis/catalogui/gxapplication.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/core/format.h"
#include "wxgis/catalogui/droptarget.h"
#include "wxgis/framework/dataobject.h"

#include <wx/tokenzr.h>
#include <wx/filename.h>
#include <wx/clipbrd.h>

#include "../../art/document_16.xpm"
#include "../../art/document_48.xpm"
#include "../../art/small_arrow.xpm"

//--------------------------------------------------------------------------------
// MyCompareFunction
//--------------------------------------------------------------------------------

int wxCALLBACK GxObjectCVCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
    wxGxCatalogBase* pCatalog = GetGxCatalog();
    if(!pCatalog)
        return 0;	
    
    wxGxContentView::LPITEMDATA pItem1 = (wxGxContentView::LPITEMDATA)item1;
 	wxGxContentView::LPITEMDATA pItem2 = (wxGxContentView::LPITEMDATA)item2;
    LPSORTDATA psortdata = (LPSORTDATA)sortData;

    wxGxObject* pGxObject1 = pCatalog->GetRegisterObject(pItem1->nObjectID);
    wxGxObject* pGxObject2 = pCatalog->GetRegisterObject(pItem2->nObjectID);

    if(psortdata->currentSortCol == 0)
        return GxObjectCompareFunction(pGxObject1, pGxObject2, psortdata->bSortAsc);
    else
    {
	    IGxObjectSort* pGxObjectSort1 = dynamic_cast<IGxObjectSort*>(pGxObject1);
        IGxObjectSort* pGxObjectSort2 = dynamic_cast<IGxObjectSort*>(pGxObject2);
        if(pGxObjectSort1 && !pGxObjectSort2)
		    return psortdata->bSortAsc == 0 ? 1 : -1;
        if(!pGxObjectSort1 && pGxObjectSort2)
		    return psortdata->bSortAsc == 0 ? -1 : 1;
        if(pGxObjectSort1 && pGxObjectSort2)
        {
            bool bAlwaysTop1 = pGxObjectSort1->IsAlwaysTop();
            bool bAlwaysTop2 = pGxObjectSort2->IsAlwaysTop();
            if(bAlwaysTop1 && !bAlwaysTop2)
		        return psortdata->bSortAsc == 0 ? 1 : -1;
            if(!bAlwaysTop1 && bAlwaysTop2)
		        return psortdata->bSortAsc == 0 ? -1 : 1;
            bool bSortEnables1 = pGxObjectSort1->IsSortEnabled();
            bool bSortEnables2 = pGxObjectSort2->IsSortEnabled();
            if(!bSortEnables1 || !bSortEnables1)
                return 0;
        }

        bool bContainerDst1 = pGxObject1->IsKindOf(wxCLASSINFO(wxGxDataset));
        bool bContainerDst2 = pGxObject2->IsKindOf(wxCLASSINFO(wxGxDataset));
        bool bContainer1 = pGxObject1->IsKindOf(wxCLASSINFO(wxGxObjectContainer));
        bool bContainer2 = pGxObject2->IsKindOf(wxCLASSINFO(wxGxObjectContainer));
        if(bContainer1 && !bContainerDst1 && bContainerDst2)
	        return psortdata->bSortAsc == 0 ? 1 : -1;
        if(bContainer2 && !bContainerDst2 && bContainerDst1)
	        return psortdata->bSortAsc == 0 ? -1 : 1;
        if(bContainer1 && !bContainer2)
	        return psortdata->bSortAsc == 0 ? 1 : -1;
        if(!bContainer1 && bContainer2)
	        return psortdata->bSortAsc == 0 ? -1 : 1;

        if(psortdata->currentSortCol == 1)
        {
            return pGxObject1->GetCategory().CmpNoCase(pGxObject2->GetCategory()) * (psortdata->bSortAsc == 0 ? -1 : 1);
        }
        else if(psortdata->currentSortCol == 2)
        {
            if(!bContainerDst1 && bContainerDst2)
                return psortdata->bSortAsc == 0 ? -1 : 1;
            else if(bContainerDst1 && !bContainerDst2)
                return psortdata->bSortAsc == 0 ? 1 : -1;
            else if(!bContainerDst1 && !bContainerDst2)
                return 0;
            else
            {
                wxGxDataset* pDSt1 = wxDynamicCast(pGxObject1, wxGxDataset);
                wxGxDataset* pDSt2 = wxDynamicCast(pGxObject2, wxGxDataset);
                long diff = long(pDSt1->GetSize().ToULong()) - long(pDSt2->GetSize().ToULong());
                return diff * (psortdata->bSortAsc == 0 ? -1 : 1);
            }
        }
        else if(psortdata->currentSortCol == 3)
        {
            if(!bContainerDst1 && bContainerDst2)
                return psortdata->bSortAsc == 0 ? 1 : -1;
            else if(bContainerDst1 && !bContainerDst2)
                return psortdata->bSortAsc == 0 ? -1 : 1;
            else if(!bContainerDst1 && !bContainerDst2)
                return 0;
            else
            {
                wxGxDataset* pDSt1 = wxDynamicCast(pGxObject1, wxGxDataset);
                wxGxDataset* pDSt2 = wxDynamicCast(pGxObject2, wxGxDataset);
                return (pDSt1->GetModificationDate() - pDSt2->GetModificationDate()).GetSeconds().ToLong() * (psortdata->bSortAsc == 0 ? -1 : 1);
            }
        }
    }
    return 0;
}

//--------------------------------------------------------------------------------
// wxGxContentView
//--------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxContentView, wxListCtrl)

BEGIN_EVENT_TABLE(wxGxContentView, wxListCtrl)
    EVT_LIST_BEGIN_LABEL_EDIT(LISTCTRLID, wxGxContentView::OnBeginLabelEdit)
    EVT_LIST_END_LABEL_EDIT(LISTCTRLID, wxGxContentView::OnEndLabelEdit)
    EVT_LIST_ITEM_SELECTED(LISTCTRLID, wxGxContentView::OnSelected)
    EVT_LIST_ITEM_DESELECTED(LISTCTRLID, wxGxContentView::OnDeselected)
    EVT_LIST_ITEM_ACTIVATED(LISTCTRLID, wxGxContentView::OnActivated)

    EVT_LIST_BEGIN_DRAG(LISTCTRLID, wxGxContentView::OnBeginDrag)
    EVT_LIST_BEGIN_RDRAG(LISTCTRLID, wxGxContentView::OnBeginDrag)

    EVT_LIST_COL_CLICK(LISTCTRLID, wxGxContentView::OnColClick)
    EVT_CONTEXT_MENU(wxGxContentView::OnContextMenu)
    EVT_CHAR(wxGxContentView::OnChar)
	EVT_GXOBJECT_REFRESHED(wxGxContentView::OnObjectRefreshed)
	EVT_GXOBJECT_ADDED(wxGxContentView::OnObjectAdded)
	EVT_GXOBJECT_DELETED(wxGxContentView::OnObjectDeleted)
	EVT_GXOBJECT_CHANGED(wxGxContentView::OnObjectChanged)
	EVT_GXSELECTION_CHANGED(wxGxContentView::OnSelectionChanged)
END_EVENT_TABLE()

wxGxContentView::wxGxContentView(void) : wxListCtrl()
{
    m_HighLightItem = wxNOT_FOUND;
}

wxGxContentView::wxGxContentView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    Create(parent, id, pos, size, style, wxT("ContentView")); 
}

wxGxContentView::~wxGxContentView(void)
{
	ResetContents();

    DestroyFillMetaThread();
}

bool wxGxContentView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
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

    wxListCtrl::Create(parent, LISTCTRLID, pos, size, LISTSTYLE);

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

    CreateAndRunFillMetaThread();

    return true;
}

bool wxGxContentView::Activate(IApplication* const pApplication, wxXmlNode* const pConf)
{
	if(!wxGxView::Activate(pApplication, pConf))
		return false;

    wxGxApplicationBase* pGxApp = dynamic_cast<wxGxApplicationBase*>(pApplication);
    if(NULL == pGxApp)
        return false;
    m_pSelection = pGxApp->GetGxSelection();

    m_pApp = dynamic_cast<wxGISApplicationBase*>(pApplication);
    if(NULL == m_pApp)
        return false;
    m_pGxApp = dynamic_cast<wxGxApplication*>(pApplication);

    if(!GetGxCatalog())
		return false;
    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);

    //delete
    m_pDeleteCmd = m_pApp->GetCommand(wxT("wxGISCatalogMainCmd"), 4);

    m_ConnectionPointCatalogCookie = m_pCatalog->Advise(this);

    InitColumns();
    Serialize(m_pXmlConf, false);

	return true;
}

void wxGxContentView::InitColumns(void)
{
    if(GetColumnCount() < 4)
    {
        InsertColumn(0, _("Name"),	wxLIST_FORMAT_LEFT, 150);
        InsertColumn(1, _("Type"),  wxLIST_FORMAT_LEFT, 250);
        InsertColumn(2, _("Size"), wxLIST_FORMAT_RIGHT, 80);
        InsertColumn(3, _("Modification date"),  wxLIST_FORMAT_LEFT, 80);
        //InsertColumn(4, _("Geometry type"),  wxLIST_FORMAT_LEFT, 100);
    }
}

void wxGxContentView::Deactivate(void)
{
	if(m_ConnectionPointCatalogCookie != wxNOT_FOUND)
        m_pCatalog->Unadvise(m_ConnectionPointCatalogCookie);

	Serialize(m_pXmlConf, true);
	wxGxView::Deactivate();
}


void wxGxContentView::Serialize(wxXmlNode* pRootNode, bool bStore)
{
	if(pRootNode == NULL)
		return;

	if(bStore)
	{
        if(pRootNode->HasAttribute(wxT("style")))
            pRootNode->DeleteAttribute(wxT("style"));
        SetDecimalValue(pRootNode, wxT("style"), m_current_style);
        if(pRootNode->HasAttribute(wxT("sort")))
            pRootNode->DeleteAttribute(wxT("sort"));
        SetBoolValue(pRootNode, wxT("sort"), m_bSortAsc);
        if(pRootNode->HasAttribute(wxT("sort_col")))
            pRootNode->DeleteAttribute(wxT("sort_col"));
        SetDecimalValue(pRootNode, wxT("sort_col"), m_currentSortCol);

        if(m_current_style == enumGISCVReport)
        {
            //store values
            m_anWidth.Clear();
            for (int i = 0; i < GetColumnCount(); ++i)
            {
                m_anWidth.Add( GetColumnWidth(i) );
            }
    #ifdef wxHAS_LISTCTRL_COLUMN_ORDER
            m_anOrder = GetColumnsOrder();
    #endif
        }

        wxString sCols;
        for(size_t i = 0; i < m_anWidth.GetCount(); ++i)
        {
            sCols += wxString::Format(wxT("%d"), m_anWidth[i]);
            sCols += wxT("|");
        }
        if(pRootNode->HasAttribute(wxT("cols_width")))
            pRootNode->DeleteAttribute(wxT("cols_width"));
        pRootNode->AddAttribute(wxT("cols_width"), sCols);

#ifdef wxHAS_LISTCTRL_COLUMN_ORDER
        wxString sOrd;
        for(size_t i = 0; i < m_anOrder.GetCount(); ++i)
        {
            sOrd += wxString::Format(wxT("%d"), m_anOrder[i]);
            sOrd += wxT("|");
        }

        if(pRootNode->HasAttribute(wxT("cols_order")))
            pRootNode->DeleteAttribute(wxT("cols_order"));
        pRootNode->AddAttribute(wxT("cols_order"), sOrd);
#endif

	}
	else
	{
		m_bSortAsc = GetBoolValue(pRootNode, wxT("sort"), true);
		m_currentSortCol = GetDecimalValue(pRootNode, wxT("sort_col"), 0);
		wxGISEnumContentsViewStyle style = (wxGISEnumContentsViewStyle)GetDecimalValue(pRootNode, wxT("style"), 0);
        //load col width
        wxString sCol = pRootNode->GetAttribute(wxT("cols_width"));
	    wxStringTokenizer tkz(sCol, wxString(wxT("|")), wxTOKEN_RET_EMPTY );
	    while ( tkz.HasMoreTokens() )
	    {
		    wxString token = tkz.GetNextToken();
		    //token.Replace(wxT("|"), wxT(""));
		    int nWidth = wxAtoi(token); //wxLIST_AUTOSIZE
            m_anWidth.Add(nWidth);
	    }

#ifdef wxHAS_LISTCTRL_COLUMN_ORDER
        //load col order
        wxString sOrd = pRootNode->GetAttribute(wxT("cols_order"));
	    wxStringTokenizer tkz_ord(sOrd, wxString(wxT("|")), wxTOKEN_RET_EMPTY );
	    while ( tkz_ord.HasMoreTokens() )
	    {
		    wxString token = tkz_ord.GetNextToken();
		    //token.Replace(wxT("|"), wxT(""));
		    m_anOrder.Add( wxAtoi(token) );          
	    }
#endif

        SetStyle(style);

        SORTDATA sortdata = {m_bSortAsc, m_currentSortCol};
		SortItems(GxObjectCVCompareFunction, (long)&sortdata);
	}
}

bool wxGxContentView::AddObject(wxGxObject* const pObject)
{
    wxCriticalSectionLocker locker(m_CritSectCont);
    if(pObject == NULL)
		return false;

    //check doubles
	for(long i = 0; i < GetItemCount(); ++i)
	{
		LPITEMDATA pItemData = (LPITEMDATA)GetItemData(i);
		if(pItemData == NULL)
			continue;
        if(pItemData->nObjectID == pObject->GetId())
			return false;
    }

    //wxLogDebug(wxT("wxGxContentView::AddObject %d '%s'"), pObject->GetId(), pObject->GetFullName());
    IGxObjectUI* pObjUI =  dynamic_cast<IGxObjectUI*>(pObject);
	wxIcon icon_small, icon_large;
	if(pObjUI != NULL)
	{
		icon_small = pObjUI->GetSmallImage();
		icon_large = pObjUI->GetLargeImage();
	}

	int pos = GetIconPos(icon_small, icon_large);

	LPITEMDATA pData = new _itemdata;
	pData->nObjectID = pObject->GetId();
	pData->iImageIndex = pos;

    wxString sName;
    if(m_pCatalog->GetShowExt())
        sName = pObject->GetName();
    else
        sName = pObject->GetBaseName();

	wxString sType = pObject->GetCategory();

	long ListItemID = InsertItem(0, sName, pos);
	if(m_current_style == enumGISCVReport)
	{
		SetItem(ListItemID, 1, sType);
        wxGxDataset* pDSet = wxDynamicCast(pObject, wxGxDataset);
        if(pDSet)
        {
            wxCriticalSectionLocker locker(m_CritSectFillMeta);
            m_anFillMetaIDs.Add(pDSet->GetId());
        }
	}
	SetItemPtrData(ListItemID, (wxUIntPtr) pData);

    return true;
}

void wxGxContentView::OnColClick(wxListEvent& event)
{
    //event.Skip();
    m_currentSortCol = event.GetColumn();
	m_bSortAsc = !m_bSortAsc;

    SORTDATA sortdata = {m_bSortAsc, m_currentSortCol};
	SortItems(GxObjectCVCompareFunction, (long)&sortdata);
	if(m_current_style == enumGISCVReport)
		SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
}

void wxGxContentView::OnContextMenu(wxContextMenuEvent& event)
{
    //event.Skip();
    wxPoint point = event.GetPosition();
    // If from keyboard
    if (point.x == -1 && point.y == -1)
	{
        wxSize size = GetSize();
        point.x = size.x / 2;
        point.y = size.y / 2;
    }
	else
	{
        point = ScreenToClient(point);
    }
    ShowContextMenu(point);
}

void wxGxContentView::OnSelected(wxListEvent& event)
{
	//event.Skip();
    m_pSelection->Clear(NOTFIRESELID);
    long nItem = wxNOT_FOUND;
	size_t nCount(0);
    for ( ;; )
    {
        nItem = GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( nItem == wxNOT_FOUND )
            break;
        LPITEMDATA pItemData = (LPITEMDATA)GetItemData(nItem);
	    if(pItemData == NULL)
            continue;
		nCount++;
        m_pSelection->Select(pItemData->nObjectID, true, NOTFIRESELID);
    }

    if (m_pGxApp != NULL)
    {
        if(nCount <= 0)
	    	m_pSelection->SetInitiator(TREECTRLID);
        m_pGxApp->UpdateNewMenu(m_pSelection);
	}

    wxGISStatusBar* pStatusBar = m_pApp->GetStatusBar();
    if(pStatusBar)
    {
        if(nCount > 1)
        {
            pStatusBar->SetMessage(wxString::Format(_("%d objects selected"), nCount));
        }
        else
        {
            pStatusBar->SetMessage(wxEmptyString);
        }
    }
}

bool wxGxContentView::Show(bool show)
{
    bool res = wxListCtrl::Show(show);
    if(show)
    {
        //deselect all items
        long nItem = wxNOT_FOUND;
        for ( ;; )
        {
            nItem = GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if ( nItem == wxNOT_FOUND )
                break;
            SetItemState(nItem, 0, wxLIST_STATE_SELECTED|wxLIST_STATE_FOCUSED);
        }
    }
    return res;
}


void wxGxContentView::OnDeselected(wxListEvent& event)
{
	//event.Skip();
    if(GetSelectedItemCount() == 0)
    {
        m_pSelection->Select(m_nParentGxObjectID, false, NOTFIRESELID);
    }
    else
    {
    	LPITEMDATA pItemData = (LPITEMDATA)event.GetData();
	    if(pItemData != NULL)
        {
            m_pSelection->Unselect(pItemData->nObjectID, NOTFIRESELID);
        }
    }

	if(GetSelectedItemCount() == 0)
    {
		m_pSelection->SetInitiator(TREECTRLID);
        if (m_pGxApp)
            m_pGxApp->UpdateNewMenu(m_pSelection);
	}

    wxGISStatusBar* pStatusBar = m_pApp->GetStatusBar();
    if(pStatusBar)
    {
        if(GetSelectedItemCount() > 1)
        {
            pStatusBar->SetMessage(wxString::Format(_("%d objects selected"), GetSelectedItemCount()));
        }
        else
        {
            pStatusBar->SetMessage(wxEmptyString);
        }
    }

}

void wxGxContentView::ShowContextMenu(const wxPoint& pos)
{
	long item = wxNOT_FOUND;
    item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if(item == wxNOT_FOUND)
	{
        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(m_nParentGxObjectID);

		IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pGxObject);
        if(pGxObjectUI)
        {
            //wxString psContextMenu = pGxObjectUI->NewMenu();
            wxString psContextMenu = pGxObjectUI->ContextMenu();
            if(m_pApp)
            {
                wxMenu* pMenu = dynamic_cast<wxMenu*>(m_pApp->GetCommandBar(psContextMenu));
                if(pMenu)
                {
                    PopupMenu(pMenu, pos.x, pos.y);
                }
            }
        }
		return;
	}

	LPITEMDATA pItemData = (LPITEMDATA)GetItemData(item);
	if(pItemData != NULL)
	{
        //bool bAdd = true;
        //m_pSelection->Select(pItemData->nObjectID, bAdd, NOTFIRESELID);

        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pItemData->nObjectID);
		IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pGxObject);
		if(pGxObjectUI != NULL)
		{
            wxString psContextMenu = pGxObjectUI->ContextMenu();
            if(m_pApp)
            {
                wxMenu* pMenu = dynamic_cast<wxMenu*>(m_pApp->GetCommandBar(psContextMenu));
                if(pMenu)
                {
                    PopupMenu(pMenu, pos.x, pos.y);
                }
            }
		}
	}
}

void wxGxContentView::SetColumnImage(int col, int image)
{
    if(col >= GetColumnCount())
        return;
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);

    //reset image
    item.SetImage(wxNOT_FOUND);
    for (int i = 0; i < GetColumnCount(); ++i)
    {
        SetColumn(i, item);
    }

    item.SetImage(image);
    SetColumn(col, item);
}

void wxGxContentView::OnActivated(wxListEvent& event)
{
	//event.Skip();
	LPITEMDATA pItemData = (LPITEMDATA)event.GetData();
	if(pItemData == NULL)
		return;

    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pItemData->nObjectID);
	IGxObjectWizard* pGxObjectWizard = dynamic_cast<IGxObjectWizard*>(pGxObject);
	if(pGxObjectWizard != NULL)
		if(!pGxObjectWizard->Invoke(this))
			return;

    if(pGxObject->IsKindOf(wxCLASSINFO(wxGxObjectContainer)))
	{
		m_pSelection->Select(pItemData->nObjectID, false, GetId());
	}
}

void wxGxContentView::SetStyle(wxGISEnumContentsViewStyle style)
{
    if(m_current_style == style)
        return;
    

    if(m_current_style == enumGISCVReport)
    {
        //store values
        m_anWidth.Clear();
        for (int i = 0; i < GetColumnCount(); ++i)
        {
            m_anWidth.Add( GetColumnWidth(i) );
        }
#ifdef wxHAS_LISTCTRL_COLUMN_ORDER
        m_anOrder = GetColumnsOrder();
#endif
    }
    m_current_style = style;

	switch(m_current_style)
	{
	case enumGISCVReport:
        SetSingleStyle(wxLC_REPORT);        

        InitColumns();

        for(size_t i = 0; i < m_anWidth.GetCount(); ++i)
            SetColumnWidth(i, m_anWidth[i]);
#ifdef wxHAS_LISTCTRL_COLUMN_ORDER
        SetColumnsOrder(m_anOrder);
#endif
                
        SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
        
		break;
	case enumGISCVSmall:
        SetSingleStyle(wxLC_SMALL_ICON);
		break;
	case enumGISCVLarge:
        SetSingleStyle(wxLC_ICON);
		break;
	case enumGISCVList:
        SetSingleStyle(wxLC_LIST);
		break;
	}
    
    RefreshAll();
}

void wxGxContentView::OnBeginLabelEdit(wxListEvent& event)
{
	LPITEMDATA pItemData = (LPITEMDATA)event.GetData();
	if(pItemData == NULL)
	{
		event.Veto();
		return;
	}

    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pItemData->nObjectID);
	IGxObjectEdit* pObjEdit =  dynamic_cast<IGxObjectEdit*>(pGxObject);
	if(pObjEdit == NULL)
	{
		event.Veto();
		return;
	}
	if(!pObjEdit->CanRename())
	{
		event.Veto();
		return;
	}

    //event.Skip();
}

void wxGxContentView::OnEndLabelEdit(wxListEvent& event)
{
    //event.Skip();
    if ( event.GetLabel().IsEmpty() )
    {
        event.Veto();
		return;
    }

	LPITEMDATA pItemData = (LPITEMDATA)event.GetData();
	if(pItemData == NULL)
	{
		event.Veto();
		return;
	}

    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pItemData->nObjectID);
	IGxObjectEdit* pObjEdit =  dynamic_cast<IGxObjectEdit*>(pGxObject);
	if(pObjEdit == NULL)
	{
		event.Veto();
		return;
	}
	if(pObjEdit->Rename(event.GetLabel()))
	{
    	m_pCatalog->ObjectChanged(pGxObject->GetId());
	}
    else
    {
		event.Veto();
		wxMessageBox(_("Rename failed!"), _("Error"), wxICON_ERROR | wxOK );

        SORTDATA sortdata = { m_bSortAsc, m_currentSortCol };
        SortItems(GxObjectCVCompareFunction, (long)&sortdata);
        SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);

		return;
    }
}

void wxGxContentView::OnObjectAdded(wxGxCatalogEvent& event)
{
    //wxLogDebug(wxT("ContentView Object %d Add"), event.GetObjectID());
    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
    if(pGxObject)
    {
//        wxLogDebug(wxT("ContentView Object %d '%s' Add"), pGxObject->GetId(), pGxObject->GetFullName());

		const wxGxObject* pParentGxObject = pGxObject->GetParent();
		if(!pParentGxObject)
			return;
	    if(pParentGxObject->GetId() == m_nParentGxObjectID)
        {
		    if(AddObject(pGxObject))
            {
                wxGxAutoRenamer* pGxAutoRenamer = dynamic_cast<wxGxAutoRenamer*>(pGxObject->GetParent());
                if(pGxAutoRenamer && pGxAutoRenamer->IsBeginRename(this, pGxObject->GetPath()))
                {
                    BeginRename(pGxObject->GetId());
                }
                else
                {
                    SORTDATA sortdata = { m_bSortAsc, m_currentSortCol };
                    SortItems(GxObjectCVCompareFunction, (long)&sortdata);
                    SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
                }
            }
        }
    }
}

void wxGxContentView::OnObjectDeleted(wxGxCatalogEvent& event)
{
    //wxLogDebug(wxT("ContentView Object %d Delete"), event.GetObjectID());
    //wxCriticalSectionLocker locker(m_CritSectCont);
	for(long i = 0; i < GetItemCount(); ++i)
	{
		LPITEMDATA pItemData = (LPITEMDATA)GetItemData(i);
		if(pItemData == NULL)
			continue;
		if(pItemData->nObjectID != event.GetObjectID())
			continue;
        SetItemData(i, 0);
		//delete pItemData;
		DeleteItem(i);
		//Refresh();
        //wxListCtrl::Refresh();
    }
}

void wxGxContentView::OnObjectChanged(wxGxCatalogEvent& event)
{
    //wxLogDebug(wxT("ContentView Object %d Change"), event.GetObjectID());
    if(event.GetObjectID() == m_nParentGxObjectID)
	{
        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
		wxGxObjectContainer* pObjectContainer = wxDynamicCast(pGxObject, wxGxObjectContainer);
		if(pObjectContainer != NULL)
		{
			wxBusyCursor wait;
			if(GetItemCount() > 0 && !pObjectContainer->HasChildren())
				ResetContents();
			else if(GetItemCount() == 0 && pObjectContainer->HasChildren())
			{
				RefreshAll();
				return;
			}
		}
	}

    bool bItemsHaveChanges = false;

	for(long i = 0; i < GetItemCount(); ++i)
	{
		LPITEMDATA pItemData = (LPITEMDATA)GetItemData(i);
		if(pItemData == NULL)
			continue;
		if(pItemData->nObjectID != event.GetObjectID())
			continue;

        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
		IGxObjectUI* pObjUI =  dynamic_cast<IGxObjectUI*>(pGxObject);
		wxIcon icon_small, icon_large;
		if(pObjUI != NULL)
		{
			icon_small = pObjUI->GetSmallImage();
			icon_large = pObjUI->GetLargeImage();
		}

        int pos = GetIconPos(icon_small, icon_large);
		pItemData->iImageIndex = pos;

		wxString sName;
		if(m_pCatalog->GetShowExt())
			sName = pGxObject->GetName();
		else
			sName = pGxObject->GetBaseName();

		wxString sType = pGxObject->GetCategory();

        if (GetItemText(i, 0) != sName)
        {
		    SetItem(i, 0, sName, pos);
            if (m_currentSortCol == 0)
            {
                bItemsHaveChanges = true;
            }
        }

        if(m_current_style == enumGISCVReport)
	    {
            if (GetItemText(i, 1) != sType)
            {
		        SetItem(i, 1, sType);
                if (m_currentSortCol == 1)
                {
                    bItemsHaveChanges = true;
                }
            }
            wxGxDataset* pDSet = wxDynamicCast(pGxObject, wxGxDataset);
            if(pDSet)
            {
                if (pDSet->IsMetadataFilled())
                {
                    if (pDSet->GetSize() > 0)
                        SetItem(i, 2, wxFileName::GetHumanReadableSize(pDSet->GetSize()));
                    if (pDSet->GetModificationDate().IsValid())
                        SetItem(i, 3, pDSet->GetModificationDate().Format()); 
                }
                else
                {
                    wxCriticalSectionLocker locker(m_CritSectFillMeta);
                    m_anFillMetaIDs.Add(pDSet->GetId());
                }
            }
	    }
	}

    if(bItemsHaveChanges)
    {
        SORTDATA sortdata = {m_bSortAsc, m_currentSortCol};
	    SortItems(GxObjectCVCompareFunction, (long)&sortdata);
    	SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
    }

	//wxListCtrl::Refresh();
}

void wxGxContentView::OnObjectRefreshed(wxGxCatalogEvent& event)
{
    //wxLogDebug(wxT("ContentView Object %d Refresh"), event.GetObjectID());
    if(m_nParentGxObjectID == event.GetObjectID())
        RefreshAll();
}

void wxGxContentView::RefreshAll(void)
{
    //wxLogDebug(wxT("wxGxContentView::RefreshAll"));
    ResetContents();
    if(m_pCatalog == NULL)
        return;
    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(m_nParentGxObjectID);
	wxGxObjectContainer* pObjectContainer = wxDynamicCast(pGxObject, wxGxObjectContainer);
	wxBusyCursor wait;
	if(pObjectContainer == NULL || !pObjectContainer->HasChildren())
		return;

	wxGxObjectList ObjectList = pObjectContainer->GetChildren();
    wxGxObjectList::iterator iter;
    for (iter = ObjectList.begin(); iter != ObjectList.end(); ++iter)
    {
        wxGxObject *current = *iter;
		AddObject(current);
    }

    SORTDATA sortdata = {m_bSortAsc, m_currentSortCol};
	SortItems(GxObjectCVCompareFunction, (long)&sortdata);
	SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);

    wxListCtrl::Refresh();
}

void wxGxContentView::OnSelectionChanged(wxGxSelectionEvent& event)
{
	if(event.GetInitiator() == GetId())
		return;

    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(m_pSelection->GetLastSelectedObjectId());
	if(!pGxObject || m_nParentGxObjectID == pGxObject->GetId())
		return;

	wxBusyCursor wait;
	//reset
	ResetContents();
	m_nParentGxObjectID = pGxObject->GetId();

	wxGxObjectContainer* pObjectContainer = wxDynamicCast(pGxObject, wxGxObjectContainer);
	if(pObjectContainer == NULL || !pObjectContainer->HasChildren())
		return;

	wxGxObjectList ObjectList = pObjectContainer->GetChildren();
    wxGxObjectList::iterator iter;
    for (iter = ObjectList.begin(); iter != ObjectList.end(); ++iter)
    {
        wxGxObject *current = *iter;
		AddObject(current);
    }

    SORTDATA sortdata = {m_bSortAsc, m_currentSortCol};
	SortItems(GxObjectCVCompareFunction, (long)&sortdata);
	SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
}

bool wxGxContentView::Applies(wxGxSelection* const pSelection)
{
    wxCHECK_MSG(pSelection, false, wxT("Input wxGxSelection pointer is null"));

	for(size_t i = 0; i < pSelection->GetCount(); ++i)
	{
        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pSelection->GetSelectedObjectId(i));
        if(pGxObject)
        {
            if(pGxObject->IsKindOf(wxCLASSINFO(wxGxObjectContainer)))
            {
                return true;
            }
        }
	}
	return false;
}

void wxGxContentView::ResetContents(void)
{
	for(long i = 0; i < GetItemCount(); ++i)
		delete (LPITEMDATA)GetItemData(i);
	DeleteAllItems();
}

void wxGxContentView::OnBeginDrag(wxListEvent& event)
{
    //TODO: wxDELETE(pDragData) somethere
    wxDataObjectComposite *pDragData = new wxDataObjectComposite();

    wxGISStringDataObject *pNamesData = new wxGISStringDataObject(wxDataFormat(wxGIS_DND_NAME));
    pDragData->Add(pNamesData, true);

    wxFileDataObject *pFileData = new wxFileDataObject();
    pDragData->Add(pFileData, false);

    //wxGISDecimalDataObject *pIDsData = new wxGISDecimalDataObject(wxDataFormat(wxT("application/x-vnd.wxgis.gxobject-id")));
    //pDragData->Add(pIDsData, false);


    //TODO: create dataobject for QGIS
//        wxDataObjectSimple* pDataObjectSimple = new wxDataObjectSimple(wxDataFormat(wxT("application/x-vnd.qgis.qgis.uri")));
//        my_data->Add(pDataObjectSimple);
//
//        wxMemoryOutputStream *pstream = new wxMemoryOutputStream();
//        wxDataOutputStream dostr(*pstream);
//


    long nItem = wxNOT_FOUND;
    int nCount(0);
    for ( ;; )
    {
        nItem = GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( nItem == wxNOT_FOUND )
            break;

        LPITEMDATA pItemData = (LPITEMDATA)GetItemData(nItem);
	    if(pItemData == NULL)
            continue;

        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pItemData->nObjectID);
	    if(pGxObject == NULL)
            continue;        
        wxString sSystemPath(pGxObject->GetPath(), wxConvUTF8);

        pFileData->AddFile(sSystemPath);
        pNamesData->AddString(pGxObject->GetFullName());
        //pIDsData->AddDecimal(pGxObject->GetId());
    }

    wxDropSource dragSource( this );
	dragSource.SetData( *pDragData );
	wxDragResult result = dragSource.DoDragDrop( wxDrag_DefaultMove );  
}


void wxGxContentView::SelectAll(void)
{
	for(long item = 0; item < GetItemCount(); ++item)
        SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void wxGxContentView::SelectItem(int nChar, bool bShift)
{
    long nSelItemMax = nChar == WXK_DOWN ? GetItemCount() - 1 : 0;
    long nSelItemNext = nChar == WXK_DOWN ? 0 : GetItemCount() - 1;
    long nSelItemNextAdd = nChar == WXK_DOWN ? 1 : -1;
    long nSelItem = GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
    if (bShift)
    {   
        if (nSelItem == wxNOT_FOUND && m_HighLightItem == wxNOT_FOUND)
        {
            SetItemState(nSelItemNext, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        }

        bool bSwitchDirection = false;
            
        if (m_bPrevChar == WXK_UP || m_bPrevChar == WXK_DOWN)
        {
            bSwitchDirection = m_bPrevChar != nChar;
        }
             
        if (bSwitchDirection)
        {
            nSelItemNextAdd = 0;
        }
        m_bPrevChar = nChar;

        long nSelItemNextNorm;
        if (m_HighLightItem == wxNOT_FOUND)
        {
            nSelItemNextNorm = nSelItem + nSelItemNextAdd;
        }
        else
        {
            nSelItemNextNorm = m_HighLightItem + nSelItemNextAdd;
        }
        m_HighLightItem = nSelItemNextNorm;
                
        if (nSelItemNextNorm == -1 || nSelItemNextNorm == GetItemCount())
            return;

        int nMask = GetItemState(nSelItemNextNorm, wxLIST_STATE_SELECTED);
        if (nMask == wxLIST_STATE_SELECTED)
        {
            SetItemState(nSelItemNextNorm, wxLIST_STATE_DONTCARE, wxLIST_STATE_SELECTED);
        }
        else
        {
            SetItemState(nSelItemNextNorm, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        }
    }
    else
    {
        if (nSelItem == wxNOT_FOUND)
        {
            SetItemState(nSelItemNext, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
        }
        else
        {

            if (m_HighLightItem != wxNOT_FOUND)
            {
                for (long item = 0; item < GetItemCount(); ++item)
                    SetItemState(item, wxLIST_STATE_DONTCARE, wxLIST_STATE_SELECTED);
                nSelItem = m_HighLightItem;
                m_HighLightItem = wxNOT_FOUND;
            }

            SetItemState(nSelItem, wxLIST_STATE_DONTCARE, wxLIST_STATE_SELECTED);
            if (nSelItem == nSelItemMax)
            {
                SetItemState(nSelItemNext, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            }
            else
            {
                SetItemState(nSelItem + nSelItemNextAdd, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            }
        }
    }
}

void wxGxContentView::OnChar(wxKeyEvent& event)
{
	if(event.GetModifiers() & wxMOD_ALT)
		return;
	if(event.GetModifiers() & wxMOD_CONTROL)
		return;
    switch(event.GetKeyCode())
    {
    case WXK_DELETE:
    case WXK_NUMPAD_DELETE:
        if (event.GetModifiers() & wxMOD_SHIFT)
        {
            //TODO: complete delete
        }
        else
        {
            if (NULL != m_pDeleteCmd)
            {
                //TODO: delete to trash can
                m_pDeleteCmd->OnClick();
            }
        }
        break;
    case WXK_UP:
    case WXK_DOWN:
        SelectItem(event.GetKeyCode(), event.GetModifiers() & wxMOD_SHIFT);
        break;
    default:
        break;
    }
}

void wxGxContentView::BeginRename(long nObjectID)
{
	long nItem;
    LPITEMDATA pItemData = NULL;
	for(nItem = 0; nItem < GetItemCount(); ++nItem)
	{
        pItemData = (LPITEMDATA)GetItemData(nItem);
		if(pItemData == NULL)
			continue;
		if(pItemData->nObjectID == nObjectID)
        {
            SetItemState(nItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            //m_pSelection->Select(nObjectID, true, NOTFIRESELID);
			break;
        }
    }

    if (NULL != pItemData)
    {
		EditLabel(nItem);
    }
}

int wxGxContentView::GetIconPos(wxIcon icon_small, wxIcon icon_large)
{
    wxCriticalSectionLocker locker(m_CritSect);
	int pos(0);
	if(icon_small.IsOk())
	{
		for(size_t i = 0; i < m_IconsArray.size(); ++i)
		{
			if(m_IconsArray[i].bLarge)
				continue;
			if(m_IconsArray[i].oIcon.IsSameAs(icon_small))
			{
				pos = m_IconsArray[i].iImageIndex;
				break;
			}
		}
		if(pos == 0)
		{
			pos = m_ImageListSmall.Add(icon_small);
			ICONDATA myicondata = {icon_small, pos, false};
			m_IconsArray.push_back(myicondata);

            wxIcon temp_large_icon(document_48_xpm);
            if(!icon_large.IsOk())
                icon_large = temp_large_icon;

            pos = m_ImageListLarge.Add(icon_large);
			ICONDATA myicondata1 = {icon_large, pos, true};
			m_IconsArray.push_back(myicondata1);
		}
	}
	else
		pos = 2;//0 col img, 1 - col img

    return pos;
}

wxDragResult wxGxContentView::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
    SetItemState(m_HighLightItem, 0, wxLIST_STATE_DROPHILITED);
    wxPoint pt(x, y);
	unsigned long nFlags(0);
	long nItemId = HitTest(pt, (int &)nFlags);
	if(nItemId != wxNOT_FOUND && (nFlags & wxLIST_HITTEST_ONITEM))
	{
        wxSize sz = GetClientSize();
        if(DNDSCROLL > y)//scroll up
            ScrollLines(-1);
        else if((sz.GetHeight() - DNDSCROLL) < y)//scroll down
            ScrollLines(1);

        m_HighLightItem = nItemId;
        SetItemState(m_HighLightItem, wxLIST_STATE_DROPHILITED, wxLIST_STATE_DROPHILITED);

    }
    return def;
}

bool wxGxContentView::OnDropObjects(wxCoord x, wxCoord y, const wxArrayString& GxObjects)
{
    bool bMove = !wxGetKeyState(WXK_CONTROL);

    SetItemState(m_HighLightItem, 0, wxLIST_STATE_DROPHILITED);
    //SetItemState(m_HighLightItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    wxPoint pt(x, y);
	unsigned long nFlags(0);
	long nItemId = HitTest(pt, (int &)nFlags);
    long nObjectID(m_nParentGxObjectID);
	if(nItemId != wxNOT_FOUND && (nFlags & wxLIST_HITTEST_ONITEM))
    {
        LPITEMDATA pItemData = (LPITEMDATA)GetItemData(nItemId);
        nObjectID = pItemData->nObjectID;
    }

    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(nObjectID);
    IGxDropTarget* pTarget = dynamic_cast<IGxDropTarget*>(pGxObject);
    if(pTarget)
    {
        wxDragResult res(bMove == true ? wxDragMove : wxDragCopy);
        if(wxIsDragResultOk(pTarget->CanDrop(res)))
            return pTarget->Drop(GxObjects, bMove);
    }
	return false;
}

void wxGxContentView::OnLeave()
{
    SetItemState(m_HighLightItem, 0, wxLIST_STATE_DROPHILITED);
}

wxGxObject* const wxGxContentView::GetParentGxObject(void) const
{
    return m_pCatalog->GetRegisterObject(m_nParentGxObjectID);
}

bool wxGxContentView::CanPaste()
{            
    wxClipboardLocker lockClip;
    return wxTheClipboard->IsSupported(wxDF_FILENAME) | wxTheClipboard->IsSupported(wxDataFormat(wxGIS_DND_NAME));
    //& wxTheClipboard->IsSupported(wxDF_TEXT); | wxDF_BITMAP | wxDF_TIFF | wxDF_DIB | wxDF_UNICODETEXT | wxDF_HTML
}

bool wxGxContentView::CreateAndRunFillMetaThread(void)
{
    if (CreateThread(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not create the thread!"));
        return false;
    }

    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the thread!"));
        return false;
    }

    return true;
}

void wxGxContentView::DestroyFillMetaThread(void)
{
    if (GetThread() && GetThread()->IsRunning())
        GetThread()->Delete();//
}

wxThread::ExitCode wxGxContentView::Entry()
{
    while (!GetThread()->TestDestroy())
    {
        long nID = wxNOT_FOUND;
        m_CritSectFillMeta.Enter();
        if (m_anFillMetaIDs.GetCount() > 0)
        {
            nID = m_anFillMetaIDs[0];
            m_anFillMetaIDs.RemoveAt(0);
        }
        m_CritSectFillMeta.Leave();

        if (nID == wxNOT_FOUND)
        {
            wxThread::Sleep(950);
        }
        else
        {
            wxGxDataset* pDSet = wxDynamicCast(m_pCatalog->GetRegisterObject(nID), wxGxDataset);
            if (NULL != pDSet)
            {
                pDSet->FillMetadata(true);
                m_pCatalog->ObjectChanged(nID);
            }
        }

    }

    return (wxThread::ExitCode)wxTHREAD_NO_ERROR;     // success
}

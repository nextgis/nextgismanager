/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISToolExecuteView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "wxgis/geoprocessingui/gxtoolexecview.h"
#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/catalog/gxobject.h"
#include "wxgis/catalogui/droptarget.h"
#include "wxgis/framework/dataobject.h"
#include "wxgis/geoprocessingui/droptarget.h"

#include <wx/tokenzr.h>
#include <wx/clipbrd.h>
#include <wx/wupdlock.h>

#include "../../art/state.xpm"

//--------------------------------------------------------------------------------
// Sort Function
//--------------------------------------------------------------------------------

WXDLLIMPEXP_GIS_GPU int wxCALLBACK GxTaskCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
    wxGxCatalogBase* pCatalog = GetGxCatalog();
    if(!pCatalog)
        return 0;	
    
//    LPSORTDATA psortdata = (LPSORTDATA)sortData;

    IGxTask* pGxTask1 = dynamic_cast<IGxTask*>(pCatalog->GetRegisterObject(item1));
    IGxTask* pGxTask2 = dynamic_cast<IGxTask*>(pCatalog->GetRegisterObject(item2));

    if(pGxTask1 && !pGxTask2)
		return 1;//psortdata->bSortAsc == 0 ? 1 : -1;
    if(!pGxTask1 && pGxTask2)
		return -1;//psortdata->bSortAsc == 0 ? -1 : 1;
    if(!pGxTask1 && !pGxTask2)
        return 0;
    if(pGxTask1->GetState() == enumGISTaskDone && pGxTask2->GetState() == enumGISTaskDone)
        return 0;
    if(pGxTask1->GetState() != enumGISTaskDone && pGxTask2->GetState() == enumGISTaskDone)
        return -1;
    if(pGxTask1->GetState() == enumGISTaskDone && pGxTask2->GetState() != enumGISTaskDone)
        return 1;
    return pGxTask1->GetPriority() - pGxTask2->GetPriority();
}

//----------------------------------------------------------------------------
// wxGISToolExecuteView
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISToolExecuteView, wxListCtrl)

BEGIN_EVENT_TABLE(wxGISToolExecuteView, wxListCtrl)
    EVT_LIST_ITEM_SELECTED(TOOLEXECUTECTRLID, wxGISToolExecuteView::OnSelected)
    EVT_LIST_ITEM_DESELECTED(TOOLEXECUTECTRLID, wxGISToolExecuteView::OnDeselected)
    EVT_LIST_ITEM_ACTIVATED(TOOLEXECUTECTRLID, wxGISToolExecuteView::OnActivated)

    EVT_LIST_BEGIN_DRAG(TOOLEXECUTECTRLID, wxGISToolExecuteView::OnBeginDrag)
    EVT_LIST_BEGIN_RDRAG(TOOLEXECUTECTRLID, wxGISToolExecuteView::OnBeginDrag)

    EVT_CONTEXT_MENU(wxGISToolExecuteView::OnContextMenu)
    EVT_CHAR(wxGISToolExecuteView::OnChar)
	EVT_GXOBJECT_REFRESHED(wxGISToolExecuteView::OnObjectRefreshed)
	EVT_GXOBJECT_ADDED(wxGISToolExecuteView::OnObjectAdded)
	EVT_GXOBJECT_DELETED(wxGISToolExecuteView::OnObjectDeleted)
	EVT_GXOBJECT_CHANGED(wxGISToolExecuteView::OnObjectChanged)
	EVT_GXSELECTION_CHANGED(wxGISToolExecuteView::OnSelectionChanged)
END_EVENT_TABLE()

wxGISToolExecuteView::wxGISToolExecuteView(void) : wxListCtrl()
{
    m_sViewName = wxString(_("Tool Execute View"));

    m_pXmlConf = NULL;
	m_nParentGxObjectId = wxNOT_FOUND;
    m_HighLightItem = wxNOT_FOUND;
    m_bDropping = false;
}

wxGISToolExecuteView::wxGISToolExecuteView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : m_ConnectionPointCatalogCookie(wxNOT_FOUND), m_pSelection(NULL)
{
    m_sViewName = wxString(_("Tool Execute View"));

    m_pXmlConf = NULL;
	m_nParentGxObjectId = wxNOT_FOUND;
    m_HighLightItem = wxNOT_FOUND;
    m_bDropping = false;
    Create(parent, id, pos, size, style);
}

wxGISToolExecuteView::~wxGISToolExecuteView(void)
{
}

bool wxGISToolExecuteView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_ConnectionPointCatalogCookie = wxNOT_FOUND;
    m_nParentGxObjectId = wxNOT_FOUND;
    m_pSelection = NULL;

    wxListCtrl::Create(parent, TOOLEXECUTECTRLID, pos, size, style);

	m_ImageList.Create(16, 16);
	//set default icons
    m_ImageList.Add(wxBitmap(state_xpm));
	SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);

    SetDropTarget(new wxGISTaskDropTarget((this)));

    return true;
}

void wxGISToolExecuteView::InitColumns(void)
{
    if(GetColumnCount() < 6)
    {
	    InsertColumn(0, _("Priority"), wxLIST_FORMAT_LEFT, 60);         //priority
        InsertColumn(1, _("Tool name"), wxLIST_FORMAT_LEFT, 150);       //toolname
        InsertColumn(2, _("Start"), wxLIST_FORMAT_LEFT, 100);           //begin
        InsertColumn(3, _("Finish"), wxLIST_FORMAT_LEFT, 100);          //end
        InsertColumn(4, _("Done %"), wxLIST_FORMAT_CENTER, 50);         //percent
        InsertColumn(5, _("Last message"), wxLIST_FORMAT_LEFT, 350);    //current message
    }
}

bool wxGISToolExecuteView::Activate(IApplication* const pApplication, wxXmlNode* const pConf)
{
	if(!wxGxView::Activate(pApplication, pConf))
		return false;

    m_pApp = dynamic_cast<wxGxApplication*>(pApplication);
    if(!m_pApp)
        return false;

    m_pSelection = m_pApp->GetGxSelection();

    if(!GetGxCatalog())
		return false;
    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);

    //delete
    m_pDeleteCmd = m_pApp->GetCommand(wxT("wxGISCatalogMainCmd"), 4);

    m_ConnectionPointCatalogCookie = m_pCatalog->Advise(this);

    if(m_pSelection)
        m_ConnectionPointSelectionCookie = m_pSelection->Advise(this);

    InitColumns();
    Serialize(m_pXmlConf, false);
	return true;
}

void wxGISToolExecuteView::Deactivate(void)
{
    Serialize(m_pXmlConf, true);	
    
    if(m_ConnectionPointCatalogCookie != wxNOT_FOUND)
        m_pCatalog->Unadvise(m_ConnectionPointCatalogCookie);
    if(m_ConnectionPointSelectionCookie != wxNOT_FOUND && m_pSelection)
        m_pSelection->Unadvise(m_ConnectionPointSelectionCookie);

	wxGxView::Deactivate();

}

void wxGISToolExecuteView::Serialize(wxXmlNode* pRootNode, bool bStore)
{
	if(pRootNode == NULL)
		return;

	if(bStore)
	{
        //store col width
        wxString sCols;
        for(size_t i = 0; i < GetColumnCount(); ++i)
        {
            sCols += wxString::Format(wxT("%d"), GetColumnWidth(i));
            sCols += wxT("|");
        }
        if(pRootNode->HasAttribute(wxT("cols_width")))
            pRootNode->DeleteAttribute(wxT("cols_width"));
        pRootNode->AddAttribute(wxT("cols_width"), sCols);
	}
	else
	{
        //load col width
        wxString sCol = pRootNode->GetAttribute(wxT("cols_width"), wxT(""));
	    wxStringTokenizer tkz(sCol, wxString(wxT("|")), wxTOKEN_RET_EMPTY );
        int col_counter(0);
	    while ( tkz.HasMoreTokens() )
	    {
            if(col_counter >= GetColumnCount())
                break;
		    wxString token = tkz.GetNextToken();
		    //token.Replace(wxT("|"), wxT(""));
		    int nWidth = wxAtoi(token);
            SetColumnWidth(col_counter, nWidth);
            col_counter++;
	    }
 	}
}

void wxGISToolExecuteView::OnContextMenu(wxContextMenuEvent& event)
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

void wxGISToolExecuteView::OnSelected(wxListEvent& event)
{
    m_pSelection->Clear(NOTFIRESELID);
    long nItem = wxNOT_FOUND;
	size_t nCount(0);
    for ( ;; )
    {
        nItem = GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( nItem == wxNOT_FOUND )
            break;
		wxGxObject* pObject = m_pCatalog->GetRegisterObject(GetItemData(nItem));
	    if(!pObject)
            continue;
		nCount++;
        m_pSelection->Select(pObject->GetId(), true, NOTFIRESELID);
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

bool wxGISToolExecuteView::Show(bool show)
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


void wxGISToolExecuteView::OnDeselected(wxListEvent& event)
{
   if(GetSelectedItemCount() == 0)
        m_pSelection->Select(m_nParentGxObjectId, false, NOTFIRESELID);

	wxGxObject* pObject = m_pCatalog->GetRegisterObject(event.GetData());
	if(!pObject)
		return;

	m_pSelection->Unselect(pObject->GetId(), NOTFIRESELID);

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

void wxGISToolExecuteView::ShowContextMenu(const wxPoint& pos)
{
	long item = wxNOT_FOUND;
    item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if(item == wxNOT_FOUND)
	{
		wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(m_nParentGxObjectId);
		IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pGxObject);
        if(pGxObjectUI)
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
		return;
	}

	wxGxObject* pObject = m_pCatalog->GetRegisterObject(GetItemData(item));
	if(!pObject)
		return;

	bool bAdd = true;
	m_pSelection->Select(pObject->GetId(), bAdd, NOTFIRESELID);

	IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pObject);
	if(pGxObjectUI)
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

void wxGISToolExecuteView::OnActivated(wxListEvent& event)
{
	//event.Skip();
	wxGxObject* pObject = m_pCatalog->GetRegisterObject(event.GetData());
	if(!pObject)
		return;

	IGxObjectWizard* pGxObjectWizard = dynamic_cast<IGxObjectWizard*>(pObject);
	if(pGxObjectWizard != NULL)
		pGxObjectWizard->Invoke(this);
}

void wxGISToolExecuteView::OnObjectAdded(wxGxCatalogEvent& event)
{
	wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
	if(!pGxObject)
		return;

	wxGxObject* pParentGxObject = pGxObject->GetParent();
	if(pParentGxObject && pParentGxObject->GetId() == m_nParentGxObjectId)
	{
		AddObject(pGxObject);
        SortItems(GxTaskCompareFunction, 0);
	}
}

void wxGISToolExecuteView::OnObjectDeleted(wxGxCatalogEvent& event)
{
	for(long i = 0; i < GetItemCount(); ++i)
	{
		if(GetItemData(i) == event.GetObjectID())
			DeleteItem(i);
	}
}

void wxGISToolExecuteView::OnObjectRefreshed(wxGxCatalogEvent& event)
{
    if(m_nParentGxObjectId == event.GetObjectID())
        RefreshAll();

	wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
	if(!pGxObject)
		return;
	wxGxObject* pParentGxObject = pGxObject->GetParent();
    if(pParentGxObject && pParentGxObject->GetId() == m_nParentGxObjectId)
    {
	    OnObjectChanged(event);
    }
}

void wxGISToolExecuteView::RefreshAll(void)
{
	wxBusyCursor wait;
    ResetContents();
	wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(m_nParentGxObjectId);
	wxGxObjectContainer* pObjContainer =  wxDynamicCast(pGxObject, wxGxObjectContainer);
	if( pObjContainer == NULL )
		return;
    if( !pObjContainer->HasChildren() )
        return;

	wxGxObjectList ObjectList = pObjContainer->GetChildren();
    wxGxObjectList::iterator iter;
    for (iter = ObjectList.begin(); iter != ObjectList.end(); ++iter)
    {
        wxGxObject *current = *iter;
		AddObject(current);
    }

    SortItems(GxTaskCompareFunction, 0);
	wxListCtrl::Refresh();
}

void wxGISToolExecuteView::OnSelectionChanged(wxGxSelectionEvent& event)
{
	if(event.GetInitiator() == GetId())
		return;
    long nSelId = m_pSelection->GetLastSelectedObjectId();
    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(nSelId);

	wxBusyCursor wait;
	//reset
	ResetContents();
	m_nParentGxObjectId = nSelId;

	wxGxObjectContainer* pObjContainer =  wxDynamicCast(pGxObject, wxGxObjectContainer);
	if(pObjContainer == NULL)
		return;
    if(!pObjContainer->HasChildren())
        return;

	wxGxObjectList ObjectList = pObjContainer->GetChildren();
    wxGxObjectList::iterator iter;
    for (iter = ObjectList.begin(); iter != ObjectList.end(); ++iter)
    {
        wxGxObject *current = *iter;
		AddObject(current);
    }

    SortItems(GxTaskCompareFunction, 0);
	//wxListCtrl::Refresh();
}

void wxGISToolExecuteView::ResetContents(void)
{
	DeleteAllItems();
}

void wxGISToolExecuteView::SelectAll(void)
{
	for(long item = 0; item < GetItemCount(); ++item)
        SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void wxGISToolExecuteView::OnChar(wxKeyEvent& event)
{
	if(event.GetModifiers() & wxMOD_ALT)
		return;
	if(event.GetModifiers() & wxMOD_CONTROL)
		return;
	if(event.GetModifiers() & wxMOD_SHIFT)
		return;
    switch(event.GetKeyCode())
    {
    case WXK_DELETE:
    case WXK_NUMPAD_DELETE:
        if(m_pDeleteCmd)
            m_pDeleteCmd->OnClick();
        break;
    case WXK_UP:
        {
            long nSelItem = GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if(nSelItem == wxNOT_FOUND)
                SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            else 
            {
                SetItemState(nSelItem, wxLIST_STATE_DONTCARE, wxLIST_STATE_SELECTED);
                if(nSelItem == 0)
                {             
                    SetItemState(GetItemCount() - 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                }
                else
                {
                    SetItemState(nSelItem - 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                }
            }            
        }
        break;
    case WXK_DOWN:
        {
            long nSelItem = GetNextItem(wxNOT_FOUND, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
            if(nSelItem == wxNOT_FOUND)
                SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
            else
            {
                SetItemState(nSelItem, wxLIST_STATE_DONTCARE, wxLIST_STATE_SELECTED);
                if(nSelItem == GetItemCount() - 1)
                {             
                    SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                }
                else
                {
                    SetItemState(nSelItem + 1, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
                }
            }
        }
        break;
    default:
        break;
    }
}


void wxGISToolExecuteView::OnBeginDrag(wxListEvent& event)
{
    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(m_nParentGxObjectId);
    if(!pGxObject)
        return;
    wxGISTaskDataObject DragData(wxThread::GetMainId(), wxDataFormat(wxT("application/x-vnd.wxgis.gxtask-id")));


    long nItem = wxNOT_FOUND;
    int nCount(0);
    for ( ;; )
    {
        nItem = GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( nItem == wxNOT_FOUND )
            break;

        DragData.AddDecimal(GetItemData(nItem));
    }

    wxDropSource dragSource( this );
	dragSource.SetData( DragData );
	wxDragResult result = dragSource.DoDragDrop( wxDrag_DefaultMove );  
}


wxDragResult wxGISToolExecuteView::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
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


wxDragResult wxGISToolExecuteView::OnEnter(wxCoord x, wxCoord y, wxDragResult def)
{
    return def;
}

bool wxGISToolExecuteView::OnDropObjects(wxCoord x, wxCoord y, long nParentPointer, const wxArrayLong& TaskIds)
{
    wxBusyCursor wait;
    SetItemState(m_HighLightItem, 0, wxLIST_STATE_DROPHILITED);
    //SetItemState(m_HighLightItem, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);

    if(nParentPointer != wxThread::GetMainId())
        return false;

    wxPoint pt(x, y);
	unsigned long nFlags(0);
	long nItemId = HitTest(pt, (int &)nFlags);
    long nObjectID(m_nParentGxObjectId);

    if(nItemId == wxNOT_FOUND)
        return false;

    IGxTask* pBeforeTask(NULL);
    int nInsertPoint = GetItemCount() - TaskIds.GetCount(); //the end
	if(nFlags & wxLIST_HITTEST_ONITEM)
    {
        nInsertPoint = nItemId;// - 1;
        pBeforeTask = dynamic_cast<IGxTask*>(m_pCatalog->GetRegisterObject(GetItemData(nInsertPoint)));
    }
 
    //1. Read all items to map sortig using priority, but not adding moving items
    m_bDropping = true;
    std::map<long, IGxTask*> ItemsMap;
    for(size_t i = 0; i < GetItemCount(); ++i)
    {
        long nTaskId = GetItemData(i);
        if(TaskIds.Index(nTaskId) == wxNOT_FOUND)
        {
            IGxTask* pTask = dynamic_cast<IGxTask*>(m_pCatalog->GetRegisterObject(nTaskId));
            if(pTask && pTask->GetState() != enumGISTaskDone)
            {
                ItemsMap[pTask->GetPriority()] = pTask;
            }
        }
    }

    //wxWindowUpdateLocker noUpdates(this);

    //get min priority
    if(nInsertPoint < 0)
    {
        long nMinPrio = ItemsMap.begin()->first;
        for(size_t i = 0; i < TaskIds.GetCount(); ++i)
        {
            IGxTask* pTask = dynamic_cast<IGxTask*>(m_pCatalog->GetRegisterObject(TaskIds[i]));
            if(pTask)
            {
                pTask->SetPriority(++nMinPrio);
            }
        }
        for(std::map<long, IGxTask*>::iterator it = ItemsMap.begin(); it != ItemsMap.end(); ++it)
        {
            it->second->SetPriority(++nMinPrio);
        }
    }
    else if(nInsertPoint >= TaskIds.GetCount())
    {
        long nMinPrio = ItemsMap.begin()->first;
        for(std::map<long, IGxTask*>::iterator it = ItemsMap.begin(); it != ItemsMap.end(); ++it)
        {
            it->second->SetPriority(++nMinPrio);
        }

        for(size_t i = 0; i < TaskIds.GetCount(); ++i)
        {
            IGxTask* pTask = dynamic_cast<IGxTask*>(m_pCatalog->GetRegisterObject(TaskIds[i]));
            if(pTask)
            {
                pTask->SetPriority(++nMinPrio);
            }
        }
    }
    else
    {
        //2. Read from map until end for insert to array
        bool bSetPrio(false);
        long nMinPrio = wxNOT_FOUND;
        for(std::map<long, IGxTask*>::iterator it = ItemsMap.begin(); it != ItemsMap.end(); ++it)
        {
            if(pBeforeTask == it->second)
            {
                nMinPrio = pBeforeTask->GetPriority();
                for(size_t i = 0; i < TaskIds.GetCount(); ++i)
                {
                    IGxTask* pTask = dynamic_cast<IGxTask*>(m_pCatalog->GetRegisterObject(TaskIds[i]));
                    if(pTask)
                    {
                        pTask->SetPriority(++nMinPrio);
                    }
                }
                bSetPrio = true;
            }
            if(bSetPrio)
                it->second->SetPriority(++nMinPrio);
        }
    }

    SortItems(GxTaskCompareFunction, 0);

    m_bDropping = false;

    return true;
}

void wxGISToolExecuteView::OnLeave()
{
    SetItemState(m_HighLightItem, 0, wxLIST_STATE_DROPHILITED);
}

/*
#include "wxgis/geoprocessingui/gptoolbox.h"
#include "wxgis/geoprocessingui/gptaskexecdlg.h"
#include "wxgis/framework/droptarget.h"
#include "../../art/small_arrow.xpm"

//----------------------------------------------------------------------------
// wxGxToolExecuteView
//----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxToolExecuteView, wxListCtrl)

BEGIN_EVENT_TABLE(wxGxToolExecuteView, wxListCtrl)
    EVT_LIST_BEGIN_DRAG(TOOLEXECUTECTRLID, wxGxToolExecuteView::OnBeginDrag)
    EVT_LIST_ITEM_SELECTED(TOOLEXECUTECTRLID, wxGxToolExecuteView::OnSelected)
    EVT_LIST_ITEM_DESELECTED(TOOLEXECUTECTRLID, wxGxToolExecuteView::OnDeselected)
    EVT_LIST_ITEM_ACTIVATED(TOOLEXECUTECTRLID, wxGxToolExecuteView::OnActivated)
    EVT_LIST_COL_CLICK(TOOLEXECUTECTRLID, wxGxToolExecuteView::OnColClick)
    EVT_CONTEXT_MENU(wxGxToolExecuteView::OnContextMenu)
    EVT_CHAR(wxGxToolExecuteView::OnChar)
	EVT_GXOBJECT_REFRESHED(wxGxToolExecuteView::OnObjectRefreshed)
	EVT_GXOBJECT_ADDED(wxGxToolExecuteView::OnObjectAdded)
	EVT_GXOBJECT_DELETED(wxGxToolExecuteView::OnObjectDeleted)
	EVT_GXOBJECT_CHANGED(wxGxToolExecuteView::OnObjectChanged)
	EVT_GXSELECTION_CHANGED(wxGxToolExecuteView::OnSelectionChanged)
END_EVENT_TABLE()

int wxCALLBACK TasksCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
	IGxObject* pObject1 = (IGxObject*)item1;
 	IGxObject* pObject2 = (IGxObject*)item2;

    IGxTask* pTask1 = dynamic_cast<IGxTask*>(pObject1);
    IGxTask* pTask2 = dynamic_cast<IGxTask*>(pObject2);
    if(!pTask1)
        return -1;
    if(!pTask2)
        return 1;

    int nRes = 0;
    LPSORTTASKDATA psortdata = (LPSORTTASKDATA)sortData;
    switch(psortdata->currentSortCol)
    {
    case 0:
        nRes = (pTask1->GetPriority() > pTask2->GetPriority()) == true ? 1 : -1;
        break;
    case 1:
        nRes = pObject1->GetName().CmpNoCase(pObject2->GetName());
        break;
    case 2:
        nRes = (pTask1->GetStart() > pTask2->GetStart()) == true ? 1 : -1;
        break;
    case 3:
        nRes = (pTask1->GetFinish() > pTask2->GetFinish()) == true ? 1 : -1;
        break;
    case 4:
        nRes = (pTask1->GetDonePercent() > pTask2->GetDonePercent()) == true ? 1 : -1;
        break;
    case 5:
        nRes = pTask1->GetLastMessage().CmpNoCase(pTask2->GetLastMessage());
        break;
    };
   return nRes * (psortdata->bSortAsc == 0 ? -1 : 1);
}

wxGxToolExecuteView::wxGxToolExecuteView(void)
{
    m_pXmlConf = NULL;
	m_nParentGxObjectID = wxNOT_FOUND;
}

wxGxToolExecuteView::wxGxToolExecuteView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : m_bSortAsc(true), m_pConnectionPointCatalog(NULL), m_ConnectionPointCatalogCookie(wxNOT_FOUND), m_currentSortCol(0), m_pSelection(NULL), m_bHideDone(false)//wxListCtrl(parent, id, pos, size, style),
{
    m_pXmlConf = NULL;
	m_nParentGxObjectID = wxNOT_FOUND;
    Create(parent, id, pos, size, style);
}

wxGxToolExecuteView::~wxGxToolExecuteView(void)
{
	ResetContents();
}

bool wxGxToolExecuteView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    //SetDropTarget(new wxGISCatalogDropTarget(static_cast<IGxViewDropTarget*>(this)));
    m_HighLightItem = wxNOT_FOUND;
    m_bSortAsc = true;
    m_bHideDone = false;
    m_pConnectionPointCatalog = NULL;
    m_ConnectionPointCatalogCookie = wxNOT_FOUND;
    m_nParentGxObjectID = wxNOT_FOUND;
    m_currentSortCol = 0;
    m_pSelection = NULL;

    m_sViewName = wxString(_("Tool Execute View"));


    wxListCtrl::Create(parent, TOOLEXECUTECTRLID, pos, size, TOOLEXECVIEWSTYLE);

	InsertColumn(0, _("Priority"), wxLIST_FORMAT_LEFT, 60);      //priority
    InsertColumn(1, _("Tool name"), wxLIST_FORMAT_LEFT, 150);    //toolname
    InsertColumn(2, _("Start"), wxLIST_FORMAT_LEFT, 100);        //begin
    InsertColumn(3, _("Finish"), wxLIST_FORMAT_LEFT, 100);       //end
    InsertColumn(4, _("Done %"), wxLIST_FORMAT_CENTER, 50);      //percent
    InsertColumn(5, _("Last message"), wxLIST_FORMAT_LEFT, 350); //current message

	m_ImageList.Create(16, 16);

	//set default icons
	//col ico & default
    wxBitmap SmallA(small_arrow_xpm);// > arrow
    wxImage SmallImg = SmallA.ConvertToImage();
    SmallImg = SmallImg.Rotate90();
    wxBitmap SmallDown(SmallImg);
    SmallImg = SmallImg.Mirror(false);
    wxBitmap SmallUp(SmallImg);

	m_ImageList.Add(wxBitmap(SmallDown));
	m_ImageList.Add(wxBitmap(SmallUp));
    m_ImageList.Add(wxBitmap(state_xpm));

	SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);
    return true;
}

bool wxGxToolExecuteView::Activate(IFrameApplication* application, wxXmlNode* pConf)
{
	if(!wxGxView::Activate(application, pConf))
		return false;

	Serialize(m_pXmlConf, false);

    m_pCatalog = dynamic_cast<wxGxCatalogUI*>(m_pGxApplication->GetCatalog());

	//delete
    m_pDeleteCmd = application->GetCommand(wxT("wxGISCatalogMainCmd"), 4);

	m_pConnectionPointCatalog = dynamic_cast<wxGISConnectionPointContainer*>( m_pCatalog );
	if(m_pConnectionPointCatalog != NULL)
		m_ConnectionPointCatalogCookie = m_pConnectionPointCatalog->Advise(this);

	if(m_pCatalog)
		m_pSelection = m_pCatalog->GetSelection();

	return true;
}

void wxGxToolExecuteView::Deactivate(void)
{
	if(m_ConnectionPointCatalogCookie != wxNOT_FOUND)
		m_pConnectionPointCatalog->Unadvise(m_ConnectionPointCatalogCookie);

	Serialize(m_pXmlConf, true);
	wxGxView::Deactivate();
}

void wxGxToolExecuteView::Serialize(wxXmlNode* pRootNode, bool bStore)
{
	if(pRootNode == NULL)
		return;

	if(bStore)
	{
        if(pRootNode->HasAttribute(wxT("sort")))
            pRootNode->DeleteAttribute(wxT("sort"));
        pRootNode->AddAttribute(wxT("sort"), wxString::Format(wxT("%d"), m_bSortAsc));
        if(pRootNode->HasAttribute(wxT("sort_col")))
            pRootNode->DeleteAttribute(wxT("sort_col"));
        pRootNode->AddAttribute(wxT("sort_col"), wxString::Format(wxT("%d"), m_currentSortCol));

        //store col width
        wxString sCols;
        for(size_t i = 0; i < GetColumnCount(); ++i)
        {
            sCols += wxString::Format(wxT("%d"), GetColumnWidth(i));
            sCols += wxT("|");
        }
        if(pRootNode->HasAttribute(wxT("cols_width")))
            pRootNode->DeleteAttribute(wxT("cols_width"));
        pRootNode->AddAttribute(wxT("cols_width"), sCols);
	}
	else
	{
		m_bSortAsc = wxAtoi(pRootNode->GetAttribute(wxT("sort"), wxT("1"))) == 1;
		m_currentSortCol = wxAtoi(pRootNode->GetAttribute(wxT("sort_col"), wxT("0")));
        //load col width
        wxString sCol = pRootNode->GetAttribute(wxT("cols_width"), wxT(""));
	    wxStringTokenizer tkz(sCol, wxString(wxT("|")), wxTOKEN_RET_EMPTY );
        int col_counter(0);
	    while ( tkz.HasMoreTokens() )
	    {
            if(col_counter >= GetColumnCount())
                break;
		    wxString token = tkz.GetNextToken();
		    //token.Replace(wxT("|"), wxT(""));
		    int nWidth = wxAtoi(token);
            SetColumnWidth(col_counter, nWidth);
            col_counter++;
	    }

        SORTTASKDATA sortdata = {m_bSortAsc, m_currentSortCol};
	    SortItems(TasksCompareFunction, (long)&sortdata);
	    SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
	}
}

void wxGxToolExecuteView::AddObject(IGxObject* pObject)
{
	if(pObject == NULL)
		return;
	wxGxTaskObject* pGxTask =  dynamic_cast<wxGxTaskObject*>(pObject);
	if(pGxTask != NULL)
	{
#ifdef __WXGTK__
        if(GetColumnCount() < 6)
        {
		InsertColumn(0, _("Priority"), wxLIST_FORMAT_LEFT, 150);    //priority
        InsertColumn(1, _("Tool name"), wxLIST_FORMAT_LEFT, 150);    //toolname
        InsertColumn(2, _("Start"), wxLIST_FORMAT_LEFT, 100);        //begin
        InsertColumn(3, _("Finish"), wxLIST_FORMAT_LEFT, 100);       //end
        InsertColumn(4, _("Done %"), wxLIST_FORMAT_CENTER, 50);      //percent
        InsertColumn(5, _("Last message"), wxLIST_FORMAT_LEFT, 350); //current message
        }
#endif
        // enumGISTaskWork = 1,    enumGISTaskDone = 2,    enumGISTaskQuered = 3,     enumGISTaskPaused = 4,     enumGISTaskError = 5
        //4 - error, 5 - warning, 6 - work, 7 - queued, 8 - done, 9 - paused, 10 - deleted
        char nIcon = wxNOT_FOUND;
        wxColour color;
        switch(pGxTask->GetState())
        {
        case enumGISTaskWork:
            color = wxColour(230,255,230);
            nIcon = 6;
            break;
        case enumGISTaskDone:
            if(m_bHideDone)
                return;
            color = wxColour(230,230,255);
            nIcon = 8;
            break;
        case enumGISTaskQuered:
            color = wxColour(255,230,255);
            nIcon = 7;
            break;
        case enumGISTaskPaused:
            color = wxColour(255,255,230);
            nIcon = 9;
            break;
        case enumGISTaskError:
            color = wxColour(255,230,230);
            nIcon = 4;
            break;
        };

		long ListItemID = InsertItem(0, wxString::Format(wxT("%d"), pGxTask->GetPriority() + 1), nIcon);
	    SetItem(ListItemID, 1, pObject->GetName());
        wxDateTime dts = pGxTask->GetStart();
        if(dts.IsValid())
            SetItem(ListItemID, 2, dts.Format(_("%d-%m-%Y %H:%M:%S")));
        wxDateTime dtf = pGxTask->GetFinish();
        if(dtf.IsValid())
            SetItem(ListItemID, 3, pGxTask->GetFinish().Format(_("%d-%m-%Y %H:%M:%S")));
        SetItem(ListItemID, 4, wxString::Format(_("%.1f%%"), pGxTask->GetDonePercent()));
        wxString sLastMsg = pGxTask->GetLastMessage();
        if(!sLastMsg.IsEmpty())
            SetItem(ListItemID, 5, sLastMsg);

        SetItemBackgroundColour(ListItemID, color);
        SetItemData(ListItemID, pObject->GetID());
	}
}

void wxGxToolExecuteView::OnColClick(wxListEvent& event)
{
    m_currentSortCol = event.GetColumn();
	m_bSortAsc = !m_bSortAsc;

    SORTTASKDATA sortdata = {m_bSortAsc, m_currentSortCol};
	SortItems(TasksCompareFunction, (long)&sortdata);
	SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
}

void wxGxToolExecuteView::OnContextMenu(wxContextMenuEvent& event)
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

void wxGxToolExecuteView::OnSelected(wxListEvent& event)
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
		IGxObjectSPtr pObject = m_pCatalog->GetRegisterObject(GetItemData(nItem));
	    if(!pObject)
            continue;
		nCount++;
        m_pSelection->Select(pObject->GetID(), true, NOTFIRESELID);
    }
}

bool wxGxToolExecuteView::Show(bool show)
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


void wxGxToolExecuteView::OnDeselected(wxListEvent& event)
{
	//event.Skip();
    if(GetSelectedItemCount() == 0)
        m_pSelection->Select(m_nParentGxObjectID, false, NOTFIRESELID);

	IGxObjectSPtr pObject = m_pCatalog->GetRegisterObject(event.GetData());
	if(!pObject)
		return;

	m_pSelection->Unselect(pObject->GetID(), NOTFIRESELID);
}

void wxGxToolExecuteView::ShowContextMenu(const wxPoint& pos)
{
	long item = wxNOT_FOUND;
    item = GetNextItem(item, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	if(item == wxNOT_FOUND)
	{
		IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(m_nParentGxObjectID);
		IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pGxObject.get());
        if(pGxObjectUI)
        {
            wxString psContextMenu = pGxObjectUI->ContextMenu();
            IFrameApplication* pApp = dynamic_cast<IFrameApplication*>(m_pGxApplication);
            if(pApp)
            {
                wxMenu* pMenu = dynamic_cast<wxMenu*>(pApp->GetCommandBar(psContextMenu));
                if(pMenu)
                {
                    PopupMenu(pMenu, pos.x, pos.y);
                }
            }
        }
		return;
	}

	IGxObjectSPtr pObject = m_pCatalog->GetRegisterObject(GetItemData(item));
	if(!pObject)
		return;

	bool bAdd = true;
	m_pSelection->Select(pObject->GetID(), bAdd, NOTFIRESELID);

	IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pObject.get());
	if(pGxObjectUI != NULL)
	{
        wxString psContextMenu = pGxObjectUI->ContextMenu();
        IFrameApplication* pApp = dynamic_cast<IFrameApplication*>(m_pGxApplication);
        if(pApp)
        {
            wxMenu* pMenu = dynamic_cast<wxMenu*>(pApp->GetCommandBar(psContextMenu));
            if(pMenu)
            {
                PopupMenu(pMenu, pos.x, pos.y);
            }
        }
	}
}

void wxGxToolExecuteView::SetColumnImage(int col, int image)
{
    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);

    //reset image
    item.SetImage(wxNOT_FOUND);
    for(size_t i = 0; i < GetColumnCount(); ++i)
        SetColumn(i, item);

    item.SetImage(image);
    SetColumn(col, item);
}

void wxGxToolExecuteView::OnActivated(wxListEvent& event)
{
	//event.Skip();
	IGxObjectSPtr pObject = m_pCatalog->GetRegisterObject(event.GetData());
	if(!pObject)
		return;

	IGxObjectWizard* pGxObjectWizard = dynamic_cast<IGxObjectWizard*>(pObject.get());
	if(pGxObjectWizard != NULL)
		pGxObjectWizard->Invoke(this);
}

void wxGxToolExecuteView::OnObjectAdded(wxGxCatalogEvent& event)
{
	IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
	if(!pGxObject)
		return;

	IGxObject* pParentGxObject = pGxObject->GetParent();
	if(!pParentGxObject)
		return;
	if(pParentGxObject->GetID() == m_nParentGxObjectID)
	{
		AddObject(pGxObject.get());
		//SORTTASKDATA sortdata = {m_bSortAsc, m_currentSortCol};
		//SortItems(TasksCompareFunction, (long)&sortdata);
	    //wxListCtrl::Refresh();
	}
}

void wxGxToolExecuteView::OnObjectDeleted(wxGxCatalogEvent& event)
{
	IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
	if(!pGxObject)
		return;
	for(long i = 0; i < GetItemCount(); ++i)
	{
		if(GetItemData(i) == pGxObject->GetID())
			DeleteItem(i);
	}
	//wxListCtrl::Refresh();
}

void wxGxToolExecuteView::OnObjectChanged(wxGxCatalogEvent& event)
{
	IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
	if(!pGxObject)
		return;
    //find item
    int nItem = wxNOT_FOUND;
    for(size_t i = 0; i < GetItemCount(); ++i)
    {
		if(GetItemData(i) == pGxObject->GetID())
        {
            nItem = i;
            break;
        }
    }
    if(nItem == wxNOT_FOUND)
        return;

    wxGxTaskObject* pGxTask =  dynamic_cast<wxGxTaskObject*>(pGxObject.get());
    if(pGxTask != NULL)
    {
        // enumGISTaskWork = 1,    enumGISTaskDone = 2,    enumGISTaskQuered = 3,     enumGISTaskPaused = 4,     enumGISTaskError = 5
        //4 - error, 5 - warning, 6 - work, 7 - queued, 8 - done, 9 - paused, 10 - deleted
        char nIcon = wxNOT_FOUND;
        wxColour color;
        switch(pGxTask->GetState())
        {
        case enumGISTaskWork:
            color = wxColour(230,255,230);
            nIcon = 6;
            break;
        case enumGISTaskDone:
            color = wxColour(230,230,255);
            nIcon = 8;
            break;
        case enumGISTaskQuered:
            color = wxColour(255,230,255);
            nIcon = 7;
            break;
        case enumGISTaskPaused:
            color = wxColour(255,255,230);
            nIcon = 9;
            break;
        case enumGISTaskError:
            color = wxColour(255,230,230);
            nIcon = 4;
            break;
        };

		SetItem(nItem, 0, wxString::Format(wxT("%d"), pGxTask->GetPriority() + 1), nIcon);

        SetItem(nItem, 1, pGxObject->GetName());
        wxDateTime dtb = pGxTask->GetStart();
        if(dtb.IsValid())
            SetItem(nItem, 2, dtb.Format(_("%d-%m-%Y %H:%M:%S")));
        wxDateTime dtf = pGxTask->GetFinish();
        if(dtf.IsValid())
            SetItem(nItem, 3, dtf.Format(_("%d-%m-%Y %H:%M:%S")));
        SetItem(nItem, 4, wxString::Format(_("%.1f%%"), pGxTask->GetDonePercent()));
        SetItem(nItem, 5, pGxTask->GetLastMessage());

        SetItemBackgroundColour(nItem, color);

	    //wxListCtrl::Refresh();
    }
}

void wxGxToolExecuteView::OnObjectRefreshed(wxGxCatalogEvent& event)
{
    if(m_nParentGxObjectID == event.GetObjectID())
        RefreshAll();

	IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
	if(!pGxObject)
		return;
	IGxObject* pParentGxObject = pGxObject->GetParent();
	if(!pParentGxObject)
		return;

    if(pParentGxObject->GetID() == m_nParentGxObjectID)
    {
	    OnObjectChanged(event);
    }
}

void wxGxToolExecuteView::RefreshAll(void)
{
	wxBusyCursor wait;
    ResetContents();
	IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(m_nParentGxObjectID);
	IGxObjectContainer* pObjContainer =  dynamic_cast<IGxObjectContainer*>(pGxObject.get());
	if(pObjContainer == NULL || !pObjContainer->HasChildren())
		return;
	GxObjectArray* pArr = pObjContainer->GetChildren();
	for(size_t i = 0; i < pArr->size(); ++i)
	{
		AddObject(pArr->at(i));
	}

	wxListCtrl::Refresh();

    //SORTTASKDATA sortdata = {m_bSortAsc, m_currentSortCol};
	//SortItems(TasksCompareFunction, (long)&sortdata);
	//SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
}

void wxGxToolExecuteView::OnSelectionChanged(wxGxSelectionEvent& event)
{
	if(event.GetInitiator() == GetId())
		return;
    long nSelID = m_pSelection->GetLastSelectedObjectID();
    IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(nSelID);
	//IGxObject* pGxObj = m_pSelection->GetLastSelectedObject();
	//if(m_pParentGxObject == pGxObj)
	//	return;

	wxBusyCursor wait;
	//reset
	ResetContents();
	m_nParentGxObjectID = nSelID;

	IGxObjectContainer* pObjContainer =  dynamic_cast<IGxObjectContainer*>(pGxObject.get());
	if(pObjContainer == NULL || !pObjContainer->HasChildren())
		return;

	GxObjectArray* pArr = pObjContainer->GetChildren();
	for(size_t i = 0; i < pArr->size(); ++i)
	{
		AddObject(pArr->at(i));
	}

	wxListCtrl::Refresh();

//    SORTTASKDATA sortdata = {m_bSortAsc, m_currentSortCol};
//	SortItems(TasksCompareFunction, (long)&sortdata);
//	SetColumnImage(m_currentSortCol, m_bSortAsc ? 0 : 1);
}

bool wxGxToolExecuteView::Applies(wxGxSelection* const pSelection)
{
	if(Selection == NULL)
		return false;

	for(size_t i = 0; i < Selection->GetCount(); ++i)
	{
        IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(Selection->GetSelectedObjectID(i));
		wxGxToolExecute* pGxToolExecute = dynamic_cast<wxGxToolExecute*>( pGxObject.get() );
		if(pGxToolExecute != NULL)
			return true;
	}
	return false;
}

void wxGxToolExecuteView::ResetContents(void)
{
	DeleteAllItems();
}

void wxGxToolExecuteView::SelectAll(void)
{
	for(long item = 0; item < GetItemCount(); item++)
        SetItemState(item, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
}

void wxGxToolExecuteView::OnChar(wxKeyEvent& event)
{
	if(event.GetModifiers() & wxMOD_ALT)
		return;
	if(event.GetModifiers() & wxMOD_CONTROL)
		return;
	if(event.GetModifiers() & wxMOD_SHIFT)
		return;
    switch(event.GetKeyCode())
    {
    case WXK_DELETE:
    case WXK_NUMPAD_DELETE:
        if(m_pDeleteCmd)
            m_pDeleteCmd->OnClick();
        break;
    default:
        break;
    }
}

void wxGxToolExecuteView::HideDone(bool bHide)
{
    m_bHideDone = bHide;
    RefreshAll();
}

void wxGxToolExecuteView::OnBeginDrag(wxListEvent& event)
{
    long nItem = wxNOT_FOUND;
	wxArrayString saArr;
	FillDataArray(saArr);
	if(saArr.GetCount() > 0)
	{

		wxFileDataObject *pMyData = new wxFileDataObject();
        //TODO: special task clipboard format support
        //wxDataFormat frm(wxT("application/x-vnd.wxgis.task.uri"));
		//if(frm.GetType() != wxDF_INVALID)
		//	pMyData->SetFormat(frm);
		for(size_t i = 0; i < saArr.GetCount(); ++i)
			pMyData->AddFile(saArr[i]);

	    wxDropSource dragSource( this );
		dragSource.SetData( *pMyData );
		wxDragResult result = dragSource.DoDragDrop( TRUE );
		wxDELETE(pMyData);
	}
}

wxDragResult wxGxToolExecuteView::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
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

        return def;
    }
    return wxDragNone;
}

bool wxGxToolExecuteView::OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames)
{
    //TODO: special task clipboard format support
	SetItemState(m_HighLightItem, 0, wxLIST_STATE_DROPHILITED);
    wxPoint pt(x, y);
	unsigned long nFlags(0);
	long nItemId = HitTest(pt, (int &)nFlags);
	if(nItemId != wxNOT_FOUND && (nFlags & wxLIST_HITTEST_ONITEM))
	{
        IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(GetItemData(nItemId));
		wxGxTaskObject* pGxTask =  dynamic_cast<wxGxTaskObject*>(pGxObject.get());
	    if(pGxTask == NULL)
            return false;

		long nNewPriority = pGxTask->GetPriority();

		for(size_t i = 0; i < filenames.GetCount(); ++i)
		{
			wxString sItem = filenames[i];
			wxString sRest;
			if( sItem.StartsWith(wxT("TaskID: "), &sRest) )
			{
				int nTaskID = wxAtoi(sRest);
				for(size_t j = 0; j < GetItemCount(); j++ )
				{
					IGxObject* pObject = (IGxObject*)GetItemData(j);
					wxGxTaskObject* pGxTaskNew =  dynamic_cast<wxGxTaskObject*>(pObject);
					if(pGxTaskNew == NULL)
						continue;
					if(pGxTaskNew->GetTaskID() == nTaskID)
					{
						pGxTaskNew->SetPriority(nNewPriority);
						nNewPriority++;
					}
				}
			}
            else
            {
                wxMessageBox(_("Unsupported data!"), _("Error"), wxICON_ERROR | wxOK );
            }
		}
		return true;
	}
    return false;
}

void wxGxToolExecuteView::OnLeave()
{
    SetItemState(m_HighLightItem, 0, wxLIST_STATE_DROPHILITED);
}

void wxGxToolExecuteView::FillDataArray(wxArrayString &saDataArr)
{
	int nItem(wxNOT_FOUND);
    for ( ;; )
    {
        nItem = GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
        if ( nItem == wxNOT_FOUND )
            break;
        IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(GetItemData(nItem));
		wxGxTaskObject* pGxTask =  dynamic_cast<wxGxTaskObject*>(pGxObject.get());
	    if(pGxTask == NULL)
            continue;
        saDataArr.Add(wxString::Format(wxT("TaskID: %d"), pGxTask->GetTaskID()));
    }
}


IGxObjectSPtr const wxGxToolExecuteView::GetParentGxObject(void)
{
    return m_pCatalog->GetRegisterObject(m_nParentGxObjectID);
}
*/
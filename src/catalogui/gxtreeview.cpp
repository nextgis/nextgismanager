/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxTreeView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2014 Dmitry Baryshnikov
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
#include "wxgis/catalogui/gxtreeview.h"
#include "wxgis/catalogui/gxapplication.h"
#include "wxgis/catalogui/droptarget.h"
#include "wxgis/framework/dataobject.h"

#include <wx/datstrm.h>
#include <wx/stream.h>
#include <wx/sstream.h>
#include <wx/mstream.h>
#include <wx/clipbrd.h>

#include "../../art/document_16.xpm"

//-------------------------------------------------------------------------------
// wxGxTreeViewBase
//-------------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxTreeViewBase, wxTreeCtrl)

BEGIN_EVENT_TABLE(wxGxTreeViewBase, wxTreeCtrl)
    EVT_TREE_ITEM_EXPANDING(TREECTRLID, wxGxTreeViewBase::OnItemExpanding)
    EVT_TREE_ITEM_RIGHT_CLICK(TREECTRLID, wxGxTreeViewBase::OnItemRightClick)
    EVT_CHAR(wxGxTreeViewBase::OnChar)
	EVT_GXOBJECT_REFRESHED(wxGxTreeViewBase::OnObjectRefreshed)
	EVT_GXOBJECT_ADDED(wxGxTreeViewBase::OnObjectAdded)
	EVT_GXOBJECT_DELETED(wxGxTreeViewBase::OnObjectDeleted)
	EVT_GXOBJECT_CHANGED(wxGxTreeViewBase::OnObjectChanged)
	EVT_GXSELECTION_CHANGED(wxGxTreeViewBase::OnSelectionChanged)
END_EVENT_TABLE()

wxGxTreeViewBase::wxGxTreeViewBase(void) : wxTreeCtrl(), m_ConnectionPointCatalogCookie(wxNOT_FOUND), m_ConnectionPointSelectionCookie(wxNOT_FOUND)
{
    m_sViewName = wxString(_("Tree view"));
}

wxGxTreeViewBase::wxGxTreeViewBase(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxTreeCtrl(parent, id, pos, size, style, wxDefaultValidator, wxT("wxGxTreeViewBase")), m_ConnectionPointCatalogCookie(wxNOT_FOUND), m_ConnectionPointSelectionCookie(wxNOT_FOUND), m_pDeleteCmd(NULL)
{
    m_TreeImageList.Create(16, 16);
	SetImageList(&m_TreeImageList);
    m_TreeImageList.Add(wxIcon(document_16_xpm));
    m_sViewName = wxString(_("Tree view"));
}

wxGxTreeViewBase::~wxGxTreeViewBase(void)
{
}

bool wxGxTreeViewBase::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    bool result = wxTreeCtrl::Create(parent, id, pos, size, style);
    if(result)
    {
        m_TreeImageList.Create(16, 16);
        SetImageList(&m_TreeImageList);
        m_TreeImageList.Add(wxIcon(document_16_xpm));
    }
    return result;
}


void wxGxTreeViewBase::AddRoot(wxGxObject* pGxObject)
{
	if (NULL == pGxObject)
	{
		return;
	}
	IGxObjectUI* pObjUI =  dynamic_cast<IGxObjectUI*>(pGxObject);
	wxIcon icon;
	if (NULL != pObjUI)
	{
		icon = pObjUI->GetSmallImage();
	}
	int pos(-1);
	if (icon.IsOk())
	{
		pos = m_TreeImageList.Add(icon);
	}

	wxGxTreeItemData* pData = new wxGxTreeItemData(pGxObject->GetId(), pos, false);

	wxTreeItemId wxTreeItemIdRoot = wxTreeCtrl::AddRoot(pGxObject->GetName(), pos, -1, pData);
	m_TreeMap[pGxObject->GetId()] = wxTreeItemIdRoot;

	wxTreeCtrl::SetItemHasChildren(wxTreeItemIdRoot);
	wxTreeCtrl::Expand(wxTreeItemIdRoot);

	wxTreeCtrl::SortChildren(wxTreeItemIdRoot);
	//wxTreeCtrl::Refresh();
}

void wxGxTreeViewBase::AddTreeItem(wxGxObject* pGxObject, wxTreeItemId hParent)
{
	if(NULL == pGxObject)
		return;

    wxTreeItemId TreeItemId = m_TreeMap[pGxObject->GetId()];
	if(TreeItemId.IsOk())
        return;

    //wxLogDebug(wxT("wxGxTreeViewBase::AddObject %d '%s'"), pGxObject->GetId(), pGxObject->GetFullName());


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
		pos = 0;//m_ImageListSmall.Add(m_ImageListSmall.GetIcon(2));//0 col img, 1 - col img

	wxGxTreeItemData* pData = new wxGxTreeItemData(pGxObject->GetId(), pos, false);

    IGxObjectTreeAttr* pGxObjectAttr = dynamic_cast<IGxObjectTreeAttr*>(pGxObject);

    wxString sName;
	if (m_pCatalog->GetShowExt())
	{
        sName = pGxObject->GetName();
	}
	else
	{
        sName = pGxObject->GetBaseName();
	}

    if(NULL != pGxObjectAttr && pGxObjectAttr->ShowCount())
    {
        sName.Append(wxString::Format(wxT(" [%ld]"), pGxObjectAttr->GetCount()));
    }


	wxTreeItemId NewTreeItem = AppendItem(hParent, sName, pos, wxNOT_FOUND, pData);
	m_TreeMap[pGxObject->GetId()] = NewTreeItem;

	wxGxObjectContainer* pContainer = wxDynamicCast(pGxObject, wxGxObjectContainer);//dynamic_cast<IGxObjectContainer*>(pGxObject);
	if(NULL != pContainer)
    {
		if(pContainer->AreChildrenViewable())
			SetItemHasChildren(NewTreeItem);
    }

    if(NULL != pGxObjectAttr)
    {
        SetItemBold(NewTreeItem, pGxObjectAttr->IsBold());
        SetItemTextColour(NewTreeItem, pGxObjectAttr->GetColor());
    }

    SortChildren(hParent);
	//wxTreeCtrl::Refresh();
}

bool wxGxTreeViewBase::Activate(IApplication* const pApplication, wxXmlNode* const pConf)
{
	if (!wxGxView::Activate(pApplication, pConf))
		return false;

    wxGxApplicationBase* pGxApp = dynamic_cast<wxGxApplicationBase*>(pApplication);
    if (NULL == pGxApp)
	{
        return false;
	}
    m_pSelection = pGxApp->GetGxSelection();

    m_pApp = dynamic_cast<wxGISApplicationBase*>(pApplication);
	if (NULL == m_pApp)
	{
        return false;
	}
    m_pGxApp = dynamic_cast<wxGxApplication*>(pApplication);

    if (NULL == GetGxCatalog())
		return false;
    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);

    //delete
    m_pDeleteCmd = m_pApp->GetCommand(wxT("wxGISCatalogMainCmd"), 4);

    AddRoot(m_pCatalog);

    m_ConnectionPointCatalogCookie = m_pCatalog->Advise(this);

	if (NULL != m_pSelection)
	{
        m_ConnectionPointSelectionCookie = m_pSelection->Advise(this);
	}
	return true;
}

void wxGxTreeViewBase::Deactivate(void)
{
	if (m_ConnectionPointCatalogCookie != wxNOT_FOUND && NULL != m_pCatalog)
	{
        m_pCatalog->Unadvise(m_ConnectionPointCatalogCookie);
	}

	if (m_ConnectionPointSelectionCookie != wxNOT_FOUND && NULL != m_pSelection)
	{
        m_pSelection->Unadvise(m_ConnectionPointSelectionCookie);
	}

	wxGxView::Deactivate();
}

void wxGxTreeViewBase::OnSelectionChanged(wxGxSelectionEvent& event)
{
	if(event.GetInitiator() == GetId())
		return;

    long nSelId = m_pSelection->GetLastSelectedObjectId();
    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(nSelId);
	if (NULL == pGxObject)
	{
        return;
	}

	wxTreeItemId ItemId = m_TreeMap[nSelId];
	if(ItemId.IsOk())
	{
		//granted event fireing
		if(wxTreeCtrl::GetSelection() == ItemId)
			UpdateGxSelection();
		else
			wxTreeCtrl::SelectItem(ItemId, true);
		SetFocus();
	}
	else
	{
		wxGxObject* pParentGxObj = pGxObject->GetParent();
        //check if parent has no visible children
        wxGxObjectContainer* pGxObjectContainer = wxDynamicCast(pParentGxObj, wxGxObjectContainer);
		if(NULL != pGxObjectContainer && !pGxObjectContainer->AreChildrenViewable())
        {
            wxTreeItemId ParentItemId = m_TreeMap[pParentGxObj->GetId()];
            //granted event fireing
            if(wxTreeCtrl::GetSelection() == ParentItemId)
                UpdateGxSelection();
            else
                wxTreeCtrl::SelectItem(ParentItemId, true);
            SetFocus();
        }

		while(pParentGxObj)
		{
			wxTreeItemId ItemId = m_TreeMap[pParentGxObj->GetId()];
			if(ItemId.IsOk())
			{
                wxTreeCtrl::SetItemHasChildren(ItemId);
				wxTreeCtrl::Expand(ItemId);
				break;
			}
			else
				pParentGxObj = pParentGxObj->GetParent();
		}
		OnSelectionChanged(event);
	}
}

void wxGxTreeViewBase::UpdateGxSelection(void)
{
    wxTreeItemId TreeItemId = GetSelection();
    m_pSelection->Clear(GetId());
    if(TreeItemId.IsOk())
    {
        wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(TreeItemId);
		if (pData)
		{
	        m_pSelection->Select(pData->m_nObjectID, true, GetId());
		}
    }
    if (m_pGxApp)
	{
        m_pGxApp->UpdateNewMenu(m_pSelection);
	}
}

int wxGxTreeViewBase::OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2)
{
    wxGxTreeItemData* pData1 = (wxGxTreeItemData*)GetItemData(item1);
    wxGxTreeItemData* pData2 = (wxGxTreeItemData*)GetItemData(item2);
    wxGxObject* pGxObject1 = m_pCatalog->GetRegisterObject(pData1->m_nObjectID);
    wxGxObject* pGxObject2 = m_pCatalog->GetRegisterObject(pData2->m_nObjectID);
    return GxObjectCompareFunction(pGxObject1, pGxObject2, 1);
}

void wxGxTreeViewBase::OnItemRightClick(wxTreeEvent& event)
{
	wxTreeItemId item = event.GetItem();

	if (!item.IsOk())
	{
		return;
	}

	wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(item);
	if(NULL != pData)
	{
        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
        IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pGxObject);
        if(NULL != pGxObjectUI)
        {
            wxString psContextMenu = pGxObjectUI->ContextMenu();
            wxMenu* pMenu = dynamic_cast<wxMenu*>(m_pApp->GetCommandBar(psContextMenu));
            if(NULL != pMenu)
            {
                PopupMenu(pMenu, event.GetPoint());
            }
        }
    }
}

void wxGxTreeViewBase::OnItemExpanding(wxTreeEvent& event)
{
	wxTreeItemId item = event.GetItem();

	if(!item.IsOk())
		return;

	wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(item);
	if(NULL != pData)
	{
        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
		wxGxObjectContainer* pGxObjectContainer = wxDynamicCast(pGxObject, wxGxObjectContainer);
		if(NULL != pGxObjectContainer)
		{
			wxBusyCursor wait;
			if(pGxObjectContainer->HasChildren() && pGxObjectContainer->AreChildrenViewable())
			{
				if(pData->m_bExpandedOnce == false)
				{
					const wxGxObjectList ObjectList = pGxObjectContainer->GetChildren();
                    wxGxObjectList::const_iterator iter;
                    for (iter = ObjectList.begin(); iter != ObjectList.end(); ++iter)
                    {
                        wxGxObject *current = *iter;
 						AddTreeItem(current, item);//false
                        m_pCatalog->ObjectAdded(current->GetId());
                    }
					pData->m_bExpandedOnce = true;
                    SetItemHasChildren(item, GetChildrenCount(item, false) > 0);
                    SortChildren(item);
					return;
                }
				else
					return;
			}
			else
			{
				SetItemHasChildren(item, false);
				return;
			}
		}
	}
	SetItemHasChildren(item, false);
}


void wxGxTreeViewBase::OnChar(wxKeyEvent& event)
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
        SelectItem(GetPrevVisible(GetSelection()));
        break;
    case WXK_DOWN:
        SelectItem(GetNextVisible(GetSelection()));
        break;
    default:
        break;
    }
}

void wxGxTreeViewBase::OnObjectRefreshed(wxGxCatalogEvent& event)
{
	wxTreeItemId TreeItemId = m_TreeMap[event.GetObjectID()];
	if(TreeItemId.IsOk())
	{
		wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(TreeItemId);
		if(NULL != pData)
		{
			if(pData->m_bExpandedOnce)
			{
                //deleted via refresh
				//DeleteChildren(TreeItemId);
				pData->m_bExpandedOnce = false;
				Expand(TreeItemId);
			}
            else
            {
                wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
			    wxGxObjectContainer* pGxObjectContainer = dynamic_cast<wxGxObjectContainer*>(pGxObject);
				wxBusyCursor wait;
			    if(NULL != pGxObjectContainer && pGxObjectContainer->HasChildren() && !ItemHasChildren(TreeItemId))
                {
                    SetItemHasChildren(TreeItemId);
                }
            }
		}
	}
}

void wxGxTreeViewBase::OnObjectDeleted(wxGxCatalogEvent& event)
{
    //wxLogDebug(wxT("TreeView Object %d Delete"), event.GetObjectID());
	wxTreeItemId TreeItemId = m_TreeMap[event.GetObjectID()];
	if(TreeItemId.IsOk())
    {
        wxTreeItemId ParentTreeItemId = GetItemParent(TreeItemId);
        if(GetChildrenCount(ParentTreeItemId, false) == 1)
        {
		    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(ParentTreeItemId);
		    if(NULL != pData)
		    {
				pData->m_bExpandedOnce = false;
            }
            CollapseAndReset(ParentTreeItemId);
        }
        else
        {
		    Delete(TreeItemId);
        }

        UpdateGxSelection();
    }
	m_TreeMap.erase(event.GetObjectID());
}

void wxGxTreeViewBase::OnObjectAdded(wxGxCatalogEvent& event)
{
    //wxLogDebug(wxT("TreeView Object %d Add"), event.GetObjectID());
    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
	if (NULL == pGxObject)
	{
		return;
	}

    wxGxObject* pParentObject = pGxObject->GetParent();
	if (NULL == pParentObject)
	{
        return;
	}

	wxTreeItemId ParentTreeItemId = m_TreeMap[pParentObject->GetId()];
	if(ParentTreeItemId.IsOk())
	{
		wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(ParentTreeItemId);
		if(NULL != pData)
		{
            wxGxObjectContainer* pGxObjectContainer = wxDynamicCast(pParentObject, wxGxObjectContainer);
            if(pGxObjectContainer->AreChildrenViewable())
            {
			    if(pData->m_bExpandedOnce)
                {
				    AddTreeItem(pGxObject, ParentTreeItemId);
                    SortChildren(ParentTreeItemId);
                }
			    else
                {
				    SetItemHasChildren(ParentTreeItemId, true);
                }
            }
		}
	}
}

void wxGxTreeViewBase::OnObjectChanged(wxGxCatalogEvent& event)
{
    //wxLogDebug(wxT("TreeView Object %d Change"), event.GetObjectID());
	wxTreeItemId TreeItemId = m_TreeMap[event.GetObjectID()];
	if(TreeItemId.IsOk())
	{
		wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(TreeItemId);
		if(NULL != pData)
		{
            wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
			IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pGxObject);
            wxGxObjectContainer* pGxObjectContainer = wxDynamicCast(pGxObject, wxGxObjectContainer);
            IGxObjectTreeAttr* pGxObjectAttr = dynamic_cast<IGxObjectTreeAttr*>(pGxObject);

            if (NULL != pGxObject)
            {
                wxString sName;
                if (m_pCatalog->GetShowExt())
                {
                    sName = pGxObject->GetName();
                }
                else
                {
                    sName = pGxObject->GetBaseName();
                }

                if (NULL != pGxObjectAttr && pGxObjectAttr->ShowCount())
                {
                    sName.Append(wxString::Format(wxT(" [%ld]"), pGxObjectAttr->GetCount()));
                }
                SetItemText(TreeItemId, sName);
            }

            if (NULL != pGxObjectUI)
            {
                wxIcon icon = pGxObjectUI->GetSmallImage();

                int pos(wxNOT_FOUND);
                if (icon.IsOk())
                {
                    for (size_t i = 0; i < m_IconsArray.size(); ++i)
                    {
                        if (m_IconsArray[i].oIcon.IsSameAs(icon))
                        {
                            pos = m_IconsArray[i].iImageIndex;
                            break;
                        }
                    }
                    if (pos == wxNOT_FOUND)
                    {
                        pos = m_TreeImageList.Add(icon);
                        ICONDATA myicondata = { icon, pos };
                        m_IconsArray.push_back(myicondata);
                    }
                    SetItemImage(TreeItemId, pos);
                    //m_TreeImageList.Replace(pData->m_smallimage_index, icon);
                }
            }

			if(NULL != pGxObjectContainer)
			{
				wxBusyCursor wait;
				bool bItemHasChildren = pGxObjectContainer->HasChildren();
				if(ItemHasChildren(TreeItemId) && !bItemHasChildren)
				{
					CollapseAndReset(TreeItemId);
					pData->m_bExpandedOnce = false;
                    UpdateGxSelection();
				}
				SetItemHasChildren(TreeItemId, bItemHasChildren && pGxObjectContainer->AreChildrenViewable());
			}

            if(NULL != pGxObjectAttr)
            {
                SetItemBold(TreeItemId, pGxObjectAttr->IsBold());
                SetItemTextColour(TreeItemId, pGxObjectAttr->GetColor());
            }
				//wxTreeCtrl::Refresh();
		}
	}
}

void wxGxTreeViewBase::RefreshAll(void)
{
	DeleteAllItems();
	AddRoot(GetGxCatalog());
}


//-----------------------------------------------------------------------------
// wxGxTreeView
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxTreeView, wxGxTreeViewBase)

BEGIN_EVENT_TABLE(wxGxTreeView, wxGxTreeViewBase)
    EVT_TREE_BEGIN_LABEL_EDIT(TREECTRLID, wxGxTreeView::OnBeginLabelEdit)
    EVT_TREE_END_LABEL_EDIT(TREECTRLID, wxGxTreeView::OnEndLabelEdit)
    EVT_TREE_SEL_CHANGED(TREECTRLID, wxGxTreeView::OnSelChanged)
    EVT_TREE_BEGIN_DRAG(TREECTRLID, wxGxTreeView::OnBeginDrag)
    EVT_TREE_ITEM_ACTIVATED(TREECTRLID, wxGxTreeView::OnActivated)
END_EVENT_TABLE()

wxGxTreeView::wxGxTreeView(void) : wxGxTreeViewBase()
{
}

wxGxTreeView::wxGxTreeView(wxWindow* parent, wxWindowID id, long style) : wxGxTreeViewBase(parent, id, wxDefaultPosition, wxDefaultSize, style)
{
    SetDropTarget(new wxGISDropTarget(static_cast<IViewDropTarget*>(this)));
}

wxGxTreeView::~wxGxTreeView(void)
{
}

void wxGxTreeView::OnBeginLabelEdit(wxTreeEvent& event)
{
	wxTreeItemId item = event.GetItem();
	if (!item.IsOk())
	{
		return;
	}

	wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(item);
	if(NULL == pData)
	{
		event.Veto();
		return;
	}

    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
	IGxObjectEdit* pObjEdit =  dynamic_cast<IGxObjectEdit*>(pGxObject);
	IGxObjectTreeAttr* pObjAttr =  dynamic_cast<IGxObjectTreeAttr*>(pGxObject);
	if(NULL == pObjEdit || NULL != pObjAttr)
	{
		event.Veto();
		return;
	}
	if(!pObjEdit->CanRename())
	{
		event.Veto();
		return;
	}
}

void wxGxTreeView::OnEndLabelEdit(wxTreeEvent& event)
{
    if ( event.GetLabel().IsEmpty() )
    {
        event.Veto();
		return;
    }

	wxTreeItemId item = event.GetItem();
	if(!item.IsOk())
	{
		event.Veto();
		return;
	}
	wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(event.GetItem());
	if(NULL == pData)
	{
		event.Veto();
		return;
	}

    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
	IGxObjectEdit* pObjEdit =  dynamic_cast<IGxObjectEdit*>(pGxObject);
	if(NULL == pObjEdit)
	{
		event.Veto();
		return;
	}

	if(!pObjEdit->Rename(event.GetLabel()))
	{
		event.Veto();
		wxMessageBox(_("Rename error!"), _("Error"), wxICON_ERROR | wxOK );
		return;
	}

	m_pCatalog->ObjectChanged(pGxObject->GetId());
}

void wxGxTreeView::OnSelChanged(wxTreeEvent& event)
{
    event.Skip();

    UpdateGxSelection();
}

void wxGxTreeView::OnItemRightClick(wxTreeEvent& event)
{
	wxTreeItemId item = event.GetItem();
	if(!item.IsOk())
		return;
    SelectItem(item);
    wxGxTreeViewBase::OnItemRightClick(event);
}

void wxGxTreeView::OnBeginDrag(wxTreeEvent& event)
{
    //event.Skip();
	wxTreeItemId item = event.GetItem();
	if(!item.IsOk())
		return;
    SelectItem(item);

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
    wxArrayTreeItemIds treearray;
    size_t count = GetSelections(treearray);
	if (count == 0)
	{
        return;
	}
    //first is catalog memory address to prevent different app drop
    //pIDsData->AddDecimal((long)GetGxCatalog());
    for(size_t i = 0; i < count; ++i)
    {
	    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(treearray[i]);
	    if(NULL == pData)
            continue;

        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
	    if(NULL == pGxObject)
            continue;
        wxString sSystemPath = wxString::FromUTF8(pGxObject->GetPath());
        pFileData->AddFile(sSystemPath); //bug: http://trac.wxwidgets.org/ticket/11744 //solution: http://jenyay.net/blog/2011/02/18/wxpython-i-dragndrop-fajjlov-v-linux/
        wxString sName = pGxObject->GetFullName();
        pNamesData->AddString(sName);
        //pIDsData->AddDecimal(pGxObject->GetId());
    }

    wxDropSource dragSource( this );
	dragSource.SetData( *pDragData );
	wxDragResult result = dragSource.DoDragDrop( wxDrag_DefaultMove );

//    IGxObject* pParentGxObject(NULL);
//    for(size_t i = 0; i < count; ++i)
//    {
//	    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(treearray[i]);
//	    if(pData == NULL)
//            continue;
//
//        IGxObjectSPtr pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
//        pParentGxObject = pGxObject->GetParent();
//        wxString sSystemPath(pGxObject->GetInternalName(), wxConvUTF8);
//        pFileData->AddFile(sSystemPath);
//        dostr.WriteString(sSystemPath);
//        wxStreamBuffer* theBuffer = pstream->GetOutputStreamBuffer();
//
//        pDataObjectSimple->SetData(theBuffer->GetBufferSize(), theBuffer->GetBufferStart());
//    }
  ////  		IGxObjectUI* pGxObjectUI = dynamic_cast<IGxObjectUI*>(pGxObject.get());
		////if(pGxObjectUI)
		////{
		////	wxDataFormat frm = pGxObjectUI->GetDataFormat();
		////	if(frm.GetType() != wxDF_INVALID)
		////		my_data.SetFormat(frm);
		////}
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

 //   wxFileDataObject my_data;

 //   wxArrayTreeItemIds treearray;
 //   size_t count = GetSelections(treearray);
 //   if(count == 0)
 //       return;
 //   wxGxObject* pParentGxObject(NULL);
 //   for(size_t i = 0; i < count; ++i)
 //   {
	//    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(treearray[i]);
	//    if(pData == NULL)
 //           continue;

 //       wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
 //       pParentGxObject = pGxObject->GetParent();
 //       wxString sSystemPath(pGxObject->GetPath(), wxConvUTF8);
 //       my_data.AddFile(sSystemPath);
 //   }
 //   wxDropSource dragSource( this );
	//dragSource.SetData( my_data );
	//wxDragResult result = dragSource.DoDragDrop(  );
}

void wxGxTreeView::OnActivated(wxTreeEvent& event)
{
    event.Skip();

	wxTreeItemId item = event.GetItem();

	if(!item.IsOk())
		return;

	wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(item);
	if (NULL == pData)
	{
		return;
	}

    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
	IGxObjectWizard* pGxObjectWizard = dynamic_cast<IGxObjectWizard*>(pGxObject);
	if (NULL != pGxObjectWizard)
	{
		if (!pGxObjectWizard->Invoke(this))
		{
            Expand(item);
			return;
		}
	}
}

void wxGxTreeView::BeginRename(long nObjectID)
{
	if (nObjectID == wxNOT_FOUND)
	{
        return;
	}

	wxTreeItemId ItemId = m_TreeMap[nObjectID];
	if(!ItemId.IsOk())
    {
        SetItemHasChildren(GetSelection());
        Expand(GetSelection());
        ItemId = m_TreeMap[nObjectID];
    }

	if(ItemId.IsOk())
	{
		SelectItem(ItemId);//, false
		SetFocus();
        m_pSelection->Select(nObjectID, true, GetId());
        EditLabel(GetSelection());
	}
}

void wxGxTreeView::OnObjectAdded(wxGxCatalogEvent& event)
{
    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
	if (NULL == pGxObject)
	{
		return;
	}

    //wxLogDebug(wxT("TreeView Object %d '%s' Add"), pGxObject->GetId(), pGxObject->GetFullName());

    wxGxObject* pParentObject = pGxObject->GetParent();
	if (NULL == pParentObject)
	{
        return;
	}

	wxTreeItemId ParentTreeItemId = m_TreeMap[pParentObject->GetId()];
	if(ParentTreeItemId.IsOk())
	{
		wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(ParentTreeItemId);
		if(NULL != pData)
		{
            wxGxAutoRenamer* pGxAutoRenamer = dynamic_cast<wxGxAutoRenamer*>(pParentObject);
			if(pData->m_bExpandedOnce)
            {
				AddTreeItem(pGxObject, ParentTreeItemId);
                SortChildren(ParentTreeItemId);
				if (NULL != pGxAutoRenamer && pGxAutoRenamer->IsBeginRename(this, pGxObject->GetPath()))
                {
                    BeginRename(pGxObject->GetId());
                }
            }
			else
            {
				if (NULL != pGxAutoRenamer && pGxAutoRenamer->IsBeginRename(this, pGxObject->GetPath()))
                {
//				    AddTreeItem(pGxObject, ParentTreeItemId);
 //				    SetItemHasChildren(ParentTreeItemId, true);
                    Expand(ParentTreeItemId);
//                    SortChildren(ParentTreeItemId);
                    BeginRename(pGxObject->GetId());
                }
                else
                {
				    SetItemHasChildren(ParentTreeItemId, true);
                }
            }
		}
	}
}

wxDragResult wxGxTreeView::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
	if (m_HighLightItemId.IsOk())
	{
        SetItemDropHighlight(m_HighLightItemId, false);
	}
    wxPoint pt(x, y);
    int flag = wxTREE_HITTEST_ONITEMINDENT;
    wxTreeItemId ItemId = wxTreeCtrl::HitTest(pt, flag);
    if(ItemId.IsOk())
    {
        SetItemDropHighlight(ItemId);
        m_HighLightItemId = ItemId;

        wxSize sz = GetClientSize();
        if(DNDSCROLL > y)//scroll up
        {
            //LineUp();
            wxScrollWinEvent evt(wxEVT_SCROLLWIN_LINEUP);
            wxPostEvent(GetEventHandler(), evt );
        }

        else if((sz.GetHeight() - DNDSCROLL) < y)//scroll down
        {
//            LineDown();
            wxScrollWinEvent evt(wxEVT_SCROLLWIN_LINEDOWN);
            wxPostEvent(GetEventHandler(), evt );
        }

	    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(ItemId);
		if (NULL == pData)
		    return wxDragNone;
        //check drop capability

        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
        IGxDropTarget* pTarget = dynamic_cast<IGxDropTarget*>(pGxObject);
		if (NULL == pTarget)
		    return wxDragNone;
        return pTarget->CanDrop(def);
    }
    return def;
}

bool wxGxTreeView::OnDropObjects(wxCoord x, wxCoord y, const wxArrayString& GxObjects, bool bIsControlOn)
{
    bool bMove = !bIsControlOn;

    wxPoint pt(x, y);
    int flag = wxTREE_HITTEST_ONITEMINDENT;
    wxTreeItemId ItemId = wxTreeCtrl::HitTest(pt, flag);
    if(ItemId.IsOk())
    {
        SetItemDropHighlight(ItemId, false);

	    wxGxTreeItemData* pData = (wxGxTreeItemData*)GetItemData(ItemId);
		if (NULL == pData)
		    return wxDragNone;
        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pData->m_nObjectID);
        IGxDropTarget* pTarget = dynamic_cast<IGxDropTarget*>(pGxObject);

		if (NULL != pTarget)
        {
            wxDragResult res(bMove == true ? wxDragMove : wxDragCopy);
            if(wxIsDragResultOk(pTarget->CanDrop(res)))
                return pTarget->Drop(GxObjects, bMove);
        }
    }
    return false;
}

void wxGxTreeView::OnLeave()
{
	if (m_HighLightItemId.IsOk())
	{
        SetItemDropHighlight(m_HighLightItemId, false);
	}
}

bool wxGxTreeView::CanPaste()
{
    wxClipboardLocker lockClip;
    return wxTheClipboard->IsSupported(wxDF_FILENAME) | wxTheClipboard->IsSupported(wxDataFormat(wxGIS_DND_NAME));
    //& wxTheClipboard->IsSupported(wxDF_TEXT); | wxDF_BITMAP | wxDF_TIFF | wxDF_DIB | wxDF_UNICODETEXT | wxDF_HTML
}

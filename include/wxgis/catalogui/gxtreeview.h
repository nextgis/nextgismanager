/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxTreeView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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
#pragma once

#include "wxgis/catalogui/gxview.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalog/gxevent.h"
#include "wxgis/catalogui/gxeventui.h"
#include "wxgis/catalogui/gxapplication.h"

#include "wx/treectrl.h"
#include "wx/imaglist.h"

class WXDLLIMPEXP_GIS_CLU wxGxApplication;

/** \class wxGxTreeItemData gxtreeview.h
 *  \brief The tree view item data.
 */

class wxGxTreeItemData : public wxTreeItemData
{
public:
	wxGxTreeItemData(long nObjectID, int smallimage_index, bool bExpandedOnce)
	{
		m_nObjectID = nObjectID;
		m_smallimage_index = smallimage_index;
		m_bExpandedOnce = bExpandedOnce;
	}

	~wxGxTreeItemData(void)
	{
	}

	long m_nObjectID;
	int m_smallimage_index;
	bool m_bExpandedOnce;
};

/** \class wxGxTreeViewBase gxtreeview.h
 *  \brief The base class for tree view of gxObjects.
 */

class WXDLLIMPEXP_GIS_CLU wxGxTreeViewBase :
	public wxTreeCtrl,
	public wxGxView
{
    DECLARE_CLASS(wxGxTreeViewBase)
public:
    wxGxTreeViewBase(void);
	wxGxTreeViewBase(wxWindow* parent, wxWindowID id = TREECTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS );
	virtual ~wxGxTreeViewBase(void);
	virtual void AddTreeItem(wxGxObject* pGxObject, wxTreeItemId hParent);
	virtual void RefreshAll(void);
//wxGxView
    virtual bool Create(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS, const wxString& name = wxT("TreeView"));
	virtual bool Activate(IApplication* const pApplication, wxXmlNode* const pConf);
	virtual void Deactivate(void);
    virtual void BeginRename(long nObjectID = wxNOT_FOUND){};
    virtual void Refresh(void){wxTreeCtrl::Refresh();};
//wxTreeCtrl
    virtual int OnCompareItems(const wxTreeItemId& item1, const wxTreeItemId& item2);

	typedef std::map<long, wxTreeItemId> wxTreeItemMap;
	typedef struct _icondata
	{
		wxIcon oIcon;
		int iImageIndex;
	} ICONDATA;
protected:
    virtual void AddRoot(wxGxObject* pGxObject);
protected:
//events
	virtual void OnItemExpanding(wxTreeEvent& event);
	virtual void OnItemRightClick(wxTreeEvent& event);
    virtual void OnChar(wxKeyEvent& event);
	virtual void OnObjectRefreshed(wxGxCatalogEvent& event);
	virtual void OnObjectAdded(wxGxCatalogEvent& event);
	virtual void OnObjectChanged(wxGxCatalogEvent& event);
	virtual void OnObjectDeleted(wxGxCatalogEvent& event);
	virtual void OnSelectionChanged(wxGxSelectionEvent& event);
    //
    virtual void UpdateGxSelection(void);
protected:
	wxImageList m_TreeImageList;
	wxTreeItemMap m_TreeMap;
    wxGISApplicationBase* m_pApp;
    wxGxApplication *m_pGxApp;
	long m_ConnectionPointCatalogCookie, m_ConnectionPointSelectionCookie;
	wxGxSelection* m_pSelection;
    wxGxCatalogUI* m_pCatalog;
    wxGISCommand* m_pDeleteCmd;
    wxVector<ICONDATA> m_IconsArray;
private:
    DECLARE_EVENT_TABLE()
};

/** \class wxGxTreeView gxtreeview.h
 *  \brief The tree view in Catalog.
 */
class WXDLLIMPEXP_GIS_CLU wxGxTreeView :
	public wxGxTreeViewBase,
    public IViewDropTarget
{
    DECLARE_DYNAMIC_CLASS(wxGxTreeView)
public:
    wxGxTreeView(void);
	wxGxTreeView(wxWindow* parent, wxWindowID id = TREECTRLID, long style = wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS | wxBORDER_NONE | wxTR_EDIT_LABELS );//wxTR_MULTIPLE| wxTR_LINES_AT_ROOT
	virtual ~wxGxTreeView(void);
//IGxDropTarget
    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    virtual bool OnDropObjects(wxCoord x, wxCoord y, const wxArrayString& GxObjects);
    virtual void OnLeave();
    virtual bool CanPaste(void);
//events
    virtual void OnBeginLabelEdit(wxTreeEvent& event);
    virtual void OnEndLabelEdit(wxTreeEvent& event);
    virtual void OnSelChanged(wxTreeEvent& event);
	virtual void OnItemRightClick(wxTreeEvent& event);
	virtual void OnBeginDrag(wxTreeEvent& event);
	virtual void OnActivated(wxTreeEvent& event);
//wxGxTreeViewBase
    virtual void OnObjectAdded(wxGxCatalogEvent& event);
//wxGxTreeView
    virtual void BeginRename(long nObjectID = wxNOT_FOUND);
protected:
    wxTreeItemId m_HighLightItemId;
private:
    DECLARE_EVENT_TABLE()
};

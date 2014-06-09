/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxContentView class.
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
#pragma once

#include "wxgis/catalogui/gxview.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalog/gxevent.h"
#include "wxgis/catalogui/gxeventui.h"
#include "wxgis/catalogui/gxapplication.h"

#include "wx/listctrl.h"
#include "wx/imaglist.h"

class wxGxApplication;

#define LISTSTYLE (wxLC_REPORT | wxBORDER_NONE | wxLC_EDIT_LABELS | wxLC_AUTOARRANGE) //wxLC_LIST|wxLC_SORT_ASCENDING

#if defined(__WINDOWS__)
    #define wxGISLIST_STATE_DROPHILITED wxLIST_STATE_DROPHILITED
#elif defined(__UNIX__)
    #define wxGISLIST_STATE_DROPHILITED wxLIST_STATE_SELECTED
#else //MAC
#endif // defined

/** @struct wxGxToolBarArt gxcontentview.h

    The struct needed for sort data in content view.

    @library{catalogui}
*/
typedef struct _sortdata
{
    bool bSortAsc;
    short currentSortCol;
} SORTDATA, *LPSORTDATA;

/** @class wxGxContentView gxcontentview.h

    The catalog content view class.

    @library{catalogui}
*/
class WXDLLIMPEXP_GIS_CLU wxGxContentView :
	public wxListCtrl,
	public wxGxView,
    public IGxContentsView,
    public IViewDropTarget,
    public wxThreadHelper
{
    DECLARE_DYNAMIC_CLASS(wxGxContentView)
public:
    wxGxContentView(void);
	wxGxContentView(wxWindow* parent, wxWindowID id = LISTCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = LISTSTYLE);
	virtual ~wxGxContentView(void);
	virtual void Serialize(wxXmlNode* pRootNode, bool bStore);
	virtual bool AddObject(wxGxObject* const pObject);
	virtual void ResetContents(void);
    virtual wxGxObject* const GetParentGxObject(void) const;
    virtual void SelectAll(void);
    virtual bool Show(bool show = true);
	virtual void RefreshAll(void);
    virtual long HitTest( const wxPoint& point, int& flags, long *pSubItem = NULL ) const;
// wxGxView
    virtual bool Create(wxWindow* parent, wxWindowID id = LISTCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = LISTSTYLE, const wxString& name = wxT("ContentView"));
	virtual bool Activate(IApplication* const pApplication, wxXmlNode* const pConf);
	virtual void Deactivate(void);
	virtual bool Applies(wxGxSelection* const pSelection);
    virtual void BeginRename(long nObjectID = wxNOT_FOUND);
// IGxContentsView
	virtual void SetStyle(wxGISEnumContentsViewStyle style);
    virtual wxGISEnumContentsViewStyle GetStyle(void){return m_current_style;};
    virtual bool CanSetStyle(void){return true;};
// IViewDropTarget
    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    virtual bool OnDropObjects(wxCoord x, wxCoord y, const wxArrayString& GxObjects);
    virtual void OnLeave();
    virtual bool CanPaste(void);
// events
	virtual void OnColClick(wxListEvent& event);
    virtual void OnContextMenu(wxContextMenuEvent& event);
	virtual void ShowContextMenu(const wxPoint& pos);
	virtual void SetColumnImage(int col, int image);
    virtual void OnActivated(wxListEvent& event);
    virtual void OnBeginLabelEdit(wxListEvent& event);
	virtual void OnEndLabelEdit(wxListEvent& event);
	virtual void OnSelected(wxListEvent& event);
	virtual void OnDeselected(wxListEvent& event);
    virtual void OnBeginDrag(wxListEvent& event);
    virtual void OnChar(wxKeyEvent& event);
	virtual void OnObjectRefreshed(wxGxCatalogEvent& event);
	virtual void OnObjectAdded(wxGxCatalogEvent& event);
	virtual void OnObjectChanged(wxGxCatalogEvent& event);
	virtual void OnObjectDeleted(wxGxCatalogEvent& event);
	virtual void OnSelectionChanged(wxGxSelectionEvent& event);

	typedef struct _itemdata
	{
		long nObjectID;
		int iImageIndex;
	} ITEMDATA, *LPITEMDATA;

	typedef struct _icondata
	{
		wxIcon oIcon;
		int iImageIndex;
        bool bLarge;
	} ICONDATA;

protected:
    int GetIconPos(wxIcon icon_small, wxIcon icon_large);
    virtual void InitColumns(void);
    virtual void SelectItem(int nChar = WXK_DOWN, bool bShift = false);
    virtual wxThread::ExitCode Entry();
    virtual bool CreateAndRunFillMetaThread(void);
    virtual void DestroyFillMetaThread(void);
protected:
	bool m_bSortAsc;
	short m_currentSortCol;
	wxGISEnumContentsViewStyle m_current_style;
    wxArrayInt m_anOrder, m_anWidth;
	wxImageList m_ImageListSmall, m_ImageListLarge;

	long m_ConnectionPointCatalogCookie;

	wxGxSelection* m_pSelection;
    wxGxCatalogUI* m_pCatalog;
    wxGISApplicationBase* m_pApp;
    wxGxApplication *m_pGxApp;
    wxGISCommand* m_pDeleteCmd;

	long m_nParentGxObjectID;
	bool m_bDragging;
    wxVector<ICONDATA> m_IconsArray;
    wxCriticalSection m_CritSect;
    wxCriticalSection m_CritSectCont;
    wxCriticalSection m_CritSectFillMeta;
    long m_HighLightItem;
    int m_bPrevChar;

    wxArrayLong m_anFillMetaIDs;
private:
    DECLARE_EVENT_TABLE()
};

/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxToolExecuteView class.
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

#pragma once

#include "wxgis/catalogui/gxview.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalog/gxevent.h"
#include "wxgis/catalogui/gxeventui.h"
#include "wxgis/catalog/gxobject.h"
#include "wxgis/catalogui/gxapplication.h"

#include "wx/listctrl.h"
#include "wx/imaglist.h"

#define TOOLEXECVIEWSTYLE (wxLC_REPORT | wxBORDER_NONE | wxLC_AUTOARRANGE)
#define TOOLEXECUTECTRLID WXGISHIGHEST + 1

WXDLLIMPEXP_GIS_GPU int wxCALLBACK GxTaskCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData);

/** \class wxGISToolExecuteView gxtoolexecview.h
    \brief The tasks execution view class.
*/

class WXDLLIMPEXP_GIS_GPU wxGISToolExecuteView :
	public wxListCtrl,
	public wxGxView,
    public IGxContentsView
{
    DECLARE_CLASS(wxGISToolExecuteView)
public:
    wxGISToolExecuteView(void);
	wxGISToolExecuteView(wxWindow* parent, wxWindowID id = TOOLEXECUTECTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = TOOLEXECVIEWSTYLE);
	virtual ~wxGISToolExecuteView(void);
	virtual void Serialize(wxXmlNode* pRootNode, bool bStore);
    //wxListCtrl
    bool Show(bool show);
//wxGxView
    virtual bool Create(wxWindow* parent, wxWindowID id = TOOLEXECUTECTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = TOOLEXECVIEWSTYLE, const wxString& name = wxT("ToolExecuteView"));
	virtual bool Activate(IApplication* const pApplication, wxXmlNode* const pConf);
	virtual void Deactivate(void);
// IGxContentsView
    virtual void SelectAll(void);
    virtual void SetStyle(wxGISEnumContentsViewStyle style){};
    virtual wxGISEnumContentsViewStyle GetStyle(void){return enumGISCVReport;};
    virtual bool CanSetStyle(void){return false;};
// DropTarget
    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    virtual wxDragResult OnEnter(wxCoord x, wxCoord y, wxDragResult def);
    virtual void OnLeave();
    virtual bool OnDropObjects(wxCoord x, wxCoord y, long nParentPointer, const wxArrayLong& TaskIds);
//events
    virtual void OnContextMenu(wxContextMenuEvent& event);
	virtual void ShowContextMenu(const wxPoint& pos);
    virtual void OnActivated(wxListEvent& event);
	virtual void OnSelected(wxListEvent& event);
	virtual void OnDeselected(wxListEvent& event);
    virtual void OnChar(wxKeyEvent& event);
    virtual void OnBeginDrag(wxListEvent& event);
//events
	virtual void OnObjectRefreshed(wxGxCatalogEvent& event);
	virtual void OnObjectAdded(wxGxCatalogEvent& event);
	virtual void OnObjectChanged(wxGxCatalogEvent& event) = 0;
	virtual void OnObjectDeleted(wxGxCatalogEvent& event);
	virtual void OnSelectionChanged(wxGxSelectionEvent& event);
protected:
    virtual void AddObject(wxGxObject* pObject) = 0;
    virtual void ResetContents(void);
    virtual void RefreshAll(void);
    virtual void InitColumns(void);
protected:
    wxGxApplication* m_pApp;    
    long m_ConnectionPointCatalogCookie, m_ConnectionPointSelectionCookie;
	wxGxSelection* m_pSelection;
    wxGxCatalogUI* m_pCatalog;
    wxGISCommand* m_pDeleteCmd;
    long m_nParentGxObjectId;
    wxImageList m_ImageList;
    long m_HighLightItem;
    bool m_bDropping;
private:
    DECLARE_EVENT_TABLE()
};

/** \class wxGxToolExecuteView gxtoolexecview.h
    \brief The tasks execution gxcatalog view class.
*/

//typedef struct _sorttaskdata
//{
//    bool bSortAsc;
//    short currentSortCol;
//} SORTTASKDATA, *LPSORTTASKDATA;

/*

#define TOOLEXECVIEWSTYLE wxLC_REPORT | wxBORDER_NONE | wxLC_SORT_ASCENDING | wxLC_AUTOARRANGE | wxLC_VRULES | wxLC_HRULES

class WXDLLIMPEXP_GIS_GPU wxGxToolExecuteView :
	public wxListCtrl,
	public wxGxView,
    public IGxContentsView,
    public IViewDropTarget
{
    DECLARE_DYNAMIC_CLASS(wxGxToolExecuteView)
public:
    wxGxToolExecuteView(void);
	wxGxToolExecuteView(wxWindow* parent, wxWindowID id = TOOLEXECUTECTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = TOOLEXECVIEWSTYLE);
	virtual ~wxGxToolExecuteView(void);
	virtual void Serialize(wxXmlNode* pRootNode, bool bStore);
	virtual void AddObject(wxGxObject* pObject);
	virtual void ResetContents(void);
    virtual IGxObjectSPtr const GetParentGxObject(void);
    virtual bool Show(bool show = true);
    virtual void HideDone(bool bHide = true);
	virtual void SetGxToolExecute(IGxObject* pObject){m_nParentGxObjectID = pObject->GetID();};
	virtual void RefreshAll(void);
//IGxView
    virtual bool Create(wxWindow* parent, wxWindowID id = TOOLEXECUTECTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = TOOLEXECVIEWSTYLE, const wxString& name = wxT("ToolExecuteView"));
	virtual bool Activate(IFrameApplication* application, wxXmlNode* pConf);
	virtual void Deactivate(void);
	virtual bool Applies(wxGxSelection* const pSelection);
    virtual void BeginRename(IGxObject* pGxObject = NULL){};
// IGxContentsView
    virtual void SelectAll(void);
    virtual void SetStyle(wxGISEnumContentsViewStyle style){};
    virtual wxGISEnumContentsViewStyle GetStyle(void){return enumGISCVReport;};
    virtual bool CanSetStyle(void){return false;};
//IGxDropTarget
    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    virtual bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames);
    virtual void OnLeave();
//events
	virtual void OnColClick(wxListEvent& event);
    virtual void OnContextMenu(wxContextMenuEvent& event);
	virtual void ShowContextMenu(const wxPoint& pos);
	virtual void SetColumnImage(int col, int image);
    virtual void OnActivated(wxListEvent& event);
	virtual void OnSelected(wxListEvent& event);
	virtual void OnDeselected(wxListEvent& event);
    virtual void OnChar(wxKeyEvent& event);
    virtual void OnBeginDrag(wxListEvent& event);
	virtual void OnObjectRefreshed(wxGxCatalogEvent& event);
	virtual void OnObjectAdded(wxGxCatalogEvent& event);
	virtual void OnObjectChanged(wxGxCatalogEvent& event);
	virtual void OnObjectDeleted(wxGxCatalogEvent& event);
	virtual void OnSelectionChanged(wxGxSelectionEvent& event);
protected:
    virtual void FillDataArray(wxArrayString &saDataArr);
protected:
	bool m_bSortAsc, m_bHideDone;
	short m_currentSortCol;
	wxImageList m_ImageList;
	wxGISConnectionPointContainer* m_pConnectionPointCatalog;
	long m_ConnectionPointCatalogCookie;
	IGxSelection* m_pSelection;
    wxGxCatalogUI* m_pCatalog;
    wxGISCommand* m_pDeleteCmd;
	long m_nParentGxObjectID;
    long m_HighLightItem;

    DECLARE_EVENT_TABLE()
};
*/
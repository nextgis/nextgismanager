/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxObjectDialog class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Dmitry Baryshnikov
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

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/combo.h>
#include <wx/treectrl.h>
#include <wx/combobox.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/toolbar.h>
#include <wx/sizer.h>
#include <wx/listctrl.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/aui/aui.h>

#include "wxgis/core/config.h"
#include "wxgis/catalogui/gxcontentview.h"
#include "wxgis/catalogui/gxtreeview.h"
#include "wxgis/framework/applicationbase.h"
#include "wxgis/version.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/framework/accelerator.h"

#define OBJDLG_NAME wxT("wxGISObjDialog")

/** @class wxGxToolBarArt

    The class to make toolbar buttons (delete, up, create folder, etc.) on GxObjDialog more native.

    @library{catalogui}
*/

class wxGxToolBarArt : public wxAuiDefaultToolBarArt
{
public:
	virtual void DrawBackground(wxDC& dc, wxWindow* wnd, const wxRect& rect)
	{
		dc.SetBackground(wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE )));
		dc.Clear();
	}
};

/** @class wxTreeViewComboPopup
    
    The tree view class on top of GxObjDialog - tree in popup.

    @library{catalogui}
*/
class wxTreeViewComboPopup : 
    public wxGxTreeViewBase,
    public wxComboPopup
{
    DECLARE_CLASS(wxTreeViewComboPopup)
public:

    // Initialize member variables
    virtual void Init();
    virtual void OnPopup();
    virtual void OnDismiss();
    // Create popup control
    virtual bool Create(wxWindow* parent);
    virtual bool Create(wxWindow* parent, wxWindowID id = TREECTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxBORDER_SIMPLE | wxTR_NO_BUTTONS | wxTR_NO_LINES | wxTR_SINGLE, const wxString& name = wxT("ViewComboPopup"));
    // Return pointer to the created control
    virtual wxWindow *GetControl() { return this; }
    // Translate string into a list selection
    virtual void SetStringValue(const wxString& s);
    // Get list selection as a string
    virtual wxString GetStringValue() const;
    //
//events
    // Do mouse hot-tracking (which is typical in list popups)
    virtual void OnMouseMove(wxMouseEvent& event);
     // On mouse left up, set the value and close the popup
    virtual void OnMouseClick(wxMouseEvent& event);
    virtual void OnDblClick(wxTreeEvent& event);
	virtual void OnSelectionChanged(wxGxSelectionEvent& event);
	//
    virtual wxSize GetAdjustedSize(int minWidth, int prefHeight, int maxHeight);
//wxGxTreeViewBase
    virtual void AddTreeItem(wxGxObject* pGxObject, wxTreeItemId hParent);
    //
    virtual wxGxSelection* const GetSelectedObjects(void);
protected:
    wxTreeItemId m_PrewItemId;
    bool m_bClicked;
private:
    DECLARE_EVENT_TABLE()
};

/** @class wxGxDialogContentView
    
    The content view class on center of GxObjDialog.

    @library{catalogui}
*/
#define OBJDLGLISTCTRLID	1012 //wxGxObjectDialog contents view

class wxGxDialogContentView : public wxGxContentView
{
    DECLARE_CLASS(wxGxDialogContentView)
public:
	wxGxDialogContentView(wxWindow* parent, wxWindowID id = OBJDLGLISTCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_LIST | wxLC_EDIT_LABELS | wxLC_SORT_ASCENDING);
	virtual ~wxGxDialogContentView();
    virtual bool Create(wxWindow* parent, wxWindowID id = OBJDLGLISTCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_LIST | wxLC_EDIT_LABELS | wxLC_SORT_ASCENDING, const wxString& name = wxT("ContentView"));
    //wxGxView
	virtual bool Activate(wxGISApplicationBase* application, wxXmlNode* pConf);
	virtual void Deactivate(void);
    // events
    virtual void OnActivated(wxListEvent& event);
    //
	virtual void SetFilters(const wxGxObjectFiltersArray &FiltersArray){m_FiltersArray = FiltersArray;};
	virtual void SetCurrentFilter(size_t nFilterIndex);
    virtual wxGxSelection* const GetSelectedObjects(void);
    //wxGxContentView
	virtual bool AddObject(wxGxObject* const pObject);
protected:
	long m_ConnectionPointSelectionCookie;
	wxGxObjectFiltersArray m_FiltersArray;
	size_t m_nFilterIndex;
private:
    DECLARE_EVENT_TABLE()
};

/** @class wxGxObjectDialog

    The dialog to open or save different objects (feature classes, raster, folders and etc.). This is like the file open/save dialog in OS, but works with wxGxObjects tree.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxObjectDialog :
    public wxDialog,
    public wxGISApplicationBase,
    public wxGxApplicationBase
{
    DECLARE_CLASS(wxGxObjectDialog)
public:
    wxGxObjectDialog(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Open"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	virtual ~wxGxObjectDialog();
//wxGISApplicationBase
    virtual wxString GetAppName(void) const {return wxString(OBJDLG_NAME);};
	virtual wxString GetAppDisplayName(void) const {return wxString(_("NextGIS Object Dialog"));};
	virtual wxString GetAppDisplayNameShort(void) const {return wxString(_("Object Dialog"));};
//wxGxObjectDialog
	virtual void SetButtonCaption(const wxString &sOkBtLabel);
	virtual void SetStartingLocation(const wxString &sStartPath);
	virtual void SetName(const wxString &sName);
	virtual void SetAllowMultiSelect(bool bAllowMultiSelect);
	virtual void SetOverwritePrompt(bool bOverwritePrompt);
	virtual void SetAllFilters(bool bAllFilters);
	virtual int ShowModalOpen();
	virtual int ShowModalSave();
	virtual void AddFilter(wxGxObjectFilter* pFilter, bool bDefault = false);
    virtual void SetOwnsFilter(bool bOwnFilter){m_bOwnFilter = bOwnFilter;};
	virtual void RemoveAllFilters(void);
    const wxGxObjectList& GetChildren() const { return m_ObjectList; };
    virtual wxString GetName(void) const;
    virtual wxString GetFullName(void) const;
    virtual CPLString GetPath(void) const;
    virtual wxGxObject* const GetLocation(void) const;
    virtual wxGxObjectFilter* GetCurrentFilter(void) const;
    virtual size_t GetCurrentFilterId(void) const;
protected:
// events
    virtual void OnCommand(wxCommandEvent& event);
	virtual void OnCommandUI(wxUpdateUIEvent& event);
	virtual void OnDropDownCommand(wxCommandEvent& event);
	virtual void OnToolDropDown(wxAuiToolBarEvent& event);
	virtual void OnItemSelected(wxListEvent& event);
	virtual void OnFilterSelect(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void Command(wxGISCommand* pCmd);
//wxGxObjectDialog
	virtual void OnInit();
    virtual void SerializeFramePos(bool bSave);
    virtual bool DoSaveObject(wxGISEnumSaveObjectResults Result);
protected:
    wxGISAcceleratorTable* m_pGISAcceleratorTable;
    wxGxCatalogUI* m_pCatalog;
    wxGxDialogContentView* m_pwxGxContentView;
    wxTreeViewComboPopup* m_PopupCtrl;
    IDropDownCommand* m_pDropDownCommand;
	wxString m_sOkBtLabel;
	wxString m_sStartPath;
	wxString m_sName;
	bool m_bAllowMultiSelect, m_bOverwritePrompt, m_bAllFilters;
    bool m_bIsSaveDlg;
	wxGxObjectFiltersArray m_FilterArray;
	size_t m_nDefaultFilter;
    wxGxObjectList m_ObjectList;
    int m_nRetCode;
    bool m_bOwnFilter;
protected:
	wxBoxSizer* bMainSizer;
	wxBoxSizer* bHeaderSizer;
	wxStaticText* m_staticText1;
	wxStaticText* m_staticText2;
	wxComboCtrl* m_TreeCombo;
	wxAuiToolBar* m_toolBar;
	wxFlexGridSizer* fgCeilSizer;
	wxStaticText* m_staticText4;
	wxTextCtrl* m_NameTextCtrl;
	wxButton* m_OkButton;
	wxStaticText* m_staticText6;
	wxComboBox* m_WildcardCombo;
	wxButton* m_CancelButton;
private:
    DECLARE_EVENT_TABLE()
};


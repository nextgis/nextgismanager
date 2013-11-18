/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxContainerDialog class.
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

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/treectrl.h>
#include <wx/toolbar.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>
#include <wx/aui/aui.h>

#include "wxgis/core/config.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/gxcontentview.h"
#include "wxgis/catalogui/gxtreeview.h"
#include "wxgis/framework/applicationbase.h"
#include "wxgis/version.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/framework/accelerator.h"


#define CONTDLG_NAME wxT("wxGISContDialog") 

/** @class wxTreeContainerView

    The class to show GxObjjects tree and select items in it. Usually the tree included containers to select.

    @library {catalogui}
*/

class wxTreeContainerView : 
    public wxGxTreeView
{
    DECLARE_DYNAMIC_CLASS(wxTreeContainerView)
public:
    wxTreeContainerView(void);
    wxTreeContainerView(wxWindow* parent, wxWindowID id = TREECTRLID, long style = wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS | wxTR_NO_LINES | wxTR_SINGLE | wxTR_EDIT_LABELS);// | wxTR_HIDE_ROOT
    virtual ~wxTreeContainerView(void);
//wxGxTreeViewBase
    virtual void AddTreeItem(wxGxObject* pGxObject, wxTreeItemId hParent);
//wxTreeContainerView
    virtual void AddShowFilter(wxGxObjectFilter* pFilter);
	virtual void RemoveAllShowFilters(void);
//events
    virtual void OnItemRightClick(wxTreeEvent& event){};
    virtual void OnBeginDrag(wxTreeEvent& event){};
    //virtual void OnActivated(wxTreeEvent& event){};
    virtual bool CanChooseObject( wxGxObject* pObject );
protected:
    wxGxObjectFiltersArray m_ShowFilter;
};

/** @class wxGxContainerDialog

    The dialog to select the contaniers (folders and etc.).

    @library {catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGxContainerDialog : 
    public wxDialog,
    public wxGISApplicationBase,
    public wxGxApplicationBase
{
    DECLARE_CLASS(wxGxContainerDialog)
    enum
    {
        ID_CREATE = wxID_HIGHEST + 4001
    };
public:
	wxGxContainerDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Open"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 540,338 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	virtual ~wxGxContainerDialog();	
//wxGISApplicationBase
    virtual wxString GetAppName(void) const{return wxString(CONTDLG_NAME);};
    virtual wxString GetAppDisplayName(void) const { return wxString(_("NextGIS Container Dialog")); };
    virtual wxString GetAppDisplayNameShort(void) const { return wxString(_("Container Dialog")); };
    //wxDialog
    int ShowModal(void);
//wxGxContainerDialog
	virtual void SetButtonCaption(const wxString &sOkBtLabel);
    virtual void SetStartingLocation(const wxString &sStartPath);
    virtual void SetDescriptionText(const wxString &sText);
    virtual void ShowCreateButton(bool bShow = false);
    virtual void ShowExportFormats(bool bShow = false);
	virtual void SetAllFilters(bool bAllFilters);
    virtual void AddFilter(wxGxObjectFilter* pFilter, bool bDefault = false);
    virtual void SetOwnsFilter(bool bOwnFilter){ m_bOwnFilter = bOwnFilter; };
    virtual void SetOwnsShowFilter(bool bOwnFilter){m_bOwnShowFilter = bOwnFilter;};
	virtual void RemoveAllFilters(void);
    virtual void AddShowFilter(wxGxObjectFilter* pFilter);
	virtual void RemoveAllShowFilters(void);
    const wxGxObjectList& GetChildren() const { return m_ObjectList; };
    virtual wxString GetName(void) const;
    virtual wxString GetFullName(void) const;
    virtual CPLString GetPath(void) const;
    virtual wxGxObject* const GetLocation(void) const;
    virtual wxGxObjectFilter* GetCurrentFilter(void) const;
    virtual size_t GetCurrentFilterId(void) const;
protected:
// events
    virtual void OnFilterSelect(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnCreate(wxCommandEvent& event);
    virtual void OnCreateUI(wxUpdateUIEvent& event);
    virtual void OnCommand(wxCommandEvent& event);
	virtual void OnCommandUI(wxUpdateUIEvent& event);
    virtual void Command(wxGISCommand* pCmd);
protected:
//wxGxContainerDialog
	virtual void OnInit();
    virtual void SerializeFramePos(bool bSave);
protected:
  	wxGxCatalogUI* m_pCatalog;
    wxGISAcceleratorTable* m_pGISAcceleratorTable;
    wxTreeContainerView* m_pTree;
	wxString m_sOkBtLabel;
	wxString m_sStartPath;
    bool m_bShowCreateButton, m_bAllFilters, m_bShowExportFormats;
    wxGxObjectFiltersArray m_FilterArray;
    wxGxObjectFiltersArray m_paShowFilter;
	size_t m_nDefaultFilter;
    wxGxObjectList m_ObjectList;
    int m_nRetCode;
    bool m_bOwnFilter, m_bOwnShowFilter;

protected:
	wxBoxSizer* bMainSizer;
	wxStaticText* m_staticDescriptionText;
	wxStaticText* m_staticWildText;
	wxBoxSizer* bFooterSizer;
	wxComboBox* m_WildcardCombo;
	wxButton* m_CancelButton;
	wxButton* m_CreateButton;
	wxButton* m_OkButton;
private:
    DECLARE_EVENT_TABLE()
};

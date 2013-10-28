/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxContainerDialog class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012 Bishop
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
/*
#define CONTDLG_NAME wxT("wxGISContDialog") 

//////////////////////////////////////////////////////////////////////////////
// wxTreeContainerView
//////////////////////////////////////////////////////////////////////////////

class wxTreeContainerView : public wxGxTreeView
{
    DECLARE_DYNAMIC_CLASS(wxTreeContainerView)
public:
    wxTreeContainerView(void);
    wxTreeContainerView(wxWindow* parent, wxWindowID id = TREECTRLID, long style = wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS | wxTR_NO_LINES | wxTR_SINGLE | wxTR_EDIT_LABELS);// | wxTR_HIDE_ROOT
    virtual ~wxTreeContainerView(void);
//wxGxTreeViewBase
    virtual void AddTreeItem(IGxObject* pGxObject, wxTreeItemId hParent);
//wxTreeContainerView
	virtual void AddShowFilter(IGxObjectFilter* pFilter);
	virtual void RemoveAllShowFilters(void);
    virtual bool CanChooseObject( IGxObject* pObject );
//events
    //virtual void OnSelChanged(wxTreeEvent& event);
    virtual void OnItemRightClick(wxTreeEvent& event){};
    virtual void OnBeginDrag(wxTreeEvent& event){};
    virtual void OnActivated(wxTreeEvent& event){};

protected:
	OBJECTFILTERS m_ShowFilterArray;

    DECLARE_EVENT_TABLE()
};

////////////////////////////////////////////////////////////////////////////////
//// wxGxContainerDialog
////////////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_CLU wxGxContainerDialog : 
    public wxDialog,
    public IGxApplication,
    public wxGISApplicationBase
{
    DECLARE_CLASS(wxGxContainerDialog)
enum
{
    ID_CREATE = wxID_HIGHEST + 4001
};
public:
	wxGxContainerDialog( wxWindow* parent, IGxCatalog* pExternalCatalog = NULL, wxWindowID id = wxID_ANY, const wxString& title = _("Open"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 540,338 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	virtual ~wxGxContainerDialog();	

//IGxApplication
    virtual IGxCatalog* const GetCatalog(void){return static_cast<IGxCatalog*>(m_pCatalog);};
//wxGISApplicationBase
    virtual wxString GetAppName(void) const{return wxString(CONTDLG_NAME);};
    virtual wxString GetAppVersionString(void) const{return wxString(wxGIS_VERSION_NUM_DOT_STRING_T);};
//wxDialog
    int ShowModal(void);
//wxGxContainerDialog
	virtual void SetButtonCaption(wxString sOkBtLabel);
	virtual void SetStartingLocation(wxString sStartPath);
	virtual void SetDescriptionText(wxString sText);
    virtual void ShowCreateButton(bool bShow = false);
    virtual void ShowExportFormats(bool bShow = false);
	virtual void SetAllFilters(bool bAllFilters);
	virtual void AddFilter(IGxObjectFilter* pFilter, bool bDefault = false);
    virtual void SetOwnsFilter(bool bOwnFilter){m_bOwnFilter = bOwnFilter;};
    virtual void SetOwnsShowFilter(bool bOwnFilter){m_bOwnShowFilter = bOwnFilter;};
	virtual void RemoveAllFilters(void);
	virtual void AddShowFilter(IGxObjectFilter* pFilter);
	virtual void RemoveAllShowFilters(void);
    virtual GxObjectArray* GetSelectedObjects(void){return &m_ObjectArray;}
    virtual wxString GetPath(void);
    virtual CPLString GetInternalPath(void);
    virtual IGxObjectFilter* GetCurrentFilter(void);
protected:
// events
	virtual void OnFilerSelect(wxCommandEvent& event);
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnCreate(wxCommandEvent& event);
    virtual void OnCreateUI(wxUpdateUIEvent& event);
    virtual void OnCommand(wxCommandEvent& event);
	virtual void OnCommandUI(wxUpdateUIEvent& event);
    virtual void Command(wxGISCommand* pCmd);
//wxGxContainerDialog
    virtual long GetLocation(void);
	virtual void OnInit();
    virtual void SerializeFramePos(bool bSave);
protected:
  	wxGxCatalogUI* m_pCatalog;
  	IGxCatalog* m_pExternalCatalog;
    wxTreeContainerView* m_pTree;
	wxString m_sOkBtLabel;
	wxString m_sStartPath;
    bool m_bShowCreateButton, m_bAllFilters, m_bShowExportFormats;
	OBJECTFILTERS m_FilterArray;
	OBJECTFILTERS m_paShowFilter;
	size_t m_nDefaultFilter;
    GxObjectArray m_ObjectArray;
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

    DECLARE_EVENT_TABLE()
};

*/
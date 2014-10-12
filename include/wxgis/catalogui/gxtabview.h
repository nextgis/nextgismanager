/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISTabView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012,2014 Dmitry Baryshnikov
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
#include "wxgis/catalog/catalog.h"
#include "wxgis/catalogui/gxeventui.h"
//#include "wxgis/framework/applicationbase.h"

#include <wx/aui/aui.h>
#include <wx/artprov.h>
#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/sizer.h>

#define LISTSTR _("Content")
#define PREVIEWSTR _("Preview")

class wxGxApplication;

/** @class wxGxTab

    A tab in tabview.

    @library{catalogui}
 */

class wxGxTab :
	public wxPanel,
    public IViewDropTarget
{
    DECLARE_CLASS(wxGxTab)

    enum
	{
		ID_WNDCHOICE = wxID_HIGHEST + 3500
	};

public:
	wxGxTab(wxGxApplication* application, wxXmlNode* pTabDesc, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL);
	virtual ~wxGxTab(void);
	virtual wxString GetName(void);
	virtual wxWindow* GetWindow(int iIndex);
	virtual wxWindow* GetCurrentWindow(void){return m_pCurrentWnd;};
	virtual size_t GetCount(void){return m_pWindows.size();};
	virtual bool Show(bool bShow);
	//virtual wxWindow* GetWindowByName(wxString sName);
	virtual void Deactivate(void);
//IGxDropTarget
    virtual wxDragResult OnDragOver(wxCoord x, wxCoord y, wxDragResult def);
    virtual bool OnDropObjects(wxCoord x, wxCoord y, const wxArrayString& GxObjects, bool bIsControlOn);
    virtual void OnLeave();
    virtual bool CanPaste(void);
//events
	virtual void OnChoice(wxCommandEvent& event);
	virtual void OnSelectionChanged(wxGxSelectionEvent& event);
protected:
	wxVector<wxWindow*> m_pWindows;
	wxString m_sName;

	wxPanel* m_tabwnd;
	wxPanel* m_tabselector;
	wxStaticText* m_staticText;
	wxChoice* m_choice;
	wxBoxSizer* m_bSizerMain;

	bool m_bShowChoices;
	wxWindow *m_pCurrentWnd, *m_pNoWnd;

    wxGxApplication* m_pApp;

DECLARE_EVENT_TABLE()
};

/** @class wxGxTabView gxtabview.h

    The tab view class.

    @library{catalogui}
 */

class WXDLLIMPEXP_GIS_CLU wxGxTabView :
	public wxAuiNotebook,
	public wxGxView
{
    DECLARE_CLASS(wxGxTabView)
public:
    wxGxTabView(void);
	wxGxTabView(wxWindow* parent, wxWindowID id = TABCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~wxGxTabView(void);
    //IView
    virtual bool Create(wxWindow* parent, wxWindowID id = TABCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL|wxNO_FULL_REPAINT_ON_RESIZE | wxCLIP_CHILDREN, const wxString& name = wxT("TabView"));
	virtual bool Activate(IApplication* const application, wxXmlNode* const pConf);
	virtual void Deactivate(void);
//events
	virtual void OnAUINotebookPageChanged(wxAuiNotebookEvent& event);
	virtual void OnSelectionChanged(wxGxSelectionEvent& event);
protected:
	wxVector<wxGxTab*> m_Tabs;
	long m_ConnectionPointSelectionCookie;
	wxGxSelection* m_pSelection;
    wxGxApplication* m_pApp;
private:
    DECLARE_EVENT_TABLE()
};

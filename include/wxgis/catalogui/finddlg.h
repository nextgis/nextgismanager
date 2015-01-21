/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISPropertyDlg class - dialog/dock window with the GxObject properties.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
*   Copyright (C) 2014 NextGIS
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

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/catalogui/gxobjdialog.h"

#include "wxgis/framework/applicationbase.h"

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/generic/stattextg.h>

/** @class wxGISSelectSearchScopeComboPopup
 * 
 * The custom combo popup for select search scope
 * 
 * @library{catalogui}
 */

class wxGISSelectSearchScopeComboPopup : public wxTreeViewComboPopup
{
    DECLARE_CLASS(wxGISSelectSearchScopeComboPopup)
public:	
	virtual void OnSelectionChanged(wxGxSelectionEvent& event);
	virtual bool Create(wxWindow* parent);
    virtual bool Create(wxWindow* parent, wxWindowID id = TREECTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS | wxBORDER_SIMPLE | wxTR_SINGLE | wxTR_EDIT_LABELS | wxTR_NO_LINES, const wxString& name = wxT("ViewComboPopup"));
    //wxGxTreeViewBase
    virtual void AddTreeItem(wxGxObject* pGxObject, wxTreeItemId hParent);
    //
	virtual wxString GetStringValue() const;
	virtual void OnPopup();
	virtual void OnMouseMove(wxMouseEvent& event);
	virtual void OnMouseClick(wxMouseEvent& event);
	virtual void OnDblClick(wxTreeEvent& event);
    virtual bool CanChooseObject(wxGxObject* pObject);
    virtual void SetSelectedObject(wxGxObject* const pGxObject);
    virtual bool CanSearch();
	virtual long GetSelectedObjectId();
protected:
	long m_nSelObject;
};

/** @class wxGISFindResultItemPanel
*
* The custom view for search results item
*
* @library{catalogui}
*/

class wxGISFindResultItemPanel :
    public wxPanel
{
    enum{
        ID_PATHTO = WXGISHIGHEST + 1
    };
public:
    wxGISFindResultItemPanel(wxGxObject* const pObject, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxBORDER_NONE);
    virtual ~wxGISFindResultItemPanel(void);
    //event
    void OnMouseMove(wxMouseEvent& event);
    void OnLeftUp(wxMouseEvent& event);
protected:
    wxGenericStaticText *m_pPath;
    long m_nSelObject;
private:
    DECLARE_EVENT_TABLE();
};

/** @class wxGISFindResultsView
 * 
 * The custom view for search results
 * 
 * @library{catalogui}
 */

class wxGISFindResultsView :
    public wxScrolledWindow
{
public:
    wxGISFindResultsView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxVSCROLL);
    virtual ~wxGISFindResultsView(void);
    virtual void AddPanel(wxGISFindResultItemPanel* const pResultPanel);
    virtual void InsertPanel(wxGISFindResultItemPanel* const pResultPanel, long nPos = 0);
    virtual void RemovePanel(wxGISFindResultItemPanel* const pResultPanel);
protected:
    wxBoxSizer* m_bMainSizer;
};

/** @class wxGISFindDlg

    The wxGISFindDlg class is dialog/panel window find GxObjects

    @library{catalogui}
 */

class wxGISFindDlg : 
	public wxPanel,
	public ITrackCancel
{
protected:
	enum
	{
        ID_WXGXFINDDLG = WXGISHIGHEST + 1
	};
	DECLARE_DYNAMIC_CLASS(wxGISFindDlg)
public:
	wxGISFindDlg(void);
	wxGISFindDlg( wxWindow* parent, wxWindowID id = ID_WXGXFINDDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	virtual ~wxGISFindDlg();
    virtual bool Create(wxWindow* parent, wxWindowID id = ID_WXGXFINDDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("FindView"));
	//events
	virtual void OnFind(wxCommandEvent& event);
	virtual void OnFindUI(wxUpdateUIEvent& event);
protected:
	wxBoxSizer* m_bMainSizer;
	wxButton* m_sdbSizerFind;
	wxXmlNode* m_pConf;
    wxGISSelectSearchScopeComboPopup* m_PopupCtrl;
	wxTextCtrl *m_pFindCtrl;
	wxGISFindResultsView* pFindResultsView;
protected:
	wxString m_sFind;
private:
    DECLARE_EVENT_TABLE()
};

/** @class wxAxFindView

    The wxAxFindView dock window with find GxObjects

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxAxFindView :
	public wxGISFindDlg,
	public IView
{
protected:
    DECLARE_DYNAMIC_CLASS(wxAxFindView)
public:
    wxAxFindView(void);
	wxAxFindView(wxWindow* parent, wxWindowID id = ID_WXGXFINDDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~wxAxFindView(void);
//IView
    virtual bool Create(wxWindow* parent, wxWindowID id = ID_WXGXFINDDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxBORDER_NONE | wxTAB_TRAVERSAL, const wxString& name = wxT("PropertyView"));
	virtual bool Activate(IApplication* const pApplication, wxXmlNode* const pConf);
	virtual void Deactivate(void);
	virtual void Refresh(void){};
	virtual wxString GetViewName(void) const {return m_sViewName;};
	virtual wxIcon GetViewIcon(void){return wxNullIcon;};
	virtual void SetViewIcon(wxIcon Icon){};
	//ITrackCancel	
	virtual IProgressor* const GetProgressor(void);
	virtual void PutMessage(const wxString &sMessage, size_t nIndex, wxGISEnumMessageType eType);
protected:
	wxString m_sViewName;
    wxGISApplicationBase* m_pApp;
private:
    DECLARE_EVENT_TABLE()
};

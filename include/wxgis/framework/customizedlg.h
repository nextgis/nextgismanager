/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  customize dialog class. Customize menues & toolbars
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2012 Bishop
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

#include "wxgis/framework/framework.h"
#include "wxgis/framework/applicationex.h"

#include "wx/intl.h"

#include "wx/gdicmn.h"
#include "wx/aui/auibook.h"
#include "wx/font.h"
#include "wx/colour.h"
#include "wx/settings.h"
#include "wx/string.h"
#include "wx/sizer.h"
#include "wx/button.h"
#include "wx/dialog.h"
#include "wx/splitter.h"
#include "wx/checklst.h"
#include "wx/listctrl.h"
#include "wx/treectrl.h"
#include "wx/panel.h"

/** \class wxBarTreeItemData customizedlg.h
    \brief wxBarTreeItemData class. 
*/
class wxBarTreeItemData : public wxTreeItemData
{
public:
	wxBarTreeItemData(wxGISCommandBar* pBar)
	{
		m_pBar = pBar;
	}

	~wxBarTreeItemData(void)
	{
	}

	wxGISCommandBar* m_pBar;
};

/** \class wxGISToolBarPanel customizedlg.h
    \brief wxGISToolBarPanel class. 
*/
class wxGISToolBarPanel : public wxPanel
{
	enum
	{
		//ID_CHKLSTBX = wxID_HIGHEST + 30,
		ID_TREECTRL = wxID_HIGHEST + 3010,
		ID_BUTTONSLST,
		ID_ONSETKEYCODE,
		ID_CREATECB,
		ID_DELETECB,
		ID_ADDBUTTON,
		ID_REMOVEBUTTON,
		ID_MOVECONTROLUP,
		ID_MOVECONTROLDOWN
	};
public:
	wxGISToolBarPanel(wxGISApplicationEx* pApp, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 540,400 ), long style = wxTAB_TRAVERSAL );
	~wxGISToolBarPanel();
	void SplitterOnIdle( wxIdleEvent& )
	{
		m_Splitter->SetSashPosition( m_nSashPos );
        m_Splitter->Unbind( wxEVT_IDLE, &wxGISToolBarPanel::SplitterOnIdle, this );
		//m_Splitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( wxGISToolBarPanel::SplitterOnIdle ), NULL, this );
	}
	//events
	void OnSelChanged(wxTreeEvent& event);
	//void OnListboxSelect(wxCommandEvent& event);
	void OnDoubleClickSash(wxSplitterEvent& event);
	//void OnCheckboxToggle(wxCommandEvent& event);
	void OnListctrlActivated(wxListEvent& event);
	void OnListctrlRClick(wxListEvent& event);
	void OnSetKeyCode(wxCommandEvent& event);
	void OnUpdateUI(wxUpdateUIEvent& event);
	void OnCreateCommandBar(wxCommandEvent& event);
	void OnDeleteCommandBar(wxCommandEvent& event);
	void OnAddButton(wxCommandEvent& event);
	void OnRemoveButton(wxCommandEvent& event);
	void OnMoveUp(wxCommandEvent& event);
	void OnMoveDown(wxCommandEvent& event);
	void OnLeftDown(wxMouseEvent& event);

	void SetKeyCode(int pos);
	long GetSelectedCommandItem(void);
	void LoadCommands(void);
//private:
//	COMMANDBARARRAY m_CategoryArray;
protected:
	wxSplitterWindow* m_Splitter;
	//wxCheckListBox* m_commandbarlist;
	wxTreeCtrl* m_pTreeCtrl;
	wxListView* m_buttonslist;
	wxButton* m_createbutton;
	wxButton* m_deletebutton;
	wxButton* m_addbutton;
	wxButton* m_rembutton;
	wxButton* m_moveup;
	wxButton* m_movedown;
	wxGISApplicationEx* m_pApp;
	wxImageList m_ImageList, m_TreeImageList;
	wxMenu* m_pContextMenu;
	bool m_bToolsFocus, m_bCmdFocus;
	//int m_nContextMenuPos, m_nMenubarPos, m_nToolbarPos;
	wxTreeItemId m_nMenubarId, m_nContextMenuesId, m_nNewMenuesId, m_nToolBarsId;
protected:
    virtual void SerializePanel(bool bSave = false);
protected:
    int m_nSashPos;
private:  
    DECLARE_EVENT_TABLE()
};

/** \class wxGISCommandPanel customizedlg.h
    \brief wxGISCommandPanel class. 
*/
class wxGISCommandPanel : public wxPanel
{
	enum
	{
		ID_LSTBX = wxID_HIGHEST + 2501,
		ID_LSTCTRL,
		ID_ONSETKEYCODE
	};
public:
	typedef std::map<wxString, wxCommandPtrArray> CATEGORYMAP;

private:
	CATEGORYMAP m_CategoryMap;

protected:
	wxSplitterWindow* m_splitter2;
	wxListBox* m_listBox1;
	wxListCtrl* m_listCtrl3;
	wxGISApplicationEx* m_pApp;
	wxImageList m_ImageList;
	wxMenu* m_pContextMenu;
	int m_CurSelection;

public:
	wxGISCommandPanel( wxGISApplicationEx* pApp, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 540,400 ), long style = wxTAB_TRAVERSAL );
	~wxGISCommandPanel();
	void m_splitter2OnIdle( wxIdleEvent& )
	{
	    m_splitter2->SetSashPosition( m_nSashPos );
        m_splitter2->Unbind( wxEVT_IDLE, &wxGISCommandPanel::m_splitter2OnIdle, this );
	    //m_splitter2->Disconnect( wxEVT_IDLE, wxIdleEventHandler( wxGISCommandPanel::m_splitter2OnIdle ), NULL, this );
	}
	//events
	void OnListboxSelect(wxCommandEvent& event);
	void OnDoubleClickSash(wxSplitterEvent& event);
	void OnListctrlActivated(wxListEvent& event);
	void OnListctrlRClick(wxListEvent& event);
	void OnSetKeyCode(wxCommandEvent& event);

	void SetKeyCode(int pos);
protected:
    virtual void SerializePanel(bool bSave = false);
protected:
    int m_nSashPos;
private:  
    DECLARE_EVENT_TABLE()
};

/** \class wxGISCustomizeDlg customizedlg.h
    \brief wxGISCustomizeDlg class. 
*/
class WXDLLIMPEXP_GIS_FRW wxGISCustomizeDlg : public wxDialog
{
public:
	wxGISCustomizeDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Customize"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 540,400 ), long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER );
	~wxGISCustomizeDlg();
    //events
    virtual void EndModal (int retCode);
protected:
    virtual void SerializeFramePos(bool bSave = false);
protected:
	wxAuiNotebook* m_AuiNotebook;
	wxStdDialogButtonSizer* m_sdbSizer;
	wxButton* m_sdbSizerOK;
	//wxButton* m_sdbSizerCancel;
	wxGISApplicationEx* m_pApp;
};


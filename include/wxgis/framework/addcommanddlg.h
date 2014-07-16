/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  add command in command bar dialog.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010,2013,2014 Dmitry Baryshnikov
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
#include "wxgis/framework/application.h"

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
#include "wx/panel.h"
#include "wx/listbox.h"

class WXDLLIMPEXP_GIS_FRW wxGISAddCommandDlg : public wxDialog
{
	enum
	{
		ID_LSTBX = wxID_HIGHEST + 2051,
		ID_LSTCTRL,
		ID_ONSETKEYCODE,
		MENUID = ID_PLUGINCMD + 1200
	};
public:
	typedef std::map<wxString, wxCommandPtrArray> CATEGORYMAP;
	wxVector<long> m_IDArray;

private:
	CATEGORYMAP m_CategoryMap;

protected:
	wxSplitterWindow* m_Splitter;
	wxListBox* m_ListBox;
	wxListView* m_ListCtrl;
	wxGISApplication* m_pGxApp;
	wxImageList m_ImageList;
	int m_CurSelection;
	wxStdDialogButtonSizer* m_sdbSizer;
	wxButton* m_sdbSizerOK;
	wxButton* m_sdbSizerCancel;

public:
	wxGISAddCommandDlg( wxGISApplication* pGxApp, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Add command"),const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
	~wxGISAddCommandDlg(void);
	void FillIDArray(void);
	void SplitterOnIdle( wxIdleEvent& )
	{
		m_Splitter->SetSashPosition( m_nSashPos );
        m_Splitter->Unbind(wxEVT_IDLE, &wxGISAddCommandDlg::SplitterOnIdle, this );
        //m_Splitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( wxGISAddCommandDlg::SplitterOnIdle ), NULL, this );
	}	
    virtual void EndModal(int retCode);
	//events
	void OnListboxSelect(wxCommandEvent& event);
	void OnDoubleClickSash(wxSplitterEvent& event);
	void OnListctrlActivated(wxListEvent& event);
	void OnUpdateOKUI(wxUpdateUIEvent& event);
	void OnOk(wxCommandEvent& event);
protected:
    virtual void SerializeDialogPos(bool bSave = false);
protected:
    int m_nSashPos;
private:
    DECLARE_EVENT_TABLE()
};

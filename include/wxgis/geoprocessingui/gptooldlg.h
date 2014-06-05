/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  tool dialog class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Dmitry Barishnikov
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
/*
#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/geoprocessingui/geoprocessingui.h"
#include "wxgis/geoprocessingui/gpcontrols.h"
#include "wxgis/geoprocessingui/gptoolbox.h"

#include <wx/intl.h>

#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/html/htmlwin.h>
#include <wx/splitter.h>
#include <wx/dialog.h>

#include <wx/scrolwin.h>
#include <wx/infobar.h>

/** \class wxGISGPToolDlg gptooldlg.h
 *  \brief The tool configuration dialog
 *//*
class WXDLLIMPEXP_GIS_GPU wxGISGPToolDlg : public wxFrame
{
public:
    wxGISGPToolDlg(wxGxRootToolbox* pGxRootToolbox, IGPToolSPtr pTool, bool bSync = false, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxString& title = _("Tool name"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCLOSE_BOX|wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU|wxFRAME_FLOAT_ON_PARENT );//wxSTAY_ON_TOP|wxDIALOG_NO_PARENT|wxCLIP_CHILDREN
    ~wxGISGPToolDlg();
    void m_splitterOnIdle( wxIdleEvent& )
    {
        m_splitter->SetSashPosition( 0 );
        m_splitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( wxGISGPToolDlg::m_splitterOnIdle ), NULL, this );
    }
    void SerializeFramePos(bool bSave = true);
//events
    virtual void OnHelp(wxCommandEvent& event);
    virtual void OnHelpUI(wxUpdateUIEvent& event);
    virtual void OnOk(wxCommandEvent& event);
    virtual void OnCancel(wxCommandEvent& event);
    virtual void OnOkUI(wxUpdateUIEvent& event);
//wxTopLevelWindow
    //virtual bool ShouldPreventAppExit();
protected:
    int m_DataWidth, m_HtmlWidth;
    IGPToolSPtr m_pTool;
    wxXmlNode* m_pPropNode;
    std::vector<wxGISDTBase*> m_pControlsArray;
    wxGxRootToolbox* m_pGxRootToolbox;
    bool m_bSync;
protected:
    wxSplitterWindow* m_splitter;
    wxPanel* m_toolpanel;
    wxStdDialogButtonSizer* m_sdbSizer1;
    wxButton* m_sdbSizer1OK;
    wxButton* m_sdbSizer1Cancel;
    wxButton* m_sdbSizer1Help;
    //wxPanel* m_helppanel;
    wxHtmlWindow* m_htmlWin;
	wxInfoBar *m_pInfoBar;
	wxString m_sCurrentErrMsg;
	int m_nCurrentErrField;
	wxBoxSizer *m_bSizer2;

    DECLARE_EVENT_TABLE()
};
*/

/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Mini map dialog
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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

#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/dialog.h>
#include <wx/aui/aui.h>
#include <wx/statline.h>

#include "wxgis/core/config.h"
#include "wxgis/framework/applicationbase.h"
#include "wxgis/framework/accelerator.h"
#include "wxgis/cartoui/drawingmap.h"

/** @class wxGISMiniMapDlg
 * 
 * The mini map dialog
 * 
 * @library{catalogui}
 */

class wxGISMiniMapDlg : 
	public wxDialog, 
	public wxGISApplicationBase
{
	DECLARE_CLASS(wxGISMiniMapDlg)
public:
	wxGISMiniMapDlg(wxDword nButtons, wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Mini map"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxCAPTION | wxCLOSE_BOX | wxMAXIMIZE_BOX | wxMINIMIZE_BOX | wxRESIZE_BORDER | wxCLIP_CHILDREN);
	virtual ~wxGISMiniMapDlg();
    //wxGISApplicationBase
    virtual wxString GetAppName(void) const { return wxString(wxT("wxGISMiniMapDlg")); };
    virtual wxString GetAppDisplayName(void) const { return wxString(_("Mini map Dialog")); };
    virtual wxString GetAppDisplayNameShort(void) const { return wxString(_("MiniMap Dialog")); };
	//wxGISMiniMapDlg
	virtual void AddLayer(wxGISLayer* pLayer);
	virtual void SetFullExtent();
protected:
    // events
    virtual void OnCommand(wxCommandEvent& event);
    virtual void OnCommandUI(wxUpdateUIEvent& event);
    virtual void OnDropDownCommand(wxCommandEvent& event);
    virtual void OnToolDropDown(wxAuiToolBarEvent& event);
    virtual void OnOK(wxCommandEvent& event);
    virtual void OnOKUI(wxUpdateUIEvent& event);
    virtual void OnMouseMove(wxMouseEvent& event);
    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
    virtual void OnMouseDoubleClick(wxMouseEvent& event);
protected:
    void SerializeFramePos(bool bSave);
protected:
    wxAuiToolBar* m_toolBar;
    wxGISDrawingMapView* m_pGISMapView;    
    IDropDownCommand* m_pDropDownCommand;
private:
    DECLARE_EVENT_TABLE()
};

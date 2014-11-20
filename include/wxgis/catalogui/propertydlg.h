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
#include "wxgis/framework/applicationbase.h"

#include <wx/panel.h>

/** @class wxGISPropertyDlg

    The wxGISPropertyDlg class is dialog/panel window with the GxObject properties

    @library{catalogui}
 */

class WXDLLIMPEXP_GIS_CTU wxGISPropertyDlg : public wxPanel
{
protected:
	enum
	{
		ID_WXGPROPERTYDLG = 1001
	};
	DECLARE_DYNAMIC_CLASS(wxGISPropertyDlg)
public:
	wxGISPropertyDlg(void);
	wxGISPropertyDlg( wxWindow* parent, wxWindowID id = ID_WXGPROPERTYDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
	virtual ~wxGISPropertyDlg();
    virtual bool Create(wxWindow* parent, wxWindowID id = ID_WXGPROPERTYDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("PropertyView"));
protected:
	wxBoxSizer* m_bMainSizer;
	wxXmlNode* m_pConf;
protected:
    wxString m_sAppName;
private:
    DECLARE_EVENT_TABLE()
};

/** @class wxAxPropertyView

    The wxAxPropertyView dock window with GxObject properties

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CTU wxAxPropertyView :
	public wxGISPropertyDlg,
	public IView
{
protected:
    DECLARE_DYNAMIC_CLASS(wxAxPropertyView)
public:
    wxAxPropertyView(void);
	wxAxPropertyView(wxWindow* parent, wxWindowID id = ID_WXGPROPERTYDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~wxAxPropertyView(void);
//IView
    virtual bool Create(wxWindow* parent, wxWindowID id = ID_WXGPROPERTYDLG, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxBORDER_NONE | wxTAB_TRAVERSAL, const wxString& name = wxT("PropertyView"));
	virtual bool Activate(IApplication* const pApplication, wxXmlNode* const pConf);
	virtual void Deactivate(void);
	virtual void Refresh(void){};
	virtual wxString GetViewName(void) const {return m_sViewName;};
	virtual wxIcon GetViewIcon(void){return wxNullIcon;};
	virtual void SetViewIcon(wxIcon Icon){};
protected:
	wxString m_sViewName;
    wxGISApplicationBase* m_pApp;
private:
    DECLARE_EVENT_TABLE()
};

/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISSpatialReferencePropertyPage class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2014 Dmitry Baryshnikov
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
#include "wxgis/datasource/gdalinh.h"
#include "wxgis/catalogui/propertydlg.h"

#include "wx/propgrid/propgrid.h"

/** @class wxGISSpatialReferencePropertyPage

    The tab of spatial reference properties.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGISSpatialReferencePropertyPage :
    public wxGxPropertyPage
{
    DECLARE_DYNAMIC_CLASS(wxGISSpatialReferencePropertyPage)
	enum
	{
		ID_PPCTRL = wxID_HIGHEST + 4503
	};

public:
    wxGISSpatialReferencePropertyPage(void);
    wxGISSpatialReferencePropertyPage(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Spatial Reference"));
	virtual ~wxGISSpatialReferencePropertyPage();
    virtual bool Create(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Spatial Reference"));
	// wxGxPropertyPage
	virtual void Apply(void);
	virtual bool CanApply() const;
	virtual bool FillProperties(wxGxSelection* const pSel);
	//events
	void OnChildFocus(wxChildFocusEvent& event);
protected:
    void FillUndefined(void);
    void FillProjected(const wxGISSpatialReference &oSRS);
    void FillGeographic(const wxGISSpatialReference &oSRS);
    void FillLoclal(const wxGISSpatialReference &oSRS);
    void FillVertical(const wxGISSpatialReference &oSRS);
    void AppendProjParam(wxPGProperty* pid, const char *pszName, const wxGISSpatialReference &oSRS);
    wxPGProperty* AppendProperty(wxPGProperty* pProp);
    wxPGProperty* AppendProperty(wxPGProperty* pid, wxPGProperty* pProp);
protected:
    wxPropertyGrid* m_pg;
private:
    DECLARE_EVENT_TABLE()
};

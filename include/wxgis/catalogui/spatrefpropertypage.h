/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISSpatialReferencePropertyPage class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010,2012,2013 Bishop
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

#include "wxgis/catalogui/catalogui.h"
#include "wxgis/datasource/gdalinh.h"

#include "wx/propgrid/propgrid.h"

/** \class wxGISSpatialReferencePropertyPage spatrefpropertypage.h
    \brief The tab of spatial reference properties.
*/

class WXDLLIMPEXP_GIS_CLU wxGISSpatialReferencePropertyPage : 
    public wxPanel
{
    DECLARE_DYNAMIC_CLASS(wxGISSpatialReferencePropertyPage)
	enum
	{
		ID_PPCTRL = wxID_HIGHEST + 4503
	};

public:
    wxGISSpatialReferencePropertyPage(void);
    wxGISSpatialReferencePropertyPage(const wxGISSpatialReference &oSRS, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Spatial Reference"));
	~wxGISSpatialReferencePropertyPage();
    virtual bool Create(const wxGISSpatialReference &oSRS, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Spatial Reference"));
    virtual wxString GetPageName(void) const {return wxString(_("Spatial Reference"));};
protected:
    void FillUndefined(void);
    void FillProjected(const wxGISSpatialReference &oSRS);
    void FillGeographic(const wxGISSpatialReference &oSRS);
    void FillLoclal(const wxGISSpatialReference &oSRS);
    void AppendProjParam(wxPGProperty* pid, const char *pszName, const wxGISSpatialReference &oSRS);
    wxPGProperty* AppendProperty(wxPGProperty* pProp);
    wxPGProperty* AppendProperty(wxPGProperty* pid, wxPGProperty* pProp);
protected:
    wxPropertyGrid* m_pg;
};

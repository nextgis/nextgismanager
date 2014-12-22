/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISTablePropertyPage class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2012,2014 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/table.h"
#include "wxgis/catalogui/propertydlg.h"

#include "wx/propgrid/propgrid.h"

/** @class wxGISTablePropertyPage

    The tab of table properties.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGISTablePropertyPage :
    public wxGxPropertyPage
{
    DECLARE_DYNAMIC_CLASS(wxGISTablePropertyPage)
	enum
	{
		ID_PPCTRL = wxID_HIGHEST + 4502
	};

public:
    wxGISTablePropertyPage(void);
    wxGISTablePropertyPage(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Table"));
	~wxGISTablePropertyPage();
    virtual bool Create(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Table"));
    virtual wxString GetPageName(void){return wxString(_("Table"));};
    wxPGProperty* AppendProperty(wxPGProperty* pProp);
    wxPGProperty* AppendProperty(wxPGProperty* pid, wxPGProperty* pProp);
    wxPGProperty* AppendMetadataProperty(wxString sMeta);
    void FillGrid(void);
    void FillLayerDef(OGRLayer *poLayer, int iLayer, CPLString soPath);
	// wxGxPropertyPage
	virtual void Apply(void);
	virtual bool CanApply() const;
	virtual bool FillProperties(wxGxSelection* const pSel);
	//events
	void OnChildFocus(wxChildFocusEvent& event);
protected:
    wxGISTable* m_pDataset;
    wxGxDataset* m_pGxDataset;

    wxPropertyGrid* m_pg;
	long m_nCounter;
private:
    DECLARE_EVENT_TABLE()
};

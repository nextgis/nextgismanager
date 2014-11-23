/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISVectorPropertyPage class.
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
#include "wxgis/catalogui/gxdatasetui.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/catalogui/propertydlg.h"

#include "wx/propgrid/propgrid.h"

/** @class wxGISVectorPropertyPage

    The tab of vector dataset properties.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGISVectorPropertyPage :
    public wxGxPropertyPage
{
    DECLARE_DYNAMIC_CLASS(wxGISVectorPropertyPage)
	enum
	{
		ID_PPCTRL = wxID_HIGHEST + 4502
	};

public:
    wxGISVectorPropertyPage(void);
    wxGISVectorPropertyPage(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Vector"));
	~wxGISVectorPropertyPage();
    virtual bool Create(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Vector"));
	// wxGxPropertyPage
	virtual void Apply(void);
	virtual bool CanApply() const;
	virtual bool FillProperties(wxGxSelection* const pSel);
	//events
	void OnChildFocus(wxChildFocusEvent& event);
protected:
    wxPGProperty* AppendProperty(wxPGProperty* pProp);
    wxPGProperty* AppendProperty(wxPGProperty* pid, wxPGProperty* pProp);
    wxPGProperty* AppendMetadataProperty(wxString sMeta);
    void FillGrid(void);
    void FillLayerDef(OGRLayer *poLayer, int iLayer, CPLString soPath);
protected:
    wxGISFeatureDataset* m_pDataset;
    wxGxDataset* m_pGxDataset;
    wxPropertyGrid* m_pg;
	long m_nCounter;

private:
    DECLARE_EVENT_TABLE()
};

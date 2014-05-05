/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISVectorPropertyPage class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2013 Bishop
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

#include "wx/propgrid/propgrid.h"

/** \class wxGISVectorPropertyPage vectorpropertypage.h
    \brief The tab of vector dataset properties.
*/

class WXDLLIMPEXP_GIS_CLU wxGISVectorPropertyPage : 
    public wxPanel
{
    DECLARE_DYNAMIC_CLASS(wxGISVectorPropertyPage)
	enum
	{
		ID_PPCTRL = wxID_HIGHEST + 4502
	};

public:
    wxGISVectorPropertyPage(void);
    wxGISVectorPropertyPage(IGxDataset* pGxDataset, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Vector"));
	~wxGISVectorPropertyPage();
    virtual bool Create(IGxDataset* pGxDataset, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Vector"));
    virtual wxString GetPageName(void) const {return wxString(_("Vector"));};
    wxPGProperty* AppendProperty(wxPGProperty* pProp);
    wxPGProperty* AppendProperty(wxPGProperty* pid, wxPGProperty* pProp);
    wxPGProperty* AppendMetadataProperty(wxString sMeta);
    void FillGrid(void);
    void FillLayerDef(OGRLayer *poLayer, int iLayer, CPLString soPath);
protected:
    wxGISFeatureDataset* m_pDataset;
    IGxDataset* m_pGxDataset;
    wxPropertyGrid* m_pg;
	long m_nCounter;

    DECLARE_EVENT_TABLE()
};

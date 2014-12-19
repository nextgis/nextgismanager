/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISRasterPropertyPage class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011,2013,2014 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/rasterdataset.h"
#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/geoprocessingui/geoprocessingui.h"
#include "wxgis/core/event.h"
#include "wxgis/catalogui/propertydlg.h"

#include "wx/wxfreechartdefs.h"
#include "wx/chartpanel.h"
#include "wx/propgrid/propgrid.h"

/** @class wxGISRasterPropertyPage
    
    The tab of raster dataset properties.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGISRasterPropertyPage :
    public wxGxPropertyPage
{
    DECLARE_DYNAMIC_CLASS(wxGISRasterPropertyPage)
	enum
	{
		ID_PPCTRL = wxID_HIGHEST + 4504
	};

public:
    wxGISRasterPropertyPage(void);
    wxGISRasterPropertyPage(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Raster"));
	~wxGISRasterPropertyPage();
    virtual bool Create(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Raster"));
	// wxGxPropertyPage
	virtual void Apply(void);
	virtual bool CanApply() const;
	virtual bool FillProperties(wxGxSelection* const pSel);
    //events
    void OnPropertyGridButtonClick ( wxCommandEvent& );
    void OnFinish(wxGISProcessEvent& event);
	void OnChildFocus(wxChildFocusEvent& event);
protected:
    wxPGProperty* AppendProperty(wxPGProperty* pProp);
    wxPGProperty* AppendProperty(wxPGProperty* pid, wxPGProperty* pProp);
    wxPGProperty* AppendMetadataProperty(wxPGProperty* pid, wxString sMeta);
    wxPGProperty* GetSubProperty(wxPGProperty* pid, const wxString &sName, wxString &sResultName);
    void FillGrid(void);	
protected:
    wxGISRasterDataset* m_pDataset;
    IGxDataset* m_pGxDataset;
    wxPropertyGrid* m_pg;
	long m_nCounter;
private:
    DECLARE_EVENT_TABLE()
};


/** @class wxGISRasterHistogramPropertyPage

    The tab of raster bands histogram.

    @library{catalogui}
*/

class WXDLLIMPEXP_GIS_CLU wxGISRasterHistogramPropertyPage :
    public wxGxPropertyPage
{
    DECLARE_DYNAMIC_CLASS(wxGISRasterHistogramPropertyPage)
public:
    wxGISRasterHistogramPropertyPage(void);
    wxGISRasterHistogramPropertyPage(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Raster"));
    ~wxGISRasterHistogramPropertyPage();
    virtual bool Create(ITrackCancel * const pTrackCancel, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Raster"));
	// wxGxPropertyPage
	virtual void Apply(void);
	virtual bool CanApply() const;
	virtual bool FillProperties(wxGxSelection* const pSel);
protected:
    virtual void FillHistogram();
protected:
    wxGISRasterDataset* m_pDataset;
    wxGxRasterDataset* m_pGxDataset;
    wxChartPanel *m_pChartPanel;
private:
    DECLARE_EVENT_TABLE()
};


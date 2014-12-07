/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDatasetUI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011,2013.2014 Dmitry Baryshnikov
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

#include "wxgis/catalogui/gxdatasetui.h"

#include "wxgis/framework/application.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/rasterdataset.h"

//propertypages
#include "wxgis/catalogui/spatrefpropertypage.h"
#include "wxgis/catalogui/rasterpropertypage.h"
#include "wxgis/catalogui/vectorpropertypage.h"
#include "wxgis/catalogui/tablepropertypage.h"

#include "../../art/properties.xpm"

#include "wx/busyinfo.h"
#include "wx/utils.h"
#include "wx/propdlg.h"

//--------------------------------------------------------------
// wxGxTableUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxTableUI, wxGxTable)

wxGxTableUI::wxGxTableUI(wxGISEnumTableType nType, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon) : wxGxTable(nType, oParent, soName, soPath)
{
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
}

wxGxTableUI::~wxGxTableUI(void)
{
}

wxIcon wxGxTableUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxTableUI::GetSmallImage(void)
{
	return m_SmallIcon;
}

wxArrayString wxGxTableUI::GetPropertyPages() const
{
	wxArrayString out;
	out.Add("wxGISTablePropertyPage");	
	return out;
}

bool wxGxTableUI::HasPropertyPages(void) const
{
	return true;
}

wxGISDataset* const wxGxTableUI::GetDataset(bool bCached, ITrackCancel* const pTrackCancel)
{
    wxGISDataset* const pOut = wxGxTable::GetDataset(bCached, pTrackCancel);
    if(NULL == pOut)
    {
 		wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Open"));
        wxGISErrorMessageBox(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()));
    }
    wsGET(pOut);
}

//--------------------------------------------------------------
// wxGxFeatureDatasetUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxFeatureDatasetUI, wxGxFeatureDataset)

wxGxFeatureDatasetUI::wxGxFeatureDatasetUI(wxGISEnumVectorDatasetType nType, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon & LargeIcon, const wxIcon & SmallIcon) : wxGxFeatureDataset(nType, oParent, soName, soPath)
{
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
}

wxGxFeatureDatasetUI::~wxGxFeatureDatasetUI(void)
{
}

wxIcon wxGxFeatureDatasetUI::GetLargeImage(void)
{
    return m_LargeIcon;
}

wxIcon wxGxFeatureDatasetUI::GetSmallImage(void)
{
    return m_SmallIcon;
}

wxArrayString wxGxFeatureDatasetUI::GetPropertyPages() const
{
	wxArrayString out;
	out.Add("wxGISVectorPropertyPage");
	out.Add("wxGISSpatialReferencePropertyPage");	
	return out;
}

bool wxGxFeatureDatasetUI::HasPropertyPages(void) const
{
	return true;
}

wxGISDataset* const wxGxFeatureDatasetUI::GetDataset(bool bCached, ITrackCancel* const pTrackCancel)
{
    wxGISDataset* const pOut = wxGxFeatureDataset::GetDataset(bCached, pTrackCancel);
    if(NULL == pOut)
    {
		wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Open"));
        wxGISErrorMessageBox(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()));
    }
    wsGET(pOut);
}

//--------------------------------------------------------------
// wxGxRasterDatasetUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxRasterDatasetUI, wxGxRasterDataset)

wxGxRasterDatasetUI::wxGxRasterDatasetUI(wxGISEnumRasterDatasetType nType, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon & LargeIcon, const wxIcon & SmallIcon) : wxGxRasterDataset(nType, oParent, soName, soPath)
{
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
}

wxGxRasterDatasetUI::~wxGxRasterDatasetUI(void)
{
}

wxIcon wxGxRasterDatasetUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxRasterDatasetUI::GetSmallImage(void)
{
	return m_SmallIcon;
}

wxArrayString wxGxRasterDatasetUI::GetPropertyPages() const
{
	wxArrayString out;
	out.Add("wxGISRasterPropertyPage");
	out.Add("wxGISSpatialReferencePropertyPage");	
	out.Add("wxGISRasterHistogramPropertyPage");
	return out;
}

bool wxGxRasterDatasetUI::HasPropertyPages(void) const
{
	return true;
}

wxGISDataset* const wxGxRasterDatasetUI::GetDataset(bool bCached, ITrackCancel* const pTrackCancel)
{
    wxGISDataset* const pOut = wxGxRasterDataset::GetDataset(bCached, pTrackCancel);
    if(NULL == pOut)
    {
		wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Open"));
        wxGISErrorMessageBox(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()));
    }
    wsGET(pOut);
}


//--------------------------------------------------------------
// wxGxRasterDatasetContainerUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxRasterDatasetContainerUI, wxGxRasterDatasetContainer)

wxGxRasterDatasetContainerUI::wxGxRasterDatasetContainerUI(wxGISEnumRasterDatasetType nType, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon & LargeIcon, const wxIcon & SmallIcon) : wxGxRasterDatasetContainer(nType, oParent, soName, soPath)
{
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
}

wxGxRasterDatasetContainerUI::~wxGxRasterDatasetContainerUI(void)
{
}

wxIcon wxGxRasterDatasetContainerUI::GetLargeImage(void)
{
    return m_LargeIcon;
}

wxIcon wxGxRasterDatasetContainerUI::GetSmallImage(void)
{
    return m_SmallIcon;
}

wxArrayString wxGxRasterDatasetContainerUI::GetPropertyPages() const
{
    wxArrayString out;
    out.Add("wxGISRasterPropertyPage");
    out.Add("wxGISSpatialReferencePropertyPage");
    out.Add("wxGISRasterHistogramPropertyPage");
    return out;
}

bool wxGxRasterDatasetContainerUI::HasPropertyPages(void) const
{
    return true;
}

wxGISDataset* const wxGxRasterDatasetContainerUI::GetDataset(bool bCached, ITrackCancel* const pTrackCancel)
{
    wxGISDataset* const pOut = wxGxRasterDatasetContainer::GetDataset(bCached, pTrackCancel);
    if (NULL == pOut)
    {
        wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Open"));
        wxGISErrorMessageBox(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()));
    }
    wsGET(pOut);
}


/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDatasetUI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011,2013 Bishop
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
//class wxGxTableDatasetUI
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxTableDatasetUI, wxGxTableDataset)

wxGxTableDatasetUI::wxGxTableDatasetUI(wxGISEnumTableDatasetType nType, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon) : wxGxTableDataset(nType, oParent, soName, soPath)
{
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
}

wxGxTableDatasetUI::~wxGxTableDatasetUI(void)
{
}

wxIcon wxGxTableDatasetUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxTableDatasetUI::GetSmallImage(void)
{
	return m_SmallIcon;
}

void wxGxTableDatasetUI::EditProperties(wxWindow *parent)
{
    wxPropertySheetDialog PropertySheetDialog;
    if (!PropertySheetDialog.Create(parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxSize( 480,640 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    PropertySheetDialog.SetIcon(properties_xpm);
    PropertySheetDialog.CreateButtons(wxOK);
    wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());

    wxGISTablePropertyPage* TablePropertyPage = new wxGISTablePropertyPage(this, pParentWnd);
    PropertySheetDialog.GetBookCtrl()->AddPage(TablePropertyPage, TablePropertyPage->GetPageName());

    //PropertySheetDialog.LayoutDialog();
    PropertySheetDialog.SetSize(480,640);
    PropertySheetDialog.Center();

    PropertySheetDialog.ShowModal();
}

wxGISDataset* const wxGxTableDatasetUI::GetDataset(bool bCached, ITrackCancel* const pTrackCancel)
{
    wxGISDataset* const pOut = wxGxTableDataset::GetDataset(bCached, pTrackCancel);
    if(NULL == pOut)
    {
        const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s"), _("Open"), wxString(err, wxConvUTF8).c_str());
        wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
    }
    wsGET(pOut);
}

bool wxGxTableDatasetUI::Invoke(wxWindow* pParentWnd)
{
    EditProperties(pParentWnd);
    return true;
}

//--------------------------------------------------------------
//class wxGxFeatureDatasetUI
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

void wxGxFeatureDatasetUI::EditProperties(wxWindow *parent)
{
    wxPropertySheetDialog PropertySheetDialog;
    if (!PropertySheetDialog.Create(parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxSize( 480,640 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    PropertySheetDialog.SetIcon(properties_xpm);
    PropertySheetDialog.CreateButtons(wxOK);
    wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());

    wxGISVectorPropertyPage* VectorPropertyPage = new wxGISVectorPropertyPage(this, pParentWnd);
    PropertySheetDialog.GetBookCtrl()->AddPage(VectorPropertyPage, VectorPropertyPage->GetPageName());

	wxGISFeatureDataset* pDset = wxDynamicCast(GetDataset(), wxGISFeatureDataset);
	if(pDset)
	{
        if(pDset->IsOpened())
            pDset->Open();
		wxGISSpatialReferencePropertyPage* SpatialReferencePropertyPage = new wxGISSpatialReferencePropertyPage(pDset->GetSpatialReference(), pParentWnd);
		PropertySheetDialog.GetBookCtrl()->AddPage(SpatialReferencePropertyPage, SpatialReferencePropertyPage->GetPageName());

        wsDELETE(pDset);
	}

    //PropertySheetDialog.LayoutDialog();
    PropertySheetDialog.SetSize(480,640);
    PropertySheetDialog.Center();

    PropertySheetDialog.ShowModal();
}

wxGISDataset* const wxGxFeatureDatasetUI::GetDataset(bool bCached, ITrackCancel* const pTrackCancel)
{
    wxGISDataset* const pOut = wxGxFeatureDataset::GetDataset(bCached, pTrackCancel);
    if(NULL == pOut)
    {
        const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s"), _("Open"), wxString(err, wxConvUTF8).c_str());
        wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
    }
    wsGET(pOut);
}

bool wxGxFeatureDatasetUI::Invoke(wxWindow* pParentWnd)
{
    EditProperties(pParentWnd);
    return true;
}

//--------------------------------------------------------------
//class wxGxRasterDatasetUI
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

void wxGxRasterDatasetUI::EditProperties(wxWindow *parent)
{
    wxPropertySheetDialog PropertySheetDialog;
    if (!PropertySheetDialog.Create(parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    PropertySheetDialog.SetIcon(properties_xpm);
    PropertySheetDialog.CreateButtons(wxOK);
    wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());

    wxGISRasterPropertyPage* RasterPropertyPage = new wxGISRasterPropertyPage(this, pParentWnd);
    PropertySheetDialog.GetBookCtrl()->AddPage(RasterPropertyPage, RasterPropertyPage->GetPageName());
	wxGISRasterDataset* pDset = wxDynamicCast(GetDataset(), wxGISRasterDataset);
	if(NULL != pDset)
	{
        if(!pDset->IsOpened())
            pDset->Open(true);
		wxGISSpatialReferencePropertyPage* SpatialReferencePropertyPage = new wxGISSpatialReferencePropertyPage(pDset->GetSpatialReference(), pParentWnd);
		PropertySheetDialog.GetBookCtrl()->AddPage(SpatialReferencePropertyPage, SpatialReferencePropertyPage->GetPageName());

        wsDELETE(pDset);
	}
    wxGISRasterHistogramPropertyPage* RasterHistogramPropertyPage = new wxGISRasterHistogramPropertyPage(this, pParentWnd);
    PropertySheetDialog.GetBookCtrl()->AddPage(RasterHistogramPropertyPage, RasterHistogramPropertyPage->GetPageName());

    //TODO: Additional page for virtual raster VRTSourcedDataset with sources files

    //PropertySheetDialog.LayoutDialog();
    PropertySheetDialog.SetSize(480,640);
    PropertySheetDialog.Center();

    PropertySheetDialog.ShowModal();
}

bool wxGxRasterDatasetUI::Invoke(wxWindow* pParentWnd)
{
    EditProperties(pParentWnd);
    return true;
}

wxGISDataset* const wxGxRasterDatasetUI::GetDataset(bool bCached, ITrackCancel* const pTrackCancel)
{
    wxGISDataset* const pOut = wxGxRasterDataset::GetDataset(bCached, pTrackCancel);
    if(NULL == pOut)
    {
        const char* err = CPLGetLastErrorMsg();
		wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s"), _("Open"), wxString(err, wxConvUTF8).c_str());
        wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);
    }
    wsGET(pOut);
}


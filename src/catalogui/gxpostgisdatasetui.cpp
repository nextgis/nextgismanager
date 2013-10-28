/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxPostGISDatasetUI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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
#include "wxgis/catalogui/gxpostgisdatasetui.h"

#ifdef wxGIS_USE_POSTGRES

//#include "wxgis/framework/application.h"

//propertypages
#include "wxgis/catalogui/spatrefpropertypage.h"
#include "wxgis/catalogui/vectorpropertypage.h"
#include "wxgis/catalogui/tablepropertypage.h"

#include "../../art/properties.xpm"

#include "wx/utils.h"
#include "wx/propdlg.h"
#include "wx/bookctrl.h"

//-----------------------------------------------------------------------------
// wxGxPostGISTableDatasetUI
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxPostGISTableDatasetUI, wxGxPostGISTableDataset)

wxGxPostGISTableDatasetUI::wxGxPostGISTableDatasetUI(const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon) : wxGxPostGISTableDataset(sSchema, pwxGISRemoteConn, oParent, soName, soPath)
{
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
}

wxGxPostGISTableDatasetUI::~wxGxPostGISTableDatasetUI(void)
{
}

wxIcon wxGxPostGISTableDatasetUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxPostGISTableDatasetUI::GetSmallImage(void)
{
	return m_SmallIcon;
}


void wxGxPostGISTableDatasetUI::EditProperties(wxWindow *parent)
{
    wxPropertySheetDialog PropertySheetDialog;
    if (!PropertySheetDialog.Create(parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxSize( 480,640 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    PropertySheetDialog.SetIcon(properties_xpm);
    PropertySheetDialog.CreateButtons(wxOK);
    wxWindow* pParentWnd = wxStaticCast(PropertySheetDialog.GetBookCtrl(), wxWindow);

    wxGISTablePropertyPage* TablePropertyPage = new wxGISTablePropertyPage(this, pParentWnd);
    PropertySheetDialog.GetBookCtrl()->AddPage(TablePropertyPage, TablePropertyPage->GetPageName());

    //PropertySheetDialog.LayoutDialog();
    PropertySheetDialog.SetSize(480,640);
    PropertySheetDialog.Center();

    PropertySheetDialog.ShowModal();
}

//-----------------------------------------------------------------------------
// wxGxPostGISFeatureDatasetUI
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxPostGISFeatureDatasetUI, wxGxPostGISFeatureDataset)

wxGxPostGISFeatureDatasetUI::wxGxPostGISFeatureDatasetUI(const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon) : wxGxPostGISFeatureDataset(sSchema, pwxGISRemoteConn, oParent, soName, soPath)
{
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
}

wxGxPostGISFeatureDatasetUI::~wxGxPostGISFeatureDatasetUI(void)
{
}

wxIcon wxGxPostGISFeatureDatasetUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxPostGISFeatureDatasetUI::GetSmallImage(void)
{
	return m_SmallIcon;
}


void wxGxPostGISFeatureDatasetUI::EditProperties(wxWindow *parent)
{
    wxPropertySheetDialog PropertySheetDialog;
    if (!PropertySheetDialog.Create(parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxSize( 480,640 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    PropertySheetDialog.SetIcon(properties_xpm);
    PropertySheetDialog.CreateButtons(wxOK);
    wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());

    wxGISVectorPropertyPage* VectorPropertyPage = new wxGISVectorPropertyPage(this, pParentWnd);
    PropertySheetDialog.GetBookCtrl()->AddPage(VectorPropertyPage, VectorPropertyPage->GetPageName());

	wxGISDataset* pDset = GetDataset();
	if(pDset)
	{
		wxGISSpatialReferencePropertyPage* SpatialReferencePropertyPage = new wxGISSpatialReferencePropertyPage(pDset->GetSpatialReference(), pParentWnd);
		PropertySheetDialog.GetBookCtrl()->AddPage(SpatialReferencePropertyPage, SpatialReferencePropertyPage->GetPageName());
	}

    //PropertySheetDialog.LayoutDialog();
    PropertySheetDialog.SetSize(480,640);
    PropertySheetDialog.Center();

    PropertySheetDialog.ShowModal();
}

#endif //wxGIS_USE_POSTGRES
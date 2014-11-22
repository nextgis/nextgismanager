/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxPostGISDatasetUI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Dmitry Baryshnikov
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
// wxGxPostGISTableUI
//-----------------------------------------------------------------------------

class wxGxPostGISTableUI;
IMPLEMENT_CLASS(wxGxPostGISTableUI, wxGxPostGISTable)

wxGxPostGISTableUI::wxGxPostGISTableUI(int nRemoteId, const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon) : wxGxPostGISTable(nRemoteId, sSchema, pwxGISRemoteConn, oParent, soName, soPath)
{
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
}

wxGxPostGISTableUI::~wxGxPostGISTableUI(void)
{
}

wxIcon wxGxPostGISTableUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxPostGISTableUI::GetSmallImage(void)
{
	return m_SmallIcon;
}

wxArrayString wxGxPostGISTableUI::GetPropertyPages() const
{
	wxArrayString out;
	out.Add("wxGISTablePropertyPage");	
	return out;
}

bool wxGxPostGISTableUI::HasPropertyPages(void) const
{
	return true;
}

//-----------------------------------------------------------------------------
// wxGxPostGISFeatureDatasetUI
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxPostGISFeatureDatasetUI, wxGxPostGISFeatureDataset)

wxGxPostGISFeatureDatasetUI::wxGxPostGISFeatureDatasetUI(int nRemoteId, const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon) : wxGxPostGISFeatureDataset(nRemoteId, sSchema, pwxGISRemoteConn, oParent, soName, soPath)
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

wxArrayString wxGxPostGISFeatureDatasetUI::GetPropertyPages() const
{
	wxArrayString out;
	out.Add("wxGISVectorPropertyPage");	
	out.Add("wxGISSpatialReferencePropertyPage");	
	return out;
}

bool wxGxPostGISFeatureDatasetUI::HasPropertyPages(void) const
{
	return true;
}

#endif //wxGIS_USE_POSTGRES

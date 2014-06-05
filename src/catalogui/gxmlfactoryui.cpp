/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMLFactoryUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011,2014 Dmitry Barishnikov
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

#include "wxgis/catalogui/gxmlfactoryui.h"
#include "wxgis/catalogui/gxmldatasetui.h"
#include "wxgis/catalogui/gxdatasetui.h"

#include "../../art/dxf_dset_16.xpm"
#include "../../art/dxf_dset_48.xpm"
#include "../../art/kml_dset_16.xpm"
#include "../../art/kml_dset_48.xpm"
#include "../../art/kmz_dset_16.xpm"
#include "../../art/kmz_dset_48.xpm"
#include "../../art/gml_dset_16.xpm"
#include "../../art/gml_dset_48.xpm"
#include "../../art/kml_subdset_16.xpm"
#include "../../art/kml_subdset_48.xpm"
#include "../../art/json_dset_16.xpm"
#include "../../art/json_dset_48.xpm"
#include "../../art/sxf_dset_16.xpm"
#include "../../art/sxf_dset_48.xpm"
#include "../../art/sxf_subdset_16.xpm"
#include "../../art/sxf_subdset_48.xpm"

IMPLEMENT_DYNAMIC_CLASS(wxGxMLFactoryUI, wxGxMLFactory)

wxGxMLFactoryUI::wxGxMLFactoryUI(void) : wxGxMLFactory()
{
    m_SmallDXFIcon = wxIcon(dxf_dset_16_xpm);
    m_LargeDXFIcon = wxIcon(dxf_dset_48_xpm);
    m_SmallKMLIcon = wxIcon(kml_dset_16_xpm);
    m_LargeKMLIcon = wxIcon(kml_dset_48_xpm);
    m_SmallKMZIcon = wxIcon(kmz_dset_16_xpm);
    m_LargeKMZIcon = wxIcon(kmz_dset_48_xpm);
    m_SmallGMLIcon = wxIcon(gml_dset_16_xpm);
    m_LargeGMLIcon = wxIcon(gml_dset_48_xpm);
    m_LargeSubKMLIcon = wxIcon(kml_subdset_48_xpm);
    m_SmallSubKMLIcon = wxIcon(kml_subdset_16_xpm);
    m_SmallJsonIcon = wxIcon(json_dset_16_xpm);
    m_LargeJsonIcon = wxIcon(json_dset_48_xpm);
    m_LargeSXFIcon = wxIcon(sxf_dset_48_xpm); 
    m_SmallSXFIcon = wxIcon(sxf_dset_16_xpm); 
    m_LargeSubSXFIcon = wxIcon(sxf_subdset_48_xpm); 
    m_SmallSubSXFIcon = wxIcon(sxf_subdset_16_xpm);

}

wxGxMLFactoryUI::~wxGxMLFactoryUI(void)
{
}

wxGxObject* wxGxMLFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumVectorDatasetType type, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    switch(type)
    {
    case enumVecKML:
        {
        wxGxMLDatasetUI* pDataset = new wxGxMLDatasetUI(type, pParent, soName, szPath, m_LargeKMLIcon, m_SmallKMLIcon, m_LargeSubKMLIcon, m_SmallSubKMLIcon);
        return wxStaticCast(pDataset, wxGxObject);
        }
    case enumVecKMZ:
        {
        wxGxMLDatasetUI* pDataset = new wxGxMLDatasetUI(type, pParent, soName, szPath, m_LargeKMZIcon, m_SmallKMZIcon, m_LargeSubKMLIcon, m_SmallSubKMLIcon);
        return wxStaticCast(pDataset, wxGxObject);
        }
    case enumVecGML:
        {
	    wxGxMLDatasetUI* pDataset = new wxGxMLDatasetUI(type, pParent, soName, szPath, m_LargeGMLIcon, m_SmallGMLIcon, m_LargeGMLIcon, m_SmallGMLIcon);
        return wxStaticCast(pDataset, wxGxObject);
        }
    case enumVecSXF:
        {
        wxGxMLDatasetUI* pDataset = new wxGxMLDatasetUI(type, pParent, soName, szPath, m_LargeSXFIcon, m_SmallSXFIcon, m_LargeSubSXFIcon, m_SmallSubSXFIcon);
        return wxStaticCast(pDataset, wxGxObject);
        }
    case enumVecDXF:
        {
	    wxGxFeatureDatasetUI* pDataset = new wxGxFeatureDatasetUI(type, pParent, soName, szPath, m_LargeDXFIcon, m_SmallDXFIcon);
        return wxStaticCast(pDataset, wxGxObject);
        }
    case enumVecGeoJSON:
    {
        wxGxFeatureDatasetUI* pDataset = new wxGxFeatureDatasetUI(type, pParent, soName, szPath, m_LargeJsonIcon, m_SmallJsonIcon);
        return wxStaticCast(pDataset, wxGxObject);
    }
    default:
        break;
    }
    return NULL;
}

/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  local db (sqlite, gdab, mdb) ui classes.
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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
#include "wxgis/catalogui/gxlocaldbui.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/datasource/datacontainer.h"

#include "../../art/shp_dset_16.xpm"
#include "../../art/shp_dset_48.xpm"
#include "../../art/table_dbf_16.xpm"
#include "../../art/table_dbf_48.xpm"

//---------------------------------------------------------------------------
// wxGxOpenFileGDBUI
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxOpenFileGDBUI, wxGxOpenFileGDB)

wxGxOpenFileGDBUI::wxGxOpenFileGDBUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon & LargeIcon, const wxIcon & SmallIcon) : wxGxOpenFileGDB(oParent, soName, soPath)
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;
}

wxGxOpenFileGDBUI::~wxGxOpenFileGDBUI(void)
{
}

wxIcon wxGxOpenFileGDBUI::GetLargeImage(void)
{
    return m_oLargeIcon;
}

wxIcon wxGxOpenFileGDBUI::GetSmallImage(void)
{
    return m_oSmallIcon;
}

void wxGxOpenFileGDBUI::LoadChildren(void)
{
    if (m_bIsChildrenLoaded)
        return;

    wxGISDataSource* pDSource = wxDynamicCast(GetDatasetFast(), wxGISDataSource);

    if (NULL == pDSource || !pDSource->Open(false))
    {
        return;
    }


    for (size_t i = 0; i < pDSource->GetSubsetsCount(); ++i)
    {
        wxGISDataset* pDSet = pDSource->GetSubset(i);
        if (NULL == pDSet)
            continue;
        switch (pDSet->GetType())
        {
        case enumGISFeatureDataset:
            if (!m_SmallSHPIcon.IsOk())
                m_SmallSHPIcon = wxIcon(shp_dset_16_xpm);
            if (!m_LargeSHPIcon.IsOk())
                m_LargeSHPIcon = wxIcon(shp_dset_48_xpm);

            new wxGxInitedFeatureDatasetUI(pDSet, wxStaticCast(this, wxGxObject), pDSet->GetName(), wxGxObjectContainer::GetPath(), m_LargeSHPIcon, m_SmallSHPIcon);
            break;
        case enumGISTable:
            if (!m_SmallDBFIcon.IsOk())
                m_SmallDBFIcon = wxIcon(table_dbf_16_xpm);
            if (!m_LargeDBFIcon.IsOk())
                m_LargeDBFIcon = wxIcon(table_dbf_48_xpm);
            new wxGxInitedTableUI(pDSet, wxStaticCast(this, wxGxObject), pDSet->GetName(), wxGxObjectContainer::GetPath(), m_LargeDBFIcon, m_SmallDBFIcon);
            break;
        case enumGISRasterDataset:
            //TODO:
            break;
        default:
            break;
        }
    }
    wsDELETE(pDSource);

    m_bIsChildrenLoaded = true;
}

//---------------------------------------------------------------------------
// wxGxInitedFeatureDatasetUI
//---------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxInitedFeatureDatasetUI, wxGxFeatureDatasetUI)

wxGxInitedFeatureDatasetUI::wxGxInitedFeatureDatasetUI(wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon) : wxGxFeatureDatasetUI(enumVecFileDBLayer, oParent, soName, soPath, LargeIcon, SmallIcon)
{
    wsSET(m_pwxGISDataset, pwxGISDataset);

    m_sPath = CPLString(CPLFormFilename(soPath, soName.mb_str(wxConvUTF8), ""));
}

wxGxInitedFeatureDatasetUI::~wxGxInitedFeatureDatasetUI()
{
    wsDELETE(m_pwxGISDataset);
}


//---------------------------------------------------------------------------
// wxGxInitedTableUI
//---------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxInitedTableUI, wxGxTableUI)

wxGxInitedTableUI::wxGxInitedTableUI(wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon) : wxGxTableUI(enumTableFileDBLayer, oParent, soName, soPath, LargeIcon, SmallIcon)
{
    wsSET(m_pwxGISDataset, pwxGISDataset);

    m_sPath = CPLString(CPLFormFilename(soPath, soName.mb_str(wxConvUTF8), ""));
}

wxGxInitedTableUI::~wxGxInitedTableUI()
{
    wsDELETE(m_pwxGISDataset);
}

/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxShapeFactoryUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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
#include "wxgis/catalogui/gxgnmfactoryui.h"

#ifdef wxGIS_USE_GDAL_GNM

#include "wxgis/catalogui/gxgnmui.h"

#include "../../art/gnm_16.xpm"
#include "../../art/gnm_48.xpm"

IMPLEMENT_DYNAMIC_CLASS(wxGxGNMFactoryUI, wxGxGNMFactory)

wxGxGNMFactoryUI::wxGxGNMFactoryUI(void) : wxGxGNMFactory()
{
    m_SmallGNMIcon = wxIcon(gnm_16_xpm);
    m_LargeGNMIcon = wxIcon(gnm_48_xpm);
}

wxGxGNMFactoryUI::~wxGxGNMFactoryUI(void)
{
}

wxGxObject* wxGxGNMFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    wxGxGNMConnectivityUI* pDataset = new wxGxGNMConnectivityUI(enumVecESRIShapefile, pParent, soName, szPath, m_LargeGNMIcon, m_SmallGNMIcon, wxNullIcon, wxNullIcon);
    return wxStaticCast(pDataset, wxGxObject);
}
#endif

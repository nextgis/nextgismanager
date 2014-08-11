/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSpreadsheetFactoryUI class.
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

#include "wxgis/catalogui/gxssfactoryui.h"
#include "wxgis/catalogui/gxssdatasetui.h"
#include "wxgis/catalogui/gxdatasetui.h"

#include "../../art/localc_16.xpm"
#include "../../art/localc_48.xpm"

IMPLEMENT_DYNAMIC_CLASS(wxGxSpreadsheetFactoryUI, wxGxSpreadsheetFactory)

wxGxSpreadsheetFactoryUI::wxGxSpreadsheetFactoryUI(void) : wxGxSpreadsheetFactory()
{
    m_SmallSSIcon = wxIcon(localc_16_xpm);
    m_LargeSSIcon = wxIcon(localc_48_xpm);
}

wxGxSpreadsheetFactoryUI::~wxGxSpreadsheetFactoryUI(void)
{
}

wxGxObject* wxGxSpreadsheetFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumTableDatasetType type, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    switch(type)
    {
    case enumTableODS:
    case enumTableXLS:
    case enumTableXLSX:
        {
        wxGxSpreadsheetDatasetUI* pDataset = new wxGxSpreadsheetDatasetUI(type, pParent, soName, szPath, m_LargeSSIcon, m_SmallSSIcon, m_LargeSSIcon, m_SmallSSIcon);
        return wxStaticCast(pDataset, wxGxObject);
        }
    default:
        break;
    }
    return NULL;
}

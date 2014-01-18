/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxCSVFileFactoryUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013,2014 Bishop
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
#include "wxgis/catalogui/gxcsvfactoryui.h"
#include "wxgis/catalogui/gxdatasetui.h"

#include "../../art/csv_48.xpm"
#include "../../art/csv_16.xpm"
//#include "../../art/md_dset_48.xpm"
//#include "../../art/md_dset_16.xpm"

IMPLEMENT_DYNAMIC_CLASS(wxGxCSVFileFactoryUI, wxGxCSVFileFactory)

wxGxCSVFileFactoryUI::wxGxCSVFileFactoryUI(void) : wxGxCSVFileFactory()
{
    m_LargeCSVIcon = wxIcon(csv_48_xpm);
    m_SmallCSVIcon = wxIcon(csv_16_xpm);
    //m_LargeMifIcon = wxIcon(md_dset_48_xpm);
    //m_SmallMifIcon = wxIcon(md_dset_16_xpm);
}

wxGxCSVFileFactoryUI::~wxGxCSVFileFactoryUI(void)
{
}

wxGxObject* wxGxCSVFileFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    wxGxTableDatasetUI* pDataset = new wxGxTableDatasetUI(enumTableCSV, pParent, soName, szPath, m_LargeCSVIcon, m_SmallCSVIcon);
    return wxStaticCast(pDataset, wxGxObject);
}


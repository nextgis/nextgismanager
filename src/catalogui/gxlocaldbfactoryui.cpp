/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  wxGxLocalDBFactoryUI class. Create new local databases GxObjectsUI
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
#include "wxgis/catalogui/gxlocaldbfactoryui.h"

#include "wxgis/catalogui/gxlocaldbui.h"

#include "../../art/filegdb_16.xpm"
#include "../../art/filegdb_48.xpm"

//---------------------------------------------------------------------------
// wxGxLocalDBFactoryUI
//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxLocalDBFactoryUI, wxGxLocalDBFactory)

wxGxLocalDBFactoryUI::wxGxLocalDBFactoryUI(void)
{
    m_oLargeFolderIcon = wxIcon(filegdb_48_xpm);
    m_oSmallFolderIcon = wxIcon(filegdb_16_xpm);
}

wxGxLocalDBFactoryUI::~wxGxLocalDBFactoryUI(void)
{
}

wxGxObject* wxGxLocalDBFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumContainerType eType, bool bCheckNames)
{
    if (eType == enumContGDBFolder && !m_bHasOFGDBDriver)
        return NULL;

    if (bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    if (eType == enumContGDBFolder)
    {
        wxGxOpenFileGDBUI* pDB = new wxGxOpenFileGDBUI(pParent, soName, szPath, m_oLargeFolderIcon, m_oSmallFolderIcon);
        return static_cast<wxGxObject*>(pDB);
    }

    return NULL;
}

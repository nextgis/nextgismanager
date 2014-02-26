/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  GIS Prolect UI classes factory
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2014 Bishop
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
#include "wxgis/catalogui/gxgisprojfactoryui.h"

#include "wxgis/catalogui/gxgisprojui.h"

#include "../../art/qgis_16.xpm"
#include "../../art/qgis_48.xpm"
#include "../../art/mi_wor_16.xpm"
#include "../../art/mi_wor_48.xpm"

//---------------------------------------------------------------------------
// wxGxGisProjectFactoryUI
//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxGisProjectFactoryUI, wxGxGisProjectFactory)

wxGxGisProjectFactoryUI::wxGxGisProjectFactoryUI(void)
{
    m_oLargeQGISIcon = wxIcon(qgis_48_xpm);
    m_oSmallQGISIcon = wxIcon(qgis_16_xpm);
    m_oLargeMIWORIcon = wxIcon(mi_wor_48_xpm);
    m_oSmallMIWORIcon = wxIcon(mi_wor_16_xpm);
}

wxGxGisProjectFactoryUI::~wxGxGisProjectFactoryUI(void)
{
}

wxGxObject* wxGxGisProjectFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumGisProjectType eType, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    switch (eType)
    {
    case enumGisProjQGIS:
    {
        wxGxQGISProjFileUI* pProjFile = new wxGxQGISProjFileUI(pParent, soName, szPath, m_oLargeQGISIcon, m_oSmallQGISIcon);
        return wxStaticCast(pProjFile, wxGxObject);// static_cast<wxGxObject*>(pProjFile);
    }
    case enumGisProjWor:
        //TODO: add support to Mapinfo WOR file
        //wxGxWORProjFile
        return NULL;
    default:
        return NULL;
    };
}


/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxPrjFactoryUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013,2014 Bishop
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

#include "wxgis/catalogui/gxprjfactoryui.h"
#include "wxgis/catalogui/gxfileui.h"

#include "../../art/sr_16.xpm"
#include "../../art/sr_48.xpm"

IMPLEMENT_DYNAMIC_CLASS(wxGxPrjFactoryUI, wxGxPrjFactory)

wxGxPrjFactoryUI::wxGxPrjFactoryUI(void)
{
    m_LargePRJIcon = wxIcon(sr_48_xpm);
    m_SmallPRJIcon = wxIcon(sr_16_xpm);
}

wxGxPrjFactoryUI::~wxGxPrjFactoryUI(void)
{
}

wxGxObject* wxGxPrjFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, wxGISEnumPrjFileType nType, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    wxGxPrjFileUI* pFile = new wxGxPrjFileUI(nType, pParent, soName, szPath, m_LargePRJIcon, m_SmallPRJIcon);
	return wxStaticCast(pFile, wxGxObject);
}

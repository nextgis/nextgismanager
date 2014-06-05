/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxArchiveFactoryUI class. Create new GxFolderUI objects
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013,2014 Dmitry Barishnikov
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
#include "wxgis/catalogui/gxarchivefactoryui.h"
#include "wxgis/catalogui/gxarchfolderui.h"

#include "wx/filename.h"
#include "wx/dir.h"

#include "../../art/folder_arch_16.xpm"
#include "../../art/folder_arch_48.xpm"


IMPLEMENT_DYNAMIC_CLASS(wxGxArchiveFactoryUI, wxGxArchiveFactory)

wxGxArchiveFactoryUI::wxGxArchiveFactoryUI(void)
{
    m_oLargeAFolderIcon = wxIcon(folder_arch_48_xpm);
    m_oSmallAFolderIcon = wxIcon(folder_arch_16_xpm);
}

wxGxArchiveFactoryUI::~wxGxArchiveFactoryUI(void)
{
}

wxGxObject* wxGxArchiveFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }
    
    wxGxArchiveUI* pFolder = new wxGxArchiveUI(pParent, soName, szPath, m_oLargeAFolderIcon, m_oSmallAFolderIcon);
	return wxStaticCast(pFolder, wxGxObject);
}

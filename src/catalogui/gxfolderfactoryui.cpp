/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolderFactoryUI class. Create new GxFolderUI objects
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011,2014 Bishop
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
#include "wxgis/catalogui/gxfolderfactoryui.h"

#include "wxgis/catalogui/gxfolderui.h"

#include "wx/filename.h"
#include "wx/dir.h"

#include "../../art/folder_16.xpm"
#include "../../art/folder_48.xpm"

//---------------------------------------------------------------------------
// wxGxFolderFactoryUI
//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxFolderFactoryUI, wxGxFolderFactory)

wxGxFolderFactoryUI::wxGxFolderFactoryUI(void)
{
    m_oLargeFolderIcon = wxIcon(folder_48_xpm);
    m_oSmallFolderIcon = wxIcon(folder_16_xpm);
}

wxGxFolderFactoryUI::~wxGxFolderFactoryUI(void)
{
}

wxGxObject* wxGxFolderFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath, bool bCheckNames)
{
    if(bCheckNames && IsNameExist(pParent, soName))
    {
        return NULL;
    }

    wxGxFolder* pFolder = new wxGxFolderUI(pParent, soName, szPath, m_oLargeFolderIcon, m_oSmallFolderIcon);
	return static_cast<wxGxObject*>(pFolder);
}

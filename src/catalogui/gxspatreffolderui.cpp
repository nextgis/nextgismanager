/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSpatialReferencesFolderUI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Dmitry Barishnikov
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

#include "wxgis/catalogui/gxspatreffolderui.h"
#include "wxgis/catalogui/gxcatalogui.h"

#include "../../art/folder_prj_16.xpm"
#include "../../art/folder_prj_48.xpm"

//--------------------------------------------------------------------------
// wxGxSpatialReferencesFolder
//--------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxSpatialReferencesFolderUI, wxGxSpatialReferencesFolder)

wxGxSpatialReferencesFolderUI::wxGxSpatialReferencesFolderUI(void) : wxGxSpatialReferencesFolder()
{
    m_LargeIcon = wxIcon(folder_prj_48_xpm);
    m_SmallIcon = wxIcon(folder_prj_16_xpm);
}

wxGxSpatialReferencesFolderUI::~wxGxSpatialReferencesFolderUI(void)
{
}

wxIcon wxGxSpatialReferencesFolderUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxSpatialReferencesFolderUI::GetSmallImage(void)
{
	return m_SmallIcon;
}

wxGxObject* wxGxSpatialReferencesFolderUI::GetArchiveFolder(wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
    wxGxPrjFolderUI* pFolder = new wxGxPrjFolderUI(oParent, soName, soPath, m_LargeIcon, m_SmallIcon);
    return wxStaticCast(pFolder, wxGxObject);
}


//--------------------------------------------------------------------------
// wxGxPrjFolderUI
//--------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxPrjFolderUI, wxGxArchiveFolderUI)

wxGxPrjFolderUI::wxGxPrjFolderUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, wxIcon LargeIcon, wxIcon SmallIcon) : wxGxArchiveFolderUI(oParent, soName, soPath, LargeIcon, SmallIcon)
{
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
}

wxGxPrjFolderUI::~wxGxPrjFolderUI(void)
{
}

wxIcon wxGxPrjFolderUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxPrjFolderUI::GetSmallImage(void)
{
	return m_SmallIcon;
}

wxGxObject* wxGxPrjFolderUI::GetArchiveFolder(wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
    wxGxPrjFolderUI* pFolder = new wxGxPrjFolderUI(oParent, soName, soPath, m_LargeIcon, m_SmallIcon);
    return wxStaticCast(pFolder, wxGxObject);
}

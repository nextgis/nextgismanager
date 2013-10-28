/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSpatialReferencesFolderUI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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

#include "wxgis/catalogui/gxspatreffolderui.h"
/*
#include "wxgis/catalogui/gxcatalogui.h"

#include "../../art/folder_prj_16.xpm"
#include "../../art/folder_prj_48.xpm"

/////////////////////////////////////////////////////////////////////////
// wxGxSpatialReferencesFolder
/////////////////////////////////////////////////////////////////////////
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

void wxGxSpatialReferencesFolderUI::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->Detach();
		wxDELETE(m_Children[i]);
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

IGxObject* wxGxSpatialReferencesFolderUI::GetArchiveFolder(CPLString szPath, wxString soName)
{
	wxGxPrjFolderUI* pFolder = new wxGxPrjFolderUI(szPath, soName, m_LargeIcon, m_SmallIcon);
	return static_cast<IGxObject*>(pFolder);
}

/////////////////////////////////////////////////////////////////////////
// wxGxPrjFolderUI
/////////////////////////////////////////////////////////////////////////

wxGxPrjFolderUI::wxGxPrjFolderUI(CPLString Path, wxString Name, wxIcon LargeIcon, wxIcon SmallIcon) : wxGxArchiveFolderUI(Path, Name)
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

IGxObject* wxGxPrjFolderUI::GetArchiveFolder(CPLString szPath, wxString soName)
{
	wxGxPrjFolderUI* pFolder = new wxGxPrjFolderUI(szPath, soName, m_LargeIcon, m_SmallIcon);
	return static_cast<IGxObject*>(pFolder);
}
*/
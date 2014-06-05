/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFileFactoryUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Dmitry Barishnikov
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

#include "wxgis/catalogui/gxfilefactoryui.h"
#include "wxgis/catalogui/gxfileui.h"

//#include "../../art/sr_16.xpm"
//#include "../../art/sr_48.xpm"

//------------------------------------------------------------------------------
// wxGxFileFactoryUI
//------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxFileFactoryUI, wxGxFileFactory)

wxGxFileFactoryUI::wxGxFileFactoryUI(void)
{
    //m_LargePRJIcon = wxIcon(sr_48_xpm);
    //m_SmallPRJIcon = wxIcon(sr_16_xpm);
}

wxGxFileFactoryUI::~wxGxFileFactoryUI(void)
{
}

wxGxObject* wxGxFileFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath)
{
    wxGxTextFileUI* pFile = new wxGxTextFileUI(pParent, soName, szPath);
    return wxStaticCast(pFile, wxGxObject);
}


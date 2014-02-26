/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  GIS Prolect UI classes
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
#include "wxgis/catalogui/gxgisprojui.h"
#include "wxgis/catalogui/gxcatalogui.h"

//---------------------------------------------------------------------------
// wxGxFolderUI
//---------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxQGISProjFileUI, wxGxQGISProjFile)

wxGxQGISProjFileUI::wxGxQGISProjFileUI(wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon & LargeIcon, const wxIcon & SmallIcon) : wxGxQGISProjFile(oParent, soName, soPath)
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;

}

wxGxQGISProjFileUI::~wxGxQGISProjFileUI(void)
{
}

wxIcon wxGxQGISProjFileUI::GetLargeImage(void)
{
    return m_oLargeIcon;
}

wxIcon wxGxQGISProjFileUI::GetSmallImage(void)
{
    return m_oSmallIcon;
}

void wxGxQGISProjFileUI::EditProperties(wxWindow *parent)
{
    //TODO:
}


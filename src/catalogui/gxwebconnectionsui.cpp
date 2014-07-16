/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxWebConnectionsUI class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013 Dmitry Baryshnikov
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

#include "wxgis/catalogui/gxwebconnectionsui.h"

#include "../../art/web_connections_16.xpm"
#include "../../art/web_connections_48.xpm"

//---------------------------------------------------------------------------
// wxGxWebConnectionsUI
//---------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxWebConnectionsUI, wxGxWebConnections)

wxGxWebConnectionsUI::wxGxWebConnectionsUI(void) : wxGxWebConnections(), wxGxAutoRenamer()
{
}

wxGxWebConnectionsUI::~wxGxWebConnectionsUI(void)
{
}

wxIcon wxGxWebConnectionsUI::GetLargeImage(void)
{
    if(!m_LargeIcon.IsOk())
        m_LargeIcon = wxIcon(web_connections_48_xpm);
    return m_LargeIcon;
}

wxIcon wxGxWebConnectionsUI::GetSmallImage(void)
{
    if(!m_SmallIcon.IsOk())
        m_SmallIcon = wxIcon(web_connections_16_xpm);
	return m_SmallIcon;
}


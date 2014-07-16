/******************************************************************************
 * Project:  wxGIS (GIS Map)
 * Purpose:  Main frame class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/map_app/mapframe.h"
#include "wxgis/app/gisaboutdlg.h"

#include "../../art/mainframemap.xpm"

//-----------------------------------------------------------
// wxGISMapFrame
//-----------------------------------------------------------


BEGIN_EVENT_TABLE(wxGISMapFrame, wxMxApplication)
END_EVENT_TABLE()


wxGISMapFrame::wxGISMapFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxMxApplication(parent, id, title, pos, size, style)
{
	m_pAppIcon = wxIcon(mainframemap_xpm);
	//set app main icon
    SetIcon(wxICON(mainframemap));
}

wxGISMapFrame::~wxGISMapFrame(void)
{
}

void wxGISMapFrame::OnAppAbout(void)
{
    wxGISAboutDialog oDialog(this);
    oDialog.ShowModal();
}

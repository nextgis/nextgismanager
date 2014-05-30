/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISMenuBar class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2012,2014 Dmitry Baryshnikov
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
#pragma once

#include "wxgis/framework/commandbar.h"

#include <wx/menu.h>

#define FILESTR _("&File")
#define EDITSTR _("&Edit")
#define GOSTR _("&Go")
#define VIEWSTR _("&View")
#define HELPSTR _("&Help")
#define WINDOWSSTR _("&Windows")
#define TOOLSSTR _("&Tools")

/** @class wxGISMenuBar
    
    The main application menu.
    
    @library{framework}
 */

class WXDLLIMPEXP_GIS_FRW wxGISMenuBar : public wxMenuBar
{
    //DECLARE_DYNAMIC_CLASS_NO_COPY(wxGISMenuBar)
public:
	wxGISMenuBar(long style = 0, wxGISApplicationBase* pApp = NULL, wxXmlNode* pConf = NULL);
	virtual ~wxGISMenuBar(void);
	virtual bool IsMenuBarMenu(wxString sMenuName);
	virtual wxGISCommandBarPtrArray GetMenuBarArray(void) const;
	virtual void MoveLeft(int pos);
	virtual void MoveRight(int pos);
	virtual void MoveLeft(wxGISCommandBar* pBar);
	virtual void MoveRight(wxGISCommandBar* pBar);
	virtual void RemoveMenu(wxGISCommandBar* pBar);
	virtual bool AddMenu(wxGISCommandBar* pBar);
	virtual void Serialize(wxXmlNode* pConf);
	virtual int GetMenuPos(wxGISCommandBar* pBar);
protected:
	wxGISCommandBarPtrArray m_MenubarArray;
};

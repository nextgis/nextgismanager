/******************************************************************************
 * Project:  wxGIS
 * Purpose:  menu to show format clases (for format coordinates etc.).
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#pragma once

#include "wxgis/cartoui/cartoui.h"
#include "wxgis/core/format.h"
#include "wxgis/framework/framework.h"

/** \class wxGISCoordinatesFormatMenu formatmenu.h
    \brief The class to format coordinates according different masks
*/
class WXDLLIMPEXP_GIS_CTU wxGISCoordinatesFormatMenu :
	public wxMenu,
	public wxGISCoordinatesFormat
{
	enum
	{
        ID_SWAPCOORS = 1050,
        ID_CHANGEMASKS,
		ID_MASKBEG
	};
public:
	wxGISCoordinatesFormatMenu(const wxString& title = wxEmptyString, long style = 0);
	virtual ~wxGISCoordinatesFormatMenu(void);
	virtual void PrepareMenu(void);
	//events
	virtual void OnSwapCoords(wxCommandEvent& event);
	virtual void OnChangeMasks(wxCommandEvent& event);
	virtual void OnSetMask(wxCommandEvent& event);
	virtual void OnSwapCoordsUI(wxUpdateUIEvent& event);
protected:
	virtual void Serialize(bool bStore);
protected:
	wxArrayString m_asCoordsMask;
private:
    DECLARE_EVENT_TABLE()
};

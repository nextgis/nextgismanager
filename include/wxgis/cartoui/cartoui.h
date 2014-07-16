/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISCartoUI main header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2014 Dmitry Baryshnikov
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

#include "wxgis/carto/carto.h"

#include "wxgisdefs.h"
#ifndef wxGIS_USE_CAIRO
#error Cairo required
#endif

/** @enum wxGISEnumMapToolState

    The map tool state enumerator.
*/
enum wxGISEnumMapToolState
{
	enumGISMapNone		= 0,
	enumGISMapPanning,
	enumGISMapZooming,
	enumGISMapRotating,
	enumGISMapDrawing,
	enumGISMapWheeling,
	enumGISMapWheelingStop,
    enumGISMapFlashing,
    enumGISMapRedraw
};

/** @enum wxGISEnumFlashStyle

    The map geometry flash styles. While identify or any other operations map can be
*/
enum wxGISEnumFlashStyle
{
	enumGISMapFlashNone		= 0,
	enumGISMapFlashNewColor,
	enumGISMapFlashWaves
};

inline bool IsURL(const wxString &sText)
{
    return sText.StartsWith(wxT("http:")) || sText.StartsWith(wxT("www.")) || sText.StartsWith(wxT("https:")) || sText.StartsWith(wxT("ftp:")) || sText.StartsWith(wxT("ftp.")) || sText.StartsWith(wxT("www2."));

}

inline bool IsLocalURL(const wxString &sText)
{
    return sText.StartsWith(wxT("file:"));
}

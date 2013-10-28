/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISCarto main header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013 Bishop
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

#include "wxgis/display/gisdisplay.h"
#include "wxgis/datasource/spatialtree.h"
#include "wxgis/datasource/rasterdataset.h"

/** \enum wxGISEnumRendererType rasterrenderer.h
    \brief A renderer color interpretation type
*/
enum wxGISEnumRendererType
{
	enumGISRenderTypeNone = 0,
	enumGISRenderTypeVector,
	enumGISRenderTypeRaster
};

WX_DECLARE_HASH_MAP( int, wxColor, wxIntegerHash, wxIntegerEqual, wxGISColorTable );


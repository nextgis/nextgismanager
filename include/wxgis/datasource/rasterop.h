/******************************************************************************
 * Project:  wxGIS
 * Purpose:  raster operations.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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

#include "wxgis/datasource/datasource.h"

#include "gdal_priv.h"

/** \fn CPLString GetWorldFilePath(const CPLString &soPath);
 *  \brief Get the path for world file if exist
 */	
WXDLLIMPEXP_GIS_DS CPLString GetWorldFilePath(const CPLString &soPath);
/*
#include "wxgis/datasource/rasterdataset.h"

int WXDLLIMPEXP_GIS_DS GetOverviewLevels(wxGISRasterDatasetSPtr pwxGISRasterDataset, int* anOverviewList);
*/


/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  WMS DataSource class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013 Dmitry Barishnikov
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

#include "wxgisdefs.h"

#ifdef wxGIS_USE_CURL

#include "wxgis/datasource/rasterdataset.h"

/** \class wxGISWMSDataSource wmsdataset.h
    \brief The WMS DataSource class.
*/
/*
class WXDLLIMPEXP_GIS_DS wxGISWMSDataSource :
	public wxGISRasterDataset
{
    DECLARE_CLASS(wxGISWMSDataSource)
public:
	wxGISWMSDataSource(const CPLString &sPath = "", wxGISEnumRasterDatasetType nType = enumRasterUnknown);
    //TODO: read from descript file and control it delete/rename/move/etc.
    //wxGISPostgresDataSource(const CPLString &szPath);
	virtual ~wxGISWMSDataSource(void);

	//wxGISRasterDataset

};
*/
#endif //wxGIS_USE_CURL


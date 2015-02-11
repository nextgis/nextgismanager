/******************************************************************************
 * Project:  wxGIS
 * Purpose:  datasource header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012 Dmitry Baryshnikov
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

#include "wxgis/core/core.h"

#define NOTNODATA -9999.0

/** @enum wxGISEnumDatasetType

    The dataset types
*/

enum wxGISEnumDatasetType
{
    enumGISAny = 1,
	enumGISFeatureDataset = 2,
	enumGISTable = 3,
	enumGISRasterDataset = 4,
	enumGISContainer = 5,
    enumGISDrawing = 6
};

/** @enum wxGISEnumVectorDatasetType

    The vector datasource types
*/
enum wxGISEnumVectorDatasetType
{
	enumVecUnknown = 0,		// 0
	enumVecESRIShapefile,	// 1
	enumVecMapinfoTab,		// 2
    enumVecMapinfoMif,		// 3
    enumVecDXF,				// 4
	enumVecPostGIS,			// 5
	enumVecGML,				// 6
    enumVecGeoJSON,			// 7
    enumVecWFS,				// 8
    enumVecMem,				// 9	
    enumVecKML,				// 10
    enumVecKMZ,    			// 11
	enumVecSXF,				// 12
    enumVecS57,				// 13	
    enumVecFileDBLayer,		// 14
    enumVecCSV,				// 15
    enumVecMAX				
};

/** @enum wxGISEnumRasterDatasetType

    The raster datasource types
*/
enum wxGISEnumRasterDatasetType
{
	enumRasterUnknown = 0,  //0
	enumRasterBmp,          //1
	enumRasterTiff,         //2
	enumRasterTil,          //3
	enumRasterImg,          //4
	enumRasterJpeg,         //5
	enumRasterPng,          //6
    enumRasterGif,          //7
    enumRasterSAGA,         //8
    enumRasterVRT,          //9
    enumRasterWMS,          //10
    enumRasterWMSTMS,       //11
    enumRasterPostGIS,      //12
    enumRasterFileDBLayer,  //13
    enumRasterMAX           //14
};

/** @enum wxGISEnumTableType

    The table datasource types
*/
enum wxGISEnumTableType
{
	enumTableUnknown = 0,
    enumTableDBF,
	enumTablePostgres,
    enumTableQueryResult,
    enumTableMapinfoTab,
    enumTableMapinfoMif,
    enumTableCSV,
    enumTableFileDBLayer,
    enumTableODS,
    enumTableXLS,
    enumTableXLSX,
    enumTableMAX
};

/** @enum wxGISEnumPrjFileType

    The projection file types
*/
enum wxGISEnumPrjFileType
{
	enumESRIPrjFile = 1,
	enumSPRfile = 2,
	enumQPJfile = 3
};

/** @enum wxGISEnumContainerType

    The container types
*/
enum wxGISEnumContainerType
{
    enumContUnknown = 0,
	enumContFolder,
	enumContGDBFolder,
    enumContDataset,
	enumContGDB,
	enumContRemoteDBConnection,
    enumContWebServiceConnection,
	enumContNGWResourceGroup,
	enumContKML = enumVecKML,		// 10
	enumContKMZ = enumVecKMZ,		// 11
	enumContSXF = enumVecSXF		// 12
};

/** @enum wxGISEnumWldExtType

    The world file ext type
*/

enum wxGISEnumWldExtType
{
	enumGISWldExt_FirstLastW = 1,
	enumGISWldExt_Wld = 2,
	enumGISWldExt_ExtPlusW = 3,
	enumGISWldExt_ExtPlusWX = 4
};




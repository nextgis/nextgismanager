/******************************************************************************
 * Project:  wxGIS
 * Purpose:  system operations.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013 Dmitry Baryshnikov
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

#include "wxgis/datasource/datasource.h"
#include "wxgis/datasource/gdalinh.h"


WXDLLIMPEXP_GIS_DS inline bool IsFileDataset(wxGISEnumDatasetType eDSType, long SubType)
{
    switch (eDSType)
    {
    case enumGISAny:
        return false;
    case enumGISFeatureDataset:
    {
        wxGISEnumVectorDatasetType eSubType = (wxGISEnumVectorDatasetType)SubType;
        switch (eSubType)
        {
        case enumVecESRIShapefile:
        case enumVecMapinfoTab:
        case enumVecMapinfoMif:
        case enumVecKML:
        case enumVecKMZ:
        case enumVecDXF:
            return true;
        case enumVecPostGIS:
            return false;
        case enumVecGML:
        case enumVecGeoJSON:
        case enumVecS57:
        case enumVecCSV:
            return true;
        case enumVecWFS:
        case enumVecMem:
        case enumVecMAX:
        case enumVecUnknown:
        case enumVecFileDBLayer:
        default:
            return false;
        }
    }
        break;
    case enumGISTableDataset:
    {
        wxGISEnumTableDatasetType eSubType = (wxGISEnumTableDatasetType)SubType;
        switch (eSubType)
        {
        case enumTableUnknown:
            return false;
        case enumTableDBF:
            return true;
        case enumTablePostgres:
            return false;
        case enumTableQueryResult:
            return false;
        case enumTableMapinfoTab:
        case enumTableMapinfoMif:
        case enumTableCSV:
            return true;
        case enumTableFileDBLayer:
        default:
            return false;
        }
    }
        break;
    case enumGISRasterDataset:
    {
        wxGISEnumRasterDatasetType eSubType = (wxGISEnumRasterDatasetType)SubType;
        switch (eSubType)
        {
        case enumRasterUnknown:
            return false;
        case enumRasterBmp:
        case enumRasterTiff:
        case enumRasterTil:
        case enumRasterImg:
        case enumRasterJpeg:
        case enumRasterPng:
        case enumRasterGif:
        case enumRasterSAGA:
        case enumRasterVRT:
            return true;
        case enumRasterFileDBLayer:
        case enumRasterWMS:
        case enumRasterWMSTMS:
            return false;
        default:
            return false;
        }
    }
        break;
    case enumGISContainer:
    {
        wxGISEnumContainerType eSubType = (wxGISEnumContainerType)SubType;
        switch (eSubType)
        {
        case enumContFolder:
        case enumContGDBFolder:
        case enumContDataset:
        case enumContGDB:
            return true;
        case enumContRemoteDBConnection:
        case enumContWebServiceConnection:
        default:
            return false;
        }
    }
        break;
    default:
        return false;
    }
}

WXDLLIMPEXP_GIS_DS inline wxString GetDriverByType(wxGISEnumDatasetType eDSType, long SubType)
{
    switch (eDSType)
    {
    case enumGISAny:
        return wxEmptyString;
    case enumGISFeatureDataset:
    {
        wxGISEnumVectorDatasetType eSubType = (wxGISEnumVectorDatasetType)SubType;
        switch (eSubType)
        {
        case enumVecESRIShapefile:
            return wxString(wxT("ESRI Shapefile"));
        case enumVecMapinfoTab:
        case enumVecMapinfoMif:
            return wxString(wxT("MapInfo File"));
        case enumVecKML:
        case enumVecKMZ:
            return wxString(wxT("LIBKML"));
        case enumVecDXF:
            return wxString(wxT("DXF"));
        case enumVecPostGIS:
            return wxString(wxT("PostgreSQL"));
        case enumVecGML:
            return wxString(wxT("GML"));
        case enumVecWFS:
            return wxString(wxT("WFS"));
        case enumVecMem:
            return wxString(wxT("Memory"));
        case enumVecGeoJSON:
            return wxString(wxT("GeoJSON"));
        case enumVecCSV:
            return wxString(wxT("CSV"));
        }
    }
    break;
    case enumGISTableDataset:
    {
        wxGISEnumTableDatasetType eSubType = (wxGISEnumTableDatasetType)SubType;
        switch (eSubType)
        {
        case enumTableDBF:
            return wxString(wxT("ESRI Shapefile"));
        case enumTableMapinfoTab:
        case enumTableMapinfoMif:
            return wxString(wxT("MapInfo File"));
        case enumTableCSV:
            return wxString(wxT("CSV"));
        case enumTablePostgres:
            return wxString(wxT("PostgreSQL"));
        case enumTableUnknown:
        case enumTableQueryResult:
            return wxEmptyString;
        }
    }
    break;
    case enumGISRasterDataset:
    {
        wxGISEnumRasterDatasetType eSubType = (wxGISEnumRasterDatasetType)SubType;
        switch (eSubType)
        {
        case enumRasterBmp:
            return wxString(wxT("BMP"));
        case enumRasterTiff:
            return wxString(wxT("GTiff"));
        case enumRasterJpeg:
            return wxString(wxT("JPEG"));
        case enumRasterImg:
            return wxString(wxT("HFA"));
        case enumRasterPng:
            return wxString(wxT("PNG"));
        case enumRasterGif:
            return wxString(wxT("GIF"));
        case enumRasterSAGA:
            return wxString(wxT("SAGA"));
        case enumRasterTil:
            return wxString(wxT("TIL"));
        case enumRasterVRT:
            return wxString(wxT("VRT"));
        case enumRasterWMS:
        case enumRasterWMSTMS:
            return wxString(wxT("WMS"));
        case enumRasterPostGIS:
            return wxString(wxT("PostGISRaster"));
        }
    }
    break;
    case enumGISContainer:
    default:
        return wxEmptyString;
    }
    return wxEmptyString;
}


WXDLLIMPEXP_GIS_DS bool DeleteDir(const CPLString &sPath, ITrackCancel* const pTrackCancel = NULL);
WXDLLIMPEXP_GIS_DS bool MoveDir(const CPLString &sPathFrom, const CPLString &sPathTo, long mode, ITrackCancel* const pTrackCancel = NULL);
WXDLLIMPEXP_GIS_DS bool CopyDir(const CPLString &sPathFrom, const CPLString &sPathTo, long mode, ITrackCancel* const pTrackCancel = NULL);
WXDLLIMPEXP_GIS_DS bool CreateDir(const CPLString &sPath, long mode = 0755, ITrackCancel* const pTrackCancel = NULL);
WXDLLIMPEXP_GIS_DS bool DeleteFile(const CPLString &sPath, ITrackCancel* const pTrackCancel = NULL);
WXDLLIMPEXP_GIS_DS bool RenameFile(const CPLString &sOldPath, const CPLString &sNewPath, ITrackCancel* const pTrackCancel = NULL);
WXDLLIMPEXP_GIS_DS bool CopyFile(const CPLString &sSrcPath, const CPLString &sDestPath, ITrackCancel* const pTrackCance = NULL);
WXDLLIMPEXP_GIS_DS bool MoveFile(const CPLString &sSrcPath, const CPLString &sDestPath, ITrackCancel* const pTrackCancel = NULL);
WXDLLIMPEXP_GIS_DS wxFontEncoding GetEncodingFromCpg(const CPLString &sPath);
WXDLLIMPEXP_GIS_DS wxFontEncoding ToFontEncoding(const CPLString &soCodePage);
//WXDLLIMPEXP_GIS_DS CPLString GetEncodingName(wxFontEncoding eEncoding);
WXDLLIMPEXP_GIS_DS wxString ClearExt(const wxString &sPath);
WXDLLIMPEXP_GIS_DS bool IsFileHidden(const CPLString &sPath);
WXDLLIMPEXP_GIS_DS wxString CheckUniqName(const CPLString &sPath, const wxString &sName, const wxString &sExt, int nCounter = 0);
WXDLLIMPEXP_GIS_DS CPLString CheckUniqPath(const CPLString &sPath, const CPLString &sName, bool bIsFolder = false, const CPLString &sAdd = "_copy", int nCounter = 0);
WXDLLIMPEXP_GIS_DS CPLString GetUniqPath(const CPLString &szOriginalFullPath, const CPLString &szNewPath, const CPLString &szNewName);
WXDLLIMPEXP_GIS_DS CPLString Transliterate(const char* str);
WXDLLIMPEXP_GIS_DS CPLString GetExtension(const CPLString &sPath, const CPLString &sName = "");
WXDLLIMPEXP_GIS_DS wxString GetConvName(const CPLString &szPath, bool bIsPath = true);

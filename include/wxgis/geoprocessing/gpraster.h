/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  raster dataset functions.
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

#pragma once

#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/rasterdataset.h"
#include "wxgis/catalog/gxfilters.h"

/** \fn bool SubrasterByVector(wxGISFeatureDatasetSPtr pSrcFeatureDataSet, wxGISRasterDatasetSPtr pSrcRasterDataSet, CPLString &szDstFolderPath)
 *  \brief Get subruster clipped by vector geometry.
 *  \param pSrcFeatureDataSet The clip geometry source dataset
 *  \param pSrcRasterDataSet The clipped raster
 *  \param szDstFolderPath The subraster store path
 *  \return False if any error or true
 */
bool WXDLLIMPEXP_GIS_DS SubrasterByVector( wxGISFeatureDatasetSPtr pSrcFeatureDataSet, wxGISRasterDatasetSPtr pSrcRasterDataSet, CPLString &szDstFolderPath, wxGxRasterFilter* pFilter = NULL, GDALDataType eOutputType = GDT_Unknown, int nBandCount = 0, int *panBandList = NULL, bool bUseCounter = true, int nCounterBegin = -1, int nFieldNo = -1, double dfOutResX = -1, double dfOutResY = -1, bool bCopyNodata = false, bool bSkipSourceMetadata = false, char** papszOptions = NULL, ITrackCancel* pTrackCancel = NULL );

bool CreateSubRaster( wxGISRasterDatasetSPtr pSrcRasterDataSet, OGREnvelope &Env, const OGRGeometry *pGeom, GDALDriver* pDriver, CPLString &szDstPath, GDALDataType eOutputType = GDT_Unknown, int nBandCount = 0, int *panBandList = NULL, double dfOutResX = -1, double dfOutResY = -1, bool bCopyNodata = false, bool bSkipSourceMetadata = false, char** papszOptions = NULL, ITrackCancel* pTrackCancel = NULL );
void CopyBandInfo( GDALRasterBand * poSrcBand, GDALRasterBand * poDstBand, int bCopyNoData );

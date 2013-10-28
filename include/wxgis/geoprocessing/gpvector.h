/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  vector dataset functions.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/core/core.h"
#include "wxgis/datasource/dataset.h"
#include "wxgis/core/format.h"
#include "wxgis/catalog/gxfilters.h"

/** \fn bool CopyRows(wxGISFeatureDatasetSPtr pSrcDataSet, wxGISFeatureDatasetSPtr pDstDataSet, wxGISQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL); 
  *  \brief Copy rows from one format (file) to another.
  *  \return True if success, false otherwise
*/
/*bool WXDLLIMPEXP_GIS_GP CopyRows(wxGISFeatureDatasetSPtr pSrcDataSet, wxGISFeatureDatasetSPtr pDstDataSet, wxGISQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL); 
bool WXDLLIMPEXP_GIS_GP ExportFormat(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, OGRFeatureDefn *pDef, OGRSpatialReference* pNewSpaRef, wxGISQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL);
bool WXDLLIMPEXP_GIS_GP ExportFormat(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, wxGISQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL);
bool WXDLLIMPEXP_GIS_GP Project(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, OGRSpatialReference* pNewSpaRef, ITrackCancel* pTrackCancel);
OGRGeometry WXDLLIMPEXP_GIS_GP *Intersection(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn, OGREnvelope* pRgnEnv);
/** \fn OGRGeometry *CheckRgnAndTransform(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn1, OGRPolygon* pRgn2, OGREnvelope* pRgnEnv1, OGREnvelope* pRgnEnv2, OGRCoordinateTransformation *poCT);
  *  \brief Check if geometry intersects the Spatial Referense limits, cut it by this limits and reproject or return NULL geometry.
  *  \return Projected geometry or NULL.
*/
/*OGRGeometry WXDLLIMPEXP_GIS_GP *CheckRgnAndTransform(OGRGeometry* pFeatureGeom, OGRPolygon* pRgn1, OGRPolygon* pRgn2, OGREnvelope* pRgnEnv1, OGREnvelope* pRgnEnv2, OGRCoordinateTransformation *poCT);
bool WXDLLIMPEXP_GIS_GP GeometryVerticesToPoints(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxString sName, IGxObjectFilter* pFilter, wxGISQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL);
bool GeometryVerticesToPointsDataset(long nGeomFID, OGRGeometry* pGeom, wxGISFeatureDatasetSPtr pDSet, OGRCoordinateTransformation *poCT, long &nFidCounter, ITrackCancel* pTrackCancel);
*/
/** \fn wxGISDataset *CreateDataset(const CPLString &sPath, wxString &sName, wxGxObjectFilter* const pFilter, OGRFeatureDefn* const poFields, wxGISSpatialReference oSpatialRef = wxNullSpatialReference, OGRwkbGeometryType eGType = wkbUnknown, char ** papszDataSourceOptions = NULL, char ** papszLayerOptions = NULL, ITrackCancel* const pTrackCancel = NULL)
  *  \brief Create new Vector table or feature class.
  *  \return The pointer on created dataset
  */	
wxGISDataset WXDLLIMPEXP_GIS_GP *CreateDataset(const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, OGRFeatureDefn* const poFields, wxGISSpatialReference oSpatialRef = wxNullSpatialReference, OGRwkbGeometryType eGType = wkbUnknown, char ** papszDataSourceOptions = NULL, char ** papszLayerOptions = NULL, ITrackCancel* const pTrackCancel = NULL); 

/** \fn bool GeometryVerticesToTextFile(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, const CPLString &osFrmt, bool bSwapXY = false, wxGISQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL)
  *  \brief Write shape coordinates to text file.
  *  \return true if write is succeeded, false overwise
  */
/*
bool WXDLLIMPEXP_GIS_GP GeometryVerticesToTextFile(wxGISFeatureDatasetSPtr pDSet, CPLString sPath, wxGISCoordinatesFormat &oFrmt, wxGISQueryFilter* pQFilter = NULL, ITrackCancel* pTrackCancel = NULL);
CPLString WXDLLIMPEXP_GIS_GP GeometryToText(long nGeomFID, OGRGeometry* pGeom, wxGISCoordinatesFormat &oFrmt, ITrackCancel* pTrackCancel = NULL);
void WXDLLIMPEXP_GIS_GP PointToText(CPLString &osData, OGRPoint* pPoint, wxGISCoordinatesFormat &oFrmt);
void WXDLLIMPEXP_GIS_GP LineToText(CPLString &osData, OGRLineString* pLine, wxGISCoordinatesFormat &oFrmt);
*/


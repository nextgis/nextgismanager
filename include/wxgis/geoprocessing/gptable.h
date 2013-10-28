/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  main table analysis functions.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
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

#include "wxgis/geoprocessing/geoprocessing.h"
#include "wxgis/datasource/table.h"

enum wxGISFieldMergeOperator
{
	enumGISFMONone,
	enumGISFMOMergeBase,
	enumGISFMOMean,
	enumGISFMOMin,
	enumGISFMOMax,
	enumGISFMOSum
};

typedef struct _FieldMergeData
{
	//CPLString sFieldName;
	int nFieldPos;
	wxGISFieldMergeOperator nOp;
} FIELDMERGEDATA;

typedef struct Val
{
	wxVariant sum;
	int count;
	OGRFieldType nType;
} VAL;

typedef std::vector<VAL> VALARRAY, *LPVALARRAY;

wxGISTableSPtr WXDLLIMPEXP_GIS_GP CreateTable(CPLString sPath, wxString sName, wxString sExt, wxString sDriver, OGRFeatureDefn *poFields, wxGISEnumTableDatasetType nType = enumTableUnknown, char ** papszDataSourceOptions = NULL, char ** papszLayerOptions = NULL);
bool WXDLLIMPEXP_GIS_GP MeanValByColumn(wxGISTableSPtr pDSet, CPLString sPath, wxString sName, std::vector<FIELDMERGEDATA> &FieldMergeData, IGxObjectFilter* pFilter, wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel);

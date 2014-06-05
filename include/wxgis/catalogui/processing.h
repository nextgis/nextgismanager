/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  geoproessing vector datasets.
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2011,2013 Dmitry Barishnikov
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
 
#include "wxgis/catalog/catalog.h"
#include "wxgisdefs.h"

#ifdef wxGIS_HAVE_GEOPROCESSING

#include "wxgis/geoprocessing/gpvector.h"

typedef struct _exported_dataset
{
    wxString sName;
    IGxDataset* pDSet;
} EXPORTED_DATASET;

WXDLLIMPEXP_GIS_CLU void ExportSingleDatasetSelect(wxWindow* pWnd, IGxDataset* const pGxDataset);
WXDLLIMPEXP_GIS_CLU void ExportSingleDatasetSelectWithParams(wxWindow* pWnd, IGxDataset* const pGxDataset);
WXDLLIMPEXP_GIS_CLU void ExportMultipleDatasetsSelect(wxWindow* pWnd, wxVector<IGxDataset*> &paDatasets);
WXDLLIMPEXP_GIS_CLU void ExportMultipleDatasetsSelectWithParams(wxWindow* pWnd, wxVector<IGxDataset*> &paDatasets);

WXDLLIMPEXP_GIS_CLU void ExportSingleVectorDataset(wxWindow* pWnd, const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, IGxDataset* const pGxDataset);

WXDLLIMPEXP_GIS_CLU void ExportSingleRasterDataset(wxWindow* pWnd, const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, IGxDataset* const pGxDataset);

WXDLLIMPEXP_GIS_CLU void ExportSingleTableDataset(wxWindow* pWnd, const CPLString &sPath, const wxString &sName, wxGxObjectFilter* const pFilter, IGxDataset* const pGxDataset);

WXDLLIMPEXP_GIS_CLU void ExportMultipleVectorDatasets(wxWindow* pWnd, const CPLString &sPath, wxGxObjectFilter* const pFilter, wxVector<EXPORTED_DATASET> &paDatasets);
WXDLLIMPEXP_GIS_CLU void ExportMultipleRasterDatasets(wxWindow* pWnd, const CPLString &sPath, wxGxObjectFilter* const pFilter, wxVector<EXPORTED_DATASET> &paDatasets);
WXDLLIMPEXP_GIS_CLU void ExportMultipleTableDatasets(wxWindow* pWnd, const CPLString &sPath, wxGxObjectFilter* const pFilter, wxVector<EXPORTED_DATASET> &paDatasets);

void ShowMessageDialog(wxWindow* pWnd, const wxVector<MESSAGE>& msgs);
#endif // wxGIS_HAVE_GEOPROCESSING

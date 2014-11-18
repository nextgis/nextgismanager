/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  RasterDataset class.
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

#include "wxgis/datasource/dataset.h"

/** @class wxGISRasterDataset
 * 
 * The GIS RasterDataset class. This class stores raster geographic data (imagery & etc.).
 * 
 * @library{datasource}
*/

class WXDLLIMPEXP_GIS_DS wxGISRasterDataset :
	public wxGISDataset
{
    DECLARE_CLASS(wxGISRasterDataset)
public:
	wxGISRasterDataset(const CPLString &sPath = "", wxGISEnumRasterDatasetType nType = enumRasterUnknown);
	virtual ~wxGISRasterDataset(void);
    // wxGISDataset
	virtual const wxGISSpatialReference GetSpatialReference(void);
    virtual wxString GetName(void);
	virtual void Close(void);
	virtual void Cache(ITrackCancel* const pTrackCancel = NULL){};
	virtual bool Rename(const wxString &sNewName, ITrackCancel* const pTrackCancel = NULL);
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel = NULL);
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel = NULL);
    virtual char **GetFileList();
	static char **GetTiles(char **papszStrList, const CPLString &szPath);
    // wxGISRasterDataset
	virtual bool Open(bool bUpdate = false, bool bShared = true);
	virtual OGREnvelope GetEnvelope(void);
	virtual GDALDataType GetDataType(void);
	virtual GDALDataset* GetRaster(void){return m_poDataset;};
	virtual GDALDataset* GetMainRaster(void){return m_poMainDataset;};
	virtual bool HasOverviews(void){return m_bHasOverviews;};
	virtual void SetHasOverviews(bool bVal){m_bHasOverviews = bVal;};
	virtual bool HasStatistics(void){return m_bHasStats;};
	virtual void SetHasStatistics(bool bVal){m_bHasStats = bVal;};
    virtual int GetWidth(void){return m_nXSize;};
    virtual int GetHeight(void){return m_nYSize;};
    virtual int GetBandCount(void){return m_nBandCount;};
	virtual bool GetPixelData(void *data, int nXOff, int nYOff, int nXSize, int nYSize, int nBufXSize, int nBufYSize, GDALDataType eDT, int nBandCount, int *panBandList);
	virtual bool HasNoData(int nBand){return !IsDoubleEquil(m_paNoData[nBand - 1], NOTNODATA);};
	virtual double GetNoData(int nBand){return m_paNoData[nBand - 1];};
	virtual bool WriteWorldFile(wxGISEnumWldExtType eType);
	virtual bool IsWarped() const;
protected:
    bool FixSAGARaster(const CPLString &szDestPath, const CPLString &szDestName);
protected:
	OGREnvelope m_stExtent;
	GDALDataset  *m_poDataset;
	GDALDataset  *m_poMainDataset;
    bool m_bHasOverviews;
    bool m_bHasStats;
	int m_nXSize;
	int m_nYSize;
	int m_nBandCount;
	GDALDataType m_nDataType;
    double *m_paNoData;
	bool m_bWarped;
};


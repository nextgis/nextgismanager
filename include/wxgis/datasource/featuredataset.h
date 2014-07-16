/******************************************************************************
 * Project:  wxGIS
 * Purpose:  FeatureDataset class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2014 Dmitry Baryshnikov
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

#include "wxgis/datasource/table.h"

/** @class wxGISFeatureDataset

    A GIS FeatureDataset class. This class stores vector geographic data.

    @library{datasource}
*/

class WXDLLIMPEXP_GIS_DS wxGISFeatureDataset :
	public wxGISTable
{
    DECLARE_CLASS(wxGISFeatureDataset)
public:
	wxGISFeatureDataset(const CPLString &sPath, int nSubType, OGRLayer* poLayer = NULL, OGRCompatibleDataSource* poDS = NULL);
	virtual ~wxGISFeatureDataset(void);
//wxGISTable
    virtual const wxGISSpatialReference GetSpatialReference(void);
    virtual void Close(void);
	virtual bool Open(int iLayer = 0, int bUpdate = TRUE, bool bCache = true, ITrackCancel* const pTrackCancel = NULL);
    virtual wxGISDataset* GetSubset(size_t nIndex);
    virtual wxGISDataset* GetSubset(const wxString & sSubsetName);
    virtual bool IsCached(void) const;
	virtual bool IsCaching(void) const;
	virtual void Cache(ITrackCancel* const pTrackCancel = NULL);
    virtual void StopCaching(void);
    virtual void SetEncoding(const wxFontEncoding &oEncoding);
    virtual char **GetFileList();
    //editing
	virtual OGRErr DeleteFeature(long nFID);
    virtual OGRErr StoreFeature(wxGISFeature &Feature);
    virtual OGRErr SetFeature(const wxGISFeature &Feature);
//wxGISFeatureDataset
    virtual void SetCached(bool bCached);
	virtual OGREnvelope GetEnvelope(void);
    virtual OGRwkbGeometryType GetGeometryType(void) const;
    virtual OGRErr SetFilter(const wxGISSpatialFilter &SpaFilter = wxGISNullSpatialFilter);
    virtual wxFeatureCursor Search(const wxGISSpatialFilter &SpaFilter, bool bOnlyFirst = false, ITrackCancel* const pTrackCancel = NULL);
 	virtual wxGISSpatialTreeCursor SearchGeometry(const OGREnvelope &Env);
 	virtual bool IsContainer() const;
protected:
    virtual void SetInternalValues(void);
protected:
    OGREnvelope m_stExtent;
    OGRwkbGeometryType m_eGeomType;
	wxGISSpatialTree* m_pSpatialTree;
    bool m_bOLCFastGetExtent;
};

/** @class wxGISFeatureDatasetCached

    A GIS FeatureDataset class. This wxGISFeatureDataset version with caching.

    @library{datasource}
*/

class WXDLLIMPEXP_GIS_DS wxGISFeatureDatasetCached :
	public wxGISFeatureDataset
{
    DECLARE_CLASS(wxGISFeatureDatasetCached)
    friend class wxGISQuadTree;
public:
	wxGISFeatureDatasetCached(const CPLString &sPath, int nSubType, OGRLayer* poLayer = NULL, OGRCompatibleDataSource* poDS = NULL);
	virtual ~wxGISFeatureDatasetCached(void);
//wxGISTable
    virtual void Close(void);
    virtual wxGISDataset* GetSubset(size_t nIndex);
    virtual wxGISDataset* GetSubset(const wxString & sSubsetName);
	virtual void Cache(ITrackCancel* const pTrackCancel = NULL);
    virtual void SetEncoding(const wxFontEncoding &oEncoding);
    //wxGISFeatureDataset
	virtual OGREnvelope GetEnvelope(void);
    /*
    virtual OGRErr SetFilter(wxGISQueryFilter* pQFilter);
    */
    //wxGISTable
    virtual void Reset(void);
    virtual wxGISFeature Next(void);
    virtual wxGISFeature GetFeatureByID(long nFID);
    virtual wxGISFeature GetFeature(long nIndex);
	virtual size_t GetFeatureCount(bool bForce = false, ITrackCancel* const pTrackCancel = NULL);
	virtual OGRErr DeleteFeature(long nFID);
    virtual OGRErr StoreFeature(wxGISFeature &Feature);
    virtual OGRErr SetFeature(const wxGISFeature &Feature);
protected:
    std::map<long, wxGISFeature> m_omFeatures;
};

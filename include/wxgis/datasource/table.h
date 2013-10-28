/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Table class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2013 Bishop
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

#include "wxgis/datasource/dataset.h"
#include "wxgis/datasource/cursor.h"
#include "wxgis/datasource/filter.h"
#include "wxgis/datasource/spatialtree.h"

#include <ogrsf_frmts.h>

/** \class wxGISTable table.h
    \brief A GIS Table class.

    This class support basic operations on datasource. No spatial data avaliable, but the OGRFeature is main part of this class, so it's posible to get some spatial information (not recommended).
*/

class WXDLLIMPEXP_GIS_DS wxGISTable : 
    public wxGISDataset,
	public wxGISConnectionPointContainer
{
    DECLARE_CLASS(wxGISTable)
    friend class wxGISSpatialTree;
public:
	wxGISTable(const CPLString &sPath, int nSubType, OGRLayer* poLayer = NULL, OGRDataSource* poDS = NULL);
	virtual ~wxGISTable(void);
	//wxGISDataset
    virtual wxString GetName(void) const;
	virtual void Close(void);
	//wxGISTable
	virtual bool Open(int iLayer = 0, int bUpdate = 0, bool bCache = true, ITrackCancel* const pTrackCancel = NULL);
    virtual size_t GetSubsetsCount(void) const;
    virtual wxGISDataset* GetSubset(size_t nIndex);
    virtual wxGISDataset* GetSubset(const wxString & sSubsetName);    
    //sysop
	virtual char **GetFileList();    
	virtual void Cache(ITrackCancel* const pTrackCancel = NULL);
    //rowop
	virtual size_t GetFeatureCount(bool bForce = false, ITrackCancel* const pTrackCancel = NULL);	
	virtual bool CanDeleteFeature(void);
	virtual OGRErr DeleteFeature(long nFID);    
    virtual OGRErr StoreFeature(wxGISFeature &Feature);
	virtual wxGISFeature CreateFeature(void);
    virtual OGRErr SetFeature(const wxGISFeature &Feature);  
    //
    virtual void Reset(void);
    virtual wxGISFeature Next(void);
    virtual wxGISFeature GetFeatureByID(long nFID);
    virtual wxGISFeature GetFeature(long nIndex);
    virtual int GetFIDColumn(void);
    //
    virtual wxFontEncoding GetEncoding(void) const;
    virtual void SetEncoding(const wxFontEncoding &oEncoding);
    virtual bool HasFID(void) const {return m_bHasFID;};
    virtual wxArrayString GetFieldNames(void);
    //
    virtual wxFeatureCursor Search(const wxGISQueryFilter &QFilter = wxGISNullQueryFilter, bool bOnlyFirst = false);
	/*    
    virtual OGRErr SetFilter(wxGISQueryFilter* pQFilter);	
    */
    virtual OGRErr SetIgnoredFields(const wxArrayString &saIgnoredFields);
	virtual OGRDataSource* const GetDataSourceRef(void) const {return m_poDS;};
    virtual OGRLayer* const GetLayerRef(int iLayer = 0) const {return m_poLayer;};
	virtual OGRFeatureDefn* const GetDefinition(void);
protected:
    virtual void SetInternalValues(void);
protected:
	OGRDataSource* m_poDS;
	OGRLayer* m_poLayer;

    wxFontEncoding m_Encoding;
    
    long m_nFeatureCount, m_nCurrentFID;
    bool m_bOLCStringsAsUTF8;
    bool m_bOLCFastFeatureCount;
    bool m_bHasFID;
};

//#define MAXSTRINGSTORE 1000000

/** \class wxGISTableCached table.h
    \brief A GIS cached Table class.

    The table with caching rows for future fast fetch.
*/

class WXDLLIMPEXP_GIS_DS wxGISTableCached : 
    public wxGISTable
{
    DECLARE_CLASS(wxGISTableCached)
public:
	wxGISTableCached(const CPLString &sPath, int nSubType, OGRLayer* poLayer = NULL, OGRDataSource* poDS = NULL);
	virtual ~wxGISTableCached(void);
	//wxGISDataset
	virtual void Close(void);
    //wxGISTable
    virtual void Reset(void);
    virtual wxGISFeature Next(void);
    virtual wxGISFeature GetFeatureByID(long nFID);
    virtual wxGISFeature GetFeature(long nIndex);
    //sysop
	virtual void Cache(ITrackCancel* const pTrackCancel = NULL);
    virtual bool IsCaching(void) const; 
    //rowop
	virtual size_t GetFeatureCount(bool bForce = false, ITrackCancel* const pTrackCancel = NULL);	
	virtual OGRErr DeleteFeature(long nFID);    
    virtual OGRErr StoreFeature(wxGISFeature &Feature);
    virtual OGRErr SetFeature(const wxGISFeature &Feature);  
protected:
    std::map<long, wxGISFeature> m_omFeatures;
    bool m_bIsCaching;
};
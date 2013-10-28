/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  PostresDataSource class.
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

#include "wxgisdefs.h"

#ifdef wxGIS_USE_POSTGRES

#include "gdal/ogr_pg.h"

#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/filter.h"

/** \class wxGISPostgresDataSource postgisdataset.h
    \brief The PostGIS DataSource class.
*/
class WXDLLIMPEXP_GIS_DS wxGISPostgresDataSource :
	public wxGISDataset
{
    DECLARE_CLASS(wxGISPostgresDataSource)
public:
	wxGISPostgresDataSource(const wxString &sName, const wxString &sPass, const wxString &sPort = wxT("5432"), const wxString &sAddres = wxT("localhost"), const wxString &sDBName = wxT("postgres"), bool bIsBinaryCursor = false);
    wxGISPostgresDataSource(const CPLString &szPath);
	virtual ~wxGISPostgresDataSource(void);

	//wxGISDataset
    virtual void Close(void);
    virtual size_t GetSubsetsCount(void) const; 
    virtual wxGISDataset* GetSubset(size_t nIndex);
    virtual wxGISDataset* GetSubset(const wxString &sTableName);
    virtual wxString GetName(void) const;
    virtual bool Open(int bUpdate = FALSE);
	//wxGISPostGISDataset
    wxGISDataset* ExecuteSQL(const wxString &sStatement, const wxString &sDialect = wxT("OGRSQL"));
    //the geometry in spatial filter should have the same SpaRef as the target layer 
    wxGISDataset* ExecuteSQL(const wxGISSpatialFilter &SpatialFilter, const wxString &sDialect = wxT("OGRSQL"));
    bool PGExecuteSQL(const wxString &sStatement);
    //wxGISDataset* PGExecuteSQL( const wxString &sStatement, bool bMultipleCommandAllowed = FALSE );
    //wxGISDataset
	virtual bool Delete(int iLayer = 0, ITrackCancel* const pTrackCancel = NULL);
	virtual bool Rename(const wxString &sNewName);
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel = NULL);
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel = NULL);    
	virtual void Cache(ITrackCancel* const pTrackCancel = NULL);
    virtual char **GetFileList(); 
protected:    
    inline void OGRPGClearResult( PGresult*& hResult )
    {
        if( NULL != hResult )
        {
            PQclear( hResult );
            hResult = NULL;
        }
    }
    PGresult *OGRPG_PQexec(PGconn *conn, const char *query, int bMultipleCommandAllowed = FALSE);
	wxGISDataset* GetDatasetFromOGRLayer(int iLayer, const CPLString &sPath, OGRLayer* poLayer);
protected:
	OGRDataSource *m_poDS;
    wxString m_sName, m_sPass, m_sPort, m_sAddres, m_sDBName;
	bool m_bIsBinaryCursor, m_bPathPresent;
    wxFontEncoding m_Encoding;
};

/** \class wxGISPostgresDataSource postgisdataset.h
    \brief The PostGIS DataSource class.
*/
class WXDLLIMPEXP_GIS_DS wxGISPostgresFeatureDataset :
	public wxGISFeatureDataset
{
    DECLARE_CLASS(wxGISPostgresFeatureDataset)
public:
	wxGISPostgresFeatureDataset(int iLayer, const CPLString &sPath, OGRLayer* poLayer = NULL, OGRDataSource* poDS = NULL);
	virtual ~wxGISPostgresFeatureDataset(void);
    //wxGISDataset
    virtual bool CanDelete(void);
    virtual bool Delete(int iLayer = 0, ITrackCancel* const pTrackCancel = NULL);
protected:
    int m_iLayer;
};

#endif //wxGIS_USE_POSTGRES


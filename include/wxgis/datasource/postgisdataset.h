/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  PostresDataSource class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2013 Bishop
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

#include "wxgisdefs.h"

#ifdef wxGIS_USE_POSTGRES

//#include "gdal/ogr_pg.h"
#include "libpq-fe.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/filter.h"

class OGRPGDataSource : public OGRDataSource
{
    typedef struct
    {
        int nMajor;
        int nMinor;
        int nRelease;
    } PGver;

    void   **papoLayers;
    int                 nLayers;

    char               *pszName;
    char               *pszDBName;

    int                 bDSUpdate;
    int                 bHavePostGIS;
    int                 bHaveGeography;

    int                 nSoftTransactionLevel;

    PGconn              *hPGConn;

    int                 DeleteLayer(int iLayer);

    Oid                 nGeometryOID;
    Oid                 nGeographyOID;

    // We maintain a list of known SRID to reduce the number of trips to
    // the database to get SRSes.
    int                 nKnownSRID;
    int                 *panSRID;
    OGRSpatialReference **papoSRS;

    void     *poLayerInCopyMode;

    CPLString           osCurrentSchema;

    int                 nUndefinedSRID;

public:
    PGver               sPostgreSQLVersion;
    PGver               sPostGISVersion;

    int                 bUseBinaryCursor;
    int                 bBinaryTimeFormatIsInt8;
    int                 bUseEscapeStringSyntax;

public:
    PGconn              *GetPGConn() { return hPGConn; }
};

/** @class wxGISPostgresDataSource

    The PostGIS DataSource class.

    @library{datasource}
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
    virtual bool Open(int bUpdate = TRUE);
	//wxGISPostGISDataset
    bool ExecuteSQL(const wxString &sStatement);
    bool CreateDatabase(const wxString &sDBName, const wxString &sTemplate = wxT("template_postgis"), const wxString &sOwner = wxT("postgres"), const wxString &sEncoding = wxT("UTF8"));
    wxGISDataset* ExecuteSQL2(const wxString &sStatement, const wxString &sDialect = wxT("OGRSQL"));
    //the geometry in spatial filter should have the same SpaRef as the target layer 
    wxGISDataset* ExecuteSQL2(const wxGISSpatialFilter &SpatialFilter, const wxString &sDialect = wxT("OGRSQL"));

    //bool PGExecuteSQL(const wxString &sStatement);
    bool CreateSchema(const wxString &sSchemaName);
    bool DeleteSchema(const wxString &sSchemaName);
    bool RenameSchema(const wxString &sSchemaName, const wxString &sSchemaNewName);
    bool RenameTable(const wxString &sSchemaName, const wxString &sTableName, const wxString &sTableNewName);
    bool MoveTable(const wxString &sTableName, const wxString &sSchemaName, const wxString &sSchemaNewName);
    //wxGISDataset
	virtual bool Rename(const wxString &sNewName);
	virtual bool Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel = NULL);
	virtual bool Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel = NULL);    
	virtual void Cache(ITrackCancel* const pTrackCancel = NULL);
    virtual char **GetFileList(); 
    //
    virtual OGRDataSource* const GetDataSourceRef(void) const { return m_poDS; };
    virtual wxFontEncoding GetEncoding() const { return m_Encoding; };
    static wxString NormalizeTableName(const wxString &sSrcName);
protected:    
	wxGISDataset* GetDatasetFromOGRLayer(const CPLString &sPath, OGRLayer* poLayer);
protected:
	OGRDataSource *m_poDS;
	OGRDataSource *m_poDS4SQL;
    wxString m_sName, m_sPass, m_sPort, m_sAddres, m_sDBName;
	bool m_bIsBinaryCursor, m_bPathPresent;
    wxFontEncoding m_Encoding;
};

/** @class wxGISPostgresFeatureDataset

    The PostGIS Feature dataset class.

    @library{datasource}
*/
class WXDLLIMPEXP_GIS_DS wxGISPostgresFeatureDataset :
	public wxGISFeatureDataset
{
    DECLARE_CLASS(wxGISPostgresFeatureDataset)
public:
    wxGISPostgresFeatureDataset(const CPLString &sPath, OGRLayer* poLayer = NULL, OGRDataSource* poDS = NULL);
	virtual ~wxGISPostgresFeatureDataset(void);
    //wxGISDataset
    virtual bool CanDelete(void);
    virtual bool Delete(ITrackCancel* const pTrackCancel = NULL);
protected:
    CPLString m_sLayerName;
};

#endif //wxGIS_USE_POSTGRES


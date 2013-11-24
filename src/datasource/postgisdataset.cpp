/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  PosGISDataset class.
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
#include "wxgis/datasource/postgisdataset.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/core/config.h"
#include "wxgisdefs.h"
#include "wxgis/core/crypt.h"
#include "wxgis/core/format.h"

//------------------------------------------------------------------------------
// wxGISPostgresDataSource
//------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISPostgresDataSource, wxGISDataset)

wxGISPostgresDataSource::wxGISPostgresDataSource(const wxString &sName, const wxString &sPass, const wxString &sPort, const wxString &sAddres, const wxString &sDBName, bool bIsBinaryCursor) : wxGISDataset("")
{
    m_bPathPresent = false;
	m_bIsOpened = false;
	m_nType = enumGISContainer;
	m_nSubType = enumContRemoteDBConnection;
    m_sName = sName;
    m_sPass = sPass;
    m_sPort = sPort;
    m_sAddres = sAddres;
    m_sDBName = sDBName;
	m_bIsBinaryCursor = bIsBinaryCursor;

    m_poDS4SQL = NULL;
    m_poDS = NULL;

    m_Encoding = wxFONTENCODING_UTF8;
}

wxGISPostgresDataSource::wxGISPostgresDataSource(const CPLString &szPath) : wxGISDataset(szPath)
{
    m_bPathPresent = true;
	m_bIsOpened = false;
	m_nType = enumGISContainer;
	m_nSubType = enumContRemoteDBConnection;

    m_poDS4SQL = NULL;
    m_poDS = NULL;

	wxXmlDocument doc(wxString(szPath,  wxConvUTF8));
	if(doc.IsOk())
    {
		wxXmlNode* pRootNode = doc.GetRoot();
		if(pRootNode)
		{
			m_sAddres = pRootNode->GetAttribute(wxT("server"), wxEmptyString);
			m_sPort = pRootNode->GetAttribute(wxT("port"), wxEmptyString);
			m_sDBName = pRootNode->GetAttribute(wxT("db"), wxEmptyString);
			m_sName = pRootNode->GetAttribute(wxT("user"), wxEmptyString);
			Decrypt(pRootNode->GetAttribute(wxT("pass"), wxEmptyString), m_sPass);
			m_bIsBinaryCursor = GetBoolValue(pRootNode, wxT("isbincursor"), false);
        }
    }
    m_Encoding = wxFONTENCODING_UTF8;
}

wxGISPostgresDataSource::~wxGISPostgresDataSource(void)
{
	Close();
}

void wxGISPostgresDataSource::Close(void)
{
	m_bIsOpened = false;
    if(m_poDS && m_poDS->Dereference() <= 0)
        OGRDataSource::DestroyDataSource( m_poDS );
	m_poDS = NULL;
    if (m_poDS4SQL && m_poDS4SQL->Dereference() <= 0)
        OGRDataSource::DestroyDataSource(m_poDS4SQL);
    m_poDS4SQL = NULL;
    
}

size_t wxGISPostgresDataSource::GetSubsetsCount(void) const
{
    if( m_poDS )
        return  m_poDS->GetLayerCount();
    return 0;
}

wxGISDataset* wxGISPostgresDataSource::GetSubset(size_t nIndex)
{
    if(m_poDS)
    {
	    OGRLayer* poLayer = m_poDS->GetLayer(nIndex);
		return GetDatasetFromOGRLayer(nIndex, m_sPath, poLayer);
    }
    return NULL;
}

wxGISDataset* wxGISPostgresDataSource::GetSubset(const wxString &sTableName)
{
    if(m_poDS)
    {
        CPLString sCmpName;
        wxString woPublicStr;
        if (sTableName.StartsWith(wxT("public."), &woPublicStr))
        {
            sCmpName = CPLString(woPublicStr.mb_str(wxConvUTF8));
        }
        else
        {
            sCmpName = CPLString(sTableName.mb_str(wxConvUTF8));
        }
        for (int i = 0; i < m_poDS->GetLayerCount(); ++i)
        {
            OGRLayer* poLayer = m_poDS->GetLayer(i);
            if (NULL != poLayer && wxGISEQUAL(sCmpName, poLayer->GetName()))
            {
                return GetDatasetFromOGRLayer(i, m_sPath, poLayer);
            }
        }

        //try to OpenTable
        OGRLayer* poLayer = m_poDS->GetLayerByName(sCmpName);
        if (NULL != poLayer)
        {
            for (int i = 0; i < m_poDS->GetLayerCount(); ++i)
            {
                OGRLayer* poLayer = m_poDS->GetLayer(i);
                if (NULL != poLayer && wxGISEQUAL(sCmpName, poLayer->GetName()))
                {
                    return GetDatasetFromOGRLayer(i, m_sPath, poLayer);
                }
            }
        }
    }
    return NULL;
}

wxGISDataset* wxGISPostgresDataSource::GetDatasetFromOGRLayer(int iLayer, const CPLString &sPath, OGRLayer* poLayer)
{
	wxCHECK_MSG(poLayer, NULL, wxT("Input layer pointer is null"));
	//check the layer type
    wxGISDataset* pDataset(NULL);
    CPLString szGeomColumn(poLayer->GetGeometryColumn());
	if(!szGeomColumn.empty())
	{
        wxGISPostgresFeatureDataset* pFeatureDataset = new wxGISPostgresFeatureDataset(iLayer, sPath, poLayer, m_poDS);
		pFeatureDataset->SetEncoding(m_Encoding);
        pDataset = static_cast<wxGISDataset*>(pFeatureDataset);
	}
	else
	{
        wxGISTable* pTable = new wxGISTable(sPath, enumTablePostgres, poLayer, m_poDS);
		pTable->SetEncoding(m_Encoding);
        pDataset = static_cast<wxGISDataset*>(pTable);
	}
    
    wsGET(pDataset);
}

wxString wxGISPostgresDataSource::GetName(void) const
{
    return m_sDBName;
}

//wxGISDataset* wxGISPostgresDataSource::PGExecuteSQL( const wxString &sStatement, bool bMultipleCommandAllowed )
//{
//    OGRPGDataSource* pDS = dynamic_cast<OGRPGDataSource*>(m_poDS);
//    if(!pDS)
//        return NULL;
//
//    CPLErrorReset();
//    PGconn* hPGConn = pDS->GetPGConn();
//
//    PGresult    *hResult;        
//
//    wxString sCursorName = wxString::Format(wxT("cur_%d"), wxNewId());
//    wxString sFullSQLCommand = wxString::Format(wxT("DECLARE %s CURSOR FOR %s"), sCursorName.c_str(), sStatement.c_str());
//
//    hResult =  OGRPG_PQexec( hPGConn, sFullSQLCommand.mb_str(wxConvUTF8), bMultipleCommandAllowed );
//    if(hResult && PQresultStatus(hResult) == PGRES_COMMAND_OK)
//    {
//        OGRPGClearResult( hResult );
//
//        sFullSQLCommand = wxString::Format(wxT("FETCH 0 in %s"), sCursorName.c_str() );
//        CPLString szFullSQLCommand(sFullSQLCommand.mb_str(wxConvUTF8));
//        hResult = OGRPG_PQexec(hPGConn, szFullSQLCommand );
//
//        OGRPGResultLayer *poLayer = new OGRPGResultLayer( pDS, szFullSQLCommand, hResult );
//        
//        OGRPGClearResult( hResult );
//
//        m_poDS->Reference();
//        wxGISTable* pTable = new wxGISTable(szFullSQLCommand, enumTableQueryResult, poLayer, m_poDS);
//		pTable->SetEncoding(m_Encoding);
//        return wxStaticCast(pTable, wxGISDataset);
//    }
//
//    CPLError( CE_Failure, CPLE_AppDefined, "%s", PQerrorMessage( hPGConn ) );
//    wxLogError(wxT("wxGISPostgresDataSource: %s"), PQerrorMessage( hPGConn ));
//     
//    OGRPGClearResult( hResult );
//    return NULL;
//}

bool wxGISPostgresDataSource::CreateSchema(const wxString &sSchemaName)
{
    return PGExecuteSQL(wxString::Format(wxT("CREATE SCHEMA \"%s\";"), sSchemaName.c_str()));
}

bool wxGISPostgresDataSource::DeleteSchema(const wxString &sSchemaName)
{
    return PGExecuteSQL(wxString::Format(wxT("DROP SCHEMA \"%s\" CASCADE;"), sSchemaName.c_str()));
}

bool wxGISPostgresDataSource::RenameSchema(const wxString &sSchemaName, const wxString &sSchemaNewName)
{
    if (sSchemaName == sSchemaNewName)
    {
        return true;
    }
    return PGExecuteSQL(wxString::Format(wxT("ALTER SCHEMA \"%s\" RENAME TO \"%s\";"), sSchemaName.c_str(), sSchemaNewName.c_str()));
}

bool wxGISPostgresDataSource::RenameTable(const wxString &sSchemaName, const wxString &sTableName, const wxString &sTableNewName)
{
    if (sTableName == sTableNewName)
    {
        return true;
    }
    wxString sResultName = wxGISPostgresDataSource::NormalizeTableName(sTableNewName);
    return PGExecuteSQL(wxString::Format(wxT("ALTER TABLE %s.\"%s\" RENAME TO \"%s\";"), sSchemaName.c_str(), sTableName.c_str(), sResultName.c_str()));
}

bool wxGISPostgresDataSource::MoveTable(const wxString &sTableName, const wxString &sSchemaName, const wxString &sSchemaNewName)
{
    return false;
}

bool wxGISPostgresDataSource::PGExecuteSQL(const wxString &sStatement)
{
	wxCriticalSectionLocker locker(m_CritSect);
    OGRPGDataSource* pDS = dynamic_cast<OGRPGDataSource*>(m_poDS4SQL);
    if(!pDS)
        return false;

    CPLErrorReset();
    PGconn* hPGConn = pDS->GetPGConn();

    PGresult    *hResult = OGRPG_PQexec( hPGConn, sStatement.mb_str(wxConvUTF8), TRUE );
    ExecStatusType status = PQresultStatus(hResult);
    if (status == PGRES_COMMAND_OK)
    {
        OGRPGClearResult( hResult );
        return true;
    }
    if( status == PGRES_NONFATAL_ERROR || status == PGRES_FATAL_ERROR )
    {
        CPLError( CE_Failure, CPLE_AppDefined, "%s", PQerrorMessage( hPGConn ) );
        wxLogError(wxT("wxGISPostgresDataSource: %s"), wxString(PQerrorMessage(hPGConn), wxCSConv(m_Encoding)));
    }

    OGRPGClearResult( hResult );
    return false;
}

wxGISDataset* wxGISPostgresDataSource::ExecuteSQL(const wxString &sStatement, const wxString &sDialect)
{
	wxCriticalSectionLocker locker(m_CritSect);
	wxGISDataset* pDataset(NULL);
    if (m_poDS4SQL)
	{
        CPLString szStatement(sStatement.mb_str(wxConvUTF8));
        OGRLayer * poLayer = m_poDS4SQL->ExecuteSQL(szStatement, NULL, sDialect.mb_str(wxConvUTF8));
		if(	poLayer )
		{
            poLayer->ResetReading();
            wxGISTableQuery* pTable = new wxGISTableQuery(szStatement, enumTableQueryResult, poLayer, m_poDS4SQL);
			pTable->SetEncoding(m_Encoding);
            pTable->Cache();
            pDataset = static_cast<wxGISDataset*>(pTable);
		}
        else
        {
            const char* err = CPLGetLastErrorMsg();
            wxLogError(_("ExecuteSQL failed! GDAL error: %s"), wxString(err, wxConvUTF8).c_str());
        }
    }
	wsGET(pDataset);
}

wxGISDataset* wxGISPostgresDataSource::ExecuteSQL(const wxGISSpatialFilter &SpatialFilter, const wxString &sDialect)
{
	wxCriticalSectionLocker locker(m_CritSect);
	wxGISDataset* pDataset(NULL);
    if (m_poDS4SQL)
	{
        CPLString szStatement(SpatialFilter.GetWhereClause().mb_str(wxConvUTF8));
        OGRLayer * poLayer = m_poDS4SQL->ExecuteSQL(szStatement, SpatialFilter.GetGeometry(), sDialect.mb_str(wxConvUTF8));
		if(	poLayer )
		{
            wxGISTableCached* pTable = new wxGISTableCached(szStatement, enumTableQueryResult, poLayer, m_poDS4SQL);
			pTable->SetEncoding(m_Encoding);
            pDataset = static_cast<wxGISDataset*>(pTable);
		}
        else
        {
            const char* err = CPLGetLastErrorMsg();
            wxLogError(_("ExecuteSQL failed! GDAL error: %s"), wxString(err, wxConvUTF8).c_str());
        }
	}
	wsGET( pDataset );
}

bool wxGISPostgresDataSource::Open(int bUpdate)
{
	wxCriticalSectionLocker locker(m_CritSect);
	if(m_bIsOpened)
		return true;

    wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
    {
        bool bReadAllTabs = oConfig.ReadBool(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/pg_list_all_tables")), true);
        CPLSetConfigOption("PG_LIST_ALL_TABLES", bReadAllTabs == true ? "YES" : "NO" );
        CPLSetConfigOption("PGCLIENTENCODING", oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/pg_client_encoding")), wxT("UTF-8")).mb_str());
        CPLSetConfigOption("OGR_PG_RETRIEVE_FID", oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/pg_retrieve_fid")), wxT("TRUE")).mb_str()); 
    }
    else
    {
    	CPLSetConfigOption("PG_LIST_ALL_TABLES", "YES");
	    CPLSetConfigOption("PGCLIENTENCODING", "UTF-8");
        CPLSetConfigOption("OGR_PG_RETRIEVE_FID", "TRUE");
    }
	CPLErrorReset();

    //"PG:host='127.0.0.1' dbname='db' port='5432' user='bishop' password='xxx'"
	wxString sConnStr = wxString::Format(wxT("%s:host='%s' dbname='%s' port='%s' user='%s' password='%s'"), m_bIsBinaryCursor == true ? wxT("PGB") : wxT("PG"), m_sAddres.c_str(), m_sDBName.c_str(), m_sPort.c_str(), m_sName.c_str(), m_sPass.c_str());
	wxLogVerbose(_("Try to connect: host='%s' dbname='%s' port='%s' user='%s'"), m_sAddres.c_str(), m_sDBName.c_str(), m_sPort.c_str(), m_sName.c_str());
    m_poDS = OGRSFDriverRegistrar::Open( sConnStr.mb_str(wxConvUTF8), bUpdate );//
	if( m_poDS == NULL )
	{
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Connect failed! GDAL error: %s, host='%s' dbname='%s' port='%s' user='%s'"), wxString(err, wxConvUTF8).c_str(), m_sAddres.c_str(), m_sDBName.c_str(), m_sPort.c_str(), m_sName.c_str());
		return false;
	}
	wxString sPath = wxString::Format(wxT("host='%s' dbname='%s' port='%s' user='%s' password='%s'"), m_sAddres.c_str(), m_sDBName.c_str(), m_sPort.c_str(), m_sName.c_str(), m_sPass.c_str());
	m_sPath = CPLString(sPath.mb_str(wxConvUTF8));

    //open second connection 
    m_poDS4SQL = OGRSFDriverRegistrar::Open(sConnStr.mb_str(wxConvUTF8), bUpdate);//


	m_bIsOpened = true;

    wxLogVerbose(_("Connect succeeded! Host='%s' dbname='%s' port='%s' user='%s'"), m_sAddres.c_str(), m_sDBName.c_str(), m_sPort.c_str(), m_sName.c_str());

	return true;
}


PGresult *wxGISPostgresDataSource::OGRPG_PQexec(PGconn *conn, const char *query, int bMultipleCommandAllowed)
{
    PGresult* hResult;
#if defined(PG_PRE74)
    /* PQexecParams introduced in PG >= 7.4 */
    hResult = PQexec(conn, query);
#else
    if (bMultipleCommandAllowed)
        hResult = PQexec(conn, query);
    else
        hResult = PQexecParams(conn, query, 0, NULL, NULL, NULL, NULL, 0);
#endif

#ifdef _DEBUG
    const char* pszRetCode = "UNKNOWN";
    char szNTuples[32];
    szNTuples[0] = '\0';
    if (hResult)
    {
        switch(PQresultStatus(hResult))
        {
            case PGRES_TUPLES_OK:
                pszRetCode = "PGRES_TUPLES_OK";
                sprintf(szNTuples, ", ntuples = %d", PQntuples(hResult));
                break;
            case PGRES_COMMAND_OK:
                pszRetCode = "PGRES_COMMAND_OK";
                break;
            case PGRES_NONFATAL_ERROR:
                pszRetCode = "PGRES_NONFATAL_ERROR";
                break;
            case PGRES_FATAL_ERROR:
                pszRetCode = "PGRES_FATAL_ERROR";
                break;
            default: break;
        }
    }
    if (bMultipleCommandAllowed)
        CPLDebug("PG", "PQexec(%s) = %s%s", query, pszRetCode, szNTuples);
    else
        CPLDebug("PG", "PQexecParams(%s) = %s%s", query, pszRetCode, szNTuples);

/* -------------------------------------------------------------------- */
/*      Generate an error report if an error occured.                   */
/* -------------------------------------------------------------------- */
    if ( !hResult || (PQresultStatus(hResult) == PGRES_NONFATAL_ERROR ||
                      PQresultStatus(hResult) == PGRES_FATAL_ERROR ) )
    {
        CPLDebug( "PG", "%s", PQerrorMessage( conn ) );
    }
#endif

    return hResult;
}

bool wxGISPostgresDataSource::Rename(const wxString &sNewName)
{
    if(m_bPathPresent)
        return wxGISDataset::Rename(sNewName);
    else
        return false;
}

bool wxGISPostgresDataSource::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(m_bPathPresent)
        return wxGISDataset::Copy(szDestPath, pTrackCancel);
    else
        return false;
}

bool wxGISPostgresDataSource::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(m_bPathPresent)
        return wxGISDataset::Move(szDestPath, pTrackCancel);
    else
        return false;
}
    
void wxGISPostgresDataSource::Cache(ITrackCancel* const pTrackCancel)
{
}

char **wxGISPostgresDataSource::GetFileList()
{
    return NULL;
}

wxString wxGISPostgresDataSource::NormalizeTableName(const wxString &sSrcName)
{
    wxString sResultName = sSrcName;

    //make PG compatible
    for (int i = 0; i < sResultName.size(); ++i)
    {
        sResultName[i] = wxTolower(sResultName[i]);
        if (sResultName[i] == '\'' || sResultName[i] == '-' || sResultName[i] == '#' || sResultName[i] == '(')
            sResultName[i] = '_';
        else if (sResultName[i] == ')')
        {
            sResultName.Remove(i);
            i--;
        }
    }
    return sResultName;
}


//---------------------------------------
// wxGISPostgresFeatureDataset
//---------------------------------------

IMPLEMENT_CLASS(wxGISPostgresFeatureDataset, wxGISFeatureDataset)

wxGISPostgresFeatureDataset::wxGISPostgresFeatureDataset(int iLayer, const CPLString &sPath, OGRLayer* poLayer, OGRDataSource* poDS) : wxGISFeatureDataset(sPath, emumVecPostGIS, poLayer, poDS)
{
    m_iLayer = iLayer;
    m_nType = enumGISFeatureDataset;
    m_eGeomType = wkbUnknown;
    m_pSpatialTree = NULL;

    if(m_bIsOpened)
        SetInternalValues();
}

wxGISPostgresFeatureDataset::~wxGISPostgresFeatureDataset(void)
{
}

bool wxGISPostgresFeatureDataset::CanDelete(void)
{
    //TODO: check permissions
    if (m_poDS)
    {
        return m_poDS->TestCapability(ODsCDeleteLayer) == 0 ? false : true;
    }
    return false;
}

bool wxGISPostgresFeatureDataset::Delete(ITrackCancel* const pTrackCancel)
{
    if(m_poDS)
    {
        OGRErr eErr = m_poDS->DeleteLayer(m_iLayer);
        if(eErr !=  OGRERR_NONE)
        {
            const char* err = CPLGetLastErrorMsg();
		    wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s"), _("Delete"), wxString(err, wxConvUTF8).c_str());
            if(pTrackCancel)
            {
                pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
            }
            return false;
        }
        else
        {
		    wxString sMsg = wxString::Format(_("Operation '%s' succeeded!"), _("Delete"));
            if(pTrackCancel)
            {
                pTrackCancel->PutMessage(sMsg, wxNOT_FOUND, enumGISMessageInfo);
            }
        }
    }
    return true;
}

#endif //wxGIS_USE_POSTGRES
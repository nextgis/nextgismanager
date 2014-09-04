/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  PosGISDataset class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2014 Dmitry Baryshnikov
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

    GDALDriverH hPostGISRaster = GDALGetDriverByName("PostGISRaster");
    if(hPostGISRaster != NULL)
    {
        GDALDeregisterDriver(hPostGISRaster);
    }

    ReadConnectionFile();

    m_Encoding = wxFONTENCODING_UTF8;
}

wxGISPostgresDataSource::~wxGISPostgresDataSource(void)
{
	Close();
}

void wxGISPostgresDataSource::ReadConnectionFile()
{
	wxXmlDocument doc(wxString(m_sPath,  wxConvUTF8));
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
}

void wxGISPostgresDataSource::Close(void)
{
    wxCriticalSectionLocker locker(m_CritSect);

	m_bIsOpened = false;
    if (m_poDS4SQL && m_poDS4SQL->Dereference() <= 0)
        OGRDataSource::DestroyDataSource(m_poDS4SQL);
    m_poDS4SQL = NULL;
    if(m_poDS && m_poDS->Dereference() <= 0)
        OGRDataSource::DestroyDataSource( m_poDS );
	m_poDS = NULL;
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
        return GetDatasetFromOGRLayer(m_sPath, poLayer);
    }
    return NULL;
}

wxGISDataset* wxGISPostgresDataSource::GetSubset(const wxString &sTableName)
{
    if(m_poDS)
    {
        CPLString sCmpName;
        wxString woPublicStr;
        if (!sTableName.StartsWith(wxT("public."), &woPublicStr))
        {
            woPublicStr = sTableName;
        }
        woPublicStr.Replace(wxT("\""), wxT(""));
        sCmpName = CPLString(woPublicStr.ToUTF8());
        //for (int i = 0; i < m_poDS->GetLayerCount(); ++i)
        //{
        //    OGRLayer* poLayer = m_poDS->GetLayer(i);
        //    if (NULL != poLayer && wxGISEQUAL(sCmpName, poLayer->GetName()))
        //    {
        //        return GetDatasetFromOGRLayer(i, m_sPath, poLayer);
        //    }
        //}

        //try to OpenTable
        OGRLayer* poLayer = m_poDS->GetLayerByName(sCmpName);
        return GetDatasetFromOGRLayer(m_sPath, poLayer);
        //if (NULL != poLayer)
        //{
        //    for (int i = 0; i < m_poDS->GetLayerCount(); ++i)
        //    {
        //        poLayer = m_poDS->GetLayer(i);
        //        if (NULL != poLayer && wxGISEQUAL(sCmpName, poLayer->GetName()))
        //        {
        //            return GetDatasetFromOGRLayer(i, m_sPath, poLayer);
        //        }
        //    }
        //}
    }
    return NULL;
}

wxGISDataset* wxGISPostgresDataSource::GetDatasetFromOGRLayer(const CPLString &sPath, OGRLayer* poLayer)
{
	wxCHECK_MSG(poLayer, NULL, wxT("Input layer pointer is null"));
	//check the layer type
    wxGISDataset* pDataset(NULL);
    CPLString szGeomColumn(poLayer->GetGeometryColumn());
	if(!szGeomColumn.empty())
	{
        wxGISPostgresFeatureDataset* pFeatureDataset = new wxGISPostgresFeatureDataset(sPath, poLayer, m_poDS);
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

bool wxGISPostgresDataSource::CreateSchema(const wxString &sSchemaName)
{
    return ExecuteSQL(wxString::Format(wxT("CREATE SCHEMA \"%s\";"), sSchemaName.c_str()));
}

bool wxGISPostgresDataSource::DeleteSchema(const wxString &sSchemaName)
{
    return ExecuteSQL(wxString::Format(wxT("DROP SCHEMA \"%s\" CASCADE;"), sSchemaName.c_str()));
}

bool wxGISPostgresDataSource::RenameSchema(const wxString &sSchemaName, const wxString &sSchemaNewName)
{
    if (sSchemaName == sSchemaNewName)
    {
        return true;
    }
    return ExecuteSQL(wxString::Format(wxT("ALTER SCHEMA \"%s\" RENAME TO \"%s\";"), sSchemaName.c_str(), sSchemaNewName.c_str()));
}

bool wxGISPostgresDataSource::RenameTable(const wxString &sSchemaName, const wxString &sTableName, const wxString &sTableNewName)
{
    if (sTableName == sTableNewName)
    {
        return true;
    }
    wxCriticalSectionLocker locker(m_CritSect);
    wxString sResultName = wxGISPostgresDataSource::NormalizeTableName(sTableNewName);
    wxString sSQL = wxString::Format(wxT("ALTER TABLE \"%s\".\"%s\" RENAME TO \"%s\";"), sSchemaName.c_str(), sTableName.c_str(), sResultName.c_str());
    sSQL.Append(wxString::Format(wxT("ALTER INDEX \"%s\".\"%s_wkb_geometry_geom_idx\" RENAME TO \"%s_wkb_geometry_geom_idx\";"), sSchemaName.c_str(), sTableName.c_str(), sResultName.c_str()));
    sSQL.Append(wxString::Format(wxT("ALTER INDEX \"%s\".\"%s_pkey\" RENAME TO \"%s_pkey\";"), sSchemaName.c_str(), sTableName.c_str(), sResultName.c_str()));
    CPLErrorReset();
    m_poDS->ExecuteSQL(sSQL.ToUTF8(), NULL, NULL);
    if (CPLGetLastErrorNo() != CE_None)
    {
        CPLError(CE_Failure, CPLE_AppDefined, CPLGetLastErrorMsg());
        wxLogError(wxT("wxGISPostgresDataSource: %s"), wxString(CPLGetLastErrorMsg(), wxCSConv(m_Encoding)));

        return false;
    }
    //sSQL = wxString::Format(wxT("ALTER INDEX \"%s\".\"%s_wkb_geometry_geom_idx\" RENAME TO \"%s_wkb_geometry_geom_idx\";"), sSchemaName.c_str(), sTableName.c_str(), sResultName.c_str());
    //m_poDS->ExecuteSQL(sSQL.ToUTF8(), NULL, NULL);
    //if (CPLGetLastErrorNo() != CE_None)
    //{
    //    CPLError(CE_Failure, CPLE_AppDefined, CPLGetLastErrorMsg());
    //    wxLogError(wxT("wxGISPostgresDataSource: %s"), wxString(CPLGetLastErrorMsg(), wxCSConv(m_Encoding)));

    //    return false;
    //}

    //sSQL = wxString::Format(wxT("ALTER INDEX \"%s\".\"%s_pkey\" RENAME TO \"%s_pkey\";"), sSchemaName.c_str(), sTableName.c_str(), sResultName.c_str());
    //m_poDS->ExecuteSQL(sSQL.ToUTF8(), NULL, NULL);
    //if (CPLGetLastErrorNo() != CE_None)
    //{
    //    CPLError(CE_Failure, CPLE_AppDefined, CPLGetLastErrorMsg());
    //    wxLogError(wxT("wxGISPostgresDataSource: %s"), wxString(CPLGetLastErrorMsg(), wxCSConv(m_Encoding)));

    //    return false;
    //}
    return true;
    //TODO: find index and rename it to gdal look like
}

bool wxGISPostgresDataSource::MoveTable(const wxString &sTableName, const wxString &sSchemaName, const wxString &sSchemaNewName)
{
    return false;
}

bool wxGISPostgresDataSource::CreateDatabase(const wxString &sDBName, const wxString &sTemplate, const wxString &sOwner, const wxString &sEncoding)
{
    if (sTemplate.IsEmpty())
    {
        wxString sCreateDb = wxString::Format(wxT("CREATE DATABASE %s WITH OWNER = %s ENCODING = '%s' TABLESPACE = pg_default;"), sDBName.c_str(), sOwner.c_str(), sEncoding.c_str() );
        //SET EXTENSION
        OGRPGDataSource *pPGDS = (OGRPGDataSource*)(m_poDS);//dynamic_cast<OGRPGDataSource*>(m_poDS);
        if (pPGDS != NULL)
        {
            PGresult* hResult = PQexec(pPGDS->GetPGConn(), sCreateDb.mb_str());
            if (hResult)
            {
                bool bRes = PQresultStatus(hResult) == PGRES_COMMAND_OK;
                PQclear(hResult);
                if (!bRes)
                {
                    return false;
                }

                //reconnect to new db
                Close();
                m_sDBName = sDBName;
                if(Open(true, true))
                {
                    //execute extenstion ...
                    return ExecuteSQL(wxT("CREATE EXTENSION IF NOT EXISTS postgis;"));
                }
                else
                {
                    CPLError(CE_Failure, CPLE_AppDefined, "Open created DB failed failed");
                }
            }
            else
            {
                CPLError(CE_Failure, CPLE_AppDefined, "PGresult is null");
            }
        }
        else
        {
            CPLError(CE_Failure, CPLE_AppDefined, "Dynamic cast to OGRPGDataSource failed");
        }
    }
    else
    {
        wxString sCreateDb = wxString::Format(wxT("CREATE DATABASE %s WITH OWNER = %s ENCODING = '%s' TEMPLATE=%s TABLESPACE = pg_default;"), sDBName.c_str(), sOwner.c_str(), sEncoding.c_str(), sTemplate.c_str() );
        OGRPGDataSource *pPGDS = (OGRPGDataSource*)(m_poDS);//dynamic_cast<OGRPGDataSource*>(m_poDS);
        if (pPGDS != NULL)
        {
            //TODO: execute drop connections on template db
            PGresult* hResult = PQexec(pPGDS->GetPGConn(), sCreateDb.mb_str());
            if (hResult)
            {
                bool bRes = PQresultStatus(hResult) == PGRES_COMMAND_OK;
                if (!bRes)
                {
                    CPLError(CE_Failure, CPLE_AppDefined, "%s", PQerrorMessage(pPGDS->GetPGConn()));
                }
                PQclear(hResult);
                return bRes;
            }
            else
            {
                CPLError(CE_Failure, CPLE_AppDefined, "PGresult is null");
            }
        }
        else
        {
            CPLError(CE_Failure, CPLE_AppDefined, "Dynamic cast to OGRPGDataSource failed");
        }
    }

    return false;
}

bool wxGISPostgresDataSource::ExecuteSQL(const wxString &sStatement)
{
    wxCriticalSectionLocker locker(m_CritSect);
    CPLErrorReset();
    if (NULL == m_poDS)
        return false;

    try
    {
        m_poDS->ExecuteSQL(sStatement.ToUTF8(), NULL, NULL);
    }
    catch (...)
    {
        CPLError(CE_Failure, CPLE_AppDefined, "Unexpected error");
        wxLogError(wxT("wxGISPostgresDataSource: Unexpected error"));

        return false;
    }

    if (CPLGetLastErrorNo() != CE_None)
    {
        CPLError(CE_Failure, CPLE_AppDefined, CPLGetLastErrorMsg());
        wxLogError(wxT("wxGISPostgresDataSource: %s"), wxString(CPLGetLastErrorMsg(), wxCSConv(m_Encoding)));

        return false;
    }
    return true;

}

wxGISDataset* wxGISPostgresDataSource::ExecuteSQL2(const wxString &sStatement, const wxString &sDialect)
{
	wxCriticalSectionLocker locker(m_CritSect);
	wxGISDataset* pDataset(NULL);
    if (NULL != m_poDS4SQL)
	{
	    CPLErrorReset();
        CPLString szStatement(sStatement.mb_str(wxConvUTF8));
        const char* szDialect = NULL;
        if (!sDialect.IsEmpty())
            szDialect = sDialect.mb_str(wxConvUTF8);
        OGRLayer * poLayer = m_poDS4SQL->ExecuteSQL(szStatement, NULL, szDialect);
		if(	poLayer )
		{
            poLayer->ResetReading();
            wxGISTableQuery* pTable = new wxGISTableQuery(szStatement, enumTableQueryResult, poLayer, m_poDS4SQL);
			pTable->SetEncoding(m_Encoding);
            pTable->Cache();
            pDataset = static_cast<wxGISDataset*>(pTable);
		}
        /*else
        {
            const char* err = CPLGetLastErrorMsg();
            wxLogError(_("ExecuteSQL failed! GDAL error: %s"), wxString(err, wxConvUTF8).c_str());
        }*/
    }
	wsGET(pDataset);
}

wxGISDataset* wxGISPostgresDataSource::ExecuteSQL2(const wxGISSpatialFilter &SpatialFilter, const wxString &sDialect)
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

bool wxGISPostgresDataSource::Open(bool bUpdate, bool bShared)
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
    CPLString szConnStr(sConnStr.ToUTF8());
    m_poDS = (OGRDataSource*) wxGISDataset::OpenInternal( szConnStr, bUpdate, bShared );
	if( m_poDS == NULL )
	{
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Connect failed! GDAL error: %s, host='%s' dbname='%s' port='%s' user='%s'"), wxString(err, wxConvUTF8).c_str(), m_sAddres.c_str(), m_sDBName.c_str(), m_sPort.c_str(), m_sName.c_str());
		return false;
	}

    //wxString sPath = wxString::Format(wxT("host='%s' dbname='%s' port='%s' user='%s' password='%s'"), m_sAddres.c_str(), m_sDBName.c_str(), m_sPort.c_str(), m_sName.c_str(), m_sPass.c_str());
	//m_sPath = CPLString(sPath.mb_str(wxConvUTF8));

    //open second connection

    m_poDS4SQL = (OGRDataSource*)wxGISDataset::OpenInternal(szConnStr, bUpdate, bShared);

	m_bIsOpened = true;

    wxLogVerbose(_("Connect succeeded! Host='%s' dbname='%s' port='%s' user='%s'"), m_sAddres.c_str(), m_sDBName.c_str(), m_sPort.c_str(), m_sName.c_str());

	return true;
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
    for (size_t i = 0; i < sResultName.size(); ++i)
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

wxGISPostgresFeatureDataset::wxGISPostgresFeatureDataset(const CPLString &sPath, OGRLayer* poLayer, OGRDataSource* poDS) : wxGISFeatureDataset(sPath, enumVecPostGIS, poLayer, poDS)
{
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
        int iLayer = wxNOT_FOUND;
        for (int i = 0; i < m_poDS->GetLayerCount(); ++i)
        {
            OGRLayer* poLayer = m_poDS->GetLayer(i);
            if (NULL != poLayer && wxGISEQUAL(m_poLayer->GetName(), poLayer->GetName()))
            {
                iLayer = i;
                break;
            }
        }

        if (iLayer == wxNOT_FOUND)
        {
            if(pTrackCancel)
            {
                wxString sErr = wxString::Format(_("Operation '%s' failed! No Layer %s found to delete"), _("Delete"), wxString(m_poLayer->GetName(), wxConvUTF8).c_str());
                pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
            }
            return false;
        }

        OGRErr eErr = m_poDS->DeleteLayer(iLayer);
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

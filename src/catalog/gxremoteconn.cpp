/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Remote Connection classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013,2014 Dmitry Baryshnikov
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

#include "wxgis/catalog/gxremoteconn.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/core/json/jsonreader.h"
#include "wxgis/core/crypt.h"
#include "wxgis/core/app.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/catalog/gxpostgisdataset.h"
#include "wxgis/datasource/postgisdataset.h"
#include "wxgis/catalog/gxdbconnfactory.h"


//--------------------------------------------------------------
//class wxGxRemoteConnection
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxRemoteConnection, wxGxObjectContainerUpdater)

wxGxRemoteConnection::wxGxRemoteConnection(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainerUpdater(oParent, soName, soPath)
{
    m_pwxGISDataset = NULL;
}

wxGxRemoteConnection::~wxGxRemoteConnection(void)
{
    wsDELETE(m_pwxGISDataset);
}

wxGISDataset* const wxGxRemoteConnection::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        wxGISPostgresDataSource* pDSet = new wxGISPostgresDataSource(m_sPath);
        m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);
}

bool wxGxRemoteConnection::Delete(void)
{
	wxGISDataset* pDSet = GetDatasetFast();

    if (NULL == pDSet)
    {
        return false;
    }

    bool bRet = pDSet->Delete();
    wsDELETE(pDSet);

    if( !bRet )
    {
		wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Delete"));
		sErr += wxT("\n") + wxString::Format(wxT("%s '%s'"), GetCategory().c_str(), wxString::FromUTF8(m_sPath));
		wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, NULL);
		return false;
    }
    return true;
}

bool wxGxRemoteConnection::Rename(const wxString &sNewName)
{
    CPLString szDirPath = CPLGetPath(m_sPath);
    CPLString szName = CPLGetBasename(m_sPath);
    CPLString szNewName(ClearExt(sNewName).mb_str(wxConvUTF8));
    CPLString szNewPath(CPLFormFilename(szDirPath, szNewName, GetExtension(m_sPath, szName)));


    if (!RenameFile(m_sPath, szNewPath))
	{
		const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Rename"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
	}
    else
    {
        m_sPath = szNewPath;
        m_sName = sNewName;
        //change event
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
    }
    return true;
}


bool wxGxRemoteConnection::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInformation);

	wxGISDataset* pDSet = GetDatasetFast();

    if(NULL == pDSet)
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("%s %s %s failed!"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageError);
        }
        return false;
    }

    bool bRet = pDSet->Copy(szDestPath, pTrackCancel);
    wsDELETE(pDSet);

    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Copy"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		wxLogError(sErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageError);
		return false;
    }

    return true;
}

bool wxGxRemoteConnection::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if (pTrackCancel)
    {
		pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInformation);
    }

	wxGISDataset* pDSet = GetDatasetFast();

    if(NULL == pDSet)
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("%s %s %s failed!"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageError);
        }
        return false;
    }

    Disconnect();
    bool bRet = pDSet->Move(szDestPath, pTrackCancel);
    wsDELETE(pDSet);

    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Move"), GetCategory().c_str(), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		wxLogError(sErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageError);
		return false;
    }

    return true;
}

bool wxGxRemoteConnection::Connect(void)
{
    if (IsConnected())
    {
        return true;
    }
    bool bRes = true;
    wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    if(NULL != pDSet)
    {
        bRes = pDSet->Open();
        if (!bRes)
        {
            wsDELETE(pDSet);
            return bRes;
        }

        LoadChildren();

        wxGIS_GXCATALOG_EVENT(ObjectChanged);
    }
    wsDELETE(pDSet);

    CreateAndRunThread();

    return bRes;
}

bool wxGxRemoteConnection::Disconnect(void)
{
    if (!IsConnected())
    {
        return true;
    }

    DestroyThreadSync();
		
    wxGISDataset* pDSet = GetDatasetFast();
    if(NULL != pDSet)
    {
        pDSet->Close();
        DestroyChildren();
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
    }
    wsDELETE(pDSet);
    m_bChildrenLoaded = false;

    return true;
}

bool wxGxRemoteConnection::IsConnected()
{
    wxGISDataset* pDSet = GetDatasetFast();
    bool bRet =  NULL != pDSet && pDSet->IsOpened();
    wsDELETE(pDSet);
    return bRet;
}

void wxGxRemoteConnection::Refresh(void)
{
    DestroyChildren();
    LoadChildren();
    wxGxObject::Refresh();
}

wxGxObjectMap wxGxRemoteConnection::GetRemoteObjects()
{
	wxGxObjectMap ret;
	wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    if (NULL == pDSet)
    {
        return ret;
    }
	
	//SELECT * FROM pg_catalog.pg_tables WHERE schemaname NOT LIKE 'pg_%' AND schemaname NOT LIKE 'information_schema' AND schemaname NOT LIKE 'layer'
    //SELECT table_schema, table_name FROM information_schema.tables WHERE table_schema NOT LIKE 'pg_%' AND table_schema NOT LIKE 'information_schema'
    //SELECT table_schema, table_name FROM information_schema.tables WHERE table_schema NOT IN ('pg_catalog', 'information_schema')"), wxT("PG"))

	wxGISTableCached* pInfoSchema = wxDynamicCast(pDSet->ExecuteSQL2(wxT("SELECT nspname,oid FROM pg_catalog.pg_namespace WHERE nspname NOT IN ('information_schema')"), wxT("PG")), wxGISTableCached);

    if (NULL == pInfoSchema)
	{
		wsDELETE(pDSet);
        return ret;
    }
	
	bool bLoadSystemTablesAndSchemas = false;
    wxGxCatalog* pGxCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
    if (pGxCatalog)
    {
        wxGxDBConnectionFactory* const pDBConnectionFactory = wxDynamicCast(pGxCatalog->GetObjectFactoryByName(_("DataBase connections")), wxGxDBConnectionFactory);
        if (pDBConnectionFactory)
        {
            bLoadSystemTablesAndSchemas = pDBConnectionFactory->GetLoadSystemTablesAndShemes();
        }
    }

    wxFeatureCursor Cursor = pInfoSchema->Search();
    wxGISFeature Feature;
    while ((Feature = Cursor.Next()).IsOk())
    {
        wxString sSchema = Feature.GetFieldAsString(0);
        int nOID = Feature.GetFieldAsInteger(1);
        if (!bLoadSystemTablesAndSchemas)
        {
            if (sSchema.IsSameAs(wxT("topology")))//TODO: add more schemas or get from config
            {
                continue;
            }
            if (sSchema.StartsWith(wxT("pg_")))
            {
                continue;
            }
        }
        ret[nOID] = sSchema;
    }

	
	wsDELETE(pInfoSchema);
	wsDELETE(pDSet);
	
    return ret;	
}

void wxGxRemoteConnection::LoadChildren(void)
{
    wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    if (NULL == pDSet)
    {
        return;
    }
	
    m_smObjects = GetRemoteObjects();

    for (wxGxObjectMap::const_iterator it = m_smObjects.begin(); it != m_smObjects.end(); ++it)
	{
		CPLString szPath(CPLFormFilename(GetPath(), it->second.ToUTF8(), ""));
		GetNewRemoteDBSchema(it->first, it->second, szPath, pDSet);
	}
		
	m_bChildrenLoaded = true;
	
    wsDELETE(pDSet);
}


wxGxRemoteDBSchema* wxGxRemoteConnection::GetNewRemoteDBSchema(int nRemoteId, const wxString &sName, const CPLString &soPath, wxGISPostgresDataSource *pwxGISRemoteConn)
{
    return new wxGxRemoteDBSchema(nRemoteId, pwxGISRemoteConn, this, sName, soPath);
}

void wxGxRemoteConnection::AddObject(int nRemoteId, const wxString &sName)
{
	if(NULL != GetChildByRemoteId(nRemoteId))
		return;
	
	wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
	CPLString szPath(CPLFormFilename(GetPath(), sName.ToUTF8(), ""));
	wxGxRemoteDBSchema* pObj = GetNewRemoteDBSchema(nRemoteId, sName, szPath, pDSet);
	if(NULL != pObj)
		wxGIS_GXCATALOG_EVENT_ID(ObjectAdded, pObj->GetId());
}

bool wxGxRemoteConnection::CanCreate(long nDataType, long DataSubtype)
{
    if (nDataType != enumGISContainer)
        return false;
    if (DataSubtype != enumContGDBFolder)
        return false;
    return true;
}

bool wxGxRemoteConnection::CreateSchema(const wxString& sSchemaName)
{
    wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    if (NULL == pDSet)
    {
        return false;
    }

    if(pDSet->CreateSchema(sSchemaName))
	{
		OnGetUpdates();
		return true;
	}
	return false;
}

wxString wxGxRemoteConnection::CheckUniqSchemaName(const wxString& sSchemaName, const wxString& sAdd, int nCounter) const
{
    wxString sResultName;
    if (nCounter > 0)
    {
        sResultName = sSchemaName + wxString::Format(wxT("%s(%d)"), sAdd.c_str(), nCounter);
    }
    else
    {
        sResultName = sSchemaName;
    }

    for (wxGxObjectMap::const_iterator it = m_smObjects.begin(); it != m_smObjects.end(); ++it)
    {
        if (it->second == sResultName)
        {
            return CheckUniqSchemaName(sSchemaName, sAdd, nCounter + 1);
        }
    }
    return sResultName;
}


//------------------------------------------------------------------------------
// wxGxRemoteDBSchema
//------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxRemoteDBSchema, wxGxObjectContainerUpdater)

wxGxRemoteDBSchema::wxGxRemoteDBSchema(int nRemoteId, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainerUpdater(oParent, soName, soPath), wxGxRemoteId(nRemoteId)
{
    wsSET(m_pwxGISRemoteConn, pwxGISRemoteConn);
}

wxGxRemoteDBSchema::~wxGxRemoteDBSchema(void)
{
    wsDELETE(m_pwxGISRemoteConn);
}

bool wxGxRemoteDBSchema::HasChildren(void)
{
    LoadChildren();

    CreateAndRunThread();

    return wxGxObjectContainer::HasChildren();
}

void wxGxRemoteDBSchema::Refresh(void)
{
    DestroyChildren();
    m_bChildrenLoaded = false;
    LoadChildren();
    wxGxObject::Refresh();
}

bool wxGxRemoteDBSchema::CanCreate(long nDataType, long DataSubtype)
{
    if (nDataType == enumGISFeatureDataset)
    {
        return DataSubtype == enumVecPostGIS;
    }
    if (nDataType == enumGISTable)
    {
        return DataSubtype == enumTablePostgres;
    }
    if (nDataType == enumGISRasterDataset)
    {
        return false;
    }
    return false;
}

bool wxGxRemoteDBSchema::CanDelete(void)
{
    //TODO: check permissions
    return m_pwxGISRemoteConn != NULL;
}

bool wxGxRemoteDBSchema::CanRename(void)
{
    //TODO: check permissions
    return m_pwxGISRemoteConn != NULL;
}

bool wxGxRemoteDBSchema::Delete(void)
{
    if( m_pwxGISRemoteConn->DeleteSchema(m_sName) )
	{
		IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
		if(pNotify)
		{
			pNotify->OnGetUpdates();
		}
		return true;
	}
	return false;
}

bool wxGxRemoteDBSchema::Rename(const wxString &sNewName)
{
    if( m_pwxGISRemoteConn->RenameSchema(m_sName, sNewName) )
	{
		IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
		if(pNotify)
		{
			pNotify->OnGetUpdates();
		}
		return true;
	}
	return false;
}

bool wxGxRemoteDBSchema::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGxRemoteDBSchema::CanCopy(const CPLString &szDestPath)
{
    return false;
}

bool wxGxRemoteDBSchema::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGxRemoteDBSchema::CanMove(const CPLString &szDestPath)
{
    return false;
}

wxGxObjectMap wxGxRemoteDBSchema::GetRemoteObjects()
{
    wxGxObjectMap ret;
	if(NULL == m_pwxGISRemoteConn)
		return ret;
	
    bool bLoadSystemTablesAndSchemas = false;
    wxGxCatalog* pGxCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
    if (pGxCatalog)
    {
        wxGxDBConnectionFactory* const pDBConnectionFactory = wxDynamicCast(pGxCatalog->GetObjectFactoryByName(_("DataBase connections")), wxGxDBConnectionFactory);
        if (pDBConnectionFactory)
        {
            bLoadSystemTablesAndSchemas = pDBConnectionFactory->GetLoadSystemTablesAndShemes();
        }
    }

    //get all tables list
	//SELECT table_name, table_name::regclass::oid AS oid FROM information_schema.tables WHERE table_schema LIKE 'public';
    //SELECT table_name FROM information_schema.tables WHERE table_schema LIKE 'public';
	//select relname,relfilenode,oid from pg_class where relnamespace = 40910 AND (relkind = 'r'::"char" OR relkind = 'v'::"char" OR relkind = 'm'::"char" OR relkind = 'f'::"char");
	wxString sSql = wxString::Format(wxT("SELECT relname,oid FROM pg_class WHERE relnamespace = %d AND (relkind = 'r'::\"char\" OR relkind = 'v'::\"char\" OR relkind = 'm'::\"char\" OR relkind = 'f'::\"char\")"), GetRemoteId());
    wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL2(sSql, wxT("PG")), wxGISTableCached);
    if (NULL != pTableList)
    {
        wxFeatureCursor Cursor = pTableList->Search();
        wxGISFeature Feature;
		bool bIsPublic = m_sName.IsSameAs(wxT("public"));
        while ((Feature = Cursor.Next()).IsOk())
        {
            wxString sTable = Feature.GetFieldAsString(0);
			int nOID = Feature.GetFieldAsInteger(1);
            //check system tables
            if (bIsPublic && !bLoadSystemTablesAndSchemas)
            {
                if (sTable.IsSameAs(wxT("geometry_columns")))
                    continue;
                else if (sTable.IsSameAs(wxT("geography_columns")))
                    continue;
                else if (sTable.IsSameAs(wxT("raster_columns")))
                    continue;
                else if (sTable.IsSameAs(wxT("spatial_ref_sys")))
                    continue;
                else if (sTable.IsSameAs(wxT("raster_overviews")))
                    continue;
            }
            ret[nOID] = sTable;
        }
        
    }
	wsDELETE(pTableList);
	
    return ret;
}

void wxGxRemoteDBSchema::LoadChildren(void)
{
    if(m_bChildrenLoaded)
        return;
    wxCHECK_RET(m_pwxGISRemoteConn, wxT("wxGISRemoteConnection pointer is NULL"));

    m_smObjects = GetRemoteObjects();
	if (m_smObjects.empty())
        return;
		
    wxArrayInt saLoaded;

    //get geometry, geography and rasters
	wxString sSql =  wxString::Format(wxT("SELECT relname,c.oid,t.typname FROM pg_class c, pg_attribute a, pg_type t WHERE c.relnamespace = %d AND (t.typname = 'geometry'::name or t.typname = 'geography'::name or t.typname = 'raster'::name) AND a.attisdropped = false AND a.atttypid = t.oid AND a.attrelid = c.oid AND (relkind = 'r'::\"char\" OR relkind = 'v'::\"char\" OR relkind = 'm'::\"char\" OR relkind = 'f'::\"char\")"), GetRemoteId());
	wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL2(sSql, wxT("PG")), wxGISTableCached);
	
    if(NULL != pTableList)
	{
		wxFeatureCursor Cursor = pTableList->Search();
		wxGISFeature Feature;
		while( (Feature = Cursor.Next()).IsOk() )
		{
			wxString sTable = Feature.GetFieldAsString(0);
			int nOID = Feature.GetFieldAsInteger(1);
			wxString sType = Feature.GetFieldAsString(2);			
			
			if(saLoaded.Index(nOID) == wxNOT_FOUND && !m_smObjects[nOID].IsEmpty())
			{ 
				if(sType.IsSameAs(wxT("geometry")) || sType.IsSameAs(wxT("geography")))
				{
					GetNewTable(nOID, sTable, enumGISFeatureDataset);
					saLoaded.Add(nOID);					
				}	
				else if(sType.IsSameAs(wxT("raster")))
				{
					GetNewTable(nOID, sTable, enumGISRasterDataset);
					saLoaded.Add(nOID);						
				}
			}
		}
		wsDELETE( pTableList );
	}    

    m_bChildrenLoaded = true;

	//add non spatial tables
    for (wxGxObjectMap::iterator it = m_smObjects.begin(); it != m_smObjects.end(); ++it)
	{
		if(saLoaded.Index(it->first) == wxNOT_FOUND)
		{
			GetNewTable(it->first, it->second, enumGISTable);
		}
	}
}

void wxGxRemoteDBSchema::AddObject(int nRemoteId, const wxString &sName)
{
	wxCHECK_RET(m_pwxGISRemoteConn, wxT("wxGISRemoteConnection pointer is NULL"));
	//get type from oid
	wxString sSql = wxString::Format(wxT("SELECT t.typname FROM pg_class c, pg_attribute a, pg_type t WHERE c.relnamespace = %d AND c.oid = %d AND (t.typname = 'geometry'::name or t.typname = 'geography'::name or t.typname = 'raster'::name) AND a.attisdropped = false AND a.atttypid = t.oid AND a.attrelid = c.oid"), GetRemoteId(), nRemoteId);
	wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL2(sSql, wxT("PG")), wxGISTableCached);
	wxGISEnumDatasetType eType = enumGISTable;
    if(NULL != pTableList)
	{
		wxFeatureCursor Cursor = pTableList->Search(wxGISNullQueryFilter, true);
		wxGISFeature Feature = Cursor.Next();
		if(Feature.IsOk())
		{
			wxString sType = Feature.GetFieldAsString(0);
			if(sType.IsSameAs(wxT("geometry")) || sType.IsSameAs(wxT("geography")))
				eType = enumGISFeatureDataset;
			else if(sType.IsSameAs(wxT("raster")))	
				eType = enumGISRasterDataset;
		}
		
		wsDELETE( pTableList );
	} 
	
	wxGxObject* pObj = GetNewTable(nRemoteId, sName, eType);
	if(NULL != pObj)
		wxGIS_GXCATALOG_EVENT_ID(ObjectAdded, pObj->GetId());
}

wxGxObject* wxGxRemoteDBSchema::GetNewTable(int nRemoteId, const wxString &sTableName, const wxGISEnumDatasetType eType)
{
    if (sTableName.IsEmpty())
        return NULL;

    CPLString szPath(CPLFormFilename(GetPath(), sTableName.mb_str(wxConvUTF8), ""));

    switch(eType)
    {
    case enumGISFeatureDataset:
        return new wxGxPostGISFeatureDataset(nRemoteId, GetName(), m_pwxGISRemoteConn, this, sTableName, szPath);
    case enumGISRasterDataset:
        return NULL;
    case enumGISTable:
    default:
        return new wxGxPostGISTable(nRemoteId, GetName(), m_pwxGISRemoteConn, this, sTableName, szPath);
    };

    return NULL;

    /*
//TODO: check inherits
            CPLString osCommand;
            osCommand.Printf("SELECT pg_class.relname FROM pg_class WHERE oid = "
                                "(SELECT pg_inherits.inhparent FROM pg_inherits WHERE inhrelid = "
                                "(SELECT c.oid FROM pg_class c, pg_namespace n WHERE c.relname = '%s' AND c.relnamespace=n.oid AND n.nspname = '%s'))",
                                sTableName.c_str(), sTableSchema.c_str() );
            wxString sSQLExpression(osCommand, wxConvUTF8);
            wxGISTableSPtr pInherits = boost::dynamic_pointer_cast<wxGISTable>(m_pwxGISRemoteConn->ExecuteSQL(sSQLExpression));
            if(pInherits)
            {
                OGRFeatureSPtr pFeature;
                while((pFeature = pInherits->Next()) != NULL)
                {
                    CPLString soParentName = pFeature->GetFieldAsString(0);
                    for(size_t j = 0; j < aDBStruct.size(); ++j)
                    {
                        if(aDBStruct[j].sTableName == soParentName && aDBStruct[j].sTableSchema == sTableSchema)
                        {
                            PGTABLEDATA data = {true, sTableName, sTableSchema};
                            aDBStructOut.push_back(data);
                            bAdd = true;
                            break;
                        }
                    }
                }
            }
        }*/
}

wxString wxGxRemoteDBSchema::CheckUniqTableName(const wxString& sTableName, const wxString& sAdd, int nCounter) const
{
    wxString sResultName;
    if (nCounter > 0)
    {
        sResultName = sTableName + wxString::Format(wxT("%s%d"), sAdd.c_str(), nCounter);
    }
    else
    {
        sResultName = sTableName;
    }

    //make PG compatible
    sResultName = wxGISPostgresDataSource::NormalizeTableName(sResultName);

	for (wxGxObjectMap::const_iterator it = m_smObjects.begin(); it != m_smObjects.end(); ++it)
	{
		if(it->second.IsSameAs(sResultName, false))
		{
			return CheckUniqTableName(sTableName, sAdd, nCounter + 1);
		}
	}
	return sResultName;
}

#endif //wxGIS_USE_POSTGRES

//------------------------------------------------------------------------------
// wxGxTMSWebService
//------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxTMSWebService, wxGxRasterDataset)

wxGxTMSWebService::wxGxTMSWebService(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxRasterDataset(enumRasterWMSTMS, oParent, soName, soPath)
{
}

wxGxTMSWebService::~wxGxTMSWebService(void)
{
}

void wxGxTMSWebService::FillMetadata(bool bForce)
{
    if(m_bIsMetadataFilled && !bForce)
        return;
    m_bIsMetadataFilled = true;

    VSIStatBufL BufL;
    wxULongLong nSize(0);
    wxDateTime dt;
    int ret = VSIStatL(m_sPath, &BufL);
    if(ret == 0)
    {
        m_nSize += BufL.st_size;
        m_dtMod = wxDateTime(BufL.st_mtime);
    }
}


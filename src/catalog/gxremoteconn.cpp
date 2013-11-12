/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Remote Connection classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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

#include "wxgis/catalog/gxremoteconn.h"

#ifdef wxGIS_USE_POSTGRES

#include "wxgis/datasource/sysop.h"
#include "wxgis/catalog/gxpostgisdataset.h"
#include "wxgis/datasource/postgisdataset.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/catalog/gxdbconnfactory.h"

//--------------------------------------------------------------
//class wxGxRemoteConnection
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxRemoteConnection, wxGxObjectContainer)

BEGIN_EVENT_TABLE(wxGxRemoteConnection, wxGxObjectContainer)
EVT_TIMER(TIMER_ID, wxGxRemoteConnection::OnTimer)
END_EVENT_TABLE()


wxGxRemoteConnection::wxGxRemoteConnection(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath), m_timer(this, TIMER_ID)
{
    m_pwxGISDataset = NULL;
    m_bHasGeom = m_bHasGeog = m_bHasRaster = false;
}

wxGxRemoteConnection::~wxGxRemoteConnection(void)
{
    m_timer.Stop();
    Disconnect();
    wsDELETE(m_pwxGISDataset);
}

//bool wxGxRemoteConnection::HasChildren()
//{
//    //if(!Connect())
//    //    return false;
//    //else
//        return wxGxObjectContainer::HasChildren();
//}

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
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Delete"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;	
    }
    return true;
}

bool wxGxRemoteConnection::Rename(const wxString &sNewName)
{
	wxGISDataset* pDSet = GetDatasetFast();
    
    if (NULL == pDSet)
    {
        return false;
    }

    if (pDSet->IsOpened())
    {
		pDSet->Close();
    }

    bool bRet = pDSet->Rename(sNewName);
    wsDELETE(pDSet);

	if( !bRet )
	{
		const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Rename"), wxString(err, wxConvUTF8).c_str(), GetCategory().c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
	}	
    return true;
}


bool wxGxRemoteConnection::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if(pTrackCancel)
        pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInfo);

	wxGISDataset* pDSet = GetDatasetFast();
    
    if(NULL == pDSet)
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("%s %s %s failed!"), _("Copy"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageErr);
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
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
		return false;	
    }
	
    return true;
}

bool wxGxRemoteConnection::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    if (pTrackCancel)
    {
		pTrackCancel->PutMessage(wxString::Format(_("%s %s %s"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageInfo);
    }

	wxGISDataset* pDSet = GetDatasetFast();
    
    if(NULL == pDSet)
    {
        if (pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("%s %s %s failed!"), _("Move"), GetCategory().c_str(), m_sName.c_str()), wxNOT_FOUND, enumGISMessageErr);
        }
        return false;
    }

    bool bRet = pDSet->Move(szDestPath, pTrackCancel);
    wsDELETE(pDSet);

    if(!bRet)
    {
        const char* err = CPLGetLastErrorMsg();
        wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s, %s '%s'"), _("Move"), GetCategory().c_str(), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		wxLogError(sErr);
        if(pTrackCancel)
            pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
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

    m_timer.Start(950, false);

    return bRes;
}

bool wxGxRemoteConnection::Disconnect(void)
{
    if (!IsConnected())
    {
        return true;
    }
    m_timer.Stop();
    wxGISDataset* pDSet = GetDatasetFast();
    if(NULL != pDSet)
    {
        pDSet->Close();
        DestroyChildren();
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
    }
    wsDELETE(pDSet);
    m_bHasGeom = m_bHasGeog = m_bHasRaster = false;

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

void wxGxRemoteConnection::LoadChildren(void)
{
    wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    if (NULL == pDSet)
    {
        return;
    }

    wxGISTableCached* pInfoSchema = wxDynamicCast(pDSet->ExecuteSQL(wxT("SELECT nspname FROM pg_catalog.pg_namespace WHERE nspname NOT IN ('information_schema')"), wxT("PG")), wxGISTableCached);

    if (NULL != pInfoSchema)
    {
        m_saSchemas = FillSchemaNames(pInfoSchema);
        wsDELETE(pInfoSchema);

        pInfoSchema = wxDynamicCast(pDSet->ExecuteSQL(wxT("SELECT table_name FROM information_schema.tables WHERE table_schema = 'public'"), wxT("PG")), wxGISTableCached);

        wxFeatureCursor Cursor = pInfoSchema->Search();
        wxGISFeature Feature;
        while ((Feature = Cursor.Next()).IsOk())
        {
            wxString sName = Feature.GetFieldAsString(0);
            if (sName.IsSameAs(wxT("geometry_columns")))
                m_bHasGeom = true;
            else if (sName.IsSameAs(wxT("geography_columns")))
                m_bHasGeog = true;
            else if (sName.IsSameAs(wxT("raster_columns")))
                m_bHasRaster = true;
        }

        for (size_t i = 0; i < m_saSchemas.size(); ++i)
        {
            GetNewRemoteDBSchema(m_saSchemas[i], pDSet);
        }
    }
    wsDELETE(pDSet);
}


wxGxRemoteDBSchema* wxGxRemoteConnection::GetNewRemoteDBSchema(const wxString &sName, wxGISPostgresDataSource *pwxGISRemoteConn)
{
    CPLString szPath(CPLFormFilename(GetPath(), sName.mb_str(wxConvUTF8), ""));
    return new wxGxRemoteDBSchema(m_bHasGeom, m_bHasGeog, m_bHasRaster, pwxGISRemoteConn, this, sName, szPath);
}

void wxGxRemoteConnection::OnTimer(wxTimerEvent & event)
{
    if (!IsConnected())
    {
        return;
    }

    wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    if (NULL == pDSet)
    {
        return;
    }
    //SELECT * FROM pg_catalog.pg_tables WHERE schemaname NOT LIKE 'pg_%' AND schemaname NOT LIKE 'information_schema' AND schemaname NOT LIKE 'layer'

    //previous sql statement
    //SELECT table_schema, table_name FROM information_schema.tables WHERE table_schema NOT LIKE 'pg_%' AND table_schema NOT LIKE 'information_schema'
    //SELECT table_schema, table_name FROM information_schema.tables WHERE table_schema NOT IN ('pg_catalog', 'information_schema')"), wxT("PG"))
    wxGISTableCached* pInfoSchema = wxDynamicCast(pDSet->ExecuteSQL(wxT("SELECT nspname FROM pg_catalog.pg_namespace WHERE nspname NOT IN ('information_schema')"), wxT("PG")), wxGISTableCached);

    if (NULL != pInfoSchema)
    {
        wxArrayString saCurrentSchemas = FillSchemaNames(pInfoSchema);
        wsDELETE(pInfoSchema);

        //delete 
        for (size_t i = 0; i < m_saSchemas.GetCount(); i++)
        {
            if (saCurrentSchemas.Index(m_saSchemas[i]) == wxNOT_FOUND)
            {
                DeleteSchema(m_saSchemas[i]);
                m_saSchemas.RemoveAt(i);
                i--;
            }
        }

        //add new
        for (size_t i = 0; i < saCurrentSchemas.GetCount(); i++)
        {
            if (m_saSchemas.Index(saCurrentSchemas[i]) == wxNOT_FOUND)
            {
                wxGxRemoteDBSchema* pObj = GetNewRemoteDBSchema(saCurrentSchemas[i], pDSet);
                m_saSchemas.Add(saCurrentSchemas[i]);
                //refresh
                wxGIS_GXCATALOG_EVENT_ID(ObjectAdded, pObj->GetId());
            }
        }
    }

    wsDELETE(pDSet);
}

void wxGxRemoteConnection::DeleteSchema(const wxString& sSchemaName)
{
    wxGxObjectList::iterator iter;
    for (iter = m_Children.begin(); iter != m_Children.end(); ++iter)
    {
        wxGxObject *current = *iter;
        if (NULL != current && current->GetName().IsSameAs(sSchemaName))
        {
            DestroyChild(current);
            break;
        }
    }

}

wxArrayString wxGxRemoteConnection::FillSchemaNames(wxGISTableCached* pInfoSchema)
{
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
    wxArrayString saCurrentSchemas;
    wxGISFeature Feature;
    while ((Feature = Cursor.Next()).IsOk())
    {
        wxString sSchema = Feature.GetFieldAsString(0);
        if (!bLoadSystemTablesAndSchemas)
        {
            if (sSchema.IsSameAs(wxT("topology")))//TODO: add more schemas
            {
                continue;
            }
            if (sSchema.StartsWith(wxT("pg_")))
            {
                continue;
            }
        }
        saCurrentSchemas.Add(sSchema);

    }

    return saCurrentSchemas;
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

    return pDSet->CreateSchema(sSchemaName);
}

wxString wxGxRemoteConnection::CheckUniqSchemaName(const wxString& sSchemaName, const wxString& sAdd, int nCounter)
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

    if (m_saSchemas.Index(sResultName) != wxNOT_FOUND)
    {
        return CheckUniqSchemaName(sSchemaName, sAdd, nCounter + 1);
    }
    else
    {
        return sResultName;
    }
}


//--------------------------------------------------------------
//class wxGxRemoteDBSchema
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxRemoteDBSchema, wxGxObjectContainer)

wxGxRemoteDBSchema::wxGxRemoteDBSchema(bool bHasGeom, bool bHasGeog, bool bHasRaster, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath)
{
    wsSET(m_pwxGISRemoteConn, pwxGISRemoteConn);
    m_bChildrenLoaded = false;
    m_bHasGeom = bHasGeom;
    m_bHasGeog = bHasGeog;
    m_bHasRaster = bHasRaster;
}

wxGxRemoteDBSchema::~wxGxRemoteDBSchema(void)
{
    wsDELETE(m_pwxGISRemoteConn);
}

bool wxGxRemoteDBSchema::HasChildren(void)
{
    LoadChildren();
    return wxGxObjectContainer::HasChildren(); 
}

void wxGxRemoteDBSchema::Refresh(void)
{
    DestroyChildren();
    m_bChildrenLoaded = false;
    LoadChildren();
    wxGxObject::Refresh();
}

bool wxGxRemoteDBSchema::CanDelete(void)
{ 
    return m_pwxGISRemoteConn != NULL;
}

bool wxGxRemoteDBSchema::CanRename(void)
{ 
    return m_pwxGISRemoteConn != NULL;
}

bool wxGxRemoteDBSchema::Delete(void)
{
    return m_pwxGISRemoteConn->DeleteSchema(m_sName);
}

bool wxGxRemoteDBSchema::Rename(const wxString &sNewName)
{
    return m_pwxGISRemoteConn->RenameSchema(m_sName, sNewName);
}

void wxGxRemoteDBSchema::LoadChildren(void)
{
/*    wxCriticalSectionLocker locker(m_CritSect);
    if(m_bChildrenLoaded)
        return;
    wxCHECK_RET(m_pwxGISRemoteConn, wxT("wxGISRemoteConnection pointer is NULL"));

    bool bLoadSystemTablesAndSchemas = false;
    wxGxCatalog* pGxCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
    if(pGxCatalog)
    {
        wxGxDBConnectionFactory* const pDBConnectionFactory = wxDynamicCast(pGxCatalog->GetObjectFactoryByName(_("DataBase connections")), wxGxDBConnectionFactory);
        if(pDBConnectionFactory)
        {
            bLoadSystemTablesAndSchemas = pDBConnectionFactory->GetLoadSystemTablesAndShemes();
        }
    }

    //get geometry and geography
    if(m_bHasGeom)
    {
        //remove table name from tables list
        wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL(wxString::Format(wxT("SELECT f_table_name FROM public.geometry_columns WHERE f_table_schema LIKE '%s'"), GetName().c_str()), wxT("PG")), wxGISTableCached);

        if(NULL != pTableList)
        {
            wxFeatureCursor Cursor = pTableList->Search();
            wxGISFeature Feature;
            while( (Feature = Cursor.Next()).IsOk() )
            {
                wxString sTable = Feature.GetFieldAsString(0);
                int nIndex = m_saTables.Index(sTable);
                if(nIndex != wxNOT_FOUND)
                {
                    AddTable(sTable, enumGISFeatureDataset);
                    m_saTables.RemoveAt(nIndex);
                }
            }
            wsDELETE( pTableList );
        }
    }

    m_bChildrenLoaded = true;
    if(m_saTables.IsEmpty())
        return;
    
    if(m_bHasGeog)
    {
        //remove table name from tables list
        wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL(wxString::Format(wxT("SELECT f_table_name FROM public.geography_columns WHERE f_table_schema LIKE '%s'"), GetName().c_str()), wxT("PG")), wxGISTableCached);

        if(NULL != pTableList)
        {
            wxFeatureCursor Cursor = pTableList->Search();
            wxGISFeature Feature;
            while( (Feature = Cursor.Next()).IsOk() )
            {
                wxString sTable = Feature.GetFieldAsString(0);
                int nIndex = m_saTables.Index(sTable);
                if(nIndex != wxNOT_FOUND)
                {
                    AddTable(sTable, enumGISFeatureDataset);
                    m_saTables.RemoveAt(nIndex);
                }
            }
            wsDELETE( pTableList );
        }
    }
        
    if(m_saTables.IsEmpty())
        return;

    if(m_bHasRaster)
    {
        //remove table name from tables list
        wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL(wxString::Format(wxT("SELECT r_table_name FROM puyblic.raster_columns WHERE r_table_schema LIKE '%s'"), GetName().c_str()), wxT("PG")), wxGISTableCached);

        if(NULL != pTableList)
        {
            wxFeatureCursor Cursor = pTableList->Search();
            wxGISFeature Feature;
            while( (Feature = Cursor.Next()).IsOk() )
            {
                wxString sTable = Feature.GetFieldAsString(0);
                int nIndex = m_saTables.Index(sTable);
                if(nIndex != wxNOT_FOUND)
                {
                    AddTable(sTable, enumGISRasterDataset);
                    m_saTables.RemoveAt(nIndex);
                }
            }
            wsDELETE( pTableList );
        }
    }

    
    for(size_t i = 0; i < m_saTables.GetCount(); ++i)
    {
        AddTable(m_saTables[i], enumGISTableDataset);
    }*/
}


void wxGxRemoteDBSchema::AddTable(const wxString &sTableName, const wxGISEnumDatasetType eType)
{
    if (sTableName.IsEmpty())
        return;

    switch(eType)
    {
    case enumGISFeatureDataset:
        m_pwxGISRemoteConn->Reference();
        new wxGxPostGISFeatureDataset(GetName(), m_pwxGISRemoteConn, this, sTableName, "");
        break;
    case enumGISRasterDataset:
        break;
    case enumGISTableDataset:
    default:
        m_pwxGISRemoteConn->Reference();
        new wxGxPostGISTableDataset(GetName(), m_pwxGISRemoteConn, this, sTableName, "");
        break;
    };
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

/*
std::map<wxString, wxArrayString> smSchema;
wxString sName = Feature.GetFieldAsString(1);
if (!bLoadSystemTablesAndSchemes && (sName.IsSameAs("raster_overviews") || sName.IsSameAs("spatial_ref_sys")))
continue;

if (smSchema[sScheme].Index(wxEmptyString) == wxNOT_FOUND)
{
    smSchema[sScheme].Add(wxEmptyString);
}

//loop for postgis special tables only in public
if (sScheme.IsSameAs(wxT("public")))
{
    if (sName.IsSameAs(wxT("geometry_columns")))
        bHasGeom = true;
    else if (sName.IsSameAs(wxT("geography_columns")))
        bHasGeog = true;
    else if (sName.IsSameAs(wxT("raster_columns")))
        bHasRaster = true;
    else
    {
        smSchema[sSchema].Add(sName);
    }
}
else
{
    smSchema[sSchema].Add(sName);
}
    }

    wsDELETE(pInfoSchema);

    for (std::map<wxString, wxArrayString>::const_iterator IT = smSchema.begin(); IT != smSchema.end(); ++IT)
    {
        GetNewRemoteDBSchema(IT->first, IT->second, bHasGeom, bHasGeog, bHasRaster, pDSet);
    }

    wsDELETE(pDSet);
*/

#endif //wxGIS_USE_POSTGRES

//--------------------------------------------------------------
//class wxGxTMSWebService
//--------------------------------------------------------------
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

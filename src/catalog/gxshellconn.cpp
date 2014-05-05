/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Remote Connection classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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
/*
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
    EVT_THREAD(wxID_ANY, wxGxRemoteConnection::OnThreadFinished)
END_EVENT_TABLE()


wxGxRemoteConnection::wxGxRemoteConnection(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath), wxThreadHelper(wxTHREAD_DETACHED)
{
    m_pwxGISDataset = NULL;
    m_bHasGeom = m_bHasGeog = m_bHasRaster = false;
    m_bChildrenLoaded = false;
}

wxGxRemoteConnection::~wxGxRemoteConnection(void)
{
    wsDELETE(m_pwxGISDataset);
}

bool wxGxRemoteConnection::Destroy(void)
{
    Disconnect();
    return wxGxObjectContainer::Destroy();
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


    CreateAndRunThread();

    return bRes;
}

bool wxGxRemoteConnection::Disconnect(void)
{
    if (!IsConnected())
    {
        return true;
    }

    if (GetThread() && GetThread()->IsRunning())
    {
        GetThread()->Delete();// Wait();
    }


    wxGISDataset* pDSet = GetDatasetFast();
    if(NULL != pDSet)
    {
        pDSet->Close();
        DestroyChildren();
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
    }
    wsDELETE(pDSet);
    m_bHasGeom = m_bHasGeog = m_bHasRaster = false;
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

void wxGxRemoteConnection::LoadChildren(void)
{
    wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    if (NULL == pDSet)
    {
        return;
    }

    wxGISTableCached* pInfoSchema = wxDynamicCast(pDSet->ExecuteSQL(wxT("SELECT nspname,oid FROM pg_catalog.pg_namespace WHERE nspname NOT IN ('information_schema')"), wxT("PG")), wxGISTableCached);

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


        for (wxGISDBShemaMap::const_iterator it = m_saSchemas.begin(); it != m_saSchemas.end(); ++it)
        {
            CPLString szPath(CPLFormFilename(GetPath(), it->second.mb_str(wxConvUTF8), ""));
            GetNewRemoteDBSchema(it->second, szPath, pDSet);
        }
        m_bChildrenLoaded = true;
    }
    wsDELETE(pDSet);
}


wxGxRemoteDBSchema* wxGxRemoteConnection::GetNewRemoteDBSchema(const wxString &sName, const CPLString &soPath, wxGISPostgresDataSource *pwxGISRemoteConn)
{
    return new wxGxRemoteDBSchema(m_bHasGeom, m_bHasGeog, m_bHasRaster, pwxGISRemoteConn, this, sName, soPath);
}

bool wxGxRemoteConnection::CreateAndRunThread(void)
{
    if (GetThread() && GetThread()->IsRunning())
        return true;

    if (CreateThread(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not create the thread!"));
        return false;
    }

    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the thread!"));
        return false;
    }
    return true;
}


wxThread::ExitCode wxGxRemoteConnection::CheckChanges()
{
    wxGISPostgresDataSource* pDSet = wxDynamicCast(GetDatasetFast(), wxGISPostgresDataSource);
    if (NULL == pDSet)
    {
        wxThreadEvent event(wxEVT_THREAD, EXIT_EVENT);
        wxQueueEvent(this, event.Clone());
        return (wxThread::ExitCode)wxTHREAD_MISC_ERROR;
    }

    while (!GetThread()->TestDestroy())
    {
        //SELECT * FROM pg_catalog.pg_tables WHERE schemaname NOT LIKE 'pg_%' AND schemaname NOT LIKE 'information_schema' AND schemaname NOT LIKE 'layer'

        //previous sql statement
        //SELECT table_schema, table_name FROM information_schema.tables WHERE table_schema NOT LIKE 'pg_%' AND table_schema NOT LIKE 'information_schema'
        //SELECT table_schema, table_name FROM information_schema.tables WHERE table_schema NOT IN ('pg_catalog', 'information_schema')"), wxT("PG"))
        wxGISTableCached* pInfoSchema = wxDynamicCast(pDSet->ExecuteSQL(wxT("SELECT nspname,oid FROM pg_catalog.pg_namespace WHERE nspname NOT IN ('information_schema')"), wxT("PG")), wxGISTableCached);

        if (NULL != pInfoSchema)
        {
            wxGISDBShemaMap saCurrentSchemas = FillSchemaNames(pInfoSchema);
            wsDELETE(pInfoSchema);

             
            for (wxGISDBShemaMap::iterator it = m_saSchemas.begin(); it != m_saSchemas.end(); ++it)
            {
                wxGISDBShemaMap::iterator cit = saCurrentSchemas.find(it->first);
                if (cit == saCurrentSchemas.end())//delete
                {
                    DeleteSchema(it->second);
                    m_saSchemas.erase(it);
                    it = m_saSchemas.begin();
                }
                else if (cit->second != it->second)//rename
                {
                    RenameSchema(it->second, cit->second);
                    it->second = cit->second;
                }
            }

            //add new
            for (wxGISDBShemaMap::iterator it = saCurrentSchemas.begin(); it != saCurrentSchemas.end(); ++it)
            {
                wxGISDBShemaMap::iterator cit = m_saSchemas.find(it->first);
                if (cit == saCurrentSchemas.end())
                {
                    CPLString szPath(CPLFormFilename(GetPath(), it->second.mb_str(wxConvUTF8), ""));
                    wxGxRemoteDBSchema* pObj = GetNewRemoteDBSchema(it->second, szPath, pDSet);
                    m_saSchemas[it->first] = it->second;
                    //refresh
                    wxGIS_GXCATALOG_EVENT_ID(ObjectAdded, pObj->GetId());
                }
            }
        }


        wxThread::Sleep(950);
    }

    wsDELETE(pDSet);

    return (wxThread::ExitCode)wxTHREAD_NO_ERROR;
}

wxThread::ExitCode wxGxRemoteConnection::Entry()
{
    if (!IsConnected())
    {
        wxThreadEvent event(wxEVT_THREAD, EXIT_EVENT);
        wxQueueEvent(this, event.Clone());
        return (wxThread::ExitCode)wxTHREAD_MISC_ERROR;
    }

    return CheckChanges();
}

void wxGxRemoteConnection::OnThreadFinished(wxThreadEvent& event)
{

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

void wxGxRemoteConnection::RenameSchema(const wxString& sSchemaName, const wxString& sNewSchemaName)
{
    wxGxObjectList::iterator iter;
    for (iter = m_Children.begin(); iter != m_Children.end(); ++iter)
    {
        wxGxObject *current = *iter;
        if (NULL != current && current->GetName().IsSameAs(sSchemaName))
        {
            current->SetName(sNewSchemaName);
            wxGIS_GXCATALOG_EVENT_ID(ObjectChanged, current->GetId());
            break;
        }
    }
}

wxGISDBShemaMap wxGxRemoteConnection::FillSchemaNames(wxGISTableCached* pInfoSchema)
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
    wxGISDBShemaMap saCurrentSchemas;
    wxGISFeature Feature;
    while ((Feature = Cursor.Next()).IsOk())
    {
        wxString sSchema = Feature.GetFieldAsString(0);
        int nOID = Feature.GetFieldAsInteger(1);
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
        saCurrentSchemas[nOID] = sSchema;
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

    for (wxGISDBShemaMap::const_iterator it = m_saSchemas.begin(); it != m_saSchemas.end(); ++it)
    {
        if (it->second == sResultName)
        {
            return CheckUniqSchemaName(sSchemaName, sAdd, nCounter + 1);
        }
    }
    return sResultName;
}


//--------------------------------------------------------------
//class wxGxRemoteDBSchema
//--------------------------------------------------------------

IMPLEMENT_CLASS(wxGxRemoteDBSchema, wxGxObjectContainer)

BEGIN_EVENT_TABLE(wxGxRemoteDBSchema, wxGxObjectContainer)
    EVT_THREAD(wxID_ANY, wxGxRemoteDBSchema::OnThreadFinished)
END_EVENT_TABLE()

wxGxRemoteDBSchema::wxGxRemoteDBSchema(bool bHasGeom, bool bHasGeog, bool bHasRaster, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath), wxThreadHelper(wxTHREAD_DETACHED)
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

bool wxGxRemoteDBSchema::Destroy()
{
    if (GetThread() && GetThread()->IsRunning())
    {
        GetThread()->Delete();// Wait();
    }

    return wxGxObjectContainer::Destroy();
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
        return DataSubtype == emumVecPostGIS;
    }
    if (nDataType == enumGISTableDataset)
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
    return m_pwxGISRemoteConn->DeleteSchema(m_sName);
}

bool wxGxRemoteDBSchema::Rename(const wxString &sNewName)
{
    return m_pwxGISRemoteConn->RenameSchema(m_sName, sNewName);
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

wxArrayString wxGxRemoteDBSchema::FillTableNames()
{
    wxArrayString saTables;
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
    //SELECT table_name FROM information_schema.tables WHERE table_schema LIKE 'public';
    wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL(wxString::Format(wxT("SELECT table_name FROM information_schema.tables WHERE table_schema LIKE '%s'"), GetName().c_str()), wxT("PG")), wxGISTableCached);
    if (NULL != pTableList)
    {
        wxFeatureCursor Cursor = pTableList->Search();
        wxGISFeature Feature;
        while ((Feature = Cursor.Next()).IsOk())
        {
            wxString sTable = Feature.GetFieldAsString(0);
            //check system tables
            if (m_sName.IsSameAs(wxT("public")) && !bLoadSystemTablesAndSchemas)
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
            saTables.Add(sTable);
        }
        wsDELETE(pTableList);
    }
    return saTables;
}

void wxGxRemoteDBSchema::LoadChildren(void)
{
    wxCriticalSectionLocker locker(m_CritSect);
    if(m_bChildrenLoaded)
        return;
    wxCHECK_RET(m_pwxGISRemoteConn, wxT("wxGISRemoteConnection pointer is NULL"));

    m_saTables = FillTableNames();
    wxArrayString saLoaded;

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
                int nIndex2 = saLoaded.Index(sTable);
                if(nIndex != wxNOT_FOUND && nIndex2 == wxNOT_FOUND)
                {
                    AddTable(sTable, enumGISFeatureDataset);
                    saLoaded.Add(sTable);
                }
            }
            wsDELETE( pTableList );
        }
    }

    m_bChildrenLoaded = true;

    if (m_saTables.IsEmpty())
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
                int nIndex2 = saLoaded.Index(sTable);
                if (nIndex != wxNOT_FOUND && nIndex2 == wxNOT_FOUND)
                {
                    AddTable(sTable, enumGISFeatureDataset);
                    saLoaded.Add(sTable);
                }
            }
            wsDELETE( pTableList );
        }
    }
        
    if (m_saTables.IsEmpty())
        return;

    if(m_bHasRaster)
    {
        //remove table name from tables list
        wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL(wxString::Format(wxT("SELECT r_table_name FROM public.raster_columns WHERE r_table_schema LIKE '%s'"), GetName().c_str()), wxT("PG")), wxGISTableCached);

        if(NULL != pTableList)
        {
            wxFeatureCursor Cursor = pTableList->Search();
            wxGISFeature Feature;
            while( (Feature = Cursor.Next()).IsOk() )
            {
                wxString sTable = Feature.GetFieldAsString(0);
                int nIndex = m_saTables.Index(sTable);
                int nIndex2 = saLoaded.Index(sTable);
                if (nIndex != wxNOT_FOUND && nIndex2 == wxNOT_FOUND)
                {
                    AddTable(sTable, enumGISRasterDataset);
                    saLoaded.Add(sTable);
                }
            }
            wsDELETE( pTableList );
        }
    }

    
    for (size_t i = 0; i < m_saTables.GetCount(); ++i)
    {
        int nIndex2 = saLoaded.Index(m_saTables[i]);
        if (nIndex2 == wxNOT_FOUND)
        {
            AddTable(m_saTables[i], enumGISTableDataset);
        }
    }
}

void wxGxRemoteDBSchema::CheckChanges()
{
    wxArrayString saCurrentTables = FillTableNames();

    //delete 
    for (size_t i = 0; i < m_saTables.GetCount(); ++i)
    {
        if (saCurrentTables.Index(m_saTables[i]) == wxNOT_FOUND)
        {
            DeleteTable(m_saTables[i]);
            m_saTables.RemoveAt(i);
            i--;
        }
    }

    //get geometry and geography
    if (m_bHasGeom)
    {
        //remove table name from tables list
        wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL(wxString::Format(wxT("SELECT f_table_name FROM public.geometry_columns WHERE f_table_schema LIKE '%s'"), GetName().c_str()), wxT("PG")), wxGISTableCached);

        if (NULL != pTableList)
        {
            wxFeatureCursor Cursor = pTableList->Search();
            wxGISFeature Feature;
            while ((Feature = Cursor.Next()).IsOk())
            {
                wxString sTable = Feature.GetFieldAsString(0);
                int nIndex = m_saTables.Index(sTable);
                int nCurrentIndex = saCurrentTables.Index(sTable);
                if (nIndex != wxNOT_FOUND)
                {
                    if (nCurrentIndex != wxNOT_FOUND)
                        saCurrentTables.RemoveAt(nCurrentIndex);
                    continue;
                }
                if (nCurrentIndex != wxNOT_FOUND)
                {
                    wxGxObject* pObj = AddTable(sTable, enumGISFeatureDataset);
                    {
                        saCurrentTables.RemoveAt(nCurrentIndex);
                        m_saTables.Add(sTable);
                        //refresh
                        wxGIS_GXCATALOG_EVENT_ID(ObjectAdded, pObj->GetId());
                    }
                }
            }
            wsDELETE(pTableList);
        }
    }

    if (saCurrentTables.IsEmpty())
        return;

    if (m_bHasGeog)
    {
        //remove table name from tables list
        wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL(wxString::Format(wxT("SELECT f_table_name FROM public.geography_columns WHERE f_table_schema LIKE '%s'"), GetName().c_str()), wxT("PG")), wxGISTableCached);

        if (NULL != pTableList)
        {
            wxFeatureCursor Cursor = pTableList->Search();
            wxGISFeature Feature;
            while ((Feature = Cursor.Next()).IsOk())
            {
                wxString sTable = Feature.GetFieldAsString(0);
                int nIndex = m_saTables.Index(sTable);
                int nCurrentIndex = saCurrentTables.Index(sTable);
                if (nIndex != wxNOT_FOUND)
                {
                    if (nCurrentIndex != wxNOT_FOUND)
                        saCurrentTables.RemoveAt(nCurrentIndex);
                    continue;
                }
                if (nCurrentIndex != wxNOT_FOUND)
                {
                    wxGxObject* pObj = AddTable(sTable, enumGISFeatureDataset);
                    {
                        saCurrentTables.RemoveAt(nCurrentIndex);
                        m_saTables.Add(sTable);
                        //refresh
                        wxGIS_GXCATALOG_EVENT_ID(ObjectAdded, pObj->GetId());
                    }
                }
            }
            wsDELETE(pTableList);
        }
    }

    if (saCurrentTables.IsEmpty())
        return;

    if (m_bHasRaster)
    {
        //remove table name from tables list
        wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL(wxString::Format(wxT("SELECT r_table_name FROM public.raster_columns WHERE r_table_schema LIKE '%s'"), GetName().c_str()), wxT("PG")), wxGISTableCached);

        if (NULL != pTableList)
        {
            wxFeatureCursor Cursor = pTableList->Search();
            wxGISFeature Feature;
            while ((Feature = Cursor.Next()).IsOk())
            {
                wxString sTable = Feature.GetFieldAsString(0);
                int nIndex = m_saTables.Index(sTable);
                int nCurrentIndex = saCurrentTables.Index(sTable);
                if (nIndex != wxNOT_FOUND)
                {
                    if (nCurrentIndex != wxNOT_FOUND)
                        saCurrentTables.RemoveAt(nCurrentIndex);
                    continue;
                }
                if (nCurrentIndex != wxNOT_FOUND)
                {
                    wxGxObject* pObj = AddTable(sTable, enumGISRasterDataset);
                    {
                        saCurrentTables.RemoveAt(nCurrentIndex);
                        m_saTables.Add(sTable);
                        //refresh
                        wxGIS_GXCATALOG_EVENT_ID(ObjectAdded, pObj->GetId());
                    }

                }
            }
            wsDELETE(pTableList);
        }
    }

    //add new
    for (size_t i = 0; i < saCurrentTables.GetCount(); i++)
    {
        if (m_saTables.Index(saCurrentTables[i]) == wxNOT_FOUND)
        {
            wxGxObject* pObj = AddTable(saCurrentTables[i], enumGISTableDataset);
            if (NULL != pObj)
            {
                m_saTables.Add(saCurrentTables[i]);
                //refresh
                wxGIS_GXCATALOG_EVENT_ID(ObjectAdded, pObj->GetId());
            }
        }
    }
}

bool wxGxRemoteDBSchema::CreateAndRunThread(void)
{
    if (!GetThread())
    {
        if (CreateThread(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)
        {
            wxLogError(_("Could not create the thread!"));
            return false;
        }
    }

    if (GetThread()->IsRunning())
        return true;

    if (GetThread()->Run() != wxTHREAD_NO_ERROR)
    {
        wxLogError(_("Could not run the thread!"));
        return false;
    }

    return true;
}

void wxGxRemoteDBSchema::OnThreadFinished(wxThreadEvent& event)
{

}


wxThread::ExitCode wxGxRemoteDBSchema::Entry()
{
    while (!GetThread()->TestDestroy())
    {
        CheckChanges();

        wxThread::Sleep(950);
    }

    return (wxThread::ExitCode)wxTHREAD_NO_ERROR;
}

void wxGxRemoteDBSchema::DeleteTable(const wxString& sSchemaName)
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


wxGxObject* wxGxRemoteDBSchema::AddTable(const wxString &sTableName, const wxGISEnumDatasetType eType)
{
    if (sTableName.IsEmpty())
        return NULL;

    CPLString szPath(CPLFormFilename(GetPath(), sTableName.mb_str(wxConvUTF8), ""));


    switch(eType)
    {
    case enumGISFeatureDataset:
        return new wxGxPostGISFeatureDataset(GetName(), m_pwxGISRemoteConn, this, sTableName, szPath);
    case enumGISRasterDataset:
        return NULL;
    case enumGISTableDataset:
    default:
        return new wxGxPostGISTableDataset(GetName(), m_pwxGISRemoteConn, this, sTableName, szPath);
    };

    return NULL;


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

    if (m_saTables.Index(sResultName) != wxNOT_FOUND)
    {
        return CheckUniqTableName(sTableName, sAdd, nCounter + 1);
    }
    else
    {
        return sResultName;
    }
}

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
*/
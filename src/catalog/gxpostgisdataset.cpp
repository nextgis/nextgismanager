/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxPostGISDataset classes.
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

#include "wxgis/catalog/gxpostgisdataset.h"

#ifdef wxGIS_USE_POSTGRES
//-----------------------------------------------------------------------------------
// wxGISPostGISBaseTable
//-----------------------------------------------------------------------------------
wxGISPostGISBaseTable::wxGISPostGISBaseTable(int nRemoteId, const wxString &sSchema, const wxString &soName, wxGISPostgresDataSource* pwxGISRemoteConn) : wxGxRemoteId(nRemoteId)
{
    wsSET(m_pwxGISRemoteConn, pwxGISRemoteConn);
    m_sFullyQualifiedName = wxT("\"") + sSchema + wxT("\".\"") + soName + wxT("\"");
    m_sSchemaName = sSchema;
	m_sTableName =	soName;
}

wxGISPostGISBaseTable::~wxGISPostGISBaseTable(void)
{
	wsDELETE(m_pwxGISRemoteConn);
}

wxULongLong wxGISPostGISBaseTable::GetTableSize()
{
	wxULongLong nSize(0);
	wxString sStatement = wxString::Format(wxT("SELECT pg_total_relation_size('%s'::regclass::oid);"), m_sFullyQualifiedName);
    
    wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL2(sStatement, wxT("PG")), wxGISTableCached);
    if (NULL != pTableList)
    {
        wxFeatureCursor Cursor = pTableList->Search(wxGISNullQueryFilter, true);
        wxGISFeature Feature = Cursor.Next();
        if (Feature.IsOk())
        {
            nSize = Feature.GetFieldAsInteger(0);
        }

        wsDELETE(pTableList);
    }
	return nSize;
}

void wxGISPostGISBaseTable::FillIndixes()
{
	if(!m_msIndexs.empty())
		return;
		
	wxString sStatement = wxString::Format(wxT("SELECT i.relname as index_name, a.attname as column_name FROM pg_namespace n, pg_class t, pg_class i, pg_index ix, pg_attribute a WHERE t.oid = ix.indrelid and i.oid = ix.indexrelid and a.attrelid = t.oid and a.attnum = ANY(ix.indkey) and t.relkind = 'r' and t.relname like '%s' and n.nspname like '%s';"), m_sTableName.c_str(), m_sSchemaName.c_str());
    
    wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL2(sStatement, wxT("PG")), wxGISTableCached);
    if (NULL != pTableList)
    {
        wxFeatureCursor Cursor = pTableList->Search();
        wxGISFeature Feature;
		while( (Feature = Cursor.Next()).IsOk() )
        {
            wxString sIndex = Feature.GetFieldAsString(0);
			wxString sCol = Feature.GetFieldAsString(1);
			m_msIndexs[sCol] = sIndex;
        }

        wsDELETE(pTableList);
    }	
}

//-----------------------------------------------------------------------------------
// wxGxPostGISTable
//-----------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxPostGISTable, wxGxTable)

wxGxPostGISTable::wxGxPostGISTable(int nRemoteId, const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxTable(enumTablePostgres, oParent, soName, soPath), wxGISPostGISBaseTable(nRemoteId, sSchema, soName, pwxGISRemoteConn)
{
}

wxGxPostGISTable::~wxGxPostGISTable(void)
{    
}

wxGISDataset* const wxGxPostGISTable::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        m_pwxGISDataset = m_pwxGISRemoteConn->GetSubset(m_sFullyQualifiedName);
        if (NULL == m_pwxGISDataset)
            return NULL;
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);
}

void wxGxPostGISTable::FillMetadata(bool bForce)
{
    if (m_bIsMetadataFilled && !bForce)
        return;
    m_bIsMetadataFilled = true;
	m_nSize = GetTableSize();
	FillIndixes();
}

bool wxGxPostGISTable::CanRename(void)
{
    //TODO: check permissions
    return m_pwxGISRemoteConn != NULL;
}

bool wxGxPostGISTable::CanCopy(const CPLString &szDestPath)
{
    //TODO: not support yet
    return false;
}

bool wxGxPostGISTable::CanMove(const CPLString &szDestPath)
{
    //TODO: check permissions
    //TODO: not support yet
    return false;
}

bool wxGxPostGISTable::Delete()
{
	if( wxGxTable::Delete() )
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

bool wxGxPostGISTable::Rename(const wxString &sNewName)
{
	wxGISTable* pDst = wxDynamicCast(GetDataset(), wxGISTable);		
	if(NULL == pDst)
		return false;
	FillIndixes();
	wxString sPkIdx = m_msIndexs[pDst->GetFIDColumn()];
	if( m_pwxGISRemoteConn->RenameTable(m_sSchemaName, m_sName, sNewName, sPkIdx) )
	{
		pDst->Close();
		wsDELETE(pDst);
		IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
		if(pNotify)
		{
			pNotify->OnGetUpdates();
		}
		return true;
	}
	wsDELETE(pDst);
	return false;
}

bool wxGxPostGISTable::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGxPostGISTable::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

//-----------------------------------------------------------------------------------
// wxGxPostGISFeatureDataset
//-----------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxPostGISFeatureDataset, wxGxFeatureDataset)

wxGxPostGISFeatureDataset::wxGxPostGISFeatureDataset(int nRemoteId, const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxFeatureDataset(enumVecPostGIS, oParent, soName, soPath), wxGISPostGISBaseTable(nRemoteId, sSchema, soName, pwxGISRemoteConn)
{
}

wxGxPostGISFeatureDataset::~wxGxPostGISFeatureDataset(void)
{
}

wxGISDataset* const wxGxPostGISFeatureDataset::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        m_pwxGISDataset = m_pwxGISRemoteConn->GetSubset(m_sFullyQualifiedName);
        if (NULL == m_pwxGISDataset)
            return NULL;
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);
}

void wxGxPostGISFeatureDataset::FillMetadata(bool bForce)
{
    if (m_bIsMetadataFilled && !bForce)
        return;
    m_bIsMetadataFilled = true;
	m_nSize = GetTableSize();
}


bool wxGxPostGISFeatureDataset::CanRename(void)
{
    //TODO: check permissions
    return m_pwxGISRemoteConn != NULL;
}

bool wxGxPostGISFeatureDataset::CanCopy(const CPLString &szDestPath)
{
    //TODO: not support yet
    return false;
}

bool wxGxPostGISFeatureDataset::CanMove(const CPLString &szDestPath)
{
    //TODO: check permissions
    //TODO: not support yet
    return false;
}

bool wxGxPostGISFeatureDataset::Delete()
{
	if( wxGxFeatureDataset::Delete() )
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

bool wxGxPostGISFeatureDataset::Rename(const wxString &sNewName)
{
	wxGISFeatureDataset* pDst = wxDynamicCast(GetDataset(), wxGISFeatureDataset);	
	if(NULL == pDst)
		return false;
	FillIndixes();
	wxString sPkIdx = m_msIndexs[pDst->GetFIDColumn()];
	wxString sGeomIdx = m_msIndexs[pDst->GetGeometryColumn()];
	
	if( m_pwxGISRemoteConn->RenameTable(m_sSchemaName, m_sName, sNewName, sPkIdx, sGeomIdx) )
	{
		pDst->Close();
		wsDELETE(pDst);
		IGxObjectNotifier *pNotify = dynamic_cast<IGxObjectNotifier*>(m_oParent);
		if(pNotify)
		{
			pNotify->OnGetUpdates();
		}
		return true;
	}
	wsDELETE(pDst);
	return false;
}

bool wxGxPostGISFeatureDataset::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGxPostGISFeatureDataset::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

#endif //wxGIS_USE_POSTGRES

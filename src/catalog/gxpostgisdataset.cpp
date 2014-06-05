/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxPostGISDataset classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Dmitry Baryshnikov
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
// wxGxPostGISTableDataset
//-----------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxPostGISTableDataset, wxGxTableDataset)

wxGxPostGISTableDataset::wxGxPostGISTableDataset(const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxTableDataset(enumTablePostgres, oParent, soName, soPath)
{
    wsSET(m_pwxGISRemoteConn, pwxGISRemoteConn);
    m_sFullyQualifiedName = wxT("\"") + sSchema + wxT("\".\"") + soName + wxT("\"");
    m_sSchemaName = sSchema;
}

wxGxPostGISTableDataset::~wxGxPostGISTableDataset(void)
{
    wsDELETE(m_pwxGISRemoteConn);
}

wxGISDataset* const wxGxPostGISTableDataset::GetDatasetFast(void)
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

void wxGxPostGISTableDataset::FillMetadata(bool bForce)
{
    if (m_bIsMetadataFilled && !bForce)
        return;
    m_bIsMetadataFilled = true;

    wxString sStatement = wxString::Format(wxT("SELECT pg_total_relation_size('%s'::regclass::oid);"), m_sFullyQualifiedName);
    m_pwxGISRemoteConn->ExecuteSQL(sStatement);

    wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL2(sStatement, wxT("PG")), wxGISTableCached);
    if (NULL != pTableList)
    {
        wxFeatureCursor Cursor = pTableList->Search(wxGISNullQueryFilter, true);
        wxGISFeature Feature = Cursor.Next();
        if (Feature.IsOk())
        {
            m_nSize = Feature.GetFieldAsInteger(0);
        }
    }
}

bool wxGxPostGISTableDataset::CanRename(void)
{
    //TODO: check permissions
    return m_pwxGISRemoteConn != NULL;
}

bool wxGxPostGISTableDataset::CanCopy(const CPLString &szDestPath)
{
    //TODO: not support yet
    return false;
}

bool wxGxPostGISTableDataset::CanMove(const CPLString &szDestPath)
{
    //TODO: check permissions
    //TODO: not support yet
    return false;
}

bool wxGxPostGISTableDataset::Rename(const wxString &sNewName)
{
    return m_pwxGISRemoteConn->RenameTable(m_sSchemaName, m_sName, sNewName);
}

bool wxGxPostGISTableDataset::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGxPostGISTableDataset::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

//-----------------------------------------------------------------------------------
// wxGxPostGISFeatureDataset
//-----------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxPostGISFeatureDataset, wxGxFeatureDataset)

//wxGxPostGISFeatureDataset::wxGxPostGISFeatureDataset(CPLString soPath, wxGISDatasetSPtr pwxGISDataset)
//{
//	m_sName = pwxGISDataset->GetName();
//
//	m_pwxGISDataset = pwxGISDataset;
//
////#ifndef PGTEST 
////    m_sPath = soPath + " ";
////    m_sPath += m_sName.mb_str(wxConvUTF8);
////#else
////	//"PG:host='gis-lab.info' dbname='firereporter' port='5432' user='firereporter' password='8QdA4'"
//	m_sPath = "vector:postgres:";
//	m_sPath += m_sName.mb_str(wxConvUTF8);
//	m_sPath += ":";
//	m_sPath += soPath;
//	m_sPath += " sslmode=disable table=";
//	m_sPath += m_sName.mb_str(wxConvUTF8);
////    m_sPath = "vector:postgres:fire:dbname=firereporter host=gis-lab.info port=5432 user=firereporter password=8QdA4 sslmode=disable table=fires";
////#endif
//}

wxGxPostGISFeatureDataset::wxGxPostGISFeatureDataset(const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxFeatureDataset(enumVecPostGIS, oParent, soName, soPath)
{
    wsSET(m_pwxGISRemoteConn, pwxGISRemoteConn);
    m_sFullyQualifiedName = wxT("\"") + sSchema + wxT("\".\"") + soName + wxT("\"");
    m_sSchemaName = sSchema;
}

wxGxPostGISFeatureDataset::~wxGxPostGISFeatureDataset(void)
{
    wsDELETE(m_pwxGISRemoteConn);
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

    wxString sStatement = wxString::Format(wxT("SELECT pg_total_relation_size('%s'::regclass::oid);"), m_sFullyQualifiedName);
    wxGISTableCached* pTableList = wxDynamicCast(m_pwxGISRemoteConn->ExecuteSQL2(sStatement, wxT("PG")), wxGISTableCached);
    if (NULL != pTableList)
    {
        wxFeatureCursor Cursor = pTableList->Search(wxGISNullQueryFilter, true);
        wxGISFeature Feature = Cursor.Next();
        if (Feature.IsOk())
        {
            m_nSize = Feature.GetFieldAsInteger(0);
        }
    }
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

bool wxGxPostGISFeatureDataset::Rename(const wxString &sNewName)
{
    return m_pwxGISRemoteConn->RenameTable(m_sSchemaName, m_sName, sNewName);
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

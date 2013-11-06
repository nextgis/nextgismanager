/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  GxPostGISDataset classes.
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

#include "wxgis/catalog/gxpostgisdataset.h"

#ifdef wxGIS_USE_POSTGRES

//-----------------------------------------------------------------------------------
// wxGxPostGISTableDataset
//-----------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGxPostGISTableDataset, wxGxTableDataset)

wxGxPostGISTableDataset::wxGxPostGISTableDataset(const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxTableDataset(enumTablePostgres, oParent, soName, soPath)
{
    m_pwxGISRemoteConn = pwxGISRemoteConn;
    m_sFullyQualifiedName = sSchema + wxT(".") + soName;
}

wxGxPostGISTableDataset::~wxGxPostGISTableDataset(void)
{
}

wxGISDataset* const wxGxPostGISTableDataset::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        m_pwxGISDataset = m_pwxGISRemoteConn->GetSubset(m_sFullyQualifiedName);
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);
}

void wxGxPostGISTableDataset::FillMetadata(bool bForce)
{
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

wxGxPostGISFeatureDataset::wxGxPostGISFeatureDataset(const wxString &sSchema, wxGISPostgresDataSource* pwxGISRemoteConn, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxFeatureDataset(emumVecPostGIS, oParent, soName, soPath)
{
    m_pwxGISRemoteConn = pwxGISRemoteConn;
    m_sFullyQualifiedName = sSchema + wxT(".") + soName;
}

wxGxPostGISFeatureDataset::~wxGxPostGISFeatureDataset(void)
{
}

wxGISDataset* const wxGxPostGISFeatureDataset::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        m_pwxGISDataset = m_pwxGISRemoteConn->GetSubset(m_sFullyQualifiedName);
        m_pwxGISDataset->Reference();
    }
    wsGET(m_pwxGISDataset);
}

void wxGxPostGISFeatureDataset::FillMetadata(bool bForce)
{
}

#endif //wxGIS_USE_POSTGRES

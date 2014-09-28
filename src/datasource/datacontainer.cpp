/******************************************************************************
* Project:  wxGIS
* Purpose:  datacontainer Folder, GeoDatabase & etc.
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
*   Copyright (C) 2014 NextGIS
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

#include "wxgis/datasource/datacontainer.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/core/app.h"

//-----------------------------------------------------------------------------
// wxGISDataSource
//-----------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISDataSource, wxGISDataset)

wxGISDataSource::wxGISDataSource(const CPLString &szPath) : wxGISDataset(szPath)
{
    m_bIsOpened = false;
    m_nType = enumGISContainer;
    m_nSubType = enumContRemoteDBConnection;

    m_poDS = NULL;

    //m_Encoding = wxFONTENCODING_UTF8;
}

wxGISDataSource::~wxGISDataSource(void)
{
    Close();
}

void wxGISDataSource::Close(void)
{
    wxCriticalSectionLocker locker(m_CritSect);

    m_bIsOpened = false;
    if (m_poDS && m_poDS->Dereference() <= 0)
        OGRCompatibleClose(m_poDS);
    m_poDS = NULL;

}

size_t wxGISDataSource::GetSubsetsCount(void) const
{
    if (m_poDS)
        return  m_poDS->GetLayerCount();
    return 0;
}

wxGISDataset* wxGISDataSource::GetSubset(size_t nIndex)
{
    if (m_poDS)
    {
        OGRLayer* poLayer = m_poDS->GetLayer(nIndex);
        return GetDatasetFromOGRLayer(m_sPath, poLayer);
    }
    return NULL;
}

wxGISDataset* wxGISDataSource::GetSubset(const wxString &sTableName)
{
    if (m_poDS)
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

wxGISDataset* wxGISDataSource::GetDatasetFromOGRLayer(const CPLString &sPath, OGRLayer* poLayer)
{
    wxCHECK_MSG(poLayer, NULL, wxT("Input layer pointer is null"));
    //check the layer type
    wxGISDataset* pDataset(NULL);
    CPLString szGeomColumn(poLayer->GetGeometryColumn());
    if (!szGeomColumn.empty())
    {
        wxGISFeatureDataset* pFeatureDataset = new wxGISFeatureDataset(sPath, enumVecFileDBLayer, poLayer, m_poDS);
        //pFeatureDataset->SetEncoding(m_Encoding);
        pDataset = static_cast<wxGISDataset*>(pFeatureDataset);
    }
    else
    {
        wxGISTable* pTable = new wxGISTable(sPath, enumTableFileDBLayer, poLayer, m_poDS);
        //pTable->SetEncoding(m_Encoding);
        pDataset = static_cast<wxGISDataset*>(pTable);
    }

    wsGET(pDataset);
}

wxString wxGISDataSource::GetName(void) const
{
    return m_sDBName;
}

bool wxGISDataSource::Open(bool bUpdate, bool bShared)
{
    wxCriticalSectionLocker locker(m_CritSect);
    if (m_bIsOpened)
        return true;

    CPLErrorReset();

    m_poDS = (OGRDataSource*) wxGISDataset::OpenInternal(m_sPath, bUpdate, bShared);
    if (m_poDS == NULL)
    {
		wxGISLogError(_("Connect failed!"), wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, NULL);
        return false;
    }

    m_sDBName = wxString(m_poDS->GetOGRCompatibleDatasourceName(), wxConvUTF8);

    m_bIsOpened = true;

    wxLogVerbose(_("Connect succeeded! Path '%s'"), m_sPath.c_str());

    return true;
}

bool wxGISDataSource::Rename(const wxString &sNewName, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGISDataSource::Copy(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

bool wxGISDataSource::Move(const CPLString &szDestPath, ITrackCancel* const pTrackCancel)
{
    return false;
}

void wxGISDataSource::Cache(ITrackCancel* const pTrackCancel)
{
}

char **wxGISDataSource::GetFileList()
{
    return NULL;
}

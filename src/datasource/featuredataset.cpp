/******************************************************************************
 * Project:  wxGIS
 * Purpose:  FeatureDataset class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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
#include "wxgis/datasource/featuredataset.h"

//---------------------------------------
// wxGISFeatureDataset
//---------------------------------------

IMPLEMENT_CLASS(wxGISFeatureDataset, wxGISTable)

wxGISFeatureDataset::wxGISFeatureDataset(const CPLString &sPath, int nSubType, OGRLayer* poLayer, OGRDataSource* poDS) : wxGISTable(sPath, nSubType, poLayer, poDS)
{
    m_nType = enumGISFeatureDataset;
    m_eGeomType = wkbUnknown;
    m_pSpatialTree = NULL;

    if(m_bIsOpened)
        SetInternalValues();
}

wxGISFeatureDataset::~wxGISFeatureDataset(void)
{
	Close();
}

void wxGISFeatureDataset::Close(void)
{
    wxDELETE(m_pSpatialTree);
	wxGISTable::Close();
}

wxGISDataset* wxGISFeatureDataset::GetSubset(size_t nIndex)
{
    if(m_poDS)
    {
	    OGRLayer* poLayer = m_poDS->GetLayer(nIndex);
        if(poLayer)
        {
            m_poDS->Reference();
			CPLString szPath;
			szPath.Printf("%s?index=%d", m_sPath.c_str(), nIndex);
			wxGISFeatureDataset* pDataSet = new wxGISFeatureDataset(szPath, m_nSubType, poLayer, m_poDS);
            return wxStaticCast(pDataSet, wxGISDataset);
        }
    }
    return NULL;
}

wxGISDataset* wxGISFeatureDataset::GetSubset(const wxString & sSubsetName)
{
    if(m_poDS)
    {
        CPLString szSubsetName(sSubsetName.mb_str(wxConvUTF8));
        OGRLayer* poLayer = m_poDS->GetLayerByName(szSubsetName);
        if(poLayer)
        {
            m_poDS->Reference();
			CPLString szPath;
            szPath.Printf("%s?name=%s", m_sPath.c_str(), szSubsetName.c_str());
			wxGISFeatureDataset* pDataSet = new wxGISFeatureDataset(szPath, m_nSubType, poLayer, m_poDS);
            return static_cast<wxGISDataset*>(pDataSet);
        }
    }
    return NULL;

}

char **wxGISFeatureDataset::GetFileList()
{
    char **papszFileList = NULL;
    CPLString szPath;
    //papszFileList = CSLAddString( papszFileList, osIMDFile );
    switch(m_nSubType)
    {
    case enumVecESRIShapefile:
        szPath = (char*)CPLResetExtension(m_sPath, "shx");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "dbf");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "prj");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "qpj");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "qix");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "sbn");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "sbx");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "shp.xml");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case enumVecMapinfoTab:
        szPath = (char*)CPLResetExtension(m_sPath, "dat");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "id");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "ind");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "map");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "tab.xml");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "tab.metadata.xml");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case enumVecMapinfoMif:
        szPath = (char*)CPLResetExtension(m_sPath, "mid");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "mif.metadata.xml");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        szPath = (char*)CPLResetExtension(m_sPath, "mif.xml");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case enumVecGML:
        szPath = (char*)CPLResetExtension(m_sPath, "gfs");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case enumVecDXF:
        szPath = (char*)CPLResetExtension(m_sPath, "dxf.xml");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case emumVecPostGIS:
        return papszFileList;
    case enumVecKML:
    case enumVecKMZ:    
    case enumVecUnknown:
    default: 
        break;
    }

    szPath = (char*)CPLResetExtension(m_sPath, "cpg");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "osf");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "sif");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );

    return papszFileList;
}

const wxGISSpatialReference wxGISFeatureDataset::GetSpatialReference(void)
{
    if(!IsOpened())
        return wxNullSpatialReference;

	if(m_SpatialReference.IsOk())
		return m_SpatialReference;

	OGRSpatialReference* pSpaRef = m_poLayer->GetSpatialRef();
	if(!pSpaRef)
		return m_SpatialReference;
	m_SpatialReference = wxGISSpatialReference(pSpaRef);
	return m_SpatialReference;
}

bool wxGISFeatureDataset::Open(int iLayer, int bUpdate, bool bCache, ITrackCancel* const pTrackCancel)
{
	if(IsOpened())
    {
		return true;
    }

	if(!wxGISTable::Open(iLayer, bUpdate, bCache, pTrackCancel))
    {
		return false;
    }

    SetInternalValues();

	m_bIsOpened = true;

    if(bCache)
    {
        Cache(pTrackCancel);
    }

	return true;
}

void wxGISFeatureDataset::SetInternalValues()
{
    if(NULL == m_poLayer)
    {
        return;
    }

    OGRFeatureDefn* const pDef = GetDefinition();
    if(NULL != pDef)
    {
        m_eGeomType = pDef->GetGeomType();
    }

    //fill extent if fast
	bool bOLCFastGetExtent = m_poLayer->TestCapability(OLCFastGetExtent) != 0;
    if(bOLCFastGetExtent)
    {
        if(m_poLayer->GetExtent(&m_stExtent, FALSE) == OGRERR_NONE)
        {
            if(IsDoubleEquil(m_stExtent.MinX, m_stExtent.MaxX))
            {
                m_stExtent.MaxX += 1;
                m_stExtent.MinX -= 1;
            }
            if(IsDoubleEquil(m_stExtent.MinY, m_stExtent.MaxY))
            {
                m_stExtent.MaxY += 1;
                m_stExtent.MinY -= 1;
            }
        }
    }

    wxGISTable::SetInternalValues();
    //in Memory DS HasFID = true
    if(m_nSubType == enumVecMem)
        m_bHasFID = true;
}

void wxGISFeatureDataset::Cache(ITrackCancel* const pTrackCancel)
{
    if(m_bIsCached)
    {
        return;
    }

	//wxGISTable::Cache(pTrackCancel);

    if(!m_pSpatialTree)
    {
        m_pSpatialTree = CreateSpatialTree(this);
    }

    if(IsCaching())
    {
        return;
    }

    m_pSpatialTree->Load(m_SpatialReference, pTrackCancel);
}

bool wxGISFeatureDataset::IsCached(void) const
{
    if(m_pSpatialTree && m_pSpatialTree->IsLoading())
    {
       return false;
    }

    return m_bIsCached;
}

bool wxGISFeatureDataset::IsCaching(void) const
{
    if(m_pSpatialTree)
    {
       return m_pSpatialTree->IsLoading();
    }

    return false;
}

void wxGISFeatureDataset::StopCaching(void)
{
    if(m_pSpatialTree && m_pSpatialTree->IsLoading())
    {
        m_pSpatialTree->CancelLoading();
    }
}

OGREnvelope wxGISFeatureDataset::GetEnvelope(void)
{
    wxCriticalSectionLocker locker(m_CritSect);
    if(m_stExtent.IsInit() || m_nFeatureCount == 0)
        if(!IsDoubleEquil(m_stExtent.MinX, m_stExtent.MaxX) && !IsDoubleEquil(m_stExtent.MinY, m_stExtent.MaxY))
            return m_stExtent;

    if(IsOpened())
    {
        if(m_poLayer->GetExtent(&m_stExtent) == OGRERR_NONE)
        {
            if(IsDoubleEquil(m_stExtent.MinX, m_stExtent.MaxX))
            {
                m_stExtent.MaxX += 1;
                m_stExtent.MinX -= 1;
            }
            if(IsDoubleEquil(m_stExtent.MinY, m_stExtent.MaxY))
            {
                m_stExtent.MaxY += 1;
                m_stExtent.MinY -= 1;
            }
		}
    }
    return m_stExtent;
}

OGRwkbGeometryType wxGISFeatureDataset::GetGeometryType(void) const
{
    return m_eGeomType;
}

wxFeatureCursor wxGISFeatureDataset::Search(const wxGISSpatialFilter &SpaFilter, bool bOnlyFirst)
{
    if(SpaFilter.GetGeometry().IsOk())
	{
        m_poLayer->SetSpatialFilter(SpaFilter.GetGeometry());
	}
	else
    {
        m_poLayer->SetSpatialFilter(NULL);
    }

    wxFeatureCursor oOutCursor = wxGISTable::Search(SpaFilter, bOnlyFirst);
    m_poLayer->SetSpatialFilter(NULL);
    return oOutCursor;
}

wxGISSpatialTreeCursor wxGISFeatureDataset::SearchGeometry(const OGREnvelope &Env)
{
	if(m_pSpatialTree)
		return m_pSpatialTree->Search(Env);
	else
		return wxNullSpatialTreeCursor;
}

OGRErr wxGISFeatureDataset::DeleteFeature(long nFID)
{
    OGRErr ret = wxGISTable::DeleteFeature(nFID);
    if( ret == OGRERR_NONE)
    {
        if(m_pSpatialTree)
        {
            m_pSpatialTree->Remove(nFID);
        }
    }
    return ret;
}

OGRErr wxGISFeatureDataset::StoreFeature(wxGISFeature &Feature)
{
    OGRErr ret = wxGISTable::StoreFeature(Feature);
    if(ret == OGRERR_NONE)
    {
        if(m_pSpatialTree)
        {
            m_pSpatialTree->Insert(Feature.GetGeometry(), Feature.GetFID());
        }
    }
    //update envelope
    if(m_stExtent.IsInit())
    {
        m_stExtent.Merge(Feature.GetGeometry().GetEnvelope());
    }
    else
    {
        m_stExtent = Feature.GetGeometry().GetEnvelope();
    }
    return ret;
}

OGRErr wxGISFeatureDataset::SetFeature(const wxGISFeature &Feature)
{
    OGRErr ret = wxGISTable::SetFeature(Feature);
    if(ret == OGRERR_NONE)
    {
        if(m_pSpatialTree)
        {
            m_pSpatialTree->Change(Feature.GetGeometry(), Feature.GetFID());
        }
    }
    //update envelope
    if(m_stExtent.IsInit())
    {
        m_stExtent.Merge(Feature.GetGeometry().GetEnvelope());
    }
    else
    {
        m_stExtent = Feature.GetGeometry().GetEnvelope();
    }

    return ret;
}

/*
OGRErr wxGISFeatureDataset::SetFilter(wxGISQueryFilter* pQFilter)
{
    if(	!m_poLayer )
		return OGRERR_FAILURE;

	wxGISSpatialFilter* pSpaFil = dynamic_cast<wxGISSpatialFilter*>(pQFilter);
	if(pSpaFil)
	{
		OGRGeometrySPtr pGeom = pSpaFil->GetGeometry();
        m_poLayer->SetSpatialFilter(pGeom.get());
	}
	else
        m_poLayer->SetSpatialFilter(NULL);

	if( wxGISTable::SetFilter(pQFilter) == OGRERR_NONE )
	{
		LoadGeometry();
		return OGRERR_NONE;
	}
    return OGRERR_FAILURE;
}
*/

void wxGISFeatureDataset::SetCached(bool bCached)
{
    m_bIsCached = bCached;
}

//---------------------------------------
// wxGISFeatureDatasetCached
//---------------------------------------

IMPLEMENT_CLASS(wxGISFeatureDatasetCached, wxGISFeatureDataset)

wxGISFeatureDatasetCached::wxGISFeatureDatasetCached(const CPLString &sPath, int nSubType, OGRLayer* poLayer, OGRDataSource* poDS) : wxGISFeatureDataset(sPath, nSubType, poLayer, poDS)
{
}

wxGISFeatureDatasetCached::~wxGISFeatureDatasetCached(void)
{
}

void wxGISFeatureDatasetCached::Close(void)
{
    wxDELETE(m_pSpatialTree);
    m_omFeatures.clear();
	wxGISTable::Close();
}

wxGISDataset* wxGISFeatureDatasetCached::GetSubset(size_t nIndex)
{
    if(m_poDS)
    {
	    OGRLayer* poLayer = m_poDS->GetLayer(nIndex);
        if(poLayer)
        {
            m_poDS->Reference();
			CPLString szPath;
			szPath.Printf("%s?index=%d", m_sPath.c_str(), nIndex);
			wxGISFeatureDatasetCached* pDataSet = new wxGISFeatureDatasetCached(szPath, m_nSubType, poLayer, m_poDS);
            //pDataSet->SetInternalValues();//SetEncoding(m_Encoding);
            return wxStaticCast(pDataSet, wxGISDataset);
        }
    }
    return NULL;
}

wxGISDataset* wxGISFeatureDatasetCached::GetSubset(const wxString & sSubsetName)
{
    if(m_poDS)
    {
        CPLString szSubsetName(sSubsetName.mb_str(wxConvUTF8));
        OGRLayer* poLayer = m_poDS->GetLayerByName(szSubsetName);
        if(poLayer)
        {
            m_poDS->Reference();
			CPLString szPath;
            szPath.Printf("%s?name=%s", m_sPath.c_str(), szSubsetName.c_str());
			wxGISFeatureDatasetCached* pDataSet = new wxGISFeatureDatasetCached(szPath, m_nSubType, poLayer, m_poDS);
            //pDataSet->SetInternalValues();//SetEncoding(m_Encoding);
            return static_cast<wxGISDataset*>(pDataSet);
        }
    }
    return NULL;

}

void wxGISFeatureDatasetCached::Cache(ITrackCancel* const pTrackCancel)
{
    if (m_bIsCached)
        return;

    m_omFeatures.clear();

    if(!m_poLayer)
        return;
    m_poLayer->ResetReading();

	IProgressor* pProgress(NULL);
	if(pTrackCancel)
	{
		pTrackCancel->Reset();
		pTrackCancel->PutMessage(wxString(_("PreLoad Features of ")) + GetName(), -1, enumGISMessageInfo);
		pProgress = pTrackCancel->GetProgressor();
		if(pProgress)
            pProgress->ShowProgress(true);
	}

	bool bOLCFastFeatureCount = m_poLayer->TestCapability(OLCFastFeatureCount) != 0;
	if(pProgress)
	{
		if(bOLCFastFeatureCount)
			pProgress->SetRange(m_poLayer->GetFeatureCount());
		else
			pProgress->Play();
	}

    //loading
    m_nCurrentFID = 1;

	OGRFeature *poFeature;
	while((poFeature = m_poLayer->GetNextFeature()) != NULL )
	{
		if(pProgress)
		{
			if(bOLCFastFeatureCount)
				pProgress->SetValue(m_nCurrentFID);
			else
				pProgress->Play();
		}

        //if(!poFeature->GetDefnRef())
        //{
        //    OGRFeature::DestroyFeature(poFeature);
        //    continue;
        //}

        long nFID;
        if(poFeature && !m_bHasFID)
		{
            nFID = m_nCurrentFID;
			poFeature->SetFID(nFID);
		}
        else
            nFID = poFeature->GetFID();

        //store features in array for speed
		m_omFeatures[nFID] = wxGISFeature(poFeature, m_Encoding);
		m_nCurrentFID++;

        //fill extent
        OGRGeometry* pGeom = poFeature->GetGeometryRef();
        OGREnvelope CurrEnv;
        if(pGeom)
            pGeom->getEnvelope(&CurrEnv);

        if(CurrEnv.IsInit())
        {
            if(m_stExtent.IsInit())
            {
                m_stExtent.Merge(CurrEnv);
            }
            else
            {
                m_stExtent = CurrEnv;
            }
        }

		if(pTrackCancel && !pTrackCancel->Continue())
		{
			if(pProgress)
				pProgress->ShowProgress(false);
			return;
		}
		m_nFeatureCount = m_omFeatures.size();
    }


	if(pProgress)
	{
		pProgress->Stop();
		pProgress->ShowProgress(false);
	}

    if(IsDoubleEquil(m_stExtent.MinX, m_stExtent.MaxX))
    {
        m_stExtent.MaxX += 1;
        m_stExtent.MinX -= 1;
    }
    if(IsDoubleEquil(m_stExtent.MinY, m_stExtent.MaxY))
    {
        m_stExtent.MaxY += 1;
        m_stExtent.MinY -= 1;
    }

    m_bIsCached = true;
    
    if(!m_pSpatialTree)
    {
        m_pSpatialTree = CreateSpatialTree(this);
    }

    if(IsCaching())
        return;

    m_pSpatialTree->Load(m_SpatialReference, pTrackCancel);
}

void wxGISFeatureDatasetCached::Reset(void)
{
    m_nCurrentFID = 1;
}

wxGISFeature wxGISFeatureDatasetCached::Next(void)
{
    if(m_omFeatures.empty())
    {
        return wxGISFeature();
    }

    return m_omFeatures[m_nCurrentFID++];
}

wxGISFeature wxGISFeatureDatasetCached::GetFeature(long nIndex)
{
    if(!m_poLayer)
    {
        return wxGISFeature();
    }

    m_nCurrentFID = nIndex + 1;

    return Next();
}

size_t wxGISFeatureDatasetCached::GetFeatureCount(bool bForce, ITrackCancel* const pTrackCancel)
{
    wxCriticalSectionLocker locker(m_CritSect);
	if(m_nFeatureCount != wxNOT_FOUND)
		return m_nFeatureCount;
    if(	m_poLayer )
    {
        if(bForce)
        {
            Cache(pTrackCancel);
        }
        else if(m_bOLCFastFeatureCount)
            m_nFeatureCount = m_poLayer->GetFeatureCount(0);
        else 
        {
        	m_nFeatureCount = m_poLayer->GetFeatureCount(0);
            if(m_nFeatureCount == wxNOT_FOUND)
            {
                Cache(pTrackCancel);
            }
            if(m_nFeatureCount == wxNOT_FOUND)
            {
                return 0;
            }
		    return m_nFeatureCount;
        }
    }
    return 0;
}

OGRErr wxGISFeatureDatasetCached::DeleteFeature(long nFID)
{
	if(!CanDeleteFeature())
		return OGRERR_UNSUPPORTED_OPERATION;

	OGRErr eErr = m_poLayer->DeleteFeature(nFID);
    if(eErr != OGRERR_NONE)
        return eErr;

	m_nFeatureCount--;

    if(m_omFeatures[nFID].IsOk())
		m_omFeatures[nFID] = wxGISFeature();

    return eErr;
}

OGRErr wxGISFeatureDatasetCached::StoreFeature(wxGISFeature &Feature)
{
    OGRErr eErr = wxGISTable::StoreFeature(Feature);
    if(eErr != OGRERR_NONE)
    {
        return eErr;
    }

    m_omFeatures[Feature.GetFID()] = Feature;
    return eErr;
}

OGRErr wxGISFeatureDatasetCached::SetFeature(const wxGISFeature &Feature)
{
    OGRErr eErr = wxGISTable::SetFeature(Feature);
    if(eErr != OGRERR_NONE)
    {
        return eErr;
    }
    //TODO: if FID in feature is changed
    m_omFeatures[Feature.GetFID()] = Feature;
    return eErr;
}

wxGISFeature wxGISFeatureDatasetCached::GetFeatureByID(long nFID)
{
    wxGISFeature ret = m_omFeatures[nFID];
    if(ret.IsOk())
		return ret;
	else 
    {
        if(m_poLayer)
	    {
		    OGRFeature* pFeature = m_poLayer->GetFeature(nFID);
		    if(pFeature)
		    {
                ret = wxGISFeature(pFeature, m_Encoding);
			    m_omFeatures[nFID] = ret;
		    }
	    }
    }
	return ret;
}

OGREnvelope wxGISFeatureDatasetCached::GetEnvelope(void)
{
    wxCriticalSectionLocker locker(m_CritSect);
    if(m_stExtent.IsInit() || m_nFeatureCount == 0)
        if(!IsDoubleEquil(m_stExtent.MinX, m_stExtent.MaxX) && !IsDoubleEquil(m_stExtent.MinY, m_stExtent.MaxY))
            return m_stExtent;

    if(!IsCached())
    {
        Cache();
    }
    return m_stExtent;
}

/*

wxFeatureCursorSPtr wxGISFeatureDataset::Search(wxGISQueryFilter* pQFilter, bool bOnlyFirst)
{
	//select by attributes
	//filter by geometry
	//return fullfill wxFeatureCursorSPtr
	return wxGISTable::Search(pQFilter, bOnlyFirst);
}

//wxGISGeometrySet* wxGISFeatureDataset::GetGeometrySet(wxGISQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
//{
//    //spatial reference of pQFilter
//	wxGISGeometrySet* pGISGeometrySet = NULL;
//	if(pQFilter)
//	{
//		wxGISSpatialFilter* pSpaFil = dynamic_cast<wxGISSpatialFilter*>(pQFilter);
//		if(pSpaFil)
//		{
//			OGREnvelope Env = pSpaFil->GetEnvelope();
//            if(m_pQuadTree)
//            {
//			    int count(0);
//			    CPLRectObj Rect = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
//			    OGRGeometry** pGeometryArr = (OGRGeometry**)CPLQuadTreeSearch(m_pQuadTree, &Rect, &count);
//                pGISGeometrySet = new wxGISGeometrySet(false);
//			    for(int i = 0; i < count; ++i)
//			    {
//				    if(pTrackCancel && !pTrackCancel->Continue())
//					    break;
//				    pGISGeometrySet->AddGeometry(pGeometryArr[i], m_pGeometrySet->operator[](pGeometryArr[i]));
//			    }
//			    wxDELETEA( pGeometryArr );
//                pGISGeometrySet->Reference();
//                return pGISGeometrySet;
//            }
//		}
//	}
//	else
//	{
//        if(m_bIsGeometryLoaded)
//            return GetGeometries();
//        else
//        {
//            LoadGeometry();
//            return GetGeometrySet(pQFilter, pTrackCancel);
//        }
//	}
//	return pGISGeometrySet;
//}

//wxGISFeatureDataset::wxGISFeatureDataset(OGRDataSource *poDS, OGRLayer* poLayer, CPLString sPath, wxGISEnumVectorDatasetType nType) : wxGISDataset(sPath), m_poDS(NULL), m_psExtent(NULL), m_poLayer(NULL), m_bIsGeometryLoaded(false), m_pQuadTree(NULL), m_FieldCount(-1), m_pGeometrySet(NULL)
//{
//	m_bIsOpened = false;
//	m_poDS = poDS;
//	m_poLayer = poLayer;
//    if(m_poLayer == NULL)
//        m_nType = enumGISContainer;
//    else
//    {
//        m_nType = enumGISFeatureDataset;
//        m_bOLCStringsAsUTF8 = m_poLayer->TestCapability(OLCStringsAsUTF8);
//    }
//    m_nSubType = (int)nType;
//
//	m_bIsOpened = true;
//
//    if(m_nSubType == enumVecKML || m_nSubType == enumVecKMZ)
//        m_Encoding = wxFONTENCODING_UTF8;
//    else
//        m_Encoding = wxFONTENCODING_DEFAULT;
//}
//
//wxGISFeatureDataset::wxGISFeatureDataset(CPLString sPath, wxGISEnumVectorDatasetType nType) : wxGISDataset(sPath), m_poDS(NULL), m_psExtent(NULL), m_poLayer(NULL), m_bIsGeometryLoaded(false), m_pQuadTree(NULL), m_FieldCount(-1), m_pGeometrySet(NULL)
//{
//	m_bIsOpened = false;
//
//    m_nType = enumGISFeatureDataset;
//    m_nSubType = (int)nType;
//    if(m_nSubType == enumVecKML || m_nSubType == enumVecKMZ)
//        m_Encoding = wxFONTENCODING_UTF8;
//    else
//        m_Encoding = wxFONTENCODING_DEFAULT;
//}
//
//wxGISGeometrySet* wxGISFeatureDataset::GetGeometries(void)
//{
//    if(!m_bIsGeometryLoaded)
//        return NULL;
//    m_pGeometrySet->Reference();
//    return m_pGeometrySet;
//}
//
//OGRErr wxGISFeatureDataset::CreateFeature(OGRFeature* poFeature)
//{
//    wxCriticalSectionLocker locker(m_CritSect);
//
////open if closed
// //   if(m_poDS)
// //   {
// //       DeleteQuadTree();
// //       wsDELETE(m_pGeometrySet);
// //       OGRDataSource::DestroyDataSource( m_poDS );
// //       m_poDS = NULL;
// //   }
//	//if( m_poDS == NULL )
// //      m_poDS = OGRSFDriverRegistrar::Open( (const char*) m_sPath.mb_str(*m_pPathEncoding), TRUE );
//	//if( m_poDS == NULL )
//	//	return false;
//
//
//    OGRErr eErr = m_poLayer->CreateFeature(poFeature);
//    if(eErr == OGRERR_NONE)
//    {
//        bool bOLCFastSetNextByIndex= m_poLayer->TestCapability(OLCFastSetNextByIndex);
//        wxString sFIDColName = wgMB2WX(m_poLayer->GetFIDColumn());
//        bool bHasFID = true;
//        if(sFIDColName.IsEmpty())
//            bHasFID = false;
//
//        long nFID;
//        if(!bHasFID)
//            nFID = m_poLayer->GetFeatureCount(false);
//        else
//            nFID = poFeature->GetFID();
//
//        OGRGeometry* pGeom(NULL);
//        //store features in array for speed
//        if(bOLCFastSetNextByIndex)
//            pGeom = poFeature->StealGeometry();
//        else
//        {
//            m_FeaturesMap[nFID] = poFeature;
//            if(poFeature->GetGeometryRef())
//                pGeom = poFeature->GetGeometryRef()->clone();
//        }
//
//        if(m_pGeometrySet)
//            m_pGeometrySet->AddGeometry(pGeom, nFID);
//  //      if(bOLCFastGetExtent)
//  //          CPLQuadTreeInsert(m_pQuadTree, (void*)pGeom);
//  //      else
//  //      {
//  //          OGREnvelope Env;
//  //          pGeom->getEnvelope(&Env);
//  //          m_psExtent->Merge(Env);
//  //      }
//		//counter++;
//        if(bOLCFastSetNextByIndex)
//            OGRFeature::DestroyFeature(poFeature);
//    }
//    return eErr;
//}
//
//void wxGISFeatureDataset::SetSpatialFilter(double dfMinX, double dfMinY, double dfMaxX, double dfMaxY)
//{
//	if(!m_bIsOpened)
//		if(!Open(0))
//			return;
//	if(	m_poLayer )
//	{
		//bool bOLCFastSpatialFilter = m_poLayer->TestCapability(OLCFastSpatialFilter);
//		m_poLayer->SetSpatialFilterRect(dfMinX, dfMinY, dfMaxX, dfMaxY);
//	}
//}
//
//wxGISFeatureSet* wxGISFeatureDataset::GetFeatureSet(IQueryFilter* pQFilter, ITrackCancel* pTrackCancel)
//{
//    //spatial reference of pQFilter
//	wxGISFeatureSet* pGISFeatureSet = NULL;
//	if(pQFilter)
//	{
//		wxGISSpatialFilter* pSpaFil = dynamic_cast<wxGISSpatialFilter*>(pQFilter);
//		if(pSpaFil)
//		{
//			OGREnvelope Env = pSpaFil->GetEnvelope();
//            if(m_pQuadTree)
//            {
//			    int count(0);
//			    CPLRectObj Rect = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
//			    OGRFeature** pFeatureArr = (OGRFeature**)CPLQuadTreeSearch(m_pQuadTree, &Rect, &count);
//                pGISFeatureSet = new wxGISFeatureSet(m_FeaturesMap.size());
//			    for(int i = 0; i < count; ++i)
//			    {
//				    if(pTrackCancel && !pTrackCancel->Continue())
//					    break;
//				    pGISFeatureSet->AddFeature(pFeatureArr[i]);
//			    }
//			    wxDELETEA( pFeatureArr );
//                return pGISFeatureSet;
//            }
//            else
//            {
//                if(!m_bIsOpened)
//                    if(!Open(0))
//                        return NULL;
//                if(m_poLayer)
//                {
// 		            bool bOLCFastSpatialFilter = m_poLayer->TestCapability(OLCFastSpatialFilter);
//                    if(bOLCFastSpatialFilter)
//                    {
//                        m_poLayer->SetSpatialFilterRect(Env.MinX, Env.MinY, Env.MaxX, Env.MaxY);
//                        pGISFeatureSet = new wxGISFeatureSet(m_poLayer->GetFeatureCount(true));
//
// 		                OGRFeature *poFeature;
//		                while((poFeature = m_poLayer->GetNextFeature()) != NULL )
//                        {
//                            pGISFeatureSet->AddFeature(poFeature);
//                            if(pTrackCancel && !pTrackCancel->Continue())
//                                break;
//                        }
//                        return pGISFeatureSet;
//                    }
//                    else
//                    {
//                        LoadFeatures();
//                        if(m_bIsFeaturesLoaded)
//                            return GetFeatureSet(pQFilter, pTrackCancel);
//                        else
//                        {
//                            pGISFeatureSet = new wxGISFeatureSet(m_poLayer->GetFeatureCount(true));
//
// 		                    OGRFeature *poFeature;
//		                    while((poFeature = m_poLayer->GetNextFeature()) != NULL )
//                            {
//                                pGISFeatureSet->AddFeature(poFeature);
//                                if(pTrackCancel && !pTrackCancel->Continue())
//                                    break;
//                            }
//                            return pGISFeatureSet;
//                        }
//                    }
//                }
//            }
//		}
//	}
//	else
//	{
//        if(m_FeaturesMap.empty())
//        {
//            LoadFeatures();
//            if(m_bIsFeaturesLoaded)
//                return GetFeatureSet(pQFilter, pTrackCancel);
//            else
//            {
//                pGISFeatureSet = new wxGISFeatureSet(m_poLayer->GetFeatureCount(true));
//
//                OGRFeature *poFeature;
//                while((poFeature = m_poLayer->GetNextFeature()) != NULL )
//                {
//                    pGISFeatureSet->AddFeature(poFeature);
//                    if(pTrackCancel && !pTrackCancel->Continue())
//                        break;
//                }
//                return pGISFeatureSet;
//            }
//        }
//        else
//        {
//            pGISFeatureSet = new wxGISFeatureSet(m_FeaturesMap.size());
//            for(Iterator IT = m_FeaturesMap.begin(); IT != m_FeaturesMap.end(); ++IT)
//		    {
//			    pGISFeatureSet->AddFeature(IT->second);
//			    if(pTrackCancel && !pTrackCancel->Continue())
//				    break;
//		    }
//	        return pGISFeatureSet;
//        }
//	}
//	return pGISFeatureSet;
//}

		    //bool bOLCFastGetExtent = pOGRLayer->TestCapability(OLCFastGetExtent);
      //      if(bOLCFastGetExtent)
      //      {
		    //    m_psExtent = new OGREnvelope();
		    //    if(m_poLayer->GetExtent(m_psExtent, true) != OGRERR_NONE)
		    //    {
		    //	    wxDELETE(m_psExtent);
		    //	    m_psExtent = NULL;
		    //    }
      //      }

		    //bool bOLCFastSpatialFilter = m_poLayer->TestCapability(OLCFastSpatialFilter);
		    //if(!bOLCFastSpatialFilter)
		    //{
		    //	if(m_psExtent)
		    //	{
		    //		OGREnvelope Env = *m_psExtent;
		    //		CPLRectObj Rect = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
		    //		m_pQuadTree = CPLQuadTreeCreate(&Rect, GetFeatureBoundsFunc);
		    //	}

		    //	//wxFileName FileName(m_sPath);
		    //	//wxString SQLStatement = wxString::Format(wxT("CREATE SPATIAL INDEX ON %s"), FileName.GetName().c_str());
		    //	//m_poDS->ExecuteSQL(wgWX2MB(SQLStatement), NULL, NULL);
		    //}
	    //	bool bOLCRandomRead = pOGRLayer->TestCapability(OLCRandomRead);
	    //	bool bOLCSequentialWrite = pOGRLayer->TestCapability(OLCSequentialWrite);
	    //	bool bOLCRandomWrite = pOGRLayer->TestCapability(OLCRandomWrite);
	    //	bool bOLCFastFeatureCount = pOGRLayer->TestCapability(OLCFastFeatureCount);
	    //	bool bOLCFastGetExtent = pOGRLayer->TestCapability(OLCFastGetExtent);
	    	//bool bOLCFastSetNextByIndex= m_poLayer->TestCapability(OLCFastSetNextByIndex);
      //      if(!bOLCFastSetNextByIndex)
      //          LoadGeometry();
	    //	bool bOLCCreateField = pOGRLayer->TestCapability(OLCCreateField);
	    //	bool bOLCDeleteFeature = pOGRLayer->TestCapability(OLCDeleteFeature);
	    //	bool bOLCTransactions = pOGRLayer->TestCapability(OLCTransactions);
	    //	wxString sFIDColName = wgMB2WX(pOGRLayer->GetFIDColumn());

//TODO:
//1. Переписать SetFilter -> HRESULT Search(
//  IQueryFilter* filter,
//  VARIANT_BOOL Recycling,
//  IFeatureCursor** Cursor
//); Блокировка получения данных по индексу или ОИД при выполнении операции
//2. На выходе Cursor -> массив OID и ссылка на table/featureclass
/*
3. Переписать Search для FeatureClass с использованием RTree и возможностью выбора произвольной геометрией
4. При загрузке в слой не создается копия RTree. Создается только при перепроецировании
5. В RTree содержиться геометрия и ОИД. RTree сохраняется в отдельном файле rtr/sig ...
6. При перепроецировании: а) перепроецируются все геомтерии с записью в новый массив; б) создается новое дерево RTree; в) многопоточно
7. При загрузке грузим RTree из файла. Создаем при первом Search если не загрузили при зауске? многопоточно.
8. При отрисовке: запрашивается по охвату (для окна или выделения) массив структуры геометрия* + ОИД. Если перепроецирование врублено - запрос идет у локальной копии массива
9. По ОИД можно получить быстро геометрию или атрибуты.
*/

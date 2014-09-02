/******************************************************************************
 * Project:  wxGIS
 * Purpose:  FeatureDataset class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2014 Dmitry Baryshnikov
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
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/sysop.h"

//---------------------------------------
// wxGISFeatureDataset
//---------------------------------------

IMPLEMENT_CLASS(wxGISFeatureDataset, wxGISTable)

wxGISFeatureDataset::wxGISFeatureDataset(const CPLString &sPath, int nSubType, OGRLayer* poLayer, OGRCompatibleDataSource* poDS) : wxGISTable(sPath, nSubType, poLayer, poDS)
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
            CPLString szPath(CPLFormFilename(m_sPath, poLayer->GetName(), ""));
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
        //TODO: get layers by index and comapre names - create wxGISFeatureSubDataset with index to delete by index
        OGRLayer* poLayer = m_poDS->GetLayerByName(szSubsetName);
        if(poLayer)
        {
            CPLString szPath(CPLFormFilename(m_sPath, szSubsetName, ""));
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
    case enumVecSXF:
        szPath = (char*)CPLResetExtension(m_sPath, "rsc");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case enumVecCSV:
        szPath = (char*)CPLResetExtension(m_sPath, "csvt");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case enumVecDXF:
        szPath = (char*)CPLResetExtension(m_sPath, "dxf.xml");
        if(CPLCheckForFile((char*)szPath.c_str(), NULL))
            papszFileList = CSLAddString( papszFileList, szPath );
        break;
    case enumVecPostGIS:
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
    if(!IsOpened() || !m_poLayer)
        return wxNullSpatialReference;

	if(m_SpatialReference.IsOk())
		return m_SpatialReference;

    wxCriticalSectionLocker locker(m_CritSect);

	OGRSpatialReference* pSpaRef = m_poLayer->GetSpatialRef();
	if(!pSpaRef)
		return m_SpatialReference;
	m_SpatialReference = wxGISSpatialReference(pSpaRef);
	return m_SpatialReference;
}

void wxGISFeatureDataset::SetEncoding(const wxFontEncoding &oEncoding)
{
    if (m_nSubType == enumVecESRIShapefile )
    {
        wxString sEnc = wxLocale::GetSystemEncodingName();
        const char* sz_enc = sEnc.mb_str();
        m_bRecodeToSystem = wxGISEQUAL(CPLGetConfigOption("SHAPE_ENCODING", sz_enc), sz_enc);
    }
    else if (m_nSubType == enumVecDXF)
    {
        wxString sEnc = wxLocale::GetSystemEncodingName();
        const char* sz_enc = sEnc.mb_str();
        m_bRecodeToSystem = wxGISEQUAL(CPLGetConfigOption("DXF_ENCODING", sz_enc), sz_enc);
    }
    m_Encoding = oEncoding;
}

bool wxGISFeatureDataset::Open(int iLayer, bool bUpdate, bool bShared, bool bCache, ITrackCancel* const pTrackCancel)
{
	if(IsOpened())
    {
		return true;
    }

    if (m_nSubType == enumVecESRIShapefile)
    {
        const char* szCPGPath = CPLResetExtension(m_sPath, "cpg");
        if (!CPLCheckForFile((char*)szCPGPath, NULL))//no cpg file
        {
            //set system encoding
            wxString sEnc = wxLocale::GetSystemEncodingName();
            const char* sz_enc = sEnc.mb_str();
            CPLSetConfigOption("SHAPE_ENCODING", sz_enc);
        }
    }
    else if (m_nSubType == enumVecDXF)
    {
        const char* szCPGPath = CPLResetExtension(m_sPath, "cpg");
        if (!CPLCheckForFile((char*)szCPGPath, NULL))//no cpg file
        {
            //set system encoding
            wxString sEnc = wxLocale::GetSystemEncodingName();
            const char* sz_enc = sEnc.mb_str();
            CPLSetConfigOption("DXF_ENCODING", sz_enc);
        }
    }

    if (!wxGISTable::Open(iLayer, bUpdate, bShared, bCache, pTrackCancel))
    {
		return false;
    }

	m_bIsOpened = true;

	return true;
}

bool wxGISFeatureDataset::IsContainer() const
{
    //for file datasources only
    return m_nSubType == enumVecKML || m_nSubType == enumVecKMZ || m_nSubType == enumVecGML || m_nSubType == enumVecSXF;
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

    OGRSpatialReference* pSpaRef = m_poLayer->GetSpatialRef();
    if (NULL != pSpaRef)
    {
        m_SpatialReference = wxGISSpatialReference(pSpaRef);
    }

    //fill extent if fast
    m_bOLCFastGetExtent = m_poLayer->TestCapability(OLCFastGetExtent) == TRUE;
    if (m_bOLCFastGetExtent)
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

wxFeatureCursor wxGISFeatureDataset::Search(const wxGISSpatialFilter &SpaFilter, bool bOnlyFirst, ITrackCancel* const pTrackCancel)
{
    if(SpaFilter.GetGeometry().IsOk())
	{
        m_poLayer->SetSpatialFilter(SpaFilter.GetGeometry());
	}
	else
    {
        m_poLayer->SetSpatialFilter(NULL);
    }

    wxFeatureCursor oOutCursor = wxGISTable::Search(SpaFilter, bOnlyFirst, pTrackCancel);
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


OGRErr wxGISFeatureDataset::SetFilter(const wxGISSpatialFilter &SpaFilter)
{
    if(NULL == m_poLayer)
		return OGRERR_FAILURE;

    if (SpaFilter.GetGeometry().IsOk())
	{
        m_poLayer->SetSpatialFilter(SpaFilter.GetGeometry());
	}
    else
    {
        m_poLayer->SetSpatialFilter(NULL);
    }

    return wxGISTable::SetFilter(SpaFilter);
}


void wxGISFeatureDataset::SetCached(bool bCached)
{
    m_bIsCached = bCached;
}

//---------------------------------------
// wxGISFeatureDatasetCached
//---------------------------------------

IMPLEMENT_CLASS(wxGISFeatureDatasetCached, wxGISFeatureDataset)

wxGISFeatureDatasetCached::wxGISFeatureDatasetCached(const CPLString &sPath, int nSubType, OGRLayer* poLayer, OGRCompatibleDataSource* poDS) : wxGISFeatureDataset(sPath, nSubType, poLayer, poDS)
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
            CPLString szPath(CPLFormFilename(m_sPath, poLayer->GetName(), ""));
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
        //TODO: get layers by index and comapre names - create wxGISFeatureSubDataset with index to delete by index
        OGRLayer* poLayer = m_poDS->GetLayerByName(szSubsetName);
        if(poLayer)
        {
            //m_poDS->Reference();
            CPLString szPath(CPLFormFilename(m_sPath, szSubsetName, ""));
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

    const char *oldlocale = setlocale(LC_NUMERIC, "C");

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
        m_omFeatures[nFID] = wxGISFeature(poFeature, m_Encoding, m_bRecodeToSystem);
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
                m_stExtent.Merge(CurrEnv); //TODO: check
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


    setlocale(LC_NUMERIC, oldlocale);

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
    //ignore force
    if (m_nFeatureCount != wxNOT_FOUND)
		return m_nFeatureCount;
    if(	m_poLayer )
    {
        if (m_omFeatures.empty())
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
                ret = wxGISFeature(pFeature, m_Encoding, m_bRecodeToSystem);
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

void wxGISFeatureDatasetCached::SetEncoding(const wxFontEncoding &oEncoding)
{
    if (IsCached())
    {
        m_omFeatures.clear();
        m_bIsCached = false;
    }

    wxGISFeatureDataset::SetEncoding(oEncoding);

    if (!IsCached())
    {
        Cache();
    }
}


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



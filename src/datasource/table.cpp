/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Table class.
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

#include "wxgis/datasource/table.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/core/config.h"

#include <wx/uri.h>

//--------------------------------------------------------------------------------
// wxGISTable
//--------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISTable, wxGISDataset)

wxGISTable::wxGISTable(const CPLString &sPath, int nSubType, OGRLayer* poLayer, OGRCompatibleDataSource* poDS) : wxGISDataset(sPath)
{
	wsSET(m_poDS, poDS);
	m_poLayer = poLayer;
    m_nType = enumGISTableDataset;
    m_nSubType = nSubType;

    m_Encoding = wxLocale::GetSystemEncoding();
    m_bRecodeToSystem = false;

    poLayer == NULL ? m_bIsOpened = false : m_bIsOpened = true;

    m_bIsReadOnly = true;
    m_bIsCached = false;

	m_bHasFID = false;
    m_bHasFilter = false;
    m_nCurrentFID = wxNOT_FOUND;
    m_nFeatureCount = wxNOT_FOUND;

    m_bOLCFastFeatureCount = false;
    m_bOLCStringsAsUTF8 = false;

    if(m_bIsOpened)
        SetInternalValues();
}

wxGISTable::~wxGISTable(void)
{
	Close();
}

wxString wxGISTable::GetName(void) const
{
    if(!m_poLayer)
        return wxEmptyString;

    OGRFeatureDefn* const pDef = GetDefinition();
    wxString sOut;
    if (NULL != pDef)
    {
        sOut = GetConvName(pDef->GetName(), false);
        if (sOut.IsEmpty())
        {
            sOut = GetConvName(CPLGetBasename(m_sPath), false);
        }
    }
    else
    {
        sOut = GetConvName(CPLGetBasename(m_sPath), false);
    }
    return sOut;
}

bool wxGISTable::Open(bool bUpdate, bool bShared)
{
    return Open(0, bUpdate, bShared, true, NULL);
}

bool wxGISTable::Open(int iLayer, bool bUpdate, bool bShared, bool bCache, ITrackCancel* const pTrackCancel)
{
	if(m_bIsOpened)
		return true;

    if (m_nSubType == enumTableDBF)
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

	wxCriticalSectionLocker locker(m_CritSect);

	m_bIsReadOnly = !bUpdate;

    if(!m_poLayer)
	{
		if(m_nSubType == enumTableQueryResult)
			bUpdate = FALSE;

		m_poDS = (OGRDataSource*)wxGISDataset::OpenInternal( m_sPath, bUpdate, bShared );
		//bug in FindFileInZip() [gdal-root\port\cpl_vsil_gzip.cpp]
        if (m_poDS == NULL)
        {
            if (m_bIsReadOnly)
            {
			    m_poDS = (OGRDataSource*)wxGISDataset::OpenInternal( FixPathSeparator(m_sPath), true,  bShared );
            }
            else
            {
		        m_poDS = (OGRDataSource*)wxGISDataset::OpenInternal( m_sPath, false, bShared );
                m_bIsReadOnly = true;
            }
        }

		if( m_poDS == NULL )
		{
			const char* err = CPLGetLastErrorMsg();
            wxString sEncodedPath = wxString(m_sPath, wxConvUTF8);
            wxURI oURLtoUnscape(sEncodedPath);
            sEncodedPath = oURLtoUnscape.BuildUnescapedURI();
			wxString sErr = wxString::Format(_("Table open failed! Path '%s'. OGR error: %s"), sEncodedPath.c_str(), wxString(err, wxConvUTF8).c_str());
			wxLogError(sErr);
			if(pTrackCancel)
				pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
			return false;
		}

		int nLayerCount = m_poDS->GetLayerCount();
		if(nLayerCount == 1)
        {
			m_poLayer = m_poDS->GetLayer(iLayer);
        }
		else
        {
			m_nType = enumGISContainer;
        }
	}

    SetInternalValues();

    if (IsContainer())
    {
        m_nType = enumGISContainer;
    }

	m_bIsOpened = true;

    if (bCache)
    {
		Cache(pTrackCancel);
    }

	return true;
}

bool wxGISTable::IsContainer() const
{
    //for file datasources only
    return m_nSubType == enumTableODS || m_nSubType == enumTableXLS || m_nSubType == enumTableXLSX;
}

size_t wxGISTable::GetSubsetsCount(void) const
{
    if (m_poDS && m_nType == enumGISContainer)
        return  m_poDS->GetLayerCount();
    return 0;
}

wxGISDataset* wxGISTable::GetSubset(size_t nIndex)
{
    if(m_poDS)
    {
	    OGRLayer* poLayer = m_poDS->GetLayer(nIndex);
        if(poLayer)
        {
			CPLString szPath;
			szPath.Printf("%s#%d", m_sPath.c_str(), nIndex);
			wxGISTable* pDataSet = new wxGISTable(szPath, m_nSubType, poLayer, m_poDS);
            pDataSet->SetEncoding(m_Encoding);
            return static_cast<wxGISDataset*>(pDataSet);
        }
    }
    return NULL;
}

wxGISDataset* wxGISTable::GetSubset(const wxString & sSubsetName)
{
    if(m_poDS)
    {
        CPLString szSubsetName(sSubsetName.ToUTF8());
        OGRLayer* poLayer = m_poDS->GetLayerByName(szSubsetName);
        if(poLayer)
        {
			CPLString szPath;
			szPath.Printf("%s#%s", m_sPath.c_str(), szSubsetName.c_str());
			wxGISTable* pDataSet = new wxGISTable(szPath, m_nSubType, poLayer, m_poDS);
            pDataSet->SetEncoding(m_Encoding);
            return static_cast<wxGISDataset*>(pDataSet);
        }
    }
    return NULL;

}

void wxGISTable::SetInternalValues()
{
    if(NULL == m_poLayer)
        return;
    //set string encoding
    //TODO: if cpg is not exist get encoding from header of dbf or etc. and write to cpg else reade from cpg
#ifdef CPL_RECODE_ICONV
    m_bOLCStringsAsUTF8 = m_poLayer->TestCapability(OLCStringsAsUTF8) == TRUE;
#else
    m_bOLCStringsAsUTF8 = false;
#endif // CPL_RECODE_ICONV


    m_bOLCFastFeatureCount = m_poLayer->TestCapability(OLCFastFeatureCount) == TRUE;

    //TODO: need strategy to decide if GDAL can encode data or not
    if (m_bOLCStringsAsUTF8)
    {
        //override default encoding
        wxFontEncoding FEnc = GetEncodingFromCpg(m_sPath);
        if (FEnc != wxFONTENCODING_DEFAULT)
            m_Encoding = FEnc;
        else
            m_Encoding = wxFONTENCODING_UTF8;
    }
    else
    {
        if(m_nType == enumGISFeatureDataset)
        {
            switch(m_nSubType)
            {
            case enumVecKML:
            case enumVecKMZ:
            case enumVecGML:
                m_nCurrentFID = 1;//TODO: check if kml FID starts with 1
                m_Encoding = wxFONTENCODING_UTF8;
                break;
            case enumVecPostGIS://TODO: check encoding
                m_Encoding = wxFONTENCODING_UTF8;
                break;
            case enumVecESRIShapefile:
                m_nCurrentFID = 0;
            case enumVecDXF:
            default:
                {

                    wxFontEncoding FEnc = GetEncodingFromCpg(m_sPath);
                    if (FEnc != wxFONTENCODING_DEFAULT)
                        m_Encoding = FEnc;
                    else
                        m_Encoding = wxLocale::GetSystemEncoding();
                }
            }
        }
        else if(m_nType == enumGISTableDataset)
        {
            {

                wxFontEncoding FEnc = GetEncodingFromCpg(m_sPath);
                if (FEnc != wxFONTENCODING_DEFAULT)
                    m_Encoding = FEnc;
                else
                    m_Encoding = wxLocale::GetSystemEncoding();
            }
        }
    }

    if(m_bOLCFastFeatureCount)
    {
        m_nFeatureCount = m_poLayer->GetFeatureCount(FALSE);
    }

    m_bHasFID = !GetFIDColumn().IsEmpty();
}

wxString wxGISTable::GetFIDColumn(void) const
{
    OGRFeatureDefn* pDefn = GetDefinition();
    if(pDefn)
    {
        CPLString szFIDCOLName = m_poLayer->GetFIDColumn();
        return wxString(szFIDCOLName, wxConvUTF8);
        //int nFIDColIndex = pDefn->GetFieldIndex(szFIDCOLName);
        //return nFIDColIndex;
    }
    return wxEmptyString;
}

void wxGISTable::Cache(ITrackCancel* pTrackCancel)
{
    m_bIsCached = true;
}

void wxGISTable::Close(void)
{
    if(IsOpened())
    {
	    m_poLayer = NULL;
        if(m_poDS)
	    {
		    if(m_nSubType == enumTableQueryResult && m_poLayer)
			    m_poDS->ReleaseResultSet(m_poLayer);
		    if( m_poDS->Dereference() <= 0)
			    OGRCompatibleClose( m_poDS );
	        m_poDS = NULL;
	    }

	    m_Encoding = wxLocale::GetSystemEncoding();

	    m_bHasFID = false;
        m_bHasFilter = false;

        m_nFeatureCount = wxNOT_FOUND;

        m_bOLCFastFeatureCount = false;
        m_bOLCStringsAsUTF8 = false;
    }
    wxGISDataset::Close();

    //Send event
    wxFeatureDSEvent event(wxDS_CLOSED);
    AddEvent(event);
}

size_t wxGISTable::GetFeatureCount(bool bForce, ITrackCancel* const pTrackCancel)
{
    wxCriticalSectionLocker locker(m_CritSect);
    if (!bForce && m_nFeatureCount != wxNOT_FOUND)
		return m_nFeatureCount;
    if(	m_poLayer )
    {
        if(bForce)
            m_nFeatureCount = m_poLayer->GetFeatureCount(TRUE);
        else if(m_bOLCFastFeatureCount)
            m_nFeatureCount = m_poLayer->GetFeatureCount(FALSE);
        else
        {
            m_nFeatureCount = m_poLayer->GetFeatureCount(FALSE);
            if(m_nFeatureCount == -1)
                m_nFeatureCount = m_poLayer->GetFeatureCount(TRUE);
            if(m_nFeatureCount == -1)
                return 0;
        }

		return m_nFeatureCount;
    }
    return 0;
}

bool wxGISTable::CanDeleteFeature(void) const
{
    if(!IsOpened())
        return false;
	return m_nSubType != enumTableQueryResult && m_poLayer && m_poLayer->TestCapability(OLCDeleteFeature);
}

bool wxGISTable::CanDeleteField(void) const
{
    if(!IsOpened())
        return false;
    return m_nSubType != enumTableQueryResult && m_poLayer && m_poLayer->TestCapability(OLCDeleteField);
}

OGRErr wxGISTable::DeleteField(int nIndex)
{
    if (!CanDeleteField())
		return OGRERR_UNSUPPORTED_OPERATION;
    OGRErr eErr = m_poLayer->DeleteField(nIndex);

    //PostEvent(new wxFeatureDSEvent(wxDS_FEATURE_DELETED, nIndex)); TODO:

    return eErr;

}


OGRErr wxGISTable::DeleteFeature(long nFID)
{
	if(!CanDeleteFeature())
		return OGRERR_UNSUPPORTED_OPERATION;

	OGRErr eErr = m_poLayer->DeleteFeature(nFID);
    if(eErr != OGRERR_NONE)
        return eErr;

	m_nFeatureCount--;

    PostEvent(new wxFeatureDSEvent(wxDS_FEATURE_DELETED, nFID));

    return eErr;
}

OGRErr wxGISTable::StoreFeature(wxGISFeature &Feature)
{
    if(!m_poLayer)
        return OGRERR_FAILURE;

    wxCriticalSectionLocker locker(m_CritSect);
    OGRErr eErr = m_poLayer->CreateFeature(Feature);
    if(eErr == OGRERR_NONE)
    {
		m_nFeatureCount++;

        if(!m_bHasFID && Feature.GetFID() == -1)
            Feature.SetFID(m_nFeatureCount);
    }

    PostEvent(new wxFeatureDSEvent(wxDS_FEATURE_ADDED, Feature.GetFID()));

    return eErr;
}

wxGISFeature wxGISTable::CreateFeature(void)
{
    if (NULL == m_poLayer)
    {
        return wxGISFeature();
    }
    OGRFeature* poFeature = OGRFeature::CreateFeature( GetDefinition() );
    if (NULL == poFeature)
    {
        return wxGISFeature();
    }

    wxGISFeature Feature(poFeature, m_Encoding, m_bRecodeToSystem);
	return Feature;
}

OGRErr wxGISTable::SetFeature(const wxGISFeature &Feature)
{
    if(!m_poLayer || m_nSubType == enumTableQueryResult || !Feature.IsOk())
		return OGRERR_FAILURE;

	OGRErr eErr = m_poLayer->SetFeature(Feature);

    PostEvent(new wxFeatureDSEvent(wxDS_FEATURE_CHANGED, Feature.GetFID()));

    return eErr;
}

OGRErr wxGISTable::CommitTransaction(void)
{
    if (!m_poLayer || m_nSubType == enumTableQueryResult)
        return OGRERR_FAILURE;
    OGRErr eErr = m_poLayer->CommitTransaction();

    return eErr;
}

OGRErr wxGISTable::StartTransaction(void)
{
    if (!m_poLayer || m_nSubType == enumTableQueryResult)
        return OGRERR_FAILURE;
    OGRErr eErr = m_poLayer->StartTransaction();

    return eErr;
}

OGRErr wxGISTable::RollbackTransaction(void)
{
    if (!m_poLayer || m_nSubType == enumTableQueryResult)
        return OGRERR_FAILURE;
    OGRErr eErr = m_poLayer->RollbackTransaction();

    return eErr;
}

wxGISFeature wxGISTable::GetFeatureByID(long nFID)
{
    if(m_poLayer)
	{
		OGRFeature* pFeature = m_poLayer->GetFeature(nFID);
		if(pFeature)
		{
            return wxGISFeature(pFeature, m_Encoding, m_bRecodeToSystem);
		}
	}
	return wxGISFeature();
}

wxGISFeature wxGISTable::GetFeature(long nIndex)
{
    if(!m_poLayer)
    {
        return wxGISFeature();
    }
    if(m_poLayer->SetNextByIndex(nIndex) != OGRERR_NONE)
    {
        return wxGISFeature();
    }
    if(!HasFID())
    {
        m_nCurrentFID = nIndex;
    }

    return Next();
}

void wxGISTable::Reset(void)
{
    if(!m_poLayer)
        return;
    m_nCurrentFID = 0;
    m_poLayer->ResetReading();
}

wxGISFeature wxGISTable::Next(void)
{
    if(!m_poLayer)
    {
        return wxGISFeature();
    }
    OGRFeature* pFeature = m_poLayer->GetNextFeature();
    if(!pFeature)
    {
        return wxGISFeature();
    }
    if(!HasFID())
    {
        pFeature->SetFID(m_nCurrentFID++);
    }
    return wxGISFeature(pFeature, m_Encoding, m_bRecodeToSystem);
}

wxFeatureCursor wxGISTable::Search(const wxGISQueryFilter &QFilter, bool bOnlyFirst, ITrackCancel* const pTrackCancel)
{
    wxFeatureCursor oOutCursor(this);
    if(	!m_poLayer )
		return oOutCursor;

    IProgressor* pProgressor(NULL);
    if (pTrackCancel)
    {
        pProgressor = pTrackCancel->GetProgressor();
    }

    int nCounter(0);
    int nRange = 100;
    if (NULL != pProgressor)
    {
        if (bOnlyFirst)
        {
            nRange = 1;
        }
        else
        {
            nRange = GetFeatureCount(false, pTrackCancel);
        }
        pProgressor->SetRange(nRange);
    }

	OGRErr eErr;
    if(QFilter.GetWhereClause().IsEmpty())
	{
        m_poLayer->ResetReading();
		//create and fill cursor
		OGRFeature* pFeature = NULL;
        while((pFeature = m_poLayer->GetNextFeature()) != NULL)
		{
            if (pTrackCancel && !pTrackCancel->Continue())
            {
                wxString sErr(_("Interrupted by user"));
                CPLString sFullErr(sErr.mb_str());
                CPLError(CE_Warning, CPLE_AppDefined, sFullErr);

                if (pTrackCancel)
                {
                    pTrackCancel->PutMessage(wxString(sFullErr), wxNOT_FOUND, enumGISMessageErr);
                }
                oOutCursor.Reset();
                return oOutCursor;
            }


            if (NULL != pProgressor)
            {
               pProgressor->SetValue(nCounter++);
            }
            oOutCursor.Add(wxGISFeature(pFeature, m_Encoding, m_bRecodeToSystem));
			if(bOnlyFirst)
				break;
		}

		oOutCursor.Reset();
		return oOutCursor;
	}
    else
	{
		wxString sFilter = QFilter.GetWhereClause();
		//if(!m_sCurrentFilter.IsEmpty() && !sFilter.IsEmpty())
		//{
		//	//combain two clauses
		//	sFilter.Prepend(wxT("(") + m_sCurrentFilter + wxT(") AND ("));
		//	sFilter.Append(wxT(")"));
		//}
        eErr = m_poLayer->SetAttributeFilter(sFilter.ToUTF8());
		if(eErr != OGRERR_NONE)
			return oOutCursor;
		//create and fill cursor
		m_poLayer->ResetReading();
		OGRFeature* poFeature = NULL;
		while((poFeature = m_poLayer->GetNextFeature()) != NULL )
		{
            if (pTrackCancel && !pTrackCancel->Continue())
            {
                wxString sErr(_("Interrupted by user"));
                CPLString sFullErr(sErr.mb_str());
                CPLError(CE_Warning, CPLE_AppDefined, sFullErr);

                if (pTrackCancel)
                {
                    pTrackCancel->PutMessage(wxString(sFullErr), wxNOT_FOUND, enumGISMessageErr);
                }
                oOutCursor.Reset();
                m_poLayer->SetAttributeFilter(NULL);
                return oOutCursor;
            }

            if (NULL != pProgressor)
            {
                pProgressor->SetValue(nCounter++);
            }

            oOutCursor.Add(wxGISFeature(poFeature, m_Encoding, m_bRecodeToSystem));
			if(bOnlyFirst)
				break;
		}

		oOutCursor.Reset();

        //set full selection back
        m_poLayer->SetAttributeFilter(NULL);

		return oOutCursor;
	}

    return oOutCursor;
}

OGRFeatureDefn* const wxGISTable::GetDefinition(void) const
{
    if (m_poLayer)
    {
        return m_poLayer->GetLayerDefn();
    }
    return NULL;
}

OGRFeatureDefn* const wxGISTable::GetDefinition(void)
{
    if (m_poLayer)
    {
        return m_poLayer->GetLayerDefn();
    }
    return NULL;
}

wxArrayString wxGISTable::GetFieldNames() const
{
    wxArrayString saFields;
    OGRFeatureDefn * const pDef = GetDefinition();
    if(pDef)
    {
        for(int i = 0; i < pDef->GetFieldCount(); ++i)
        {
            wxString sFieldName(pDef->GetFieldDefn(i)->GetNameRef(), wxConvUTF8);
            if(!sFieldName.IsEmpty())
                saFields.Add(sFieldName);
        }
    }

    return saFields;
}

wxString wxGISTable::GetFieldName(int nIndex) const
{
    OGRFeatureDefn * const pDef = GetDefinition();
    if (NULL != pDef)
    {
        OGRFieldDefn* pFDefn = pDef->GetFieldDefn(nIndex);
        if (pFDefn)
        {
            return wxString(pFDefn->GetNameRef(), wxConvUTF8);
        }
    }
    return wxEmptyString;
}

OGRErr wxGISTable::SetFilter(const wxGISQueryFilter &QFilter)
{
    if (NULL == m_poLayer)
    {
		return OGRERR_FAILURE;
    }

	OGRErr eErr;
    if (QFilter.GetWhereClause().IsEmpty())
    {
        eErr = m_poLayer->SetAttributeFilter(NULL);
        m_nFeatureCount = wxNOT_FOUND;
        GetFeatureCount(true);
        m_bHasFilter = false;
        return eErr;
    }
    else
	{
        eErr = m_poLayer->SetAttributeFilter(QFilter.GetWhereClause().ToUTF8());
        m_nFeatureCount = wxNOT_FOUND;
        GetFeatureCount(true);
        m_bHasFilter = true;
        return eErr;
	}
}


OGRErr wxGISTable::SetIgnoredFields(const wxArrayString &saIgnoredFields)
{
    if(	m_poLayer )
    {
        bool bOLCIgnoreFields = m_poLayer->TestCapability(OLCIgnoreFields) != 0;
        if(!bOLCIgnoreFields)
            return OGRERR_UNSUPPORTED_OPERATION;

        char **papszIgnoredFields = NULL;
        for(size_t i = 0; i < saIgnoredFields.GetCount(); ++i)
            papszIgnoredFields = CSLAddString( papszIgnoredFields, saIgnoredFields[i].ToUTF8() );

        OGRErr eErr = m_poLayer->SetIgnoredFields((const char**)papszIgnoredFields);
        CSLDestroy( papszIgnoredFields );

        return eErr;
    }
    return OGRERR_FAILURE;
}

char **wxGISTable::GetFileList()
{
    char **papszFileList = NULL;
    CPLString szPath;
    //papszFileList = CSLAddString( papszFileList, osIMDFile );
    switch(m_nSubType)
    {
    case enumTableMapinfoTab:
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
	case enumTableMapinfoMif:
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
	case enumTablePostgres:
	case enumTableQueryResult:
	case enumTableDBF:
    default:
        break;
    }

    szPath = (char*)CPLResetExtension(m_sPath, "cpg");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );
    szPath = (char*)CPLResetExtension(m_sPath, "prj");
    if(CPLCheckForFile((char*)szPath.c_str(), NULL))
        papszFileList = CSLAddString( papszFileList, szPath );

    return papszFileList;
}

wxFontEncoding wxGISTable::GetEncoding(void) const
{
     return m_Encoding;
}

void wxGISTable::SetEncoding(const wxFontEncoding &oEncoding)
{
    if(oEncoding == wxFONTENCODING_DEFAULT)
    {
        m_bRecodeToSystem = false;
        return SetInternalValues();
    }
    else if (m_nSubType == enumTableDBF)
    {
        wxString sEnc = wxLocale::GetSystemEncodingName();
        const char* sz_enc = sEnc.mb_str();
        m_bRecodeToSystem = wxGISEQUAL(CPLGetConfigOption("SHAPE_ENCODING", sz_enc), sz_enc);
    }
    m_Encoding = oEncoding;
}

//--------------------------------------------------------------------------------
// wxGISTableCached
//--------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISTableCached, wxGISTable)

wxGISTableCached::wxGISTableCached(const CPLString &sPath, int nSubType, OGRLayer* poLayer, OGRCompatibleDataSource* poDS) : wxGISTable(sPath, nSubType, poLayer, poDS)
{
}

wxGISTableCached::~wxGISTableCached()
{
}

void wxGISTableCached::Cache(ITrackCancel* const pTrackCancel)
{
    if(IsCached())
        return;

    m_omFeatures.clear();

    if(!m_poLayer)
        return;
    m_bIsCaching = true;
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
        //    wxGISGDALClose(poFeature);
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

		if(pTrackCancel && !pTrackCancel->Continue())
		{
			if(pProgress)
				pProgress->ShowProgress(false);
            m_bIsCaching = false;
			return;
		}
		m_nFeatureCount = m_omFeatures.size();
    }


	if(pProgress)
	{
		pProgress->Stop();
		pProgress->ShowProgress(false);
	}
    m_bIsCaching = false;
    m_bIsCached = true;
}

bool wxGISTableCached::IsCaching(void) const
{
    return m_bIsCaching;
}

void wxGISTableCached::Close(void)
{
    m_omFeatures.clear();
    wxGISTable::Close();
}

void wxGISTableCached::Reset(void)
{
    m_nCurrentFID = 1;
}

wxGISFeature wxGISTableCached::Next(void)
{
    if(m_omFeatures.empty())
    {
        return wxGISFeature();
    }

    return m_omFeatures[m_nCurrentFID++];
}

wxGISFeature wxGISTableCached::GetFeature(long nIndex)
{
    if(!m_poLayer)
    {
        return wxGISFeature();
    }

    m_nCurrentFID = nIndex + 1;

    return Next();
}

size_t wxGISTableCached::GetFeatureCount(bool bForce, ITrackCancel* const pTrackCancel)
{
    wxCriticalSectionLocker locker(m_CritSect);
    if (!bForce && m_nFeatureCount != wxNOT_FOUND)
		return m_nFeatureCount;
    if(	m_poLayer )
    {
        if(bForce)
        {
            m_bIsCached = false;
            Cache(pTrackCancel);
        }
        else if(m_bOLCFastFeatureCount)
            m_nFeatureCount = m_poLayer->GetFeatureCount(FALSE);
        else
        {
        	m_nFeatureCount = m_poLayer->GetFeatureCount(FALSE);
            if(m_nFeatureCount == wxNOT_FOUND)
            {
                m_bIsCached = false;
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

OGRErr wxGISTableCached::DeleteFeature(long nFID)
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

OGRErr wxGISTableCached::StoreFeature(wxGISFeature &Feature)
{
    OGRErr eErr = wxGISTable::StoreFeature(Feature);
    if(eErr != OGRERR_NONE)
    {
        return eErr;
    }

    m_omFeatures[Feature.GetFID()] = Feature;
    return eErr;
}

OGRErr wxGISTableCached::SetFeature(const wxGISFeature &Feature)
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

wxGISFeature wxGISTableCached::GetFeatureByID(long nFID)
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


wxFeatureCursor wxGISTableCached::Search(const wxGISQueryFilter &QFilter, bool bOnlyFirst, ITrackCancel* const pTrackCancel)
{
    wxFeatureCursor oOutCursor(this);

    IProgressor* pProgressor(NULL);
    if (pTrackCancel)
    {
        pProgressor = pTrackCancel->GetProgressor();
    }

    int nCounter(0);
    int nRange = 100;
    if (NULL != pProgressor)
    {
        if (bOnlyFirst)
        {
            nRange = 1;
        }
        else
        {
            nRange = GetFeatureCount(false, pTrackCancel);
        }
        pProgressor->SetRange(nRange);
    }

    OGRErr eErr;
    if (QFilter.GetWhereClause().IsEmpty())
    {
        for (std::map<long, wxGISFeature>::iterator it = m_omFeatures.begin(); it != m_omFeatures.end(); ++it)
        {
            if (pTrackCancel && !pTrackCancel->Continue())
            {
                wxString sErr(_("Interrupted by user"));
                CPLString sFullErr(sErr.mb_str());
                CPLError(CE_Warning, CPLE_AppDefined, sFullErr);

                if (pTrackCancel)
                {
                    pTrackCancel->PutMessage(wxString(sFullErr), wxNOT_FOUND, enumGISMessageErr);
                }
                oOutCursor.Reset();
                return oOutCursor;
            }


            if (NULL != pProgressor)
            {
                pProgressor->SetValue(nCounter++);
            }
            oOutCursor.Add(it->second);
            if (bOnlyFirst)
                break;
        }

        oOutCursor.Reset();
        return oOutCursor;
    }
    else
    {
        if (!m_poLayer)
            return oOutCursor;
        wxString sFilter = QFilter.GetWhereClause();
        //if(!m_sCurrentFilter.IsEmpty() && !sFilter.IsEmpty())
        //{
        //	//combain two clauses
        //	sFilter.Prepend(wxT("(") + m_sCurrentFilter + wxT(") AND ("));
        //	sFilter.Append(wxT(")"));
        //}
        eErr = m_poLayer->SetAttributeFilter(sFilter.ToUTF8());
        if (eErr != OGRERR_NONE)
            return oOutCursor;
        //create and fill cursor
        m_poLayer->ResetReading();
        OGRFeature* poFeature = NULL;
        while ((poFeature = m_poLayer->GetNextFeature()) != NULL)
        {
            if (pTrackCancel && !pTrackCancel->Continue())
            {
                wxString sErr(_("Interrupted by user"));
                CPLString sFullErr(sErr.mb_str());
                CPLError(CE_Warning, CPLE_AppDefined, sFullErr);

                if (pTrackCancel)
                {
                    pTrackCancel->PutMessage(wxString(sFullErr), wxNOT_FOUND, enumGISMessageErr);
                }
                oOutCursor.Reset();
                m_poLayer->SetAttributeFilter(NULL);
                return oOutCursor;
            }

            if (NULL != pProgressor)
            {
                pProgressor->SetValue(nCounter++);
            }

            oOutCursor.Add(wxGISFeature(poFeature, m_Encoding, m_bRecodeToSystem));
            if (bOnlyFirst)
                break;
        }

        oOutCursor.Reset();

        //set full selection back
        m_poLayer->SetAttributeFilter(NULL);

        return oOutCursor;
    }

    return oOutCursor;
}
//--------------------------------------------------------------------------------
// wxGISTableQuery
//--------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISTableQuery, wxGISTableCached)

wxGISTableQuery::wxGISTableQuery(const CPLString &sPath, int nSubType, OGRLayer* poLayer, OGRCompatibleDataSource* poDS) : wxGISTableCached(sPath, nSubType, poLayer, poDS)
{
}

wxGISTableQuery::~wxGISTableQuery()
{
}


void wxGISTableQuery::Cache(ITrackCancel* const pTrackCancel)
{
    if (!m_poLayer)
        return;
    m_bIsCaching = true;
    m_poLayer->ResetReading();

    //loading
    m_nCurrentFID = 1;

    OGRFeature *poFeature;
    while ((poFeature = m_poLayer->GetNextFeature()) != NULL)
    {

        long nFID;
        if (poFeature && !m_bHasFID)
        {
            nFID = m_nCurrentFID;
            poFeature->SetFID(nFID);
        }
        else
            nFID = poFeature->GetFID();

        //store features in array for speed
        m_omFeatures[nFID] = wxGISFeature(poFeature, m_Encoding, m_bRecodeToSystem);
        m_nCurrentFID++;

    }


    m_nFeatureCount = m_omFeatures.size();
    m_bIsCaching = false;
    m_bIsCached = true;

    if (m_poDS)
    {
        m_poDS->ReleaseResultSet(m_poLayer);
        m_poLayer = NULL;
        if (m_poDS->Dereference() <= 0)
            OGRCompatibleClose(m_poDS);
        m_poDS = NULL;
    }
}

size_t wxGISTableQuery::GetFeatureCount(bool bForce, ITrackCancel* const pTrackCancel)
{
    wxCriticalSectionLocker locker(m_CritSect);
    return m_nFeatureCount;
}

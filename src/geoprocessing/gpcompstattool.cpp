/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  compute raster statistics tools.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/geoprocessing/gpcompstattool.h"
#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/datasource/rasterdataset.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPOrthoCorrectTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPCompStatTool, wxGISGPTool)

wxGISGPCompStatTool::wxGISGPCompStatTool(void) : wxGISGPTool()
{
}

wxGISGPCompStatTool::~wxGISGPCompStatTool(void)
{
}

const wxString wxGISGPCompStatTool::GetDisplayName(void)
{
    return wxString(_("Compute statistics"));
}

const wxString wxGISGPCompStatTool::GetName(void)
{
    return wxString(wxT("comp_stats"));
}

const wxString wxGISGPCompStatTool::GetCategory(void)
{
    return wxString(_("Data Management Tools/Raster"));
}

GPParameters wxGISGPCompStatTool::GetParameterInfo(void)
{
    if(m_paParam.IsEmpty())
    {
        //src path
        wxGISGPParameter* pParam1 = new wxGISGPParameter();
        pParam1->SetName(wxT("src_path"));
        pParam1->SetDisplayName(_("Source raster"));
        pParam1->SetParameterType(enumGISGPParameterTypeRequired);
        pParam1->SetDataType(enumGISGPParamDTPath);
        pParam1->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain1 = new wxGISGPGxObjectDomain();
        pDomain1->AddFilter(new wxGxDatasetFilter(enumGISRasterDataset));
        pParam1->SetDomain(pDomain1);

        m_paParam.Add(static_cast<IGPParameter*>(pParam1));

        //build approx
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("approx_ok"));
        pParam2->SetDisplayName(_("Compute statistics based on overviews or a subset of all tiles"));
        pParam2->SetParameterType(enumGISGPParameterTypeOptional);
        pParam2->SetDataType(enumGISGPParamDTBool);
        pParam2->SetDirection(enumGISGPParameterDirectionInput);
        pParam2->SetValue(false);

        m_paParam.Add(static_cast<IGPParameter*>(pParam2));
    }
    return m_paParam;
}

bool wxGISGPCompStatTool::Validate(void)
{
    return true;
}

bool wxGISGPCompStatTool::Execute(ITrackCancel* pTrackCancel)
{
    if(!Validate())
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Unexpected error occurred"), -1, enumGISMessageErr);
        return false;
    }

    IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
    if(!pGxObjectContainer)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting catalog object"), -1, enumGISMessageErr);
        return false;
    }

    wxString sSrcPath = m_paParam[0]->GetValue();
    IGxObject* pGxObject = pGxObjectContainer->SearchChild(sSrcPath);
    if(!pGxObject)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting source object"), -1, enumGISMessageErr);
        return false;
    }
    IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pGxObject);
    if(!pGxDataset)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Source object is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }
    wxGISRasterDatasetSPtr pSrcDataSet = boost::dynamic_pointer_cast<wxGISRasterDataset>(pGxDataset->GetDataset());

    if(!pSrcDataSet)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Source dataset is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }

	if(!pSrcDataSet->Open(false))
		return false;

    GDALDataset* poGDALDataset = pSrcDataSet->GetRaster();
    if(!poGDALDataset)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting raster"), -1, enumGISMessageErr);
        return false;
    }

    bool bApproxOK = m_paParam[1]->GetValue();

    for(int nBand = 0; nBand < poGDALDataset->GetRasterCount(); nBand++ )
    {
        double      dfMin(0), dfMax(255), dfMean(127), dfStdDev(2);
        CPLErr      eErr;

        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("Proceed band %d"), nBand + 1), -1, enumGISMessageInfo);
        GDALRasterBand* pBand = poGDALDataset->GetRasterBand(nBand + 1);
        eErr = pBand->ComputeStatistics(bApproxOK, &dfMin, &dfMax, &dfMean, &dfStdDev, ExecToolProgress, (void*)pTrackCancel);   
	    if(eErr != CE_None)
	    {
            if(pTrackCancel)
            {
                const char* pszErr = CPLGetLastErrorMsg();
				pTrackCancel->PutMessage(wxString::Format(_("ComputeStatistics failed! GDAL error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
            }
            return false;
        }
        pTrackCancel->PutMessage(wxString::Format(_("Band %d: min - %.2f, max - %.2f, mean - %.2f, StdDev - %.2f"), nBand + 1, dfMin, dfMax, dfMean, dfStdDev), -1, enumGISMessageNorm);
     //   eErr = pBand->SetStatistics(dfMin, dfMax, dfMean, dfStdDev);   
	    //if(eErr != CE_None)
	    //{
     //       if(pTrackCancel)
     //       {
     //           const char* pszErr = CPLGetLastErrorMsg();
     //           pTrackCancel->PutMessage(wxString::Format(_("ComputeStatistics failed! GDAL error: %s"), wgMB2WX(pszErr)), -1, enumGISMessageErr);
     //       }
     //       return false;
     //   }
    }
    //poGDALDataset->FlushCache();

    pSrcDataSet->SetHasStatistics(true);

    return true;
}

//TODO: PostExecute
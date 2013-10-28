/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  get subraster from vector cutline geoprocessing tool.
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

#include "wxgis/geoprocessing/gpsubrasterbyvectortool.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/geoprocessing/gpraster.h"
#include "wxgis/catalog/gxfilters.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPSubRasterByVectorTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPSubRasterByVectorTool, wxGISGPTool)

wxGISGPSubRasterByVectorTool::wxGISGPSubRasterByVectorTool(void) : wxGISGPTool()
{
}

wxGISGPSubRasterByVectorTool::~wxGISGPSubRasterByVectorTool(void)
{
}

const wxString wxGISGPSubRasterByVectorTool::GetDisplayName(void)
{
    return wxString(_("Create subraster by vector cutline"));
}

const wxString wxGISGPSubRasterByVectorTool::GetName(void)
{
    return wxString(wxT("subraster_by_vector"));
}

const wxString wxGISGPSubRasterByVectorTool::GetCategory(void)
{
    return wxString(_("Data Management Tools/Raster"));
}

GPParameters wxGISGPSubRasterByVectorTool::GetParameterInfo(void)
{
    if(m_paParam.IsEmpty())
    {
        //src vector path
        wxGISGPParameter* pParam1 = new wxGISGPParameter();
        pParam1->SetName(wxT("src_vector_path"));
        pParam1->SetDisplayName(_("Source vector"));
        pParam1->SetParameterType(enumGISGPParameterTypeRequired);
        pParam1->SetDataType(enumGISGPParamDTPath);
        pParam1->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain1 = new wxGISGPGxObjectDomain();
        AddAllVectorFilters(pDomain1);
        pParam1->SetDomain(pDomain1);

        m_paParam.Add(static_cast<IGPParameter*>(pParam1));

        //src raster path
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("src_raster_path"));
        pParam2->SetDisplayName(_("Source raster"));
        pParam2->SetParameterType(enumGISGPParameterTypeRequired);
        pParam2->SetDataType(enumGISGPParamDTPath);
        pParam2->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain2 = new wxGISGPGxObjectDomain();
		AddAllRasterFilters(pDomain2);
        pParam2->SetDomain(pDomain2);

        m_paParam.Add(static_cast<IGPParameter*>(pParam2));

        //dst path
        wxGISGPParameter* pParam3 = new wxGISGPParameter();
        pParam3->SetName(wxT("dst_path"));
        pParam3->SetDisplayName(_("Destination folder"));
        pParam3->SetParameterType(enumGISGPParameterTypeRequired);
        pParam3->SetDataType(enumGISGPParamDTFolderPath);
        pParam3->SetDirection(enumGISGPParameterDirectionOutput);

        wxGISGPGxObjectDomain* pDomain3 = new wxGISGPGxObjectDomain();
		pDomain3->AddFilter(new wxGxFolderFilter());
        pParam3->SetDomain(pDomain3);

		m_paParam.Add(static_cast<IGPParameter*>(pParam3));

		//TODO: create group

        //file name mask or field from vector file

		//output raster type and parameters compres & etc.

		//nodata value

		//band list

		//SQL select

        //wxGxRasterFilter* pFilter = NULL, GDALDataType eOutputType = GDT_Unknown, int nBandCount = 0, int *panBandList = NULL, bool bUseCounter = true, int nCounterBegin = -1, int nFieldNo = -1, double dfOutResX = -1, double dfOutResY = -1, bool bCopyNodata = false, bool bSkipSourceMetadata = false, char** papszOptions = NULL

    }
    return m_paParam;
}

bool wxGISGPSubRasterByVectorTool::Validate(void)
{
    //if(!m_paParam[1]->GetAltered())
    //{
    //    if(m_paParam[0]->GetIsValid())
    //    {
    //        //generate temp name
    //        wxString sPath = m_paParam[0]->GetValue();
    //        wxFileName Name(sPath);
    //        Name.SetName(Name.GetName() + wxT("_ortho"));
    //        m_paParam[1]->SetValue(wxVariant(Name.GetFullPath(), wxT("path")));
    //        m_paParam[1]->SetAltered(true);//??
    //    }
    //}
    return true;
}

bool wxGISGPSubRasterByVectorTool::Execute(ITrackCancel* pTrackCancel)
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

    wxString sSrcVecPath = m_paParam[0]->GetValue();
    IGxObject* pGxObject = pGxObjectContainer->SearchChild(sSrcVecPath);
    if(!pGxObject)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting source vector object"), -1, enumGISMessageErr);
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

    wxGISFeatureDatasetSPtr pSrcFeatureDataSet = boost::dynamic_pointer_cast<wxGISFeatureDataset>(pGxDataset->GetDataset());
    if(!pSrcFeatureDataSet)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Source vector dataset is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }

	wxString sSrcRasterPath = m_paParam[1]->GetValue();
    pGxObject = pGxObjectContainer->SearchChild(sSrcRasterPath);
    if(!pGxObject)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting source raster object"), -1, enumGISMessageErr);
        return false;
    }

    pGxDataset = dynamic_cast<IGxDataset*>(pGxObject);
    if(!pGxDataset)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Source object is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }

    wxGISRasterDatasetSPtr pSrcRasterDataSet = boost::dynamic_pointer_cast<wxGISRasterDataset>(pGxDataset->GetDataset());
    if(!pSrcRasterDataSet)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Source raster dataset is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }

   wxString sDstPath = m_paParam[2]->GetValue();
   IGxObject* pGxDstObject = pGxObjectContainer->SearchChild(sDstPath);
    if(!pGxDstObject)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error get destination object"), -1, enumGISMessageErr);
        return false;
    }

    CPLString szDstPath = pGxDstObject->GetInternalName();

    bool bRes = true;

    wxGxRasterFilter* pFilter = new wxGxRasterFilter(enumRasterTiff);
    GDALDataType eOutputType = GDT_Byte;
    int nBandCount = 0;
    int *panBandList = NULL;
    bool bUseCounter = false;
    int nCounterBegin = -1;
    int nFieldNo = 0;
    double dfOutResX = -1;
    double dfOutResY = -1;
    bool bCopyNodata = false;
    bool bSkipSourceMetadata = false;
    char** papszOptions = NULL;
    papszOptions = CSLAddNameValue(papszOptions, "DEST_RESAMPLING", "near");
    papszOptions = CSLAddNameValue(papszOptions, "COMPRESS", "LZW");
    papszOptions = CSLAddNameValue(papszOptions, "PREDICTOR", "2");

	bRes = SubrasterByVector(pSrcFeatureDataSet, pSrcRasterDataSet, szDstPath, pFilter, eOutputType, nBandCount, panBandList, bUseCounter, nCounterBegin, nFieldNo, dfOutResX, dfOutResY, bCopyNodata, bSkipSourceMetadata, papszOptions, pTrackCancel);

    if(pGxObjectContainer)
    {
        IGxObject* pParentLoc = pGxObjectContainer->SearchChild(sDstPath);
        if(pParentLoc)
            pParentLoc->Refresh();
    }

    return bRes;
}


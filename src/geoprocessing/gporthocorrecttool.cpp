/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  ortho correct geoprocessing tools.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "wxgis/geoprocessing/gporthocorrecttool.h"
#include "wxgis/geoprocessing/gptoolmngr.h"
#include "wxgis/geoprocessing/gpdomain.h"
#include "wxgis/geoprocessing/gpparam.h"
#include "wxgis/catalog/gxfilters.h"
#include "wxgis/catalog/catop.h"
#include "wxgis/datasource/rasterdataset.h"
#include "wxgis/framework/application.h"

/////////////////////////////////////////////////////////////////////////
// wxGISGPOrthoCorrectTool
/////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxGISGPOrthoCorrectTool, wxGISGPTool)

wxGISGPOrthoCorrectTool::wxGISGPOrthoCorrectTool(void) : wxGISGPTool()
{
}

wxGISGPOrthoCorrectTool::~wxGISGPOrthoCorrectTool(void)
{
}

const wxString wxGISGPOrthoCorrectTool::GetDisplayName(void)
{
    return wxString(_("Create Ortho-Corrected Raster"));
}

const wxString wxGISGPOrthoCorrectTool::GetName(void)
{
    return wxString(wxT("create_ortho"));
}

const wxString wxGISGPOrthoCorrectTool::GetCategory(void)
{
    return wxString(_("Data Management Tools/Raster"));
}

GPParameters wxGISGPOrthoCorrectTool::GetParameterInfo(void)
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
        pDomain1->AddFilter(new wxGxRasterFilter(enumRasterTiff));
        pDomain1->AddFilter(new wxGxRasterFilter(enumRasterTil));
        pParam1->SetDomain(pDomain1);

        m_paParam.Add(static_cast<IGPParameter*>(pParam1));

        //dst path
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("dst_path"));
        pParam2->SetDisplayName(_("Destination raster"));
        pParam2->SetParameterType(enumGISGPParameterTypeRequired);
        pParam2->SetDataType(enumGISGPParamDTPath);
        pParam2->SetDirection(enumGISGPParameterDirectionOutput);

        wxGISGPGxObjectDomain* pDomain2 = new wxGISGPGxObjectDomain();
        AddAllRasterFilters(pDomain2);
        pParam2->SetDomain(pDomain2);

        //pParam2->AddParameterDependency(wxT("src_path"));

        m_paParam.Add(static_cast<IGPParameter*>(pParam2));

        //elevation interpolation type
        wxGISGPParameter* pParam3 = new wxGISGPParameter();
        pParam3->SetName(wxT("pixel_interpol"));
        pParam3->SetDisplayName(_("Output raster interpolation"));
        pParam3->SetParameterType(enumGISGPParameterTypeOptional);
        pParam3->SetDataType(enumGISGPParamDTIntegerChoice);
        pParam3->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPValueDomain* pDomain3 = new wxGISGPValueDomain();
        pDomain3->AddValue(wxVariant(GRA_NearestNeighbour), _("Nearest neighbour (select on one input pixel)"));
        pDomain3->AddValue(wxVariant(GRA_Bilinear), _("Bilinear (2x2 kernel)"));
        pDomain3->AddValue(wxVariant(GRA_Cubic), _("Cubic Convolution Approximation (4x4 kernel)"));
        pDomain3->AddValue(wxVariant(GRA_CubicSpline), _("Cubic B-Spline Approximation (4x4 kernel)"));
        pDomain3->AddValue(wxVariant(GRA_Lanczos), _("Lanczos windowed sinc interpolation (6x6 kernel)"));
        pParam3->SetDomain(pDomain3);

        pParam3->SetValue(wxVariant(GRA_Bilinear));

        m_paParam.Add(static_cast<IGPParameter*>(pParam3));

        //DEM_raster
        wxGISGPParameter* pParam4 = new wxGISGPParameter();
        pParam4->SetName(wxT("dem_path"));
        pParam4->SetDisplayName(_("DEM raster"));
        pParam4->SetParameterType(enumGISGPParameterTypeOptional);
        pParam4->SetDataType(enumGISGPParamDTPath);
        pParam4->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain4 = new wxGISGPGxObjectDomain();
        pDomain4->AddFilter(new wxGxDatasetFilter(enumGISRasterDataset));
        pParam4->SetDomain(pDomain4);

        m_paParam.Add(static_cast<IGPParameter*>(pParam4));

        //constant_ elevation double
        wxGISGPParameter* pParam5 = new wxGISGPParameter();
        pParam5->SetName(wxT("cons_elev"));
        pParam5->SetDisplayName(_("Constant elevation value or base value to the DEM"));
        pParam5->SetParameterType(enumGISGPParameterTypeOptional);
        pParam5->SetDataType(enumGISGPParamDTDouble);
        pParam5->SetDirection(enumGISGPParameterDirectionInput);
        pParam5->SetValue(0.0);

        m_paParam.Add(static_cast<IGPParameter*>(pParam5));

        //constant_ elevation double
        wxGISGPParameter* pParam6 = new wxGISGPParameter();
        pParam6->SetName(wxT("elev_scale"));
        pParam6->SetDisplayName(_("Scaling factor to convert elevation values"));
        pParam6->SetParameterType(enumGISGPParameterTypeOptional);
        pParam6->SetDataType(enumGISGPParamDTDouble);
        pParam6->SetDirection(enumGISGPParameterDirectionInput);
        pParam6->SetValue(1.0);

        m_paParam.Add(static_cast<IGPParameter*>(pParam6));

        //elevation interpolation type
        wxGISGPParameter* pParam7 = new wxGISGPParameter();
        pParam7->SetName(wxT("elev_interpol"));
        pParam7->SetDisplayName(_("Elevation values interpolation"));
        pParam7->SetParameterType(enumGISGPParameterTypeOptional);
        pParam7->SetDataType(enumGISGPParamDTStringChoice);
        pParam7->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPValueDomain* pDomain7 = new wxGISGPValueDomain();
        pDomain7->AddValue(wxVariant(wxT("near")), _("Nearest Neighbour"));
        pDomain7->AddValue(wxVariant(wxT("bilinear")), _("Bilinear"));
        pDomain7->AddValue(wxVariant(wxT("cubic")), _("Cubic"));
        pParam7->SetDomain(pDomain7);

        pParam7->SetValue(wxVariant(wxT("bilinear")));

        m_paParam.Add(static_cast<IGPParameter*>(pParam7));

        //elevation interpolation type
        wxGISGPParameter* pParam8 = new wxGISGPParameter();
        pParam8->SetName(wxT("geoid_corr"));
        pParam8->SetDisplayName(_("Geoid correction"));
        pParam8->SetParameterType(enumGISGPParameterTypeOptional);
        pParam8->SetDataType(enumGISGPParamDTBool);
        pParam8->SetDirection(enumGISGPParameterDirectionInput);
        pParam8->SetValue(wxVariant(false));
        m_paParam.Add(static_cast<IGPParameter*>(pParam8));
    }
    return m_paParam;
}

bool wxGISGPOrthoCorrectTool::Validate(void)
{
    if(!m_paParam[1]->GetAltered())
    {
        if(m_paParam[0]->GetIsValid())
        {
            //generate temp name
            wxString sPath = m_paParam[0]->GetValue();
            wxFileName Name(sPath);
            Name.SetName(Name.GetName() + wxT("_ortho"));
            m_paParam[1]->SetValue(wxVariant(Name.GetFullPath(), wxT("path")));
            m_paParam[1]->SetAltered(true);//??
        }
    }
    return true;
}

bool wxGISGPOrthoCorrectTool::Execute(ITrackCancel* pTrackCancel)
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
	if(!pSrcDataSet->IsOpened())
		if(!pSrcDataSet->Open(true))
			return false;

    wxString sDstPath = m_paParam[1]->GetValue();

	//check overwrite & do it!
	if(!OverWriteGxObject(pGxObjectContainer->SearchChild(sDstPath), pTrackCancel))
		return false;

    wxFileName sDstFileName(sDstPath);
    wxString sPath = sDstFileName.GetPath();
    IGxObject* pGxDstObject = pGxObjectContainer->SearchChild(sPath);
    if(!pGxDstObject)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error get destination object"), -1, enumGISMessageErr);
        return false;
    }

    CPLString szPath = pGxDstObject->GetInternalName();
    wxString sName = sDstFileName.GetName();

    wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_paParam[1]->GetDomain());
	IGxObjectFilter* pFilter = pDomain->GetFilter(m_paParam[1]->GetSelDomainValue());
    if(!pFilter)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting selected destination filter"), -1, enumGISMessageErr);
        return false;
    }

    wxString sDriver = pFilter->GetDriver();
    wxString sExt = pFilter->GetExt();
    int nNewSubType = pFilter->GetSubType();

    GDALDataset* poGDALDataset = pSrcDataSet->GetMainRaster();    
    if(!poGDALDataset)
    {
        poGDALDataset = pSrcDataSet->GetRaster();
        if(!poGDALDataset)
        {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting raster"), -1, enumGISMessageErr);
        return false;
        }
    }

	GDALDriver* poDriver = (GDALDriver*)GDALGetDriverByName( sDriver.mb_str() );
    GDALRasterBand * poGDALRasterBand = poGDALDataset->GetRasterBand(1);

    if(!poGDALRasterBand)
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("The raster has no bands"), -1, enumGISMessageErr);
        return false;
    }
    GDALDataType eDT = poGDALRasterBand->GetRasterDataType();

    //wxString soChoice = m_paParam[5]->GetValue();
    //if(soChoice == wxString(_("Cubic")))
    //    CPLSetConfigOption( "GDAL_RPCDEMINTERPOLATION", "CUBIC" ); //BILINEAR


    //CPLString osSRCSRSOpt = "SRC_SRS=";
    //osSRCSRSOpt += poGDALDataset->GetProjectionRef();
    CPLString osDSTSRSOpt = "DST_SRS=";
    char *pszProjection = (char*)poGDALDataset->GetProjectionRef();
    if(CPLStrnlen(pszProjection, 10) == 0)
    {
        OGRSpatialReferenceSPtr oOGRSpatialReference = pSrcDataSet->GetSpatialReference();
        if( oOGRSpatialReference->exportToWkt( &pszProjection ) != OGRERR_NONE )
        {
            if(pTrackCancel)
                pTrackCancel->PutMessage(_("Error getting output Spatial Reference (input Spatial Reference undefined)"), -1, enumGISMessageErr);
            return false;
        }
    }

    osDSTSRSOpt += pszProjection;

    const char *apszOptions[7] = { osDSTSRSOpt.c_str(), "METHOD=RPC", NULL, NULL, NULL, NULL, NULL};//, NULL  osSRCSRSOpt.c_str(),
    wxString soDEMPath = m_paParam[3]->GetValue();

    CPLString soCPLDemPath;
    if(pGxObjectContainer)
    {
        IGxObject* pGxDemObj = pGxObjectContainer->SearchChild(soDEMPath);
        if(pGxDemObj)
        {
            soCPLDemPath = pGxDemObj->GetInternalName();
        }
    }

    CPLString osDEMFileOpt = "RPC_DEM=";
    osDEMFileOpt += soCPLDemPath;
    apszOptions[2] = osDEMFileOpt.c_str();

///////////////////////////////////////////////////////////////////////
    bool bGeoidCorr = m_paParam[7]->GetValue();
    double dfAdditionalHeight(0);
    if(bGeoidCorr)
    {
        OGRSpatialReferenceSPtr oOGRSpatialReference = pSrcDataSet->GetSpatialReference();
//        int nEPSG = oOGRSpatialReference->GetEPSGGeogCS();
//        CPLString pszSpaRefStr;
//        pszSpaRefStr.Printf("EPSG:4326+5773
        //OGRSpatialReference* poTestOGRSpatialReference = oOGRSpatialReference->CloneGeogCS();
        //poTestOGRSpatialReference->

        OGRSpatialReference oTestOGRSpatialReference;
        oTestOGRSpatialReference.SetFromUserInput("EPSG:4326+5773");//TODO: set from config
        OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( oOGRSpatialReference.get(), &oTestOGRSpatialReference);
        if(poCT)
        {
            OGREnvelope Env = pSrcDataSet->GetEnvelope();
            double dfX = (Env.MaxX + Env.MinX) /2;                                
            double dfY = (Env.MaxY + Env.MinY) /2;
            int nResult = poCT->Transform(1, &dfX, &dfY, &dfAdditionalHeight);
            if(nResult == 0)
            {
                const char* err = CPLGetLastErrorMsg();               
                if(pTrackCancel)
                    pTrackCancel->PutMessage(wxString::Format(_("Error getting geoid correction. PROJ4 error: %s"), err), -1, enumGISMessageErr);
            }
            else
            {
                if(pTrackCancel)
                    pTrackCancel->PutMessage(wxString::Format(_("Geoid correction value set to %.3f m"), -dfAdditionalHeight), -1, enumGISMessageInfo);
            }

            OCTDestroyCoordinateTransformation(poCT);
        }
    }
///////////////////////////////////////////////////////////////////////
    double dfHeight = m_paParam[4]->GetValue();
    dfHeight -= dfAdditionalHeight;
    //wxString soHeight = m_paParam[3]->GetValue();
    CPLString osHeightOpt;
    osHeightOpt.Printf( "RPC_HEIGHT=%f", dfHeight);
    //osHeightOpt += soHeight.mb_str();
    apszOptions[3] = osHeightOpt.c_str();

    wxString soHeightScale = m_paParam[5]->GetValue();
    CPLString osHeightScaleOpt = "RPC_HEIGHT_SCALE=";
	osHeightScaleOpt += soHeightScale.mb_str();
    apszOptions[4] = osHeightScaleOpt.c_str();

    wxString sChoice = m_paParam[6]->GetValue();
    CPLString osInterpolOpt = "RPC_DEMINTERPOLATION=";
    osInterpolOpt += sChoice.mb_str();
    apszOptions[5] = osInterpolOpt.c_str();

    //double dfPixErrThreshold = MIN(adfDstGeoTransform[1], adfDstGeoTransform[5]);

    void *hTransformArg = GDALCreateGenImgProjTransformer2( poGDALDataset, NULL, (char **)apszOptions );
    if(!hTransformArg)
    {
        const char* pszErr = CPLGetLastErrorMsg();
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("Error CreateGenImgProjTransformer. GDAL Error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
        return false;
    }

    double adfDstGeoTransform[6] = {0,0,0,0,0,0};
    int nPixels=0, nLines=0;

    CPLErr eErr = GDALSuggestedWarpOutput( poGDALDataset, GDALGenImgProjTransform, hTransformArg, adfDstGeoTransform, &nPixels, &nLines );
    if(eErr != CE_None)
    {
        const char* pszErr = CPLGetLastErrorMsg();
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("Error determining output raster size. GDAL Error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
        return false;
    }


    GDALDestroyGenImgProjTransformer( hTransformArg );

    // Create the output file.
	CPLString sFullPath = CPLFormFilename(szPath, sName.mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
    GDALDataset * poOutputGDALDataset = poDriver->Create( sFullPath, nPixels, nLines, poGDALDataset->GetRasterCount(), eDT, NULL );
    if(poOutputGDALDataset == NULL)
    {
        const char* pszErr = CPLGetLastErrorMsg();
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("Error creating output raster. GDAL Error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
        return false;
    }

    poOutputGDALDataset->SetProjection(pszProjection);
    poOutputGDALDataset->SetGeoTransform( adfDstGeoTransform );

    // Copy the color table, if required.
    GDALColorTableH hCT;

    hCT = GDALGetRasterColorTable( GDALGetRasterBand(poGDALDataset,1) );
    if( hCT != NULL )
        GDALSetRasterColorTable( GDALGetRasterBand(poOutputGDALDataset,1), hCT );


    // Setup warp options.

    GDALWarpOptions *psWarpOptions = GDALCreateWarpOptions();

    psWarpOptions->hSrcDS = poGDALDataset;
    psWarpOptions->hDstDS = poOutputGDALDataset;

    //psWarpOptions->nBandCount = 1;
    //psWarpOptions->panSrcBands =
    //    (int *) CPLMalloc(sizeof(int) * psWarpOptions->nBandCount );
    //psWarpOptions->panSrcBands[0] = 1;
    //psWarpOptions->panDstBands =
    //    (int *) CPLMalloc(sizeof(int) * psWarpOptions->nBandCount );
    //psWarpOptions->panDstBands[0] = 1;

    psWarpOptions->pfnProgress = ExecToolProgress;
    psWarpOptions->pProgressArg = (void*)pTrackCancel;

    // Establish reprojection transformer.

    psWarpOptions->pTransformerArg = GDALCreateGenImgProjTransformer2( poGDALDataset, poOutputGDALDataset, (char **)apszOptions );
    psWarpOptions->pfnTransformer = GDALGenImgProjTransform;
    
    //TODO: Add to config memory limit in % of free memory
    double dfMemLim = wxMemorySize(wxGetFreeMemory() / wxThread::GetCPUCount()).ToDouble();
    if(dfMemLim > 135000000) //128Mb in bytes
    {
        psWarpOptions->dfWarpMemoryLimit = dfMemLim;
        double dfMemLimMb = dfMemLim / 1048576;
        wxLogDebug(wxT("wxGISGPOrthoCorrectTool: The dfWarpMemoryLimit set to %f Mb"), dfMemLimMb);
        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("The dfWarpMemoryLimit set to %f Mb"), dfMemLimMb), -1, enumGISMessageInfo);
    }

    psWarpOptions->nBandCount = poGDALDataset->GetRasterCount();
    
    psWarpOptions->panSrcBands = (int *) CPLMalloc(psWarpOptions->nBandCount*sizeof(int));
    psWarpOptions->panDstBands = (int *) CPLMalloc(psWarpOptions->nBandCount*sizeof(int));

    for( int i = 0; i < psWarpOptions->nBandCount; ++i )
    {
        psWarpOptions->panSrcBands[i] = i + 1;
        psWarpOptions->panDstBands[i] = i + 1;
    }

    psWarpOptions->papszWarpOptions = CSLSetNameValue(psWarpOptions->papszWarpOptions, "SOURCE_EXTRA", "5" );//TODO: Set from config
    psWarpOptions->papszWarpOptions = CSLSetNameValue(psWarpOptions->papszWarpOptions, "SAMPLE_STEPS", "101" );
    psWarpOptions->papszWarpOptions = CSLSetNameValue(psWarpOptions->papszWarpOptions, "INIT_DEST", "NO_DATA" );//INIT_DEST=[value]
    psWarpOptions->eResampleAlg = (GDALResampleAlg)m_paParam[2]->GetValue().GetLong();

/* -------------------------------------------------------------------- */
/*                     If the data has nodata values                    */
/* -------------------------------------------------------------------- */
    int bHaveNodata = FALSE;
    double dfReal = 0.0;

    for( int i = 0; i < psWarpOptions->nBandCount; ++i )
    {
        GDALRasterBand* pBand = poGDALDataset->GetRasterBand( i + 1 );
        dfReal = pBand->GetNoDataValue(&bHaveNodata);
    }

    if( bHaveNodata )
    {
        if (CPLIsNan(dfReal))
        {
            wxLogDebug(wxT("wxGISGPOrthoCorrectTool: Using internal nodata values (eg. nan) for source image"));
            if(pTrackCancel)
                pTrackCancel->PutMessage(_("Using internal nodata values (eg. nan) for source image"), -1, enumGISMessageInfo);
        }            
        else
        {
            wxLogDebug(wxT("wxGISGPOrthoCorrectTool: Using internal nodata values (eg. %g) for source image"), dfReal);
            if(pTrackCancel)
                pTrackCancel->PutMessage(wxString::Format(_("Using internal nodata values (eg. %g) for source image"), dfReal), -1, enumGISMessageInfo);
        }

        psWarpOptions->padfSrcNoDataReal = (double *) CPLMalloc(psWarpOptions->nBandCount*sizeof(double));
        psWarpOptions->padfSrcNoDataImag = (double *) CPLMalloc(psWarpOptions->nBandCount*sizeof(double));
                
        for( int i = 0; i < psWarpOptions->nBandCount; ++i )
        {
            GDALRasterBand* pBand = poGDALDataset->GetRasterBand( i + 1 );
            dfReal = pBand->GetNoDataValue(&bHaveNodata);
            if( bHaveNodata )
            {
                psWarpOptions->padfSrcNoDataReal[i] = dfReal;
                psWarpOptions->padfSrcNoDataImag[i] = 0.0;
            }
            else
            {
                psWarpOptions->padfSrcNoDataReal[i] = -123456.789;
                psWarpOptions->padfSrcNoDataImag[i] = 0.0;
            }
        }
    }

/* -------------------------------------------------------------------- */
/*      If the output dataset was created, and we have a destination    */
/*      nodata value, go through marking the bands with the information.*/
/* -------------------------------------------------------------------- */
    psWarpOptions->padfDstNoDataReal = (double *) CPLMalloc(psWarpOptions->nBandCount*sizeof(double));
    psWarpOptions->padfDstNoDataImag = (double *) CPLMalloc(psWarpOptions->nBandCount*sizeof(double));

    for( int i = 0; i < psWarpOptions->nBandCount; ++i )
    {
        psWarpOptions->padfDstNoDataReal[i] = 0.0;
        psWarpOptions->padfDstNoDataImag[i] = 0.0;


        GDALRasterBand* pBand = poOutputGDALDataset->GetRasterBand( i + 1 );
        int bClamped = FALSE, bRounded = FALSE;

#define CLAMP(val,type,minval,maxval) \
    do { if (val < minval) { bClamped = TRUE; val = minval; } \
    else if (val > maxval) { bClamped = TRUE; val = maxval; } \
    else if (val != (type)val) { bRounded = TRUE; val = (type)(val + 0.5); } } \
    while(0)

        switch(pBand->GetRasterDataType())
        {
        case GDT_Byte:
            CLAMP(psWarpOptions->padfDstNoDataReal[i], GByte, 0.0, 255.0);
            break;
        case GDT_Int16:
            CLAMP(psWarpOptions->padfDstNoDataReal[i], GInt16, -32768.0, 32767.0);
            break;
        case GDT_UInt16:
            CLAMP(psWarpOptions->padfDstNoDataReal[i], GUInt16, 0.0, 65535.0);
            break;
        case GDT_Int32:
            CLAMP(psWarpOptions->padfDstNoDataReal[i], GInt32, -2147483648.0, 2147483647.0);
            break;
        case GDT_UInt32:
            CLAMP(psWarpOptions->padfDstNoDataReal[i], GUInt32, 0.0, 4294967295.0);
            break;
        default:
            break;
        }

        if (bClamped)
        {
            wxLogDebug(wxT("wxGISGPOrthoCorrectTool: For band %d, destination nodata value has been clamped to %.0f, the original value being out of range"), i + 1, psWarpOptions->padfDstNoDataReal[i]);
            if(pTrackCancel)
                pTrackCancel->PutMessage(wxString::Format(_("For band %d, destination nodata value has been clamped to %.0f, the original value being out of range"), i + 1, psWarpOptions->padfDstNoDataReal[i]), -1, enumGISMessageInfo);
        }
        else if(bRounded)
        {
            wxLogDebug(wxT("wxGISGPOrthoCorrectTool: For band %d, destination nodata value has been rounded to %.0f, %s being an integer datatype"), i + 1, psWarpOptions->padfDstNoDataReal[i], GDALGetDataTypeName(pBand->GetRasterDataType()) );
            if(pTrackCancel)
                pTrackCancel->PutMessage(wxString::Format(_("For band %d, destination nodata value has been clamped to %.0f, the original value being out of range"), i + 1, psWarpOptions->padfDstNoDataReal[i], GDALGetDataTypeName(pBand->GetRasterDataType()) ), -1, enumGISMessageInfo); 
        }

        pBand->SetNoDataValue( psWarpOptions->padfDstNoDataReal[i] );
    }

    // Initialize and execute the warp operation.

    GDALWarpOperation oOperation;

    oOperation.Initialize( psWarpOptions );
    eErr = oOperation.ChunkAndWarpImage( 0, 0, nPixels, nLines );//ChunkAndWarpMulti( 0, 0, nPixels, nLines );//
    if(eErr != CE_None)
    {
        const char* pszErr = CPLGetLastErrorMsg();
        if(pTrackCancel)
        {
            pTrackCancel->PutMessage(wxString::Format(_("OrthoCorrect failed! GDAL error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
        }
        GDALClose(poOutputGDALDataset);
        return false;
    }

    GDALDestroyGenImgProjTransformer( psWarpOptions->pTransformerArg );
    GDALDestroyWarpOptions( psWarpOptions );

    GDALClose(poOutputGDALDataset);

    if(pGxObjectContainer)
    {
        IGxObject* pParentLoc = pGxObjectContainer->SearchChild(sPath);
        if(pParentLoc)
            pParentLoc->Refresh();
    }

    return true;
}


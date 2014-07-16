/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  raster dataset functions.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#include "wxgis/geoprocessing/gpraster.h"
#include "vrtdataset.h"
//#include "vrt/vrtdataset.h"
#include "wxgis/datasource/sysop.h"

bool SubrasterByVector(wxGISFeatureDatasetSPtr pSrcFeatureDataSet, wxGISRasterDatasetSPtr pSrcRasterDataSet, CPLString &szDstFolderPath, wxGxRasterFilter* pFilter, GDALDataType eOutputType, int nBandCount, int *panBandList, bool bUseCounter, int nCounterBegin, int nFieldNo, double dfOutResX, double dfOutResY, bool bCopyNodata, bool bSkipSourceMetadata, char** papszOptions, ITrackCancel* pTrackCancel)
{
	//check if openned or/and open dataset
	if(!pSrcFeatureDataSet->IsOpened())
	{
		if(!pSrcFeatureDataSet->Open(true))
		{
			if(pTrackCancel)
				pTrackCancel->PutMessage(_("Source vector dataset open failed"), -1, enumGISMessageErr);
			return false;
		}
	}

	if(!pSrcRasterDataSet->IsOpened())
	{
		if(!pSrcRasterDataSet->Open(true))
		{
			if(pTrackCancel)
				pTrackCancel->PutMessage(_("Source raster dataset open failed"), -1, enumGISMessageErr);
			return false;
		}
	}

    const OGRSpatialReferenceSPtr pSrsSRS = pSrcFeatureDataSet->GetSpatialReference();
    const OGRSpatialReferenceSPtr pDstSRS = pSrcRasterDataSet->GetSpatialReference();
    OGRCoordinateTransformation *poCT(NULL);
    bool bSame = pSrsSRS == NULL || pDstSRS == NULL || pSrsSRS->IsSame(pDstSRS.get());
    if( !bSame )
	{
        poCT = OGRCreateCoordinateTransformation( pSrsSRS.get(), pDstSRS.get() );
		if(poCT == NULL)
		{
		    const char* err = CPLGetLastErrorMsg();
			wxString sWarn = wxString::Format(_("Create OGRCreateCoordinateTransformation failed! GDAL error: %s"), wxString(err, wxConvUTF8).c_str());
            wxLogWarning(sWarn);
			if(pTrackCancel)
				pTrackCancel->PutMessage(sWarn, -1, enumGISMessageWarning);
		}
	}

    IProgressor* pProgressor(NULL);
    if(pTrackCancel)
    {
       pProgressor = pTrackCancel->GetProgressor();
       pTrackCancel->PutMessage(wxString::Format(_("Start clip '%s' by geometry from '%s'"), wxString(pSrcRasterDataSet->GetPath(), wxConvUTF8).c_str(), wxString(pSrcFeatureDataSet->GetPath(), wxConvUTF8).c_str()), -1, enumGISMessageNorm);
    }
    int nCounter(0);
    if(pProgressor)
        pProgressor->SetRange(pSrcFeatureDataSet->GetFeatureCount());


/* -------------------------------------------------------------------- */
/*	Build band list to translate					                    */
/* -------------------------------------------------------------------- */
    if( nBandCount == 0 )
    {
        nBandCount = pSrcRasterDataSet->GetBandCount();
        if( nBandCount == 0 )
        {
		    if(pTrackCancel)
			    pTrackCancel->PutMessage(_("Input file has no bands, and so cannot be translated."), -1, enumGISMessageErr);
            return false;
        }

        panBandList = (int *) CPLMalloc(sizeof(int)*nBandCount);
        for(size_t i = 0; i < nBandCount; ++i )
            panBandList[i] = i + 1;
    }
    else
    {
        for(size_t i = 0; i < nBandCount; ++i )
        {
            if( panBandList[i] > pSrcRasterDataSet->GetBandCount() )
            {
		        if(pTrackCancel)
                    pTrackCancel->PutMessage(wxString::Format(_("Band %d requested, but only bands 1 to %d available."), panBandList[i], pSrcRasterDataSet->GetBandCount()), -1, enumGISMessageErr);
                return false;
            }
        }
    }

    bool bDefaultfilter(false);
    if(pFilter == NULL)
    {
        pFilter = new wxGxRasterFilter(enumRasterTiff);
        bDefaultfilter = true;
    }

    CPLString szDriver(pFilter->GetDriver().mb_str());
    CPLString szExt(pFilter->GetExt().mb_str());
    CPLString szBaseName = CPLGetBasename(pSrcRasterDataSet->GetPath());

	GDALDriver* pDriver = (GDALDriver*)GDALGetDriverByName( szDriver );
	if( pDriver == NULL )
    {
		if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("Output driver '%s' not recognised."), szDriver.c_str()), -1, enumGISMessageErr);
        if(bDefaultfilter)
            wxDELETE(pFilter);
		return false;
    }

    pSrcFeatureDataSet->Reset();
    OGRFeatureSPtr pFeature;
    size_t nNameCounter(nCounterBegin);
    while((pFeature = pSrcFeatureDataSet->Next()) != NULL)
    {
        if(pTrackCancel && !pTrackCancel->Continue())
        {
            wxString sErr(_("Interrupted by user"));
            CPLString sFullErr(sErr.mb_str());
            CPLError( CE_Warning, CPLE_AppDefined, sFullErr );

            if(pTrackCancel)
                pTrackCancel->PutMessage(wxString(sFullErr, wxConvLocal), -1, enumGISMessageErr);

            if(bDefaultfilter)
                wxDELETE(pFilter);
            return false;
        }

        OGRGeometry *pGeom = pFeature->GetGeometryRef();
		if(wkbFlatten(pSrcFeatureDataSet->GetGeometryType()) != wkbUnknown && !pGeom)
            continue;

		OGRGeometry *pNewGeom(NULL);
        if( !bSame && poCT )
        {
            if(pGeom)
            {
				pNewGeom = pGeom->clone();
                OGRErr eErr = pNewGeom->transform(poCT);
                if(eErr != OGRERR_NONE)
	                wxDELETE(pNewGeom);
            }
        }
		else
			pNewGeom = pGeom->clone();

		OGREnvelope GeomEnv;
		pNewGeom->getEnvelope(&GeomEnv);
		OGREnvelope RasterEnv = pSrcRasterDataSet->GetEnvelope();
		GeomEnv.Intersect(RasterEnv);
		if(GeomEnv.IsInit())
		{
            CPLString szPath;
			if(bUseCounter)
            {
                szPath.Printf("%s_%d", szBaseName.c_str(), nNameCounter++);
                CPLString sNewName(CheckUniqName(szDstFolderPath, szPath, szExt).mb_str(wxConvUTF8));
                szPath = CPLFormFilename(szDstFolderPath, sNewName, szExt);
            }
            else
            {
                CPLString szName = pFeature->GetFieldAsString(nFieldNo);
                CPLString sNewName(CheckUniqName(szDstFolderPath, szName, szExt).mb_str(wxConvUTF8));
                szPath = CPLFormFilename(szDstFolderPath, sNewName, szExt);
            }
			CreateSubRaster(pSrcRasterDataSet, GeomEnv, pNewGeom, pDriver, szPath, eOutputType, nBandCount, panBandList, dfOutResX, dfOutResY, bCopyNodata, bSkipSourceMetadata, papszOptions, pTrackCancel);
		}
        nCounter++;
        if(pProgressor)
            pProgressor->SetValue(nCounter);
    }

    if(poCT)
        OCTDestroyCoordinateTransformation(poCT);

    if(bDefaultfilter)
        wxDELETE(pFilter);

	return true;
}

bool CreateSubRaster( wxGISRasterDatasetSPtr pSrcRasterDataSet, OGREnvelope &Env, const OGRGeometry *pGeom, GDALDriver* pDriver, CPLString &szDstPath, GDALDataType eOutputType, int nBandCount, int *panBandList, double dfOutResX, double dfOutResY, bool bCopyNodata, bool bSkipSourceMetadata, char** papszOptions, ITrackCancel* pTrackCancel )
{
	GDALDataset* pDset = pSrcRasterDataSet->GetRaster();
	if(!pDset)
	{
		if(pTrackCancel)
			pTrackCancel->PutMessage(_("Get raster failed"), -1, enumGISMessageErr);
		return false;
	}

    double adfGeoTransform[6] = { 0, 0, 0, 0, 0, 0 };
	CPLErr err = pDset->GetGeoTransform(adfGeoTransform);
	if(err == CE_Fatal)
	{
		if(pTrackCancel)
			pTrackCancel->PutMessage(_("Get raster failed"), -1, enumGISMessageErr);
		return false;
	}
	if( adfGeoTransform[2] != 0.0 || adfGeoTransform[4] != 0.0 )
	{
		if(pTrackCancel)
			pTrackCancel->PutMessage(_("The geotransform is rotated. This configuration is not supported."), -1, enumGISMessageErr);
		return false;
    }
	int anSrcWin[4] = {0, 0, 0, 0};

    anSrcWin[0] = floor ((Env.MinX - adfGeoTransform[0]) / adfGeoTransform[1] + 0.001);
    anSrcWin[1] = floor ((Env.MaxY - adfGeoTransform[3]) / adfGeoTransform[5] + 0.001);
	anSrcWin[2] = ceil ((Env.MaxX - Env.MinX) / adfGeoTransform[1]);
	anSrcWin[3] = ceil ((Env.MinY - Env.MaxY) / adfGeoTransform[5]);
	if(pTrackCancel)
		pTrackCancel->PutMessage(wxString::Format(_("Computed source pixel window %d %d %d %d from geographic window."), anSrcWin[0], anSrcWin[1], anSrcWin[2], anSrcWin[3] ), -1, enumGISMessageInfo);

	if( anSrcWin[0] < 0 || anSrcWin[1] < 0 || anSrcWin[0] + anSrcWin[2] > pSrcRasterDataSet->GetWidth() || anSrcWin[1] + anSrcWin[3] > pSrcRasterDataSet->GetHeight() )
    {
		if(pTrackCancel)
			pTrackCancel->PutMessage(wxString::Format(_("Computed source pixel window falls outside raster size of %dx%d."), pSrcRasterDataSet->GetWidth(), pSrcRasterDataSet->GetHeight()), -1, enumGISMessageErr);
		return false;
    }

	int nOXSize = 0, nOYSize = 0;

    if(IsDoubleEquil(dfOutResX, -1) && IsDoubleEquil(dfOutResY, -1))
    {
        nOXSize = anSrcWin[2];
        nOYSize = anSrcWin[3];
    }
    else
    {
        nOXSize = ceil ((Env.MaxX - Env.MinX) / dfOutResX);
        nOYSize = ceil ((Env.MinY - Env.MaxY) / (adfGeoTransform[5] < 0 ? dfOutResY * -1 : dfOutResY));
    }

/* ==================================================================== */
/*      Create a virtual dataset.                                       */
/* ==================================================================== */
    VRTDataset *poVDS;
/* -------------------------------------------------------------------- */
/*      Make a virtual clone.                                           */
/* -------------------------------------------------------------------- */
    poVDS = (VRTDataset *) VRTCreate( nOXSize, nOYSize );

    if( pSrcRasterDataSet->GetSpatialReference() != NULL )
    {
		poVDS->SetProjection( pDset->GetProjectionRef() );
    }

	adfGeoTransform[0] += anSrcWin[0] * adfGeoTransform[1] + anSrcWin[1] * adfGeoTransform[2];
    adfGeoTransform[3] += anSrcWin[0] * adfGeoTransform[4] + anSrcWin[1] * adfGeoTransform[5];

    adfGeoTransform[1] *= anSrcWin[2] / (double) nOXSize;
    adfGeoTransform[2] *= anSrcWin[3] / (double) nOYSize;
    adfGeoTransform[4] *= anSrcWin[2] / (double) nOXSize;
    adfGeoTransform[5] *= anSrcWin[3] / (double) nOYSize;

    poVDS->SetGeoTransform( adfGeoTransform );

    int nGCPs = pDset->GetGCPCount();
    if( nGCPs > 0 )
    {
        GDAL_GCP *pasGCPs = GDALDuplicateGCPs( nGCPs, pDset->GetGCPs() );

        for(size_t i = 0; i < nGCPs; ++i )
        {
            pasGCPs[i].dfGCPPixel -= anSrcWin[0];
            pasGCPs[i].dfGCPLine  -= anSrcWin[1];
            pasGCPs[i].dfGCPPixel *= (nOXSize / (double) anSrcWin[2] );
            pasGCPs[i].dfGCPLine  *= (nOYSize / (double) anSrcWin[3] );
        }

        poVDS->SetGCPs( nGCPs, pasGCPs, pDset->GetGCPProjection() );
        GDALDeinitGCPs( nGCPs, pasGCPs );
        CPLFree( pasGCPs );
    }

/* -------------------------------------------------------------------- */
/*      Transfer generally applicable metadata.                         */
/* -------------------------------------------------------------------- */
    if(!bSkipSourceMetadata)
        poVDS->SetMetadata( pDset->GetMetadata() );

/* ==================================================================== */
/*      Process all bands.                                              */
/* ==================================================================== */
    for(size_t i = 0; i < nBandCount; ++i )
    {
        VRTSourcedRasterBand *poVRTBand;
        GDALRasterBand *poSrcBand;
        GDALDataType eBandType;
        int nComponent = 0;

        poSrcBand = pDset->GetRasterBand(panBandList[i]);

/* -------------------------------------------------------------------- */
/*      Select output data type to match source.                        */
/* -------------------------------------------------------------------- */
        if( eOutputType == GDT_Unknown )
            eBandType = poSrcBand->GetRasterDataType();
        else
            eBandType = eOutputType;
/* -------------------------------------------------------------------- */
/*      Create this band.                                               */
/* -------------------------------------------------------------------- */
        poVDS->AddBand( eBandType, NULL );
        poVRTBand = (VRTSourcedRasterBand *) poVDS->GetRasterBand( i + 1 );
/* -------------------------------------------------------------------- */
/*      Create a simple data source depending on the                    */
/*      translation type required.                                      */
/* -------------------------------------------------------------------- */
        //if( bUnscale || bScale || (nRGBExpand != 0 && i < nRGBExpand) )
        //{
        //    poVRTBand->AddComplexSource( poSrcBand,
        //                                 anSrcWin[0], anSrcWin[1],
        //                                 anSrcWin[2], anSrcWin[3],
        //                                 0, 0, nOXSize, nOYSize,
        //                                 dfOffset, dfScale,
        //                                 VRT_NODATA_UNSET,
        //                                 nComponent );
        //}
        //else
        CPLString pszResampling = CSLFetchNameValueDef(papszOptions, "DEST_RESAMPLING", "near");
        poVRTBand->AddSimpleSource( poSrcBand, anSrcWin[0], anSrcWin[1], anSrcWin[2], anSrcWin[3], 0, 0, nOXSize, nOYSize, pszResampling );

/* -------------------------------------------------------------------- */
/*      copy some other information of interest.                        */
/* -------------------------------------------------------------------- */
		CopyBandInfo( poSrcBand, poVRTBand, bCopyNodata );
/* -------------------------------------------------------------------- */
/*      Set a forcable nodata value?                                    */
/* -------------------------------------------------------------------- */
//        if( bSetNoData )
//        {
//            double dfVal = dfNoDataReal;
//            int bClamped = FALSE, bRounded = FALSE;
//
//#define CLAMP(val,type,minval,maxval) \
//    do { if (val < minval) { bClamped = TRUE; val = minval; } \
//    else if (val > maxval) { bClamped = TRUE; val = maxval; } \
//    else if (val != (type)val) { bRounded = TRUE; val = (type)(val + 0.5); } } \
//    while(0)
//
//            switch(eBandType)
//            {
//                case GDT_Byte:
//                    CLAMP(dfVal, GByte, 0.0, 255.0);
//                    break;
//                case GDT_Int16:
//                    CLAMP(dfVal, GInt16, -32768.0, 32767.0);
//                    break;
//                case GDT_UInt16:
//                    CLAMP(dfVal, GUInt16, 0.0, 65535.0);
//                    break;
//                case GDT_Int32:
//                    CLAMP(dfVal, GInt32, -2147483648.0, 2147483647.0);
//                    break;
//                case GDT_UInt32:
//                    CLAMP(dfVal, GUInt32, 0.0, 4294967295.0);
//                    break;
//                default:
//                    break;
//            }
//
//            if (bClamped)
//            {
//                printf( "for band %d, nodata value has been clamped "
//                       "to %.0f, the original value being out of range.\n",
//                       i + 1, dfVal);
//            }
//            else if(bRounded)
//            {
//                printf("for band %d, nodata value has been rounded "
//                       "to %.0f, %s being an integer datatype.\n",
//                       i + 1, dfVal,
//                       GDALGetDataTypeName(eBandType));
//            }
//
//            poVRTBand->SetNoDataValue( dfVal );
//        }

        //if (eMaskMode == MASK_AUTO &&
        //    (GDALGetMaskFlags(GDALGetRasterBand(hDataset, 1)) & GMF_PER_DATASET) == 0 &&
        //    (poSrcBand->GetMaskFlags() & (GMF_ALL_VALID | GMF_NODATA)) == 0)
        //{
        //    if (poVRTBand->CreateMaskBand(poSrcBand->GetMaskFlags()) == CE_None)
        //    {
        //        VRTSourcedRasterBand* hMaskVRTBand =
        //            (VRTSourcedRasterBand*)poVRTBand->GetMaskBand();
        //        hMaskVRTBand->AddMaskBandSource(poSrcBand,
        //                                anSrcWin[0], anSrcWin[1],
        //                                anSrcWin[2], anSrcWin[3],
        //                                0, 0, nOXSize, nOYSize );
        //    }
        //}
    }

    //if (eMaskMode == MASK_USER)
    //{
    //    GDALRasterBand *poSrcBand =
    //        (GDALRasterBand*)GDALGetRasterBand(hDataset, ABS(nMaskBand));
    //    if (poSrcBand && poVDS->CreateMaskBand(GMF_PER_DATASET) == CE_None)
    //    {
    //        VRTSourcedRasterBand* hMaskVRTBand = (VRTSourcedRasterBand*)
    //            GDALGetMaskBand(GDALGetRasterBand((GDALDatasetH)poVDS, 1));
    //        if (nMaskBand > 0)
    //            hMaskVRTBand->AddSimpleSource(poSrcBand,
    //                                    anSrcWin[0], anSrcWin[1],
    //                                    anSrcWin[2], anSrcWin[3],
    //                                    0, 0, nOXSize, nOYSize );
    //        else
    //            hMaskVRTBand->AddMaskBandSource(poSrcBand,
    //                                    anSrcWin[0], anSrcWin[1],
    //                                    anSrcWin[2], anSrcWin[3],
    //                                    0, 0, nOXSize, nOYSize );
    //    }
    //}
    //else
    //if (eMaskMode == MASK_AUTO && nSrcBandCount > 0 &&
    //    GDALGetMaskFlags(GDALGetRasterBand(hDataset, 1)) == GMF_PER_DATASET)
    //{
    //    if (poVDS->CreateMaskBand(GMF_PER_DATASET) == CE_None)
    //    {
    //        VRTSourcedRasterBand* hMaskVRTBand = (VRTSourcedRasterBand*)
    //            GDALGetMaskBand(GDALGetRasterBand((GDALDatasetH)poVDS, 1));
    //        hMaskVRTBand->AddMaskBandSource((GDALRasterBand*)GDALGetRasterBand(hDataset, 1),
    //                                    anSrcWin[0], anSrcWin[1],
    //                                    anSrcWin[2], anSrcWin[3],
    //                                    0, 0, nOXSize, nOYSize );
    //    }
    //}

/* -------------------------------------------------------------------- */
/*      Write to the output file using CopyCreate().                    */
/* -------------------------------------------------------------------- */
    GDALDataset* pOutDS = pDriver->CreateCopy(szDstPath, poVDS, false, papszOptions, GDALDummyProgress, NULL);

    //hOutDS = GDALCreateCopy( hDriver, pszDest, (GDALDatasetH) poVDS, bStrict, papszCreateOptions, pfnProgress, NULL );
    if( pOutDS )
    {
        CPLErrorReset();
        GDALFlushCache( pOutDS );
        if (CPLGetLastErrorType() != CE_None)
        {
		    if(pTrackCancel)
			    pTrackCancel->PutMessage(_("GDALFlushCache failed!"), -1, enumGISMessageErr);
        }
        GDALClose( pOutDS );

        GDALClose( poVDS );
        return true;
    }
    else
    {
        GDALClose( poVDS );
        return false;
    }


    //CPLFree( panBandList );
    //
    //CPLFree( pszOutputSRS );

    //if( !bSubCall )
    //{
    //    GDALDumpOpenDatasets( stderr );
    //    GDALDestroyDriverManager();
    //}
    //CSLDestroy( papszCreateOptions );

	return true;
}

/************************************************************************/
/*                           CopyBandInfo()                            */
/************************************************************************/

/* A bit of a clone of VRTRasterBand::CopyCommonInfoFrom(), but we need */
/* more and more custom behaviour in the context of gdal_translate ... */

void CopyBandInfo( GDALRasterBand * poSrcBand, GDALRasterBand * poDstBand, int bCopyNoData )

{
    int bSuccess;
    double dfNoData;

    char** papszMetadata = poSrcBand->GetMetadata();
    char** papszMetadataNew = NULL;
    for( int i = 0; papszMetadata != NULL && papszMetadata[i] != NULL; i++ )
    {
        if (strncmp(papszMetadata[i], "STATISTICS_", 11) != 0)
            papszMetadataNew = CSLAddString(papszMetadataNew, papszMetadata[i]);
    }
    poDstBand->SetMetadata( papszMetadataNew );
    CSLDestroy(papszMetadataNew);

    poDstBand->SetColorTable( poSrcBand->GetColorTable() );
    poDstBand->SetColorInterpretation(poSrcBand->GetColorInterpretation());
    if( strlen(poSrcBand->GetDescription()) > 0 )
        poDstBand->SetDescription( poSrcBand->GetDescription() );

    if (bCopyNoData)
    {
        dfNoData = poSrcBand->GetNoDataValue( &bSuccess );
        if( bSuccess )
            poDstBand->SetNoDataValue( dfNoData );
    }

    poDstBand->SetCategoryNames( poSrcBand->GetCategoryNames() );
    if( !EQUAL(poSrcBand->GetUnitType(),"") )
        poDstBand->SetUnitType( poSrcBand->GetUnitType() );
}

 //   wxString sName = sDstFileName.GetName();

 //   wxGISGPGxObjectDomain* pDomain = dynamic_cast<wxGISGPGxObjectDomain*>(m_paParam[1]->GetDomain());
	//IGxObjectFilter* pFilter = pDomain->GetFilter(m_paParam[1]->GetSelDomainValue());
 //   if(!pFilter)
 //   {
 //       //add messages to pTrackCancel
 //       if(pTrackCancel)
 //           pTrackCancel->PutMessage(_("Error getting selected destination filter"), -1, enumGISMessageErr);
 //       return false;
 //   }

 //   wxString sDriver = pFilter->GetDriver();
 //   wxString sExt = pFilter->GetExt();
 //   int nNewSubType = pFilter->GetSubType();

 //   GDALDataset* poGDALDataset = pSrcDataSet->GetRaster();
 //   if(!poGDALDataset)
 //   {
 //       //add messages to pTrackCancel
 //       if(pTrackCancel)
 //           pTrackCancel->PutMessage(_("Error getting raster"), -1, enumGISMessageErr);
 //       return false;
 //   }

	//GDALDriver* poDriver = (GDALDriver*)GDALGetDriverByName( sDriver.mb_str() );
 //   GDALRasterBand * poGDALRasterBand = poGDALDataset->GetRasterBand(1);

 //   if(!poGDALRasterBand)
 //   {
 //       //add messages to pTrackCancel
 //       if(pTrackCancel)
 //           pTrackCancel->PutMessage(_("The raster has no bands"), -1, enumGISMessageErr);
 //       return false;
 //   }
 //   GDALDataType eDT = poGDALRasterBand->GetRasterDataType();

 //   wxString soChoice = m_paParam[5]->GetValue();
 //   if(soChoice == wxString(_("Cubic")))
 //       CPLSetConfigOption( "GDAL_RPCDEMINTERPOLATION", "CUBIC" ); //BILINEAR


 //   //CPLString osSRCSRSOpt = "SRC_SRS=";
 //   //osSRCSRSOpt += poGDALDataset->GetProjectionRef();
 //   CPLString osDSTSRSOpt = "DST_SRS=";
 //   osDSTSRSOpt += poGDALDataset->GetProjectionRef();

 //   const char *apszOptions[6] = { osDSTSRSOpt.c_str(), "METHOD=RPC", NULL, NULL, NULL, NULL};//, NULL  osSRCSRSOpt.c_str(),
 //   wxString soDEMPath = m_paParam[2]->GetValue();

 //   CPLString soCPLDemPath;
 //   if(pGxObjectContainer)
 //   {
 //       IGxObject* pGxDemObj = pGxObjectContainer->SearchChild(soDEMPath);
 //       if(pGxDemObj)
 //       {
 //           soCPLDemPath = pGxDemObj->GetInternalName();
 //       }
 //   }

 //   CPLString osDEMFileOpt = "RPC_DEM=";
 //   osDEMFileOpt += soCPLDemPath;
 //   apszOptions[2] = osDEMFileOpt.c_str();

 //   wxString soHeight = m_paParam[3]->GetValue();
 //   CPLString osHeightOpt = "RPC_HEIGHT=";
 //   osHeightOpt += soHeight.mb_str();
 //   apszOptions[3] = osHeightOpt.c_str();

 //   wxString soHeightScale = m_paParam[4]->GetValue();
 //   CPLString osHeightScaleOpt = "RPC_HEIGHT_SCALE=";
	//osHeightScaleOpt += soHeightScale.mb_str();
 //   apszOptions[4] = osHeightScaleOpt.c_str();

 //   //double dfPixErrThreshold = MIN(adfDstGeoTransform[1], adfDstGeoTransform[5]);

 //   void *hTransformArg = GDALCreateGenImgProjTransformer2( poGDALDataset, NULL, (char **)apszOptions );
 //   if(!hTransformArg)
 //   {
 //       const char* pszErr = CPLGetLastErrorMsg();
 //       if(pTrackCancel)
 //           pTrackCancel->PutMessage(wxString::Format(_("Error CreateGenImgProjTransformer. GDAL Error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
 //       return false;
 //   }

 //   double adfDstGeoTransform[6] = {0,0,0,0,0,0};
 //   int nPixels=0, nLines=0;

 //   CPLErr eErr = GDALSuggestedWarpOutput( poGDALDataset, GDALGenImgProjTransform, hTransformArg, adfDstGeoTransform, &nPixels, &nLines );
 //   if(eErr != CE_None)
 //   {
 //       const char* pszErr = CPLGetLastErrorMsg();
 //       if(pTrackCancel)
 //           pTrackCancel->PutMessage(wxString::Format(_("Error determining output raster size. GDAL Error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
 //       return false;
 //   }


 //   GDALDestroyGenImgProjTransformer( hTransformArg );

 //   // Create the output file.
	//CPLString sFullPath = CPLFormFilename(szPath, sName.mb_str(wxConvUTF8), sExt.mb_str(wxConvUTF8));
 //   GDALDataset * poOutputGDALDataset = poDriver->Create( sFullPath, nPixels, nLines, poGDALDataset->GetRasterCount(), eDT, NULL );
 //   if(poOutputGDALDataset == NULL)
 //   {
 //       const char* pszErr = CPLGetLastErrorMsg();
 //       if(pTrackCancel)
 //           pTrackCancel->PutMessage(wxString::Format(_("Error creating output raster. GDAL Error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
 //       return false;
 //   }

 //   poOutputGDALDataset->SetProjection(poGDALDataset->GetProjectionRef());
 //   poOutputGDALDataset->SetGeoTransform( adfDstGeoTransform );

 //   // Copy the color table, if required.
 //   GDALColorTableH hCT;

 //   hCT = GDALGetRasterColorTable( GDALGetRasterBand(poGDALDataset,1) );
 //   if( hCT != NULL )
 //       GDALSetRasterColorTable( GDALGetRasterBand(poOutputGDALDataset,1), hCT );


 //   // Setup warp options.

 //   GDALWarpOptions *psWarpOptions = GDALCreateWarpOptions();

 //   psWarpOptions->hSrcDS = poGDALDataset;
 //   psWarpOptions->hDstDS = poOutputGDALDataset;

 //   //psWarpOptions->nBandCount = 1;
 //   //psWarpOptions->panSrcBands =
 //   //    (int *) CPLMalloc(sizeof(int) * psWarpOptions->nBandCount );
 //   //psWarpOptions->panSrcBands[0] = 1;
 //   //psWarpOptions->panDstBands =
 //   //    (int *) CPLMalloc(sizeof(int) * psWarpOptions->nBandCount );
 //   //psWarpOptions->panDstBands[0] = 1;

 //   psWarpOptions->pfnProgress = ExecToolProgress;
 //   psWarpOptions->pProgressArg = (void*)pTrackCancel;

 //   // Establish reprojection transformer.

 //   psWarpOptions->pTransformerArg = GDALCreateGenImgProjTransformer2( poGDALDataset, poOutputGDALDataset, (char **)apszOptions );
 //   psWarpOptions->pfnTransformer = GDALGenImgProjTransform;
 //
 //   //TODO: Add to config memory limit in % of free memory
 //   double dfMemLim = wxMemorySize(wxGetFreeMemory() / wxThread::GetCPUCount()).ToDouble();
 //   if(dfMemLim > 135000000) //128Mb in bytes
 //   {
 //       psWarpOptions->dfWarpMemoryLimit = dfMemLim;
 //       wxLogDebug(wxT("wxGISGPOrthoCorrectTool: The dfWarpMemoryLimit set to %f Mb"), dfMemLim / 1048576);
 //   }

 //   psWarpOptions->papszWarpOptions = CSLSetNameValue(psWarpOptions->papszWarpOptions, "SOURCE_EXTRA", "5" );
 //   psWarpOptions->papszWarpOptions = CSLSetNameValue(psWarpOptions->papszWarpOptions, "SAMPLE_STEPS", "101" );
 //   psWarpOptions->eResampleAlg = GRA_Bilinear;

 //   // Initialize and execute the warp operation.

 //   GDALWarpOperation oOperation;

 //   oOperation.Initialize( psWarpOptions );
 //   eErr = oOperation.ChunkAndWarpImage( 0, 0, nPixels, nLines );//ChunkAndWarpMulti( 0, 0, nPixels, nLines );//
 //   if(eErr != CE_None)
 //   {
 //       const char* pszErr = CPLGetLastErrorMsg();
 //       if(pTrackCancel)
 //       {
 //           pTrackCancel->PutMessage(wxString::Format(_("OrthoCorrect failed! GDAL error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
 //       }
 //       GDALClose(poOutputGDALDataset);
 //       //wsDELETE(pSrcDataSet);
 //       return false;
 //   }

 //   GDALDestroyGenImgProjTransformer( psWarpOptions->pTransformerArg );
 //   GDALDestroyWarpOptions( psWarpOptions );

 //   GDALClose(poOutputGDALDataset);
 //   //wsDELETE(pSrcDataSet);


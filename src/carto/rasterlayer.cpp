/******************************************************************************
 * Project:  wxGIS
 * Purpose:  RasterLayer header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013 Bishop
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
#include "wxgis/carto/rasterlayer.h"
#include "wxgis/carto/rasterrenderer.h"
/*
#include "wxgis/display/displaytransformation.h"
*/

//----------------------------------------------------------------------------
// wxGISRasterLayer
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISRasterLayer, wxGISLayer)

wxGISRasterLayer::wxGISRasterLayer(const wxString &sName, wxGISDataset* pwxGISDataset) : wxGISLayer(sName, pwxGISDataset)
{
    wxGISRasterDataset* pwxGISRasterDataset = wxDynamicCast(pwxGISDataset, wxGISRasterDataset);
	if(pwxGISRasterDataset)
	{
        pwxGISRasterDataset->Reference();
        if(m_sName.IsEmpty())
            m_sName = pwxGISRasterDataset->GetName();

        m_SpatialReference = pwxGISRasterDataset->GetSpatialReference();
		m_FullEnvelope = pwxGISRasterDataset->GetEnvelope();

        //TODO: load or get all renderers and check if i render can draw this dataset. If yes - set it as current
		if(pwxGISRasterDataset->GetBandCount() >= 3)
        {
            m_pRenderer = new wxGISRasterRGBARenderer(this);
        }
        else
        {
		    GDALDataset* poGDALDataset = pwxGISRasterDataset->GetMainRaster();
		    if(!poGDALDataset)
			    poGDALDataset = pwxGISRasterDataset->GetRaster();
		    if(!poGDALDataset)
			    return;

		    GDALRasterBand* pBand = poGDALDataset->GetRasterBand(1);
            GDALColorInterp eColorInterpretation = pBand->GetColorInterpretation();
            if( eColorInterpretation == GCI_PaletteIndex )
            {
                m_pRenderer = new wxGISRasterRasterColormapRenderer(this);
            }
            else if(pBand->GetRasterDataType() == GDT_Int32)
            {
                m_pRenderer = new wxGISRasterPackedRGBARenderer(this);
            }
            else// if( eColorInterpretation == GCI_GrayIndex )
            {
    		    //TODO: else RasterStretchColorRampRenderer
                m_pRenderer = new wxGISRasterGreyScaleRenderer(this);
            }
        }
	}
}

wxGISRasterLayer::~wxGISRasterLayer(void)
{
}

bool wxGISRasterLayer::IsValid(void) const
{
    return m_pRenderer && m_pwxGISDataset && m_pwxGISDataset->IsOpened();
}


	////TODO: reprojection via vrt
	////GDALCreateGenImgProjTransformer
	////GDALCreateReprojectionTransformer

bool wxGISRasterLayer::Draw(wxGISEnumDrawPhase DrawPhase, ITrackCancel * const pTrackCancel)
{
    wxCHECK_MSG(m_pRenderer, false, wxT("The current renderer point is NULL"));
    return m_pRenderer->Draw(DrawPhase, m_pDisplay, pTrackCancel);
    /*
	//if(m_pRasterRenderer && m_pRasterRenderer->CanRender(m_pwxGISRasterDataset))
	//{
	    //bool bSetFilter(false);
		////Check if get all features
		//OGREnvelope Env = pDisplay->GetBounds();
		//if(!IsDoubleEquil(m_PreviousEnvelope.MaxX, Env.MaxX) || !IsDoubleEquil(m_PreviousEnvelope.MaxY, Env.MaxY) || !IsDoubleEquil(m_PreviousEnvelope.MinX, Env.MinX) || !IsDoubleEquil(m_PreviousEnvelope.MinY, Env.MinY))
		//{
		//	OGREnvelope TempFullEnv = m_FullEnvelope;
		//	//use angle
		//	if(!IsDoubleEquil(pDisplay->GetRotate(), 0.0))
		//	{
		//		double dCenterX = Env.MinX + (Env.MaxX - Env.MinX) / 2;
		//		double dCenterY = Env.MinY + (Env.MaxY - Env.MinY) / 2;

		//		RotateEnvelope(TempFullEnv, pDisplay->GetRotate(), dCenterX, dCenterY);
		//	}
		//	bSetFilter = TempFullEnv.Contains(Env) != 0;
		//}

		//store envelope
		//m_PreviousEnvelope = Env;

	 //   //Get pixel dataset
	 //   if(bSetFilter)
	 //   {
		//	//const CPLRectObj Rect = {Env.MinX, Env.MinY, Env.MaxX, Env.MaxY};
		//	//pCursor = m_pwxGISFeatureDataset->SearchGeometry(&Rect);
		//}
		//else
		//{
		//	//pCursor = m_pwxGISFeatureDataset->SearchGeometry();
		//}

		RAWPIXELDATA stPixelData;
		stPixelData.pPixelData = NULL;
		stPixelData.nOutputWidth = stPixelData.nOutputHeight = -1;
		stPixelData.nPixelDataWidth = stPixelData.nPixelDataHeight = -1;


		if(GetPixelData(stPixelData, pDisplay, pTrackCancel))
			m_pRasterRenderer->Draw(stPixelData, DrawPhase, pDisplay, pTrackCancel);
		if(stPixelData.pPixelData)
			CPLFree (stPixelData.pPixelData);
	//}
	return true;

	//IDisplayTransformation* pDisplayTransformation = pDisplay->GetDisplayTransformation();
	//if(!pDisplayTransformation)
	//	return;
	////1. get envelope
 //   OGREnvelope Env = pDisplayTransformation->GetVisibleBounds();
 //   const OGREnvelope* LayerEnv = m_pwxGISRasterDataset->GetEnvelope();
 //   OGRSpatialReference* pEnvSpaRef = pDisplayTransformation->GetSpatialReference();
 //   const OGRSpatialReferenceSPtr pLayerSpaRef = m_pwxGISRasterDataset->GetSpatialReference();

 //   if(pLayerSpaRef && pEnvSpaRef)
 //   {
 //       if(!pLayerSpaRef->IsSame(pEnvSpaRef))
 //       {
 //           OGRCoordinateTransformation *poCT = OGRCreateCoordinateTransformation( pEnvSpaRef, pLayerSpaRef.get() );
 //           poCT->Transform(1, &Env.MaxX, &Env.MaxY);
 //           poCT->Transform(1, &Env.MinX, &Env.MinY);
 //           OCTDestroyCoordinateTransformation(poCT);
 //       }
 //   }

	////2. set spatial filter
	//pDisplay->StartDrawing(GetCacheID());
	//if(m_pRasterRenderer && m_pRasterRenderer->CanRender(m_pwxGISRasterDataset))
	//{
	//	m_pRasterRenderer->Draw(m_pwxGISRasterDataset, DrawPhase, pDisplay, pTrackCancel);
	//////	wxGISFeatureSet* pGISFeatureSet(NULL);
	////	//3. get raster set
	////	wxGISSpatialFilter pFilter;
	////	pFilter.SetEnvelope(Env);
	//////	pGISFeatureSet = m_pwxGISFeatureDataset->GetFeatureSet(&pFilter, pTrackCancel);
	////	wxImage Image = m_pwxGISRasterDataset->GetSubimage(pDisplayTransformation, &pFilter, pTrackCancel);
	////	if(Image.IsOk())
	////	{
	////		pDisplay->DrawBitmap(Image, 0, 0);
	////		//4. send it to renderer
	//////	m_pFeatureRenderer->Draw(pGISFeatureSet, DrawPhase, pDisplay, pTrackCancel);
	//	m_pPreviousDisplayEnv = Env;
	////	}
	//}
	////5. clear a spatial filter		
	//pDisplay->FinishDrawing();
    */
}

//void wxGISRasterLayer::SetSpatialReference(OGRSpatialReferenceSPtr pSpatialReference)
//{
//    if(NULL == pSpatialReference)
//        return;
//    if(m_pSpatialReference && m_pSpatialReference->IsSame(pSpatialReference.get()))
//        return;
//    m_pSpatialReference = pSpatialReference;
//	//TODO: перепроецирования растра делать поблочно для его текущего уровня пирамид. После смены уровня оставлять запомненым в массиве - подумать об использовании in memory raster для этих целей.
//}

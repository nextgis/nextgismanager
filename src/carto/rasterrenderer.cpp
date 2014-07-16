/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISRasterRenderer classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013 Dmitry Baryshnikov
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
#include "wxgis/carto/rasterrenderer.h"
#include "wxgis/carto/rasterlayer.h"
#include "wxgis/core/config.h"
#include "wxgis/core/app.h"
#include "wxgis/display/displayop.h"

//-----------------------------------
// Interpolation functions
//-----------------------------------

void NearestNeighbourInterpolation(void *pInputData, int nInputXSize, double dInputXSize, double dInputYSize, double dDeltaX, double dDeltaY , GDALDataType eSrcType, unsigned char *pOutputData, int nOutXSize, int nOutYSize, int nBegY, int nEndY, int nBandCount, wxGISRasterRenderer* const pRasterRenderer, ITrackCancel* const pTrackCancel)
{
	if(nEndY > nOutYSize || pRasterRenderer == NULL)
		return;

	double dXRatio = dInputXSize / nOutXSize;
	double dYRatio = dInputYSize / nOutYSize;
    double dfR, dfG, dfB, dfA;
    int nIndex;
    wxGISColorTable mColorTable = pRasterRenderer->GetColorTable();

    for(int nDestPixY = nBegY; nDestPixY < nEndY; ++nDestPixY)
    {
        int nOrigPixY = (int)(dYRatio * double(nDestPixY) + dDeltaY);
        int scan_line = nOrigPixY * nInputXSize;
        for(int nDestPixX = 0; nDestPixX < nOutXSize; ++nDestPixX)
        {
            int nOrigPixX = (int)(dXRatio * double(nDestPixX) + dDeltaX);
            int src_pixel_index = scan_line + nOrigPixX;

            src_pixel_index *= nBandCount;

            switch(pRasterRenderer->GetRasterRenderType())
            {
            case enumGISRasterRenderTypeIndexed:
                {
                    nIndex = (int)SRCVAL(pInputData, eSrcType, src_pixel_index);
                    wxGISColor Color = mColorTable[nIndex];
                    dfR = Color.Red();
                    dfG = Color.Green();
                    dfB = Color.Blue();
                    dfA = Color.Alpha();
                    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, &dfA);
                }
                break;
            case enumGISRasterRenderTypeGreyScale:
				{
                    dfR = (double)SRCVAL(pInputData, eSrcType, src_pixel_index);
					pRasterRenderer->FillPixel(pOutputData, &dfR, NULL, NULL, NULL);
				}
				break;
            case enumGISRasterRenderTypeRGBA:
			    switch(nBandCount)
			    {
			    case 1:
				    {
                        dfR = (double)SRCVAL(pInputData, eSrcType, src_pixel_index);
					    pRasterRenderer->FillPixel(pOutputData, &dfR, NULL, NULL, NULL);
				    }
				    break;
			    case 3:
				    {
					    dfR = (double)SRCVAL(pInputData, eSrcType, src_pixel_index);
					    dfG = (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 1);
					    dfB = (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 2);
					    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, NULL);
				    }
				    break;
			    case 4:
				    {
					    dfR = (double)SRCVAL(pInputData, eSrcType, src_pixel_index);
					    dfG = (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 1);
					    dfB = (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 2);
					    dfA = (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 3);
					    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, &dfA);
				    }
				    break;
			    default:
				    break;
			    }
                break;
            case enumGISRasterRenderTypePackedRGBA:
                {
                    wxColor Color(SRCVAL(pInputData, eSrcType, src_pixel_index));
                    dfR = Color.Red();
                    dfG = Color.Green();
                    dfB = Color.Blue();
                    dfA = Color.Alpha();
                    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, &dfA);
                }
                break;
            };

            pOutputData += 4;//ARGB32

            if(pTrackCancel && !pTrackCancel->Continue())
                return;//TODO: return false to indicate not to draw anything on layer
		}
	}
}


void BilinearInterpolation(void *pInputData, int nInputXSize, int nInputYSize, double dInputXSize, double dInputYSize, double dDeltaX, double dDeltaY , GDALDataType eSrcType, unsigned char *pOutputData, int nOutXSize, int nOutYSize, int nBegY, int nEndY, int nBandCount, wxGISRasterRenderer* const pRasterRenderer, ITrackCancel* const pTrackCancel)
{
	if(nEndY > nOutYSize || pRasterRenderer == NULL)
		return;

	double dXRatio = dInputXSize / nOutXSize;
	double dYRatio = dInputYSize / nOutYSize;

	int srcpixymax = nInputYSize - 1;
	int srcpixxmax = nInputXSize - 1;

    double srcpixy, srcpixy2, dy, dy1;
    double srcpixx, srcpixx2, dx, dx1;
    int srcpixy1, srcpixx1;
	double r1, g1, b1, a1;
	double r2, g2, b2, a2;
    int x_offset1, x_offset2, y_offset1, y_offset2;
    int src_pixel_index00, src_pixel_index01, src_pixel_index10, src_pixel_index11;
    double dfR, dfG, dfB, dfA;
    long nIndex1, nIndex2, nIndex3, nIndex4;

    wxGISColorTable mColorTable = pRasterRenderer->GetColorTable();

     for(int nDestPixY = nBegY; nDestPixY < nEndY; ++nDestPixY)
     {
        srcpixy = dYRatio * double(nDestPixY) + dDeltaY;
        srcpixy1 = (int)(srcpixy);
        srcpixy2 = ( srcpixy1 == srcpixymax ) ? srcpixy1 : srcpixy1 + 1.0;
        dy = srcpixy - srcpixy1;
        dy1 = 1.0 - dy;

        for(int nDestPixX = 0; nDestPixX < nOutXSize; ++nDestPixX)
        {
            // X-axis of pixel to interpolate from
            srcpixx = dXRatio * double(nDestPixX) + dDeltaX;
			srcpixx1 = (int)(srcpixx);
			srcpixx2 = ( srcpixx1 == srcpixxmax ) ? srcpixx1 : srcpixx1 + 1.0;
			dx = srcpixx - srcpixx1;
			dx1 = 1.0 - dx;

			x_offset1 = srcpixx1 < 0 ? 0 : srcpixx1 > srcpixxmax ? srcpixxmax : srcpixx1;
			x_offset2 = srcpixx2 < 0 ? 0 : srcpixx2 > srcpixxmax ? srcpixxmax : srcpixx2;
			y_offset1 = srcpixy1 < 0 ? 0 : srcpixy1 > srcpixymax ? srcpixymax : srcpixy1;
			y_offset2 = srcpixy2 < 0 ? 0 : srcpixy2 > srcpixymax ? srcpixymax : srcpixy2;

			src_pixel_index00 = (y_offset1 * nInputXSize + x_offset1) * nBandCount;
			src_pixel_index01 = (y_offset1 * nInputXSize + x_offset2) * nBandCount;
			src_pixel_index10 = (y_offset2 * nInputXSize + x_offset1) * nBandCount;
			src_pixel_index11 = (y_offset2 * nInputXSize + x_offset2) * nBandCount;

            switch(pRasterRenderer->GetRasterRenderType())
            {
            case enumGISRasterRenderTypeIndexed:
                {
                    nIndex1 = (int)SRCVAL(pInputData, eSrcType, src_pixel_index00);
                    nIndex2 = (int)SRCVAL(pInputData, eSrcType, src_pixel_index01);
                    if(nIndex1 == nIndex2)
                    {
                        wxGISColor Color00 = mColorTable[nIndex1];
                        //first line
                        r1 = Color00.Red() * dx1 + Color00.Red() * dx;
                        g1 = Color00.Green() * dx1 + Color00.Green() * dx;
                        b1 = Color00.Blue() * dx1 + Color00.Blue() * dx;
                        a1 = Color00.Alpha() * dx1 + Color00.Alpha() * dx;
                    }
                    else
                    {
                        wxGISColor Color00 = mColorTable[nIndex1];
                        wxGISColor Color01 = mColorTable[nIndex2];
                        //first line
                        r1 = Color00.Red() * dx1 + Color01.Red() * dx;
                        g1 = Color00.Green() * dx1 + Color01.Green() * dx;
                        b1 = Color00.Blue() * dx1 + Color01.Blue() * dx;
                        a1 = Color00.Alpha() * dx1 + Color01.Alpha() * dx;
                    }

                    nIndex3 = (int)SRCVAL(pInputData, eSrcType, src_pixel_index10);
                    nIndex4 = (int)SRCVAL(pInputData, eSrcType, src_pixel_index11);
                    if(nIndex1 == nIndex3 && nIndex2 == nIndex4)
                    {
                        r2 = r1;
                        g2 = g1;
                        b2 = b1;
                        a2 = a1;
                    }
                    else if(nIndex3 == nIndex4)
                    {
                        wxGISColor Color10 = mColorTable[nIndex3];
                        //second line
                        r2 = Color10.Red() * dx1 + Color10.Red() * dx;
                        g2 = Color10.Green() * dx1 + Color10.Green() * dx;
                        b2 = Color10.Blue() * dx1 + Color10.Blue() * dx;
                        a2 = Color10.Alpha() * dx1 + Color10.Alpha() * dx;
                    }
                    else
                    {
                        wxGISColor Color10 = mColorTable[nIndex3];
                        wxGISColor Color11 = mColorTable[nIndex4];
                        //second line
                        r2 = Color10.Red() * dx1 + Color11.Red() * dx;
                        g2 = Color10.Green() * dx1 + Color11.Green() * dx;
                        b2 = Color10.Blue() * dx1 + Color11.Blue() * dx;
                        a2 = Color10.Alpha() * dx1 + Color11.Alpha() * dx;
                    }

                    if(IsDoubleEquil(r1, r2))
                        dfR = r1;
                    else
                        dfR = r1 * dy1 + r2 * dy;
                    if(IsDoubleEquil(g1, g2))
                        dfG = g1;
                    else
                        dfG = g1 * dy1 + g2 * dy;
                    if(IsDoubleEquil(b1, b2))
                        dfB = b1;
                    else
                        dfB = b1 * dy1 + b2 * dy;
                    if(IsDoubleEquil(a1, a2))
                        dfA = a1;
                    else
                        dfA = a1 * dy1 + a2 * dy;

                    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, &dfA);
                }
                break;
            case enumGISRasterRenderTypeGreyScale:
            case enumGISRasterRenderTypeRGBA:
			    switch(nBandCount)
			    {
			    case 1:
				    {
                        //first line
                        r1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01) * dx;

                        //second line
                        r2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11) * dx;


                        if(IsDoubleEquil(r1, r2))
                            dfR = r1;
                        else
                            dfR = r1 * dy1 + r2 * dy;

					    pRasterRenderer->FillPixel(pOutputData, &dfR, NULL, NULL, NULL);
				    }
				    break;
			    case 3:
				    {
                        //first line
                        r1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01) * dx;
                        g1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00 + 1) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01 + 1) * dx;
                        b1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00 + 2) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01 + 2) * dx;

                        //second line
                        r2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11) * dx;
                        g2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10 + 1) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11 + 1) * dx;
                        b2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10 + 2) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11 + 2) * dx;


                        if(IsDoubleEquil(r1, r2))
                            dfR = r1;
                        else
                            dfR = r1 * dy1 + r2 * dy;
                        if(IsDoubleEquil(g1, g2))
                            dfG = g1;
                        else
                            dfG = g1 * dy1 + g2 * dy;
                        if(IsDoubleEquil(b1, b2))
                            dfB = b1;
                        else
                            dfB = b1 * dy1 + b2 * dy;
					    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, NULL);
				    }
				    break;
			    case 4:
				    {
                        //first line
                        r1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01) * dx;
                        g1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00 + 1) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01 + 1) * dx;
                        b1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00 + 2) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01 + 2) * dx;
                        a1 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index00 + 3) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index01 + 3) * dx;

                        //second line
                        r2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11) * dx;
                        g2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10 + 1) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11 + 1) * dx;
                        b2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10 + 2) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11 + 2) * dx;
                        a2 = (double)SRCVAL(pInputData, eSrcType, src_pixel_index10 + 3) * dx1 + (double)SRCVAL(pInputData, eSrcType, src_pixel_index11 + 3) * dx;


                        if(IsDoubleEquil(r1, r2))
                            dfR = r1;
                        else
                            dfR = r1 * dy1 + r2 * dy;
                        if(IsDoubleEquil(g1, g2))
                            dfG = g1;
                        else
                            dfG = g1 * dy1 + g2 * dy;
                        if(IsDoubleEquil(b1, b2))
                            dfB = b1;
                        else
                            dfB = b1 * dy1 + b2 * dy;
                        if(IsDoubleEquil(a1, a2))
                            dfA = a1;
                        else
                            dfA = a1 * dy1 + a2 * dy;

					    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, &dfA);
				    }
				    break;
			    default:
				    break;
			    }
                break;
             case enumGISRasterRenderTypePackedRGBA:
                {
                    wxGISColor Color00(SRCVAL(pInputData, eSrcType, src_pixel_index00));
                    wxGISColor Color01(SRCVAL(pInputData, eSrcType, src_pixel_index01));
                    wxGISColor Color10(SRCVAL(pInputData, eSrcType, src_pixel_index10));
                    wxGISColor Color11(SRCVAL(pInputData, eSrcType, src_pixel_index11));

                    //first line
                    r1 = Color00.Red() * dx1 + Color01.Red() * dx;
                    g1 = Color00.Green() * dx1 + Color01.Green() * dx;
                    b1 = Color00.Blue() * dx1 + Color01.Blue() * dx;
                    a1 = Color00.Alpha() * dx1 + Color01.Alpha() * dx;
                    //second line
                    r2 = Color10.Red() * dx1 + Color11.Red() * dx;
                    g2 = Color10.Green() * dx1 + Color11.Green() * dx;
                    b2 = Color10.Blue() * dx1 + Color11.Blue() * dx;
                    a2 = Color10.Alpha() * dx1 + Color11.Alpha() * dx;

                    dfR = r1 * dy1 + r2 * dy;
                    dfG = g1 * dy1 + g2 * dy;
                    dfB = b1 * dy1 + b2 * dy;
                    dfA = a1 * dy1 + a2 * dy;

                    pRasterRenderer->FillPixel(pOutputData, &dfR, &dfG, &dfB, &dfA);
                }
                break;
           };

            pOutputData += 4;//ARGB32

            if(pTrackCancel && !pTrackCancel->Continue())
                return;//TODO: return false to indicate not to draw anything on layer
		}
	}
}

void BicubicInterpolation(void *pInputData, int nInputXSize, int nInputYSize, double dInputXSize, double dInputYSize, double dDeltaX, double dDeltaY , GDALDataType eSrcType, unsigned char *pOutputData, int nOutXSize, int nOutYSize, int nBegY, int nEndY, int nBandCount, wxGISRasterRenderer* const pRasterRenderer, ITrackCancel* const pTrackCancel)
{
	if(nEndY > nOutYSize || pRasterRenderer == NULL)
		return;

	double dXRatio = dInputXSize / nOutXSize;
	double dYRatio = dInputYSize / nOutYSize;

	int srcpixymax = nInputYSize - 1;
	int srcpixxmax = nInputXSize - 1;

    double srcpixy, dy;
    double srcpixx, dx;
    wxGISColorTable mColorTable = pRasterRenderer->GetColorTable();

    double sum_r = 0, sum_g = 0, sum_b = 0, sum_a = 0;

     for(int nDestPixY = nBegY; nDestPixY < nEndY; ++nDestPixY)
     {
        srcpixy = dYRatio * double(nDestPixY) + dDeltaY;
        dy = srcpixy - (int)srcpixy;

        for(int nDestPixX = 0; nDestPixX < nOutXSize; ++nDestPixX)
        {
            // X-axis of pixel to interpolate from
            srcpixx = dXRatio * double(nDestPixX) + dDeltaX;
			dx = srcpixx - (int)srcpixx;

            sum_r = sum_g = sum_b = sum_a = 0;

            // Here we actually determine the RGBA values for the destination pixel
            for ( int k = -1; k <= 2; k++ )
            {
                // Y offset
                int y_offset = srcpixy + k < 0.0 ? 0 : srcpixy + k >= nInputYSize ? nInputYSize - 1 : (int)(srcpixy + k);

                // Loop across the X axis
                for ( int i = -1; i <= 2; ++i )
                {
                    // X offset
                    int x_offset = srcpixx + i < 0.0 ? 0 : srcpixx + i >= nInputXSize ? nInputXSize - 1 : (int)(srcpixx + i);

                    // Calculate the exact position where the source data
                    // should be pulled from based on the x_offset and y_offset
                    int src_pixel_index = (y_offset * nInputXSize + x_offset) * nBandCount;

                    // Calculate the weight for the specified pixel according
                    // to the bicubic b-spline kernel we're using for
                    // interpolation
                    double pixel_weight = BiCubicKernel(i - dx) * BiCubicKernel(k - dy);

                    // Create a sum of all velues for each color channel
                    // adjusted for the pixel's calculated weight

                    switch(pRasterRenderer->GetRasterRenderType())
                    {
                    case enumGISRasterRenderTypeIndexed:
                        {
                            wxGISColor Color = mColorTable[SRCVAL(pInputData, eSrcType, src_pixel_index)];
                            sum_r += Color.Red() * pixel_weight;
                            sum_g += Color.Green() * pixel_weight;
                            sum_b += Color.Blue() * pixel_weight;
                            sum_a += Color.Alpha() * pixel_weight;
                        }
                        break;
                    case enumGISRasterRenderTypeRGBA:
			            switch(nBandCount)
			            {
			            case 1:
                            sum_r += (double)SRCVAL(pInputData, eSrcType, src_pixel_index) * pixel_weight;
				            break;
			            case 3:
                            sum_r += (double)SRCVAL(pInputData, eSrcType, src_pixel_index) * pixel_weight;
                            sum_g += (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 1) * pixel_weight;
                            sum_b += (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 2) * pixel_weight;
				            break;
			            case 4:
                            sum_r += (double)SRCVAL(pInputData, eSrcType, src_pixel_index) * pixel_weight;
                            sum_g += (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 1) * pixel_weight;
                            sum_b += (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 2) * pixel_weight;
                            sum_a += (double)SRCVAL(pInputData, eSrcType, src_pixel_index + 3) * pixel_weight;
				            break;
			            default:
				            break;
			            }
                        break;
                     case enumGISRasterRenderTypePackedRGBA:
                        {
                            wxColor Color(SRCVAL(pInputData, eSrcType, src_pixel_index));
                            sum_r += Color.Red() * pixel_weight;
                            sum_g += Color.Green() * pixel_weight;
                            sum_b += Color.Blue() * pixel_weight;
                            sum_a += Color.Alpha() * pixel_weight;
                       }
                        break;
                    };
                }
            }

            switch(pRasterRenderer->GetRasterRenderType())
            {
            case enumGISRasterRenderTypeIndexed:
                pRasterRenderer->FillPixel(pOutputData, &sum_r, &sum_g, &sum_b, &sum_a);
                break;
            case enumGISRasterRenderTypeRGBA:
			    switch(nBandCount)
			    {
			    case 1:
                    pRasterRenderer->FillPixel(pOutputData, &sum_r, NULL, NULL, NULL);
				    break;
			    case 3:
                    pRasterRenderer->FillPixel(pOutputData, &sum_r, &sum_g, &sum_b, NULL);
				    break;
			    case 4:
                    pRasterRenderer->FillPixel(pOutputData, &sum_r, &sum_g, &sum_b, &sum_a);
				    break;
			    default:
				    break;
			    }
                break;
            case enumGISRasterRenderTypePackedRGBA:
                pRasterRenderer->FillPixel(pOutputData, &sum_r, &sum_g, &sum_b, &sum_a);
                break;
            };

            pOutputData += 4;//ARGB32

            if(pTrackCancel && !pTrackCancel->Continue())
                return;//TODO: return false to indicate not to draw anything on layer
		}
	}
}

//-----------------------------------
// wxRasterDrawThread
//-----------------------------------

wxRasterDrawThread::wxRasterDrawThread(RAWPIXELDATA &stPixelData, GDALDataType eSrcType, int nBandCount, unsigned char *pTransformData, wxGISEnumDrawQuality eQuality, int nOutXSize, int nOutYSize, int nBegY, int nEndY, wxGISRasterRenderer *pRasterRenderer, ITrackCancel * const pTrackCancel) : wxThread(wxTHREAD_JOINABLE), m_stPixelData(stPixelData), m_pTrackCancel(pTrackCancel)
{
	m_eSrcType = eSrcType;
	m_pTransformData = pTransformData;
	m_nBandCount = nBandCount;
	m_eQuality = eQuality;
	m_nOutXSize = nOutXSize;
	m_nOutYSize = nOutYSize;
	m_nBegY = nBegY;
	m_nEndY = nEndY;
	m_pRasterRenderer = pRasterRenderer;
}

void *wxRasterDrawThread::Entry()
{
	switch(m_eQuality)
	{
	case enumGISQualityBilinear:
        BilinearInterpolation(m_stPixelData.pPixelData, m_stPixelData.nPixelDataWidth, m_stPixelData.nPixelDataHeight, m_stPixelData.dPixelDataWidth, m_stPixelData.dPixelDataHeight, m_stPixelData.dPixelDeltaX, m_stPixelData.dPixelDeltaY, m_eSrcType, m_pTransformData, m_nOutXSize, m_nOutYSize, m_nBegY, m_nEndY, m_nBandCount, m_pRasterRenderer, m_pTrackCancel);
		break;
	case enumGISQualityBicubic:
		BicubicInterpolation(m_stPixelData.pPixelData, m_stPixelData.nPixelDataWidth, m_stPixelData.nPixelDataHeight, m_stPixelData.dPixelDataWidth, m_stPixelData.dPixelDataHeight, m_stPixelData.dPixelDeltaX, m_stPixelData.dPixelDeltaY, m_eSrcType, m_pTransformData, m_nOutXSize, m_nOutYSize, m_nBegY, m_nEndY, m_nBandCount, m_pRasterRenderer, m_pTrackCancel);
		break;
	//case enumGISQualityHalfBilinear:
	//	OnHalfBilinearInterpolation(m_pOrigData, m_pDestData, m_nYbeg, m_nYend, m_nOrigX, m_nOrigY, m_nDestX,  rWRatio, rHRatio, m_rDeltaX, m_rDeltaY, m_pTrackCancel);
	//	break;
	//case enumGISQualityHalfQuadBilinear:
	//	OnHalfQuadBilinearInterpolation(m_pOrigData, m_pDestData, m_nYbeg, m_nYend, m_nOrigX, m_nOrigY, m_nDestX,  rWRatio, rHRatio, m_rDeltaX, m_rDeltaY, m_pTrackCancel);
	//	break;
	case enumGISQualityNearest:
	default:
	    NearestNeighbourInterpolation(m_stPixelData.pPixelData, m_stPixelData.nPixelDataWidth, m_stPixelData.dPixelDataWidth, m_stPixelData.dPixelDataHeight, m_stPixelData.dPixelDeltaX, m_stPixelData.dPixelDeltaY, m_eSrcType, m_pTransformData, m_nOutXSize, m_nOutYSize, m_nBegY, m_nEndY, m_nBandCount, m_pRasterRenderer, m_pTrackCancel);
		break;
	};

	return (wxThread::ExitCode)wxTHREAD_NO_ERROR;
}

void wxRasterDrawThread::OnExit()
{
}

//-----------------------------------
// wxGISRasterRenderer
//-----------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxGISRasterRenderer, wxGISRenderer)

wxGISRasterRenderer::wxGISRasterRenderer(wxGISLayer* pwxGISLayer) : wxGISRenderer(pwxGISLayer)
{
    m_nTileSizeX = m_nTileSizeY = 256;
	wxGISAppConfig oConfig = GetConfig();

    if(oConfig.IsOk())
    {
    	wxString sAppName = GetApplication()->GetAppName();

    	m_eQuality = (wxGISEnumDrawQuality)oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/renderer/raster/quality")), enumGISQualityBilinear);
        //TODO: tiled draw
        m_nTileSizeX = oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/renderer/raster/tile_size_x")), m_nTileSizeX);
        m_nTileSizeY = oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/renderer/raster/tile_size_y")), m_nTileSizeY);
    }
    else
        m_eQuality = enumGISQualityBilinear;
	m_oNoDataColor = wxColor(0,0,0,0);

    if(pwxGISLayer)
        m_pwxGISRasterDataset = wxDynamicCast(pwxGISLayer->GetDataset(), wxGISRasterDataset);
    else
        m_pwxGISRasterDataset = NULL;
}

wxGISRasterRenderer::~wxGISRasterRenderer(void)
{
    wsDELETE(m_pwxGISRasterDataset);
}

bool wxGISRasterRenderer::CanRender(wxGISLayer* const pwxGISLayer) const
{
    wxCHECK_MSG(pwxGISLayer, false, wxT("Input dataset pointer is NULL"));
	return pwxGISLayer->GetType() == enumGISRasterDataset ? true : false;
}

bool wxGISRasterRenderer::Apply(ITrackCancel* const pTrackCancel)
{
    return true;
}

bool wxGISRasterRenderer::Draw(wxGISEnumDrawPhase DrawPhase, wxGISDisplay* const pDisplay, ITrackCancel* const pTrackCancel)
{
    wxCHECK_MSG(pDisplay, false, wxT("Display pointer is NULL"));

	OGREnvelope stRasterExtent = m_pwxGISRasterDataset->GetEnvelope();
	OGREnvelope stDisplayExtentRotated = pDisplay->GetBounds(true);
	OGREnvelope stRasterExtentRotated = stRasterExtent;

	//rotate raster extent
	if(!IsDoubleEquil(pDisplay->GetRotate(), 0.0))
	{
        wxRealPoint dfCenter = pDisplay->GetBoundsCenter();
		//double dCenterX = stDisplayExtentRotated.MinX + (stDisplayExtentRotated.MaxX - stDisplayExtentRotated.MinX) / 2;
		//double dCenterY = stDisplayExtentRotated.MinY + (stDisplayExtentRotated.MaxY - stDisplayExtentRotated.MinY) / 2;

		RotateEnvelope(stRasterExtentRotated, pDisplay->GetRotate(), dfCenter.x, dfCenter.y);//dCenterX, dCenterY);
	}

	//if envelopes don't intersect exit
    if(!stDisplayExtentRotated.Intersects(stRasterExtentRotated))
        return false;

	//get intersect envelope to fill raster data
	OGREnvelope stDrawBounds = stDisplayExtentRotated;
	stDrawBounds.Intersect(stRasterExtentRotated);
	if(!stDrawBounds.IsInit())
		return false;

	if(!stRasterExtent.Contains(stDrawBounds))
		stDrawBounds = stRasterExtent;


	GDALDataset* pRaster = m_pwxGISRasterDataset->GetRaster();
	//create inverse geo transform to get pixel data
	double adfGeoTransform[6] = { 0, 0, 0, 0, 0, 0 };
	double adfReverseGeoTransform[6] = { 0, 0, 0, 0, 0, 0 };
	CPLErr err = pRaster->GetGeoTransform(adfGeoTransform);
	bool bNoTransform(false);
	if(err != CE_None)
	{
		bNoTransform = true;
	}
	else
	{
		int nRes = GDALInvGeoTransform( adfGeoTransform, adfReverseGeoTransform );
	}

	//width & height of extent
    double dOutWidth = stDrawBounds.MaxX - stDrawBounds.MinX;
    double dOutHeight = stDrawBounds.MaxY - stDrawBounds.MinY;

	//get width & height in pixels of draw area
	pDisplay->World2DCDist(&dOutWidth, &dOutHeight, false);
    dOutWidth = abs(dOutWidth);
    dOutHeight = abs(dOutHeight);

	//round float pixel to int using ceil
	int nOutWidth = ceil(dOutWidth);
	int nOutHeight = ceil(dOutHeight);

	//raster size
    int nXSize = m_pwxGISRasterDataset->GetWidth();
    int nYSize = m_pwxGISRasterDataset->GetHeight();

	//transform from world extent to pixel bounds
	OGREnvelope stPixelBounds = stDrawBounds;
    if(bNoTransform)
    {
		//swap min/max
        stPixelBounds.MaxY = nYSize - stDrawBounds.MinY;
        stPixelBounds.MinY = nYSize - stDrawBounds.MaxY;
    }
    else
    {
		GDALApplyGeoTransform( adfReverseGeoTransform, stDrawBounds.MinX, stDrawBounds.MinY, &stPixelBounds.MinX, &stPixelBounds.MaxY );
		GDALApplyGeoTransform( adfReverseGeoTransform, stDrawBounds.MaxX, stDrawBounds.MaxY, &stPixelBounds.MaxX, &stPixelBounds.MinY );
    }

    if (stPixelBounds.MaxX < stPixelBounds.MinX)
    {
        wxSwap(stPixelBounds.MaxX, stPixelBounds.MinX);
    }

    if (stPixelBounds.MaxY < stPixelBounds.MinY)
    {
        wxSwap(stPixelBounds.MaxY, stPixelBounds.MinY);
    }

	//get width & height in pixels of raster area
    double dWidth = stPixelBounds.MaxX - stPixelBounds.MinX;
    double dHeight = stPixelBounds.MaxY - stPixelBounds.MinY;

	int nWidth = ceil(dWidth);
	int nHeight = ceil(dHeight);
    int nMinX = floor(stPixelBounds.MinX);
    int nMinY = floor(stPixelBounds.MinY);

    //correct data
    if(nWidth > nXSize) nWidth = nXSize;
    if(nHeight > nYSize) nHeight = nYSize;
    if(nMinX < 0) nMinX = 0;
    if(nMinY < 0) nMinY = 0;

	GDALDataType eDT = m_pwxGISRasterDataset->GetDataType();
	int nDataSize = GDALGetDataTypeSize(eDT) / 8;

	void *data = NULL;
	int nBandCount(0);
	int *panBands = GetBandsCombination(&nBandCount);

	RAWPIXELDATA stPixelData;
	stPixelData.pPixelData = NULL;
	stPixelData.nOutputWidth = stPixelData.nOutputHeight = -1;
	//stPixelData.nPixelDataWidth = stPixelData.nPixelDataHeight = -1;

	if( nOutWidth > nWidth && nOutHeight > nHeight ) // not scale
	{
		//increase little in data
		//nWidth += 2; nHeight += 2;
		if(nWidth > nXSize) nWidth = nXSize;
		if(nHeight > nYSize) nHeight = nYSize;

		stPixelData.dPixelDeltaX = stPixelBounds.MinX - double(nMinX);// - 0.5;
		stPixelData.dPixelDeltaY = stPixelBounds.MinY - double(nMinY);// - 0.5;
		stPixelData.dPixelDataWidth = dWidth;
		stPixelData.dPixelDataHeight = dHeight;
		stPixelData.nPixelDataWidth = nWidth;
		stPixelData.nPixelDataHeight = nHeight;
		stPixelData.nOutputWidth = nOutWidth;
		stPixelData.nOutputHeight = nOutHeight;

	}
	else
	{
        //get closest overview and get overview data
        int nMinXTmp = nMinX;
        int nMinYTmp = nMinY;
        int nOutWidthOv = nWidth;
        int nOutHeightOv = nHeight;

        int nOverview = GDALBandGetBestOverviewLevel(pRaster->GetRasterBand(panBands[0]), nMinXTmp, nMinYTmp, nOutWidthOv, nOutHeightOv, nOutWidth, nOutHeight);
        if (nOverview >= 0)
        {
    		stPixelData.dPixelDeltaX = stPixelData.dPixelDeltaY = 0;
		    stPixelData.dPixelDataWidth = nOutWidthOv;
		    stPixelData.dPixelDataHeight = nOutHeightOv;
		    stPixelData.nPixelDataWidth = nOutWidthOv;
		    stPixelData.nPixelDataHeight = nOutHeightOv;
		    stPixelData.nOutputWidth = nOutWidth;
		    stPixelData.nOutputHeight = nOutHeight;
        }
        else
        {
		    stPixelData.dPixelDeltaX = stPixelData.dPixelDeltaY = 0;
		    stPixelData.dPixelDataWidth = nOutWidth;
		    stPixelData.dPixelDataHeight = nOutHeight;
		    stPixelData.nPixelDataWidth = nOutWidth;
		    stPixelData.nPixelDataHeight = nOutHeight;
        }
	}

	data = CPLMalloc (stPixelData.nPixelDataWidth * stPixelData.nPixelDataHeight * nDataSize * nBandCount);
	if(!m_pwxGISRasterDataset->GetPixelData(data, nMinX, nMinY, nWidth, nHeight, stPixelData.nPixelDataWidth, stPixelData.nPixelDataHeight, eDT, nBandCount, panBands))
    {
        CPLFree (data);
		return false;
    }

	stPixelData.pPixelData = data;
	stPixelData.stWorldBounds = stDrawBounds;

    bool bRes = Draw(stPixelData, DrawPhase, pDisplay, pTrackCancel);
    CPLFree (data);
	return bRes;
}

bool wxGISRasterRenderer::Draw(RAWPIXELDATA &stPixelData, wxGISEnumDrawPhase DrawPhase, wxGISDisplay * const pDisplay, ITrackCancel * const pTrackCancel)
{
    bool bScale = false;
	int nOutXSize, nOutYSize;
	if(stPixelData.nOutputHeight == -1 || stPixelData.nOutputWidth == -1)
	{
		nOutXSize = stPixelData.nPixelDataWidth;
		nOutYSize = stPixelData.nPixelDataHeight;
	}
	else
	{
        bScale = true;
		nOutXSize = stPixelData.nOutputWidth;
		nOutYSize = stPixelData.nOutputHeight;
	}

	int stride = cairo_format_stride_for_width (CAIRO_FORMAT_ARGB32, nOutXSize);
	if(stride == -1)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("cairo_format_stride_for_width failed"), wxNOT_FOUND, enumGISMessageErr);
		return false;
    }
	unsigned char *pTransformPixelData = (unsigned char *)CPLMalloc (stride * nOutYSize);

	cairo_surface_t *surface;
	surface = cairo_image_surface_create_for_data (pTransformPixelData, CAIRO_FORMAT_ARGB32, nOutXSize, nOutYSize, stride);

    //TODO: more parts
    int CPUCount = wxThread::GetCPUCount();//1;//
    wxVector<wxRasterDrawThread*> threadarray;
    int nPartSize = nOutYSize / CPUCount;
    int nBegY(0), nEndY;
    for(int i = 0; i < CPUCount; ++i)
    {
        if(i == CPUCount - 1)
            nEndY = nOutYSize;
        else
            nEndY = nPartSize * (i + 1);

		unsigned char* pDestInputData = pTransformPixelData + (nBegY * 4 * nOutXSize);
		wxRasterDrawThread *thread = new wxRasterDrawThread(stPixelData, m_pwxGISRasterDataset->GetDataType(), GetBandCount(), pDestInputData, bScale == true ? m_eQuality : enumGISQualityNearest, nOutXSize, nOutYSize, nBegY, nEndY, this, pTrackCancel);
		if(CreateAndRunThread(thread, wxT("wxRasterDrawThread"), wxT("RasterDrawThread")))
            threadarray.push_back(thread);
        nBegY = nEndY;
    }

    for(size_t i = 0; i < threadarray.size(); ++i)
    {
        wgDELETE(threadarray[i], Wait());

        cairo_surface_mark_dirty(surface);
        pDisplay->DrawRaster(surface, stPixelData.stWorldBounds);
        cairo_surface_flush(surface);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
	//pDisplay->DrawRaster(surface, stPixelData.stWorldBounds);

	cairo_surface_destroy(surface);

	CPLFree((void*)pTransformPixelData);

    return true;
}

//-----------------------------------
// wxGISRasterRGBARenderer
//-----------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISRasterRGBARenderer, wxGISRasterRenderer)

wxGISRasterRGBARenderer::wxGISRasterRGBARenderer(wxGISLayer* pwxGISLayer) : wxGISRasterRenderer(pwxGISLayer)
{
	wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
    {
        wxString sAppName = GetApplication()->GetAppName();
        m_bNodataNewBehaviour = oConfig.ReadBool(enumGISHKCU, sAppName + wxString(wxT("/renderer/raster/nodata_newbehaviour")), true);
    }
    else
        m_bNodataNewBehaviour = true;

	m_nRedBand = 1;
	m_nGreenBand = 2;
	m_nBlueBand = 3;
	m_nAlphaBand = -1;

	m_paStretch[0] = new wxGISStretch();
	m_paStretch[1] = new wxGISStretch();
	m_paStretch[2] = new wxGISStretch();
	m_paStretch[3] = new wxGISStretch();

	//m_oBkColorSet =
	//m_oBkColorGet = wxNullColour;//TODO: May be array of  ColorGet/ColorSet

    OnFillStats();
}

wxGISRasterRGBARenderer::~wxGISRasterRGBARenderer(void)
{
    for(size_t i = 0; i < 4; ++i)
        wxDELETE(m_paStretch[i]);
}

bool wxGISRasterRGBARenderer::CanRender(wxGISLayer* const pwxGISLayer) const
{
    if(wxGISRasterRenderer::CanRender(pwxGISLayer))
    {
        //check for more than 3 bands
        wxGISRasterDataset* pwxGISRasterDataset = wxDynamicCast(pwxGISLayer->GetDataset(), wxGISRasterDataset);
        if(NULL != pwxGISRasterDataset)
        {
            GDALDataset* poGDALDataset = pwxGISRasterDataset->GetRaster();
            if(poGDALDataset)
            {
                return poGDALDataset->GetRasterCount() > 2;
            }
            wsDELETE(pwxGISRasterDataset);
        }
    }
	return false;
}

void wxGISRasterRGBARenderer::OnFillStats(void)
{
	GDALDataset* poGDALDataset = m_pwxGISRasterDataset->GetMainRaster();
	if(!poGDALDataset)
		poGDALDataset = m_pwxGISRasterDataset->GetRaster();
	if(!poGDALDataset)
		return;

    for(int i = 1; i <= poGDALDataset->GetRasterCount(); ++i)
    {
        GDALRasterBand* pBand = poGDALDataset->GetRasterBand(i);
        GDALColorInterp eColorInterpretation = pBand->GetColorInterpretation();
        switch(eColorInterpretation)
        {
        case GCI_RedBand:
            m_nRedBand = i;
            break;
        case GCI_GreenBand:
            m_nGreenBand = i;
            break;
        case GCI_BlueBand:
            m_nBlueBand = i;
            break;
        case GCI_AlphaBand:
            m_nAlphaBand = i;
            break;
        default:
            break;
        };
    }

	//set nodata color for each band
    if(m_pwxGISRasterDataset->HasNoData(m_nRedBand) )
    {
        m_paStretch[0]->SetNoData(m_pwxGISRasterDataset->GetNoData(m_nRedBand));
    }

    if(m_pwxGISRasterDataset->HasNoData(m_nGreenBand) )
    {
        m_paStretch[1]->SetNoData(m_pwxGISRasterDataset->GetNoData(m_nGreenBand));
    }

    if(m_pwxGISRasterDataset->HasNoData(m_nBlueBand) )
    {
        m_paStretch[2]->SetNoData(m_pwxGISRasterDataset->GetNoData(m_nBlueBand));
    }

    if (m_nAlphaBand != -1)
    {
        if(m_pwxGISRasterDataset->HasNoData(m_nAlphaBand) )
        {
            m_paStretch[3]->SetNoData(m_pwxGISRasterDataset->GetNoData(m_nAlphaBand));
        }
    }

	//set min/max values
	if(m_pwxGISRasterDataset->HasStatistics())
	{

        double dfMin, dfMax, dfMean, dfStdDev;
		GDALRasterBand* pRedBand = poGDALDataset->GetRasterBand(m_nRedBand);

        if(pRedBand->GetStatistics(FALSE, FALSE, &dfMin, &dfMax, &dfMean, &dfStdDev) == CE_None)
        {
            m_paStretch[0]->SetStats(dfMin, dfMax, dfMean, dfStdDev);
        }

		GDALRasterBand* pGreenBand = poGDALDataset->GetRasterBand(m_nGreenBand);
        if(pGreenBand->GetStatistics(FALSE, FALSE, &dfMin, &dfMax, &dfMean, &dfStdDev) == CE_None)
        {
            m_paStretch[1]->SetStats(dfMin, dfMax, dfMean, dfStdDev);
        }

		GDALRasterBand* pBlueBand = poGDALDataset->GetRasterBand(m_nBlueBand);
         if(pBlueBand->GetStatistics(FALSE, FALSE, &dfMin, &dfMax, &dfMean, &dfStdDev) == CE_None)
        {
            m_paStretch[2]->SetStats(dfMin, dfMax, dfMean, dfStdDev);
        }

        if(m_nAlphaBand != -1)
        {
 		    GDALRasterBand* pAlphaBand = poGDALDataset->GetRasterBand(m_nAlphaBand);
            if(pAlphaBand->GetStatistics(FALSE, FALSE, &dfMin, &dfMax, &dfMean, &dfStdDev) == CE_None)
            {
                m_paStretch[3]->SetStats(dfMin, dfMax, dfMean, dfStdDev);
            }
            //TODO: set stretch type to min-max
        }
	}
}

int *wxGISRasterRGBARenderer::GetBandsCombination(int *pnBandCount)
{
	int *pBands = NULL;
	if(m_nAlphaBand == -1)
	{
		*pnBandCount = 3;
		pBands = new int[3];
		pBands[0] = m_nRedBand;
		pBands[1] = m_nGreenBand;
		pBands[2] = m_nBlueBand;
	}
	else
	{
		*pnBandCount = 4;
		pBands = new int[4];
		pBands[0] = m_nRedBand;
		pBands[1] = m_nGreenBand;
		pBands[2] = m_nBlueBand;
		pBands[3] = m_nAlphaBand;
	}
	return pBands;
}

short wxGISRasterRGBARenderer::GetBandCount() const
{
    return m_nAlphaBand == -1 ? 3 : 4;
}

void wxGISRasterRGBARenderer::FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA)
{
	if(pSrcValR == NULL)
		return;
	//wxBYTE_ORDER          //      x
	//pOutputData[0] = 0;	//	A	B
	//pOutputData[1] = 0;	//	R	G
	//pOutputData[2] = 255;	//	G	R
	//pOutputData[3] = 255;	//	B	A

    double dfR, dfG, dfB, dfA;

	if(pSrcValA == NULL)
    {
		pOutputData[3] = 255;
        dfA = 255;
    }
	else
    {
		pOutputData[3] = m_paStretch[3]->GetValue(pSrcValA);
        dfA = *pSrcValA;
    }

    dfR = *pSrcValR;

	unsigned char RPixVal = m_paStretch[0]->GetValue(pSrcValR);
	pOutputData[2] = RPixVal;

	if(pSrcValG == NULL || pSrcValB == NULL)
	{
		pOutputData[1] = RPixVal;
		pOutputData[0] = RPixVal;
        dfG = dfB = dfR;
	}
	else
	{
		pOutputData[1] = m_paStretch[1]->GetValue(pSrcValG);
		pOutputData[0] = m_paStretch[2]->GetValue(pSrcValB);
        dfG = *pSrcValG;
        dfB = *pSrcValB;
	}

	//check for nodata
	if(m_bNodataNewBehaviour)
	{
        //if(m_paStretch[0]->IsNoData(pOutputData[2]) && m_paStretch[1]->IsNoData(pOutputData[1]) && m_paStretch[2]->IsNoData(pOutputData[0]))
		if(m_paStretch[0]->IsNoData(dfR) && m_paStretch[1]->IsNoData(dfG) && m_paStretch[2]->IsNoData(dfB))
        {
			pOutputData[3] = m_oNoDataColor.Alpha();
			pOutputData[2] = m_oNoDataColor.Red();
			pOutputData[1] = m_oNoDataColor.Green();
			pOutputData[0] = m_oNoDataColor.Blue();
		}
	}
	else
	{
        //if(m_paStretch[0]->IsNoData(pOutputData[2]) || m_paStretch[1]->IsNoData(pOutputData[1]) || m_paStretch[2]->IsNoData(pOutputData[0]))
        if(m_paStretch[0]->IsNoData(dfR) || m_paStretch[1]->IsNoData(dfG) || m_paStretch[2]->IsNoData(dfB))
		{
			pOutputData[3] = m_oNoDataColor.Alpha();
			pOutputData[2] = m_oNoDataColor.Red();
			pOutputData[1] = m_oNoDataColor.Green();
			pOutputData[0] = m_oNoDataColor.Blue();
		}
	}

}

//-----------------------------------
// wxGISRasterRasterColormapRenderer
//-----------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISRasterRasterColormapRenderer, wxGISRasterRenderer)

wxGISRasterRasterColormapRenderer::wxGISRasterRasterColormapRenderer(wxGISLayer* pwxGISLayer) : wxGISRasterRenderer(pwxGISLayer)
{
	m_nBandNumber = 1;
    OnFillColorTable();
}

wxGISRasterRasterColormapRenderer::~wxGISRasterRasterColormapRenderer(void)
{
}

bool wxGISRasterRasterColormapRenderer::CanRender(wxGISLayer* const pwxGISLayer) const
{
    if(wxGISRasterRenderer::CanRender(pwxGISLayer))
    {
        //check for Palette & GDALPaletteInterp == GPI_RGB or
        //GPI_Gray 	 Grayscale (in GDALColorEntry.c1)
        //GPI_RGB 	 Red, Green, Blue and Alpha in (in c1, c2, c3 and c4)
        //GPI_CMYK 	 Cyan, Magenta, Yellow and Black (in c1, c2, c3 and c4)
        //GPI_HLS 	 Hue, Lightness and Saturation (in c1, c2, and c3)
        wxGISRasterDataset* pwxGISRasterDataset = wxDynamicCast(pwxGISLayer->GetDataset(), wxGISRasterDataset);
        if(NULL != pwxGISRasterDataset)
        {
            GDALDataset* poGDALDataset = pwxGISRasterDataset->GetRaster();
            if(poGDALDataset)
            {
                GDALRasterBand* pBand = poGDALDataset->GetRasterBand(1);
	            if(pBand)
                {
                    GDALColorTable* pGDALColorTable = pBand->GetColorTable();
	                if(pGDALColorTable)
                    {
                        GDALColorInterp eColorInterpretation = pBand->GetColorInterpretation();
                        GDALPaletteInterp ePaletteInterpretation = pGDALColorTable->GetPaletteInterpretation();
                        return eColorInterpretation == GCI_PaletteIndex && (ePaletteInterpretation == GPI_RGB || ePaletteInterpretation == GPI_CMYK || ePaletteInterpretation == GPI_HLS);
                    }
                }
            }
            wsDELETE(pwxGISRasterDataset);
        }
    }
	return false;
}

int *wxGISRasterRasterColormapRenderer::GetBandsCombination(int *pnBandCount)
{
	int *pBands = NULL;
	*pnBandCount = 1;
	pBands = new int[1];
	pBands[0] = m_nBandNumber;
	return pBands;
}

short wxGISRasterRasterColormapRenderer::GetBandCount() const
{
    return 1;
}

void wxGISRasterRasterColormapRenderer::OnFillColorTable(void)
{
	GDALDataset* poGDALDataset = m_pwxGISRasterDataset->GetMainRaster();
	if(!poGDALDataset)
		poGDALDataset = m_pwxGISRasterDataset->GetRaster();
	if(!poGDALDataset)
		return;
    GDALRasterBand* pBand = poGDALDataset->GetRasterBand(m_nBandNumber);
	if(!pBand)
		return;
    GDALColorTable* pGDALColorTable = pBand->GetColorTable();
	if(!pGDALColorTable)
		return;
	m_mColorTable.clear();

	GDALColorInterp eColorInterpretation = pBand->GetColorInterpretation();
	GDALPaletteInterp ePaletteInterpretation = pGDALColorTable->GetPaletteInterpretation();

    if(m_pwxGISRasterDataset->HasNoData(m_nBandNumber) )
    {
        m_nNoDataIndex = m_pwxGISRasterDataset->GetNoData(m_nBandNumber);
        m_bHasNoData = true;
    }

    const GDALColorEntry* pstColorEntry = 0;
	for(int i = 0; i < pGDALColorTable->GetColorEntryCount(); ++i )
    {
        if(m_bHasNoData && i == m_nNoDataIndex)
        {
			m_mColorTable[i] = wxColor( 0, 0, 0, 0 );
            continue;
        }
		pstColorEntry = pGDALColorTable->GetColorEntry(i);
		if( !pstColorEntry )
			continue;

        if( eColorInterpretation == GCI_GrayIndex )//TODO: think if needed
        {
			m_mColorTable[i] = wxColor( pstColorEntry->c1, pstColorEntry->c1, pstColorEntry->c1, pstColorEntry->c4 );
        }
        else if( eColorInterpretation == GCI_PaletteIndex )
        {
			switch(ePaletteInterpretation)
			{
			default:
			case GPI_RGB:
				m_mColorTable[i] = wxColor( pstColorEntry->c1, pstColorEntry->c2, pstColorEntry->c3, pstColorEntry->c4 );
				break;
			case GPI_CMYK:
				m_mColorTable[i] = CMYKtoRGB( pstColorEntry->c1, pstColorEntry->c2, pstColorEntry->c3, pstColorEntry->c4 );
				break;
			case GPI_HLS:
				m_mColorTable[i] = HSVtoRGB( pstColorEntry->c1, pstColorEntry->c3, pstColorEntry->c2, pstColorEntry->c4 );
				break;
			};
		}
	}
}

void wxGISRasterRasterColormapRenderer::FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA)
{
	if(pSrcValA == NULL)
		pOutputData[3] = 255;
	else
		pOutputData[3] = (unsigned char)(*pSrcValA);

	unsigned char RPixVal = (unsigned char)(*pSrcValR);
	pOutputData[2] = RPixVal;

	if(pSrcValG == NULL || pSrcValB == NULL)
	{
		pOutputData[1] = RPixVal;
		pOutputData[0] = RPixVal;
	}
	else
	{
		pOutputData[1] = (unsigned char)(*pSrcValG);
		pOutputData[0] = (unsigned char)(*pSrcValB);
	}

	////check for nodata
	//bool bIsChanged(false);
	//if(m_bNodataNewBehaviour)
	//{
 //       if(m_paStretch[0]->IsNoData(dfR) && m_paStretch[1]->IsNoData(dfG) && m_paStretch[2]->IsNoData(dfB))
	//	{
	//		bIsChanged = true;
	//		pOutputData[3] = m_oNoDataColor.Alpha();
	//		pOutputData[2] = m_oNoDataColor.Red();
	//		pOutputData[1] = m_oNoDataColor.Green();
	//		pOutputData[0] = m_oNoDataColor.Blue();
	//	}
	//}
	//else
	//{
 //       if(m_paStretch[0]->IsNoData(dfR) || m_paStretch[1]->IsNoData(dfG) || m_paStretch[2]->IsNoData(dfB))
	//	{
	//		bIsChanged = true;
	//		pOutputData[3] = m_oNoDataColor.Alpha();
	//		pOutputData[2] = m_oNoDataColor.Red();
	//		pOutputData[1] = m_oNoDataColor.Green();
	//		pOutputData[0] = m_oNoDataColor.Blue();
	//	}
	//}

	//if(bIsChanged)
	//	return;

	//check for background data
}

wxColor wxGISRasterRasterColormapRenderer::HSVtoRGB( const short &h, const short &s, const short &v, const short &alpha )
{
	int f;
	long p, q, t;

	if( s == 0 )
		return wxColor(v, v, v, alpha);

	f = ( (h % 60) * 255) / 60;
	long hh = h / 60;

	p = (v * ( 256 - s )) / 256;
	q = (v * ( 256 - (s * f) / 256 )) / 256;
	t = (v * ( 256 - (s * ( 256 - f )) / 256)) / 256;

	switch( hh )
	{
	case 0:
		return wxColor(v, t, p, alpha);
	case 1:
		return wxColor(q, v, p, alpha);
	case 2:
		return wxColor(p, v, t, alpha);
	case 3:
		return wxColor(p, q, v, alpha);
	case 4:
		return wxColor(t, p, v, alpha);
	default:
		return wxColor(v, p, q, alpha);
	};
}

wxColor wxGISRasterRasterColormapRenderer::CMYKtoRGB( const short &c, const short &m, const short &y, const short &k )
{
    short nK = 255 - k;
    short nR = ( (255 - c) * nK ) / 255;
    short nG = ( (255 - m) * nK ) / 255;
    short nB = ( (255 - y) * nK ) / 255;

	return wxColor(nR, nG, nB);
}


//-----------------------------------
// wxGISRasterGreyScaleRenderer
//-----------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISRasterGreyScaleRenderer, wxGISRasterRenderer)

wxGISRasterGreyScaleRenderer::wxGISRasterGreyScaleRenderer(wxGISLayer* const pwxGISLayer) : wxGISRasterRenderer(pwxGISLayer)
{
	m_nBand = 1;
    OnFillStats();
}

wxGISRasterGreyScaleRenderer::~wxGISRasterGreyScaleRenderer(void)
{
}

void wxGISRasterGreyScaleRenderer::OnFillStats(void)
{
    wxCHECK_RET(m_pwxGISRasterDataset, wxT("Raster dataset pointer is NULL"));

	GDALDataset* poGDALDataset = m_pwxGISRasterDataset->GetMainRaster();
	if(!poGDALDataset)
		poGDALDataset = m_pwxGISRasterDataset->GetRaster();
	if(!poGDALDataset)
		return;

	//set min/max values
	//set nodata color for each band
	if(m_pwxGISRasterDataset->HasStatistics())
	{
        double dfMin, dfMax, dfMean, dfStdDev;
		GDALRasterBand* pBand = poGDALDataset->GetRasterBand(m_nBand);

        if(pBand->GetStatistics(FALSE, FALSE, &dfMin, &dfMax, &dfMean, &dfStdDev) == CE_None)
        {
            m_oStretch.SetStats(dfMin, dfMax, dfMean, dfStdDev);
        }

        if(m_pwxGISRasterDataset->HasNoData(m_nBand) )
        {
            m_oStretch.SetNoData(m_pwxGISRasterDataset->GetNoData(m_nBand));
        }
	}
}

int *wxGISRasterGreyScaleRenderer::GetBandsCombination(int *pnBandCount)
{
	int *pBands = NULL;
	*pnBandCount = 1;
	pBands = new int[1];
	pBands[0] = m_nBand;
	return pBands;
}

short wxGISRasterGreyScaleRenderer::GetBandCount() const
{
    return 1;
}

void wxGISRasterGreyScaleRenderer::FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA)
{
	if(pSrcValR == NULL)
		return;
	//wxBYTE_ORDER          //      x
	//pOutputData[0] = 0;	//	A	B
	//pOutputData[1] = 0;	//	R	G
	//pOutputData[2] = 255;	//	G	R
	//pOutputData[3] = 255;	//	B	A

	unsigned char RPixVal = m_oStretch.GetValue(pSrcValR);

	//check for nodata
    if( m_oStretch.IsNoData(*pSrcValR) )
	{
		pOutputData[3] = m_oNoDataColor.Alpha();
		pOutputData[2] = m_oNoDataColor.Red();
		pOutputData[1] = m_oNoDataColor.Green();
		pOutputData[0] = m_oNoDataColor.Blue();
	}
    else
    {
	    pOutputData[3] = 255;
	    pOutputData[2] = RPixVal;
	    pOutputData[1] = RPixVal;
	    pOutputData[0] = RPixVal;
    }
}


//-----------------------------------
// wxGISRasterPackedRGBARenderer
//-----------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISRasterPackedRGBARenderer, wxGISRasterGreyScaleRenderer)

wxGISRasterPackedRGBARenderer::wxGISRasterPackedRGBARenderer(wxGISLayer* pwxGISLayer) : wxGISRasterGreyScaleRenderer(pwxGISLayer)
{
	wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
    {
    	wxString sAppName = GetApplication()->GetAppName();

        m_bNodataNewBehaviour = oConfig.ReadBool(enumGISHKCU, sAppName + wxString(wxT("/renderer/raster/nodata_newbehaviour")), true);

	    m_eQuality = (wxGISEnumDrawQuality)oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/renderer/raster/quality")), enumGISQualityBilinear);
    }
    else
    {
        m_bNodataNewBehaviour = true;
	    m_eQuality = enumGISQualityBilinear;
    }

	m_oNoDataColor = wxColor(0,0,0,0);
	m_nBand = 1;
}

wxGISRasterPackedRGBARenderer::~wxGISRasterPackedRGBARenderer(void)
{
}

void wxGISRasterPackedRGBARenderer::FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA)
{
	if(pSrcValR == NULL)
		return;
	//wxBYTE_ORDER          //      x
	//pOutputData[0] = 0;	//	A	B
	//pOutputData[1] = 0;	//	R	G
	//pOutputData[2] = 255;	//	G	R
	//pOutputData[3] = 255;	//	B	A

	double dfR, dfG, dfB, dfA;
    if(pSrcValA == NULL)
    {
		pOutputData[3] = 255;
        dfA = 255;
    }
	else
    {
		pOutputData[3] = m_oStretch.GetValue(pSrcValA);
        dfA = *pSrcValA;
    }

	unsigned char RPixVal = m_oStretch.GetValue(pSrcValR);
	pOutputData[2] = RPixVal;

	if(pSrcValG == NULL || pSrcValB == NULL)
	{
		pOutputData[1] = RPixVal;
		pOutputData[0] = RPixVal;
        dfG = dfB = *pSrcValR;
	}
	else
	{
		pOutputData[1] = m_oStretch.GetValue(pSrcValG);
		pOutputData[0] = m_oStretch.GetValue(pSrcValB);
        dfG = *pSrcValG;
        dfB = *pSrcValB;
	}

	//check for nodata
	if(m_bNodataNewBehaviour)
	{
        if(m_oStretch.IsNoData(dfR) && m_oStretch.IsNoData(dfG) && m_oStretch.IsNoData(dfB))
		{
			pOutputData[3] = m_oNoDataColor.Alpha();
			pOutputData[2] = m_oNoDataColor.Red();
			pOutputData[1] = m_oNoDataColor.Green();
			pOutputData[0] = m_oNoDataColor.Blue();
		}
	}
	else
	{
        if(m_oStretch.IsNoData(dfR) || m_oStretch.IsNoData(dfG) || m_oStretch.IsNoData(dfB))
		{
			pOutputData[3] = m_oNoDataColor.Alpha();
			pOutputData[2] = m_oNoDataColor.Red();
			pOutputData[1] = m_oNoDataColor.Green();
			pOutputData[0] = m_oNoDataColor.Blue();
		}
	}
}

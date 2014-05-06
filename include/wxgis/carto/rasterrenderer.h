/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISRasterRenderer classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013 Bishop
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
#pragma once

#include "wxgis/carto/renderer.h"

#include "wxgis/carto/stretch.h"

/** \enum wxGISEnumRendererType rasterrenderer.h
    \brief A renderer color interpretation type
*/
enum wxGISEnumRasterRendererType
{
	enumGISRasterRenderTypeNone = 0,
	enumGISRasterRenderTypeGreyScale,
	enumGISRasterRenderTypeRGBA,
	enumGISRasterRenderTypeIndexed,
	enumGISRasterRenderTypePackedRGBA
};

/** \typedef _rawpixeldata rasterrenderer.h
    \brief A pixel data structure
*/

typedef struct _rawpixeldata
{
	void* pPixelData;
	int nPixelDataWidth, nPixelDataHeight;
	double dPixelDataWidth, dPixelDataHeight;
	double dPixelDeltaX, dPixelDeltaY;
	int nOutputWidth, nOutputHeight;
	OGREnvelope stWorldBounds;
}RAWPIXELDATA;

class wxGISRasterRenderer;

inline double BiCubicKernel(double x)
{
	if ( x > 2.0 )
		return 0.0;

	double a, b, c, d;
	double xm1 = x - 1.0;
	double xp1 = x + 1.0;
	double xp2 = x + 2.0;

	a = ( xp2 <= 0.0 ) ? 0.0 : xp2 * xp2 * xp2;
	b = ( xp1 <= 0.0 ) ? 0.0 : xp1 * xp1 * xp1;
	c = ( x   <= 0.0 ) ? 0.0 : x * x * x;
	d = ( xm1 <= 0.0 ) ? 0.0 : xm1 * xm1 * xm1;

	return ( 0.16666666666666666667 * ( a - ( 4.0 * b ) + ( 6.0 * c ) - ( 4.0 * d ) ) );
};

static void NearestNeighbourInterpolation(void *pInputData, int nInputXSize, double dInputXSize, double dInputYSize, double dDeltaX, double dDeltaY, GDALDataType eSrcType, unsigned char *pOutputData, int nOutXSize, int nOutYSize, int nBegY, int nEndY, int nBandCount, wxGISRasterRenderer* const pRasterRenderer, ITrackCancel* const pTrackCancel = NULL);

static void BilinearInterpolation(void *pInputData, int nInputXSize, int nInputYSize, double dInputXSize, double dInputYSize, double dDeltaX, double dDeltaY, GDALDataType eSrcType, unsigned char *pOutputData, int nOutXSize, int nOutYSize, int nBegY, int nEndY, int nBandCount, wxGISRasterRenderer* const pRasterRenderer, ITrackCancel* const pTrackCancel = NULL);

static void BicubicInterpolation(void *pInputData, int nInputXSize, int nInputYSize, double dInputXSize, double dInputYSize, double dDeltaX, double dDeltaY, GDALDataType eSrcType, unsigned char *pOutputData, int nOutXSize, int nOutYSize, int nBegY, int nEndY, int nBandCount, wxGISRasterRenderer* const pRasterRenderer, ITrackCancel* const pTrackCancel = NULL);

// static void OnHalfBilinearInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel);
// static void OnHalfQuadBilinearInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel);
// static void OnFourQuadBilinearInterpolation(const unsigned char* pOrigData, unsigned char* pDestData, int nYbeg, int nYend, int nOrigWidth, int nOrigHeight, int nDestWidth, double rWRatio, double rHRatio, double rDeltaX, double rDeltaY, ITrackCancel* pTrackCancel);

/** \class wxRasterDrawThread rasterrenderer.h
    \brief The raster layer draw thread
*/

class wxRasterDrawThread : public wxThread
{
    friend class wxGISRasterRenderer;
public:
	wxRasterDrawThread(RAWPIXELDATA &stPixelData, GDALDataType eSrcType, int nBandCount, unsigned char *pTransformData, wxGISEnumDrawQuality eQuality, int nOutXSize, int nOutYSize, int nBegY, int nEndY, wxGISRasterRenderer *pRasterRenderer, ITrackCancel * const pTrackCancel = NULL);
    virtual void *Entry();
    virtual void OnExit();
private:
    ITrackCancel* const m_pTrackCancel;
	wxGISRasterRenderer *m_pRasterRenderer;
	RAWPIXELDATA &m_stPixelData;
	GDALDataType m_eSrcType;
	unsigned char *m_pTransformData;
	wxGISEnumDrawQuality m_eQuality;
	int m_nOutXSize;
	int m_nOutYSize;
	int m_nBegY;
	int m_nEndY;
	int m_nBandCount;
};

/** \class wxGISRasterRenderer rasterrenderer.h
    \brief The base class for renderers
*/

class wxGISRasterRenderer :
	public wxGISRenderer
{
    DECLARE_ABSTRACT_CLASS(wxGISRasterRenderer)
public:
	wxGISRasterRenderer(wxGISLayer* pwxGISLayer = NULL);
	virtual ~wxGISRasterRenderer(void);
	virtual bool CanRender(wxGISLayer* const pwxGISLayer) const;
	virtual int *GetBandsCombination(int *pnBandCount) = 0;
    virtual bool Draw(wxGISEnumDrawPhase DrawPhase, wxGISDisplay* const pDisplay, ITrackCancel* const pTrackCancel = NULL);
    virtual bool Apply(ITrackCancel* const pTrackCancel = NULL);
    virtual wxGISColorTable GetColorTable(void) const {return m_mColorTable;};
    virtual wxGISEnumRasterRendererType GetRasterRenderType(void) const {return enumGISRasterRenderTypeNone;};
    virtual wxGISEnumRendererType GetType(void) const {return enumGISRenderTypeRaster;};
	virtual void FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA) = 0;
protected:
	virtual bool Draw(RAWPIXELDATA &stPixelData, wxGISEnumDrawPhase DrawPhase, wxGISDisplay* const pDisplay, ITrackCancel * const pTrackCancel = NULL);
    virtual short GetBandCount() const = 0;
protected:
	wxColour m_oNoDataColor;
	//statistics - current display extent, each raster dataset, custom settings
	wxGISRasterDataset* m_pwxGISRasterDataset;
	wxGISEnumDrawQuality m_eQuality;
    wxGISColorTable m_mColorTable;
    unsigned short m_nTileSizeX, m_nTileSizeY; 
};

/** \class wxGISRasterRGBARenderer rasterrenderer.h
    \brief The raster layer renderer for RGB data and Alpha channel
*/

class wxGISRasterRGBARenderer :
	public wxGISRasterRenderer
{
    DECLARE_DYNAMIC_CLASS(wxGISRasterRGBARenderer)
public:
	wxGISRasterRGBARenderer(wxGISLayer* pwxGISLayer = NULL);
	virtual ~wxGISRasterRGBARenderer(void);
//wxGISRasterRenderer
	virtual bool CanRender(wxGISLayer* const pwxGISLayer) const;
	virtual int *GetBandsCombination(int *pnBandCount);
	virtual void FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA);
    virtual wxGISEnumRasterRendererType GetRasterRenderType(void) const {return enumGISRasterRenderTypeRGBA;};
protected:
	virtual void OnFillStats(void);
    virtual short GetBandCount() const;
protected:
	int m_nRedBand, m_nGreenBand, m_nBlueBand, m_nAlphaBand;
	//wxColour m_oBkColorGet, m_oBkColorSet, 
	//statistics - current display extent, each raster dataset, custom settings
    wxGISStretch *m_paStretch[4];
	bool m_bNodataNewBehaviour;
};

/** \class wxGISRasterRasterColormapRenderer rasterrenderer.h
    \brief The raster layer renderer for Palette Index data
*/

class wxGISRasterRasterColormapRenderer :
	public wxGISRasterRenderer
{
    DECLARE_DYNAMIC_CLASS(wxGISRasterRasterColormapRenderer)
public:
	wxGISRasterRasterColormapRenderer(wxGISLayer* pwxGISLayer = NULL);
	virtual ~wxGISRasterRasterColormapRenderer(void);
//wxGISRasterRenderer
	virtual bool CanRender(wxGISLayer* const pwxGISLayer) const;
	virtual int *GetBandsCombination(int *pnBandCount);
	virtual void FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA);
    virtual wxGISEnumRasterRendererType GetRasterRenderType(void) const {return enumGISRasterRenderTypeIndexed;};
    //virtual const wxColor *GetColorByIndex(long nIndex);
protected:
	virtual void OnFillColorTable(void);
    virtual short GetBandCount() const;
	wxColor HSVtoRGB( const short &h, const short &s, const short &v, const short &alpha );
	wxColor CMYKtoRGB( const short &c, const short &m, const short &y, const short &k );
protected:
	int m_nBandNumber;
    bool m_bHasNoData;
    short m_nNoDataIndex;
};

/** \class wxGISRasterGreyScaleRenderer rasterrenderer.h
    \brief The raster layer renderer for grey scale data
*/

class wxGISRasterGreyScaleRenderer :
	public wxGISRasterRenderer
{
    DECLARE_DYNAMIC_CLASS(wxGISRasterGreyScaleRenderer)
public:
	wxGISRasterGreyScaleRenderer(wxGISLayer* pwxGISLayer = NULL);
	virtual ~wxGISRasterGreyScaleRenderer(void);
	virtual int *GetBandsCombination(int *pnBandCount);
	virtual void FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA);
    virtual wxGISEnumRasterRendererType GetRasterRenderType(void) const {return enumGISRasterRenderTypeGreyScale;};
protected:
	virtual void OnFillStats(void);
    virtual short GetBandCount() const;
protected:
	int m_nBand;
    wxGISStretch m_oStretch;
};

/** \class wxGISRasterPackedRGBARenderer rasterrenderer.h
    \brief The raster layer renderer for packed RGB data and Alpha channel
*/

class wxGISRasterPackedRGBARenderer :
	public wxGISRasterGreyScaleRenderer
{
    DECLARE_DYNAMIC_CLASS(wxGISRasterPackedRGBARenderer)
public:
	wxGISRasterPackedRGBARenderer(wxGISLayer* pwxGISLayer = NULL);
	virtual ~wxGISRasterPackedRGBARenderer(void);
//wxGISRasterGreyScaleRenderer
	virtual void FillPixel(unsigned char* pOutputData, const double *pSrcValR, const double *pSrcValG, const double *pSrcValB, const double *pSrcValA);
    virtual wxGISEnumRasterRendererType GetRasterRenderType(void) const{return enumGISRasterRenderTypePackedRGBA;};
protected:
	bool m_bNodataNewBehaviour;
};

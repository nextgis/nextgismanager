/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISDisplay class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2014 Bishop
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
#include "wxgis/display/gisdisplay.h"
#include "wxgis/datasource/vectorop.h"
#include "wxgis/display/displayop.h"

#include <wx/graphics.h>
#include <wx/dcgraph.h>

wxGISDisplay::wxGISDisplay(void)
{
	//default background color
	m_BackGroudnColour = wxGISColor(230, 255, 255, 255);

	//default frame size
	m_oDeviceFrameRect = wxRect(0,0,800,600);
	m_dFrameCenterX = 400;
	m_dFrameCenterY = 300;
	m_dFrameRatio = 1.3333333333333333333333333333333;
    m_nSysCacheCount = 0;
    m_nLastCacheID = 0;

	//create first cached layer
	m_nMax_X = wxSystemSettings::GetMetric(wxSYS_SCREEN_X);
	m_nMax_Y = wxSystemSettings::GetMetric(wxSYS_SCREEN_Y);
	LAYERCACHEDATA layercachedata;
	layercachedata.bIsDerty = true;
	layercachedata.pCairoSurface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, m_nMax_X, m_nMax_Y);
	layercachedata.pCairoContext = cairo_create (layercachedata.pCairoSurface);
	m_saLayerCaches.push_back(layercachedata);
    m_nSysCacheCount++;

    //add flash cache
    layercachedata.bIsDerty = false;
	layercachedata.pCairoSurface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, m_nMax_X, m_nMax_Y);
	layercachedata.pCairoContext = cairo_create (layercachedata.pCairoSurface);
	m_saLayerCaches.push_back(layercachedata);
    m_nSysCacheCount++;

	m_dCacheCenterX = m_nMax_X / 2;
	m_dCacheCenterY = m_nMax_Y / 2;

	m_pMatrix = new cairo_matrix_t;
	m_pDisplayMatrix = new cairo_matrix_t;
	m_pDisplayMatrixNoRotate = new cairo_matrix_t;

	m_surface_tmp = cairo_image_surface_create (CAIRO_FORMAT_RGB24, m_oDeviceFrameRect.GetWidth(), m_oDeviceFrameRect.GetHeight());
	m_cr_tmp = cairo_create (m_surface_tmp);

	Clear();
}

wxGISDisplay::~wxGISDisplay(void)
{
    for(size_t i = 0; i < m_saLayerCaches.size(); ++i)
	{
		cairo_destroy(m_saLayerCaches[i].pCairoContext);
		cairo_surface_destroy(m_saLayerCaches[i].pCairoSurface);
	}
	wxDELETE(m_pMatrix);
	wxDELETE(m_pDisplayMatrix);
	wxDELETE(m_pDisplayMatrixNoRotate);
    cairo_destroy (m_cr_tmp);
    cairo_surface_destroy (m_surface_tmp);
}

void wxGISDisplay::Clear()
{
    wxCriticalSectionLocker locker(m_CritSect);

    wxCHECK_RET(m_saLayerCaches.size() - m_nLastCacheID == m_nSysCacheCount, wxT("Draw caches inconsistent"));

    if (m_saLayerCaches.size() > m_nSysCacheCount)
    {
	    for(size_t i = 1; i <= m_nLastCacheID; ++i)
	    {
		    cairo_destroy(m_saLayerCaches[i].pCairoContext);
		    cairo_surface_destroy(m_saLayerCaches[i].pCairoSurface);
	    }
        m_saLayerCaches.erase(m_saLayerCaches.begin() + 1, m_saLayerCaches.end() - (m_nSysCacheCount - 1));
    }

	//default map bounds
	m_RealBounds.MinX = ENVMIN_X;
	m_RealBounds.MaxX = ENVMAX_X;
	m_RealBounds.MinY = ENVMIN_Y;
	m_RealBounds.MaxY = ENVMAX_Y;
	m_CurrentBounds = m_RealBounds;

	SetEnvelopeRatio(m_CurrentBounds, m_dFrameRatio);
	m_CurrentBoundsRotated = m_CurrentBounds;
	m_dRotatedBoundsCenterX = m_CurrentBoundsRotated.MinX + (m_CurrentBoundsRotated.MaxX - m_CurrentBoundsRotated.MinX) / 2;
	m_dRotatedBoundsCenterY = m_CurrentBoundsRotated.MinY + (m_CurrentBoundsRotated.MaxY - m_CurrentBoundsRotated.MinY) / 2;

	m_CurrentBoundsX8 = m_CurrentBounds;
	IncreaseEnvelope(m_CurrentBoundsX8, 8);

	m_nLastCacheID = 0;
	m_nCurrentLayer = 0;
	m_dAngleRad = 0;
	cairo_matrix_init(m_pMatrix, 1, 0, 0, 1, 0, 0);
	cairo_matrix_init(m_pDisplayMatrix, 1, 0, 0, 1, 0, 0);
	cairo_matrix_init(m_pDisplayMatrixNoRotate, 1, 0, 0, 1, 0, 0);

	m_bZeroCacheSet = false;

    InitTransformMatrix();
	OnEraseBackground();
}

cairo_t* wxGISDisplay::CreateContext(wxDC* dc)
{
    cairo_t *cr(NULL);
#ifdef __WXMSW__
//#if CAIRO_HAS_WIN32_SURFACE
     HDC hdc = (HDC)dc->GetHDC();
     cr = cairo_create(cairo_win32_surface_create( hdc ));
#endif

#ifdef __WXGTK__
     wxGraphicsRenderer * const renderer = wxGraphicsRenderer::GetCairoRenderer();
     wxWindowDC* pwdc =  wxDynamicCast(dc, wxWindowDC);
     wxGraphicsContext * gc = renderer->CreateContext(*pwdc);
     if(gc)
          cr =  (cairo_t*)gc->GetNativeContext();
#endif
     return cr;
}

void wxGISDisplay::OnEraseBackground(void)
{
	wxCriticalSectionLocker locker(m_CritSect);
	cairo_set_source_rgb(m_saLayerCaches[0].pCairoContext, m_BackGroudnColour.GetRed(), m_BackGroudnColour.GetGreen(), m_BackGroudnColour.GetBlue());
	cairo_paint(m_saLayerCaches[0].pCairoContext);
}

void wxGISDisplay::ClearCache(size_t nCacheId)
{
	wxCriticalSectionLocker locker(m_CritSect);
    if (nCacheId == 0)
    {
        return OnEraseBackground();
    }
    else
    {
        cairo_save(m_saLayerCaches[nCacheId].pCairoContext);

        cairo_matrix_t mat = { 1, 0, 0, 1, 0, 0 };
        cairo_set_matrix(m_saLayerCaches[nCacheId].pCairoContext, &mat);


        cairo_set_source_surface(m_saLayerCaches[nCacheId].pCairoContext, m_saLayerCaches[nCacheId - 1].pCairoSurface, 0, 0);
        cairo_set_operator(m_saLayerCaches[nCacheId].pCairoContext, CAIRO_OPERATOR_SOURCE);
        cairo_paint(m_saLayerCaches[nCacheId].pCairoContext);

        cairo_restore(m_saLayerCaches[nCacheId].pCairoContext);
    }
}

void wxGISDisplay::Output(wxDC* pDC)
{
    //TODO: draw all 1 - size caches to tmp_surface
	wxCriticalSectionLocker locker(m_CritSect);

	cairo_set_source_rgb(m_cr_tmp, m_BackGroudnColour.GetRed(), m_BackGroudnColour.GetGreen(), m_BackGroudnColour.GetBlue());
	cairo_paint(m_cr_tmp);

	cairo_set_source_surface (m_cr_tmp, m_saLayerCaches[m_nCurrentLayer].pCairoSurface, -m_dOrigin_X, -m_dOrigin_Y);
	cairo_paint (m_cr_tmp);

    Output(m_surface_tmp, pDC);
}

void wxGISDisplay::Output(cairo_surface_t *pSurface, wxDC* pDC)
{
    cairo_t *cr = CreateContext(pDC);
        
	cairo_set_source_surface (cr, pSurface, 0, 0);
	cairo_paint (cr);

#ifdef __WXMSW__
    cairo_destroy (cr);
#endif
}

bool wxGISDisplay::Output(GDALDataset *pGDALDataset)
{
    wxCriticalSectionLocker locker(m_CritSect);
    cairo_set_source_surface(m_cr_tmp, m_saLayerCaches[m_nCurrentLayer].pCairoSurface, -m_dOrigin_X, -m_dOrigin_Y);
    cairo_paint(m_cr_tmp);

    unsigned char *pData = cairo_image_surface_get_data(m_surface_tmp);
    int nWidth = cairo_image_surface_get_width(m_surface_tmp);
    int nHeight = cairo_image_surface_get_height(m_surface_tmp);

    int anBandMap[4] = {3,2,1,4};

    CPLErr eErr = pGDALDataset->RasterIO(GF_Write, 0, 0, nWidth, nHeight, pData, nWidth, nHeight, GDT_Byte, 4, anBandMap, 4, nWidth * 4, 1);
    return eErr == CE_None;
}

void wxGISDisplay::ZoomingDraw(const wxRect& rc, wxDC* pDC)
{
	wxCriticalSectionLocker locker(m_CritSect);

	//compute scale
	double dScaleX = double(rc.GetWidth()) / m_oDeviceFrameRect.GetWidth();
	double dScaleY = double(rc.GetHeight()) / m_oDeviceFrameRect.GetHeight();
#ifdef PROPORTIONAL_ZOOM
    double dZoom = MIN(dScaleX,dScaleY);
#endif
    double dFrameCenterX = rc.GetWidth() / 2;
    double dFrameCenterY = rc.GetHeight() / 2;

    cairo_surface_t *pSurfaceTmp = cairo_image_surface_create (CAIRO_FORMAT_RGB24, rc.GetWidth(), rc.GetHeight());
	cairo_t * pCrTmp = cairo_create (pSurfaceTmp);

#ifdef PROPORTIONAL_ZOOM
	double dDCXDelta = dFrameCenterX / dZoom;
	double dDCYDelta = dFrameCenterY / dZoom;
#else
	double dDCXDelta = dFrameCenterX / dScaleX;
	double dDCYDelta = dFrameCenterY / dScaleY;
#endif

	double dOrigin_X = m_dCacheCenterX - dDCXDelta;
	double dOrigin_Y = m_dCacheCenterY - dDCYDelta;
        
#ifdef PROPORTIONAL_ZOOM
    cairo_scale(pCrTmp, dZoom, dZoom);
#else
    cairo_scale(pCrTmp, dScaleX, dScaleY);
#endif

    cairo_set_source_rgb(pCrTmp, m_BackGroudnColour.GetRed(), m_BackGroudnColour.GetGreen(), m_BackGroudnColour.GetBlue());
	cairo_paint(pCrTmp);

	cairo_set_source_surface (pCrTmp, m_saLayerCaches[m_nLastCacheID].pCairoSurface, -dOrigin_X, -dOrigin_Y);

    cairo_paint (pCrTmp);

    Output(pSurfaceTmp, pDC);

    cairo_surface_destroy (pSurfaceTmp);
    cairo_destroy (pCrTmp);
}

void wxGISDisplay::WheelingDraw(double dZoom, wxDC* pDC)
{
	wxCriticalSectionLocker locker(m_CritSect);

	if(IsDoubleEquil(dZoom, 1)) // no zoom
	{
		cairo_set_source_surface (m_cr_tmp, m_saLayerCaches[m_nCurrentLayer].pCairoSurface, -m_dOrigin_X, -m_dOrigin_Y);
		cairo_set_operator (m_cr_tmp, CAIRO_OPERATOR_SOURCE);

        cairo_paint (m_cr_tmp);

	}
	else if(dZoom > 1) // zoom in
	{
		double dDCXDelta = m_dFrameCenterX / dZoom;
		double dDCYDelta = m_dFrameCenterY / dZoom;
		double dOrigin_X = m_dCacheCenterX - dDCXDelta;
		double dOrigin_Y = m_dCacheCenterY - dDCYDelta;
        
        cairo_scale(m_cr_tmp, dZoom, dZoom);

        cairo_set_source_rgb(m_cr_tmp, m_BackGroudnColour.GetRed(), m_BackGroudnColour.GetGreen(), m_BackGroudnColour.GetBlue());
	    cairo_paint(m_cr_tmp);

		cairo_set_source_surface (m_cr_tmp, m_saLayerCaches[m_nLastCacheID].pCairoSurface, -dOrigin_X, -dOrigin_Y);

        cairo_paint (m_cr_tmp);
	}
	else //zoom out
	{
		double dDCXDelta = m_dFrameCenterX * dZoom;
		double dDCYDelta = m_dFrameCenterY * dZoom;
		double dOrigin_X = m_dFrameCenterX - dDCXDelta;
		double dOrigin_Y = m_dFrameCenterY - dDCYDelta;

        cairo_set_source_rgb(m_cr_tmp, m_BackGroudnColour.GetRed(), m_BackGroudnColour.GetGreen(), m_BackGroudnColour.GetBlue());
	    cairo_paint(m_cr_tmp);

        cairo_translate (m_cr_tmp, dOrigin_X, dOrigin_Y);
		cairo_scale(m_cr_tmp, dZoom, dZoom);

		cairo_set_source_surface (m_cr_tmp, m_saLayerCaches[m_nLastCacheID].pCairoSurface, -m_dOrigin_X, -m_dOrigin_Y);

        cairo_paint (m_cr_tmp);
	}

    Output(m_surface_tmp, pDC);

    cairo_matrix_t mat = {1, 0, 0, 1, 0, 0};
	cairo_set_matrix (m_cr_tmp, &mat);
}

void wxGISDisplay::PanningDraw(wxCoord x, wxCoord y, wxDC* pDC)
{
	wxCriticalSectionLocker locker(m_CritSect);

	cairo_set_source_rgb(m_cr_tmp, m_BackGroudnColour.GetRed(), m_BackGroudnColour.GetGreen(), m_BackGroudnColour.GetBlue());
	cairo_paint(m_cr_tmp);

	double dNewX = m_dOrigin_X + double(x);
	double dNewY = m_dOrigin_Y + double(y);
	cairo_set_source_surface (m_cr_tmp, m_saLayerCaches[m_nLastCacheID].pCairoSurface, -dNewX, -dNewY);

	cairo_paint (m_cr_tmp);

	//cairo_surface_t *surface;
    cairo_t *cr;

	cr = CreateContext(pDC);
	//surface = cairo_get_target(cr);

	cairo_set_source_surface (cr, m_surface_tmp, 0, 0);
	cairo_paint (cr);

#ifdef __WXMSW__
    //cairo_surface_destroy (surface);
    cairo_destroy (cr);
#endif
}

void wxGISDisplay::RotatingDraw(double dAngle, wxDC* pDC)
{
	wxCriticalSectionLocker locker(m_CritSect);

	cairo_set_source_rgb(m_cr_tmp, m_BackGroudnColour.GetRed(), m_BackGroudnColour.GetGreen(), m_BackGroudnColour.GetBlue());
	cairo_paint(m_cr_tmp);

	int w = cairo_image_surface_get_width (cairo_get_target(m_cr_tmp));
	int h = cairo_image_surface_get_height (cairo_get_target(m_cr_tmp));
	//cairo_translate (m_cr_tmp, 0.5 * m_oDeviceFrameRect.GetWidth(), 0.5 * m_oDeviceFrameRect.GetHeight());

	//double dWorldCenterX = m_CurrentBounds.MinX + double(m_CurrentBounds.MaxX - m_CurrentBounds.MinX) / 2;
	//double dWorldCenterY = m_CurrentBounds.MinY + double(m_CurrentBounds.MaxY - m_CurrentBounds.MinY) / 2;
	//World2DC(&dWorldCenterX, &dWorldCenterY);

	//cairo_translate (m_cr_tmp, dWorldCenterX, dWorldCenterY);
	cairo_translate (m_cr_tmp, m_dFrameCenterX, m_dFrameCenterY);
	cairo_rotate (m_cr_tmp, dAngle);
	//cairo_translate (m_cr_tmp, -0.5 * m_oDeviceFrameRect.GetWidth(), -0.5 * m_oDeviceFrameRect.GetHeight());
	//cairo_translate (m_cr_tmp, -dWorldCenterX, -dWorldCenterY);
	cairo_translate (m_cr_tmp, -m_dFrameCenterX, -m_dFrameCenterY);
	cairo_set_source_surface (m_cr_tmp, m_saLayerCaches[m_nLastCacheID].pCairoSurface, -m_dOrigin_X, -m_dOrigin_Y);

	cairo_paint (m_cr_tmp);

	//cairo_surface_t *surface;
    cairo_t *cr;

	cr = CreateContext(pDC);
	//surface = cairo_get_target(cr);

	cairo_set_source_surface (cr, m_surface_tmp, 0, 0);
	cairo_paint (cr);

#ifdef __WXMSW__
    //cairo_surface_destroy (surface);
    cairo_destroy (cr);
#endif
	cairo_matrix_t mat = {1, 0, 0, 1, 0, 0};
	cairo_set_matrix (m_cr_tmp, &mat);
}

size_t wxGISDisplay::AddCache(void)
{
	wxCriticalSectionLocker locker(m_CritSect);
	if(m_bZeroCacheSet)
	{
		LAYERCACHEDATA layercachedata;
		layercachedata.bIsDerty = true;
		layercachedata.pCairoSurface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, m_nMax_X, m_nMax_Y);
		layercachedata.pCairoContext = cairo_create (layercachedata.pCairoSurface);
        m_nLastCacheID++;
        m_saLayerCaches.insert(m_saLayerCaches.begin() + m_nLastCacheID, layercachedata);
	}
	else
		m_bZeroCacheSet = true;
	return m_nLastCacheID;
}

size_t wxGISDisplay::GetLastCacheID(void) const
{
	return m_nLastCacheID;
}

size_t wxGISDisplay::GetFlashCacheID(void) const
{
	return m_saLayerCaches.size() - 1;
}

bool wxGISDisplay::IsCacheDerty(size_t nCacheId) const
{
    return m_saLayerCaches[nCacheId].bIsDerty;
}

void wxGISDisplay::SetCacheDerty(size_t nCacheId, bool bIsDerty)
{
	wxCriticalSectionLocker locker(m_CritSect);
    m_saLayerCaches[nCacheId].bIsDerty = bIsDerty;
}

void wxGISDisplay::SetDrawCache(size_t nCacheId, bool bNoDerty)
{
	wxCriticalSectionLocker locker(m_CritSect);
    if (bNoDerty || nCacheId == 0)
        m_nCurrentLayer = nCacheId;
    else
    {
		//merge previous cache
        if (m_nCurrentLayer > nCacheId)
        {
            m_nCurrentLayer = nCacheId - 1;
        }
		//TODO: clip by frame size cairo_clip()?
        //TODO: rewrite to parallel cache drawing
        for (int i = m_nCurrentLayer; i < nCacheId; ++i)
        {
		    cairo_save(m_saLayerCaches[i + 1].pCairoContext);

		    cairo_matrix_t mat = {1, 0, 0, 1, 0, 0};
            cairo_set_matrix(m_saLayerCaches[i + 1].pCairoContext, &mat);


            cairo_set_source_surface(m_saLayerCaches[i + 1].pCairoContext, m_saLayerCaches[i].pCairoSurface, 0, 0);
            cairo_set_operator(m_saLayerCaches[i + 1].pCairoContext, CAIRO_OPERATOR_SOURCE);
            cairo_paint(m_saLayerCaches[i + 1].pCairoContext);

            cairo_restore(m_saLayerCaches[i + 1].pCairoContext);
        }

        m_nCurrentLayer = nCacheId;
    }
}

void wxGISDisplay::SetAllCachesDerty(bool bIsDerty)
{
	wxCriticalSectionLocker locker(m_CritSect);
	for(size_t i = 0; i <= m_nLastCacheID; ++i)
		m_saLayerCaches[i].bIsDerty = bIsDerty;
}

void wxGISDisplay::SetUpperCachesDerty(size_t nFromCacheNo, bool bIsDerty)
{
	wxCriticalSectionLocker locker(m_CritSect);
    if(nFromCacheNo > m_nLastCacheID)
        return;
	for(size_t i = nFromCacheNo; i <= m_nLastCacheID; ++i)
        m_saLayerCaches[i].bIsDerty = bIsDerty;
}

bool wxGISDisplay::IsDerty(void) const
{
	for(size_t i = 0; i <= m_nLastCacheID; ++i)
		if(m_saLayerCaches[i].bIsDerty)
			return true;
	return false;
}

void wxGISDisplay::SetDeviceFrame(wxRect &rc)
{
	m_oDeviceFrameRect = rc;
	if(m_oDeviceFrameRect.GetWidth() % 2 != 0)
		m_oDeviceFrameRect.SetWidth(m_oDeviceFrameRect.GetWidth() + 1);
	if(m_oDeviceFrameRect.GetHeight() % 2 != 0)
		m_oDeviceFrameRect.SetHeight(m_oDeviceFrameRect.GetHeight() + 1);

	m_dFrameRatio = double(m_oDeviceFrameRect.GetWidth()) / m_oDeviceFrameRect.GetHeight();

	m_CurrentBounds = m_RealBounds;

	SetEnvelopeRatio(m_CurrentBounds, m_dFrameRatio);
	m_dRotatedBoundsCenterX = m_CurrentBounds.MinX + (m_CurrentBounds.MaxX - m_CurrentBounds.MinX) / 2;
	m_dRotatedBoundsCenterY = m_CurrentBounds.MinY + (m_CurrentBounds.MaxY - m_CurrentBounds.MinY) / 2;
	m_CurrentBoundsRotated = m_CurrentBounds;
	if(!IsDoubleEquil(m_dAngleRad, 0.0))
	{
		RotateEnvelope(m_CurrentBoundsRotated, m_dAngleRad, m_dRotatedBoundsCenterX, m_dRotatedBoundsCenterY);
		SetEnvelopeRatio(m_CurrentBoundsRotated, m_dFrameRatio);//test
	}
	m_CurrentBoundsX8 = m_CurrentBoundsRotated;
	IncreaseEnvelope(m_CurrentBoundsX8, 8);

    cairo_destroy (m_cr_tmp);
    cairo_surface_destroy (m_surface_tmp);

	m_surface_tmp = cairo_image_surface_create (CAIRO_FORMAT_RGB24, m_oDeviceFrameRect.GetWidth(), m_oDeviceFrameRect.GetHeight());
	m_cr_tmp = cairo_create (m_surface_tmp);


	//compute current transform matrix
	InitTransformMatrix();
}

wxRect wxGISDisplay::GetDeviceFrame(void) const
{
	return m_oDeviceFrameRect;
}

void wxGISDisplay::SetBounds(const OGREnvelope& Bounds)
{
	//update bounds to frame ratio
	m_RealBounds = Bounds;
	m_CurrentBounds = m_RealBounds;

	SetEnvelopeRatio(m_CurrentBounds, m_dFrameRatio);
	m_dRotatedBoundsCenterX = m_CurrentBounds.MinX + (m_CurrentBounds.MaxX - m_CurrentBounds.MinX) / 2;
	m_dRotatedBoundsCenterY = m_CurrentBounds.MinY + (m_CurrentBounds.MaxY - m_CurrentBounds.MinY) / 2;
	m_CurrentBoundsRotated = m_CurrentBounds;
	if(!IsDoubleEquil(m_dAngleRad, 0.0))
	{
		RotateEnvelope(m_CurrentBoundsRotated, m_dAngleRad, m_dRotatedBoundsCenterX, m_dRotatedBoundsCenterY);
		SetEnvelopeRatio(m_CurrentBoundsRotated, m_dFrameRatio);//test
	}
	m_CurrentBoundsX8 = m_CurrentBoundsRotated;
	IncreaseEnvelope(m_CurrentBoundsX8, 8);

	SetAllCachesDerty(true);
	//compute current transform matrix
	InitTransformMatrix();
}

OGREnvelope wxGISDisplay::GetBounds(bool bRotated) const
{
	if(bRotated)
		return m_CurrentBoundsRotated;
	else
		return m_CurrentBounds;
}

void wxGISDisplay::InitTransformMatrix(void)
{
	m_dFrameCenterX = m_oDeviceFrameRect.GetWidth() / 2;
	m_dFrameCenterY = m_oDeviceFrameRect.GetHeight() / 2;
	double dWorldCenterX = (m_CurrentBounds.MaxX - m_CurrentBounds.MinX) / 2;
	double dWorldCenterY = (m_CurrentBounds.MaxY - m_CurrentBounds.MinY) / 2;

	//origin (UL corner)
	m_dOrigin_X = m_dCacheCenterX - m_dFrameCenterX;
	m_dOrigin_Y = m_dCacheCenterY - m_dFrameCenterY;
	//get scale
	double dScaleX = fabs(m_dFrameCenterX / dWorldCenterX);
	double dScaleY = fabs(m_dFrameCenterY / dWorldCenterY);
	m_dScale = std::min(dScaleX, dScaleY);

	double dWorldDeltaX = dWorldCenterX + m_CurrentBounds.MinX;
	double dWorldDeltaY = dWorldCenterY + m_CurrentBounds.MinY;

	double dWorldDeltaXSt = m_dScale * dWorldDeltaX;// + m_dAngleRad * dWorldDeltaY;
	double dWorldDeltaYSt = m_dScale * dWorldDeltaY;//m_dAngleRad * dWorldDeltaX +

	//double dCenterX = m_dCacheCenterX - dWorldDeltaXSt;//(dWorldCenterX + m_CurrentBounds.MinX) * dScale;//
	//double dCenterY = m_dCacheCenterY + dWorldDeltaYSt;//(dWorldCenterY + m_CurrentBounds.MinY) * dScale;//
	m_dFrameXShift = m_dFrameCenterX - dWorldDeltaXSt;//(dWorldCenterX + m_CurrentBounds.MinX) * dScale;//
	m_dFrameYShift = m_dFrameCenterY + dWorldDeltaYSt;//(dWorldCenterY + m_CurrentBounds.MinY) * dScale;//

//	cairo_matrix_init (m_pMatrix, 1, 0, 0, 1, m_dCacheCenterX, m_dCacheCenterY);
	//cairo_matrix_init (m_pMatrix, dScale, 0.0, 0.0, -dScale, dCenterX, dCenterY);
	cairo_matrix_init_translate (m_pMatrix, m_dCacheCenterX, m_dCacheCenterY);
	//rotate
	//cairo_matrix_rotate(m_pMatrix, 45.0 * M_PI / 180.0);
	if(!IsDoubleEquil(m_dAngleRad, 0.0))
	//{
		//cairo_matrix_translate(m_pMatrix, dWorldDeltaXSt, dWorldDeltaYSt);
		cairo_matrix_rotate(m_pMatrix, m_dAngleRad);
		//cairo_matrix_translate(m_pMatrix, -dWorldDeltaXSt, dWorldDeltaYSt);
	//}
	//else
	cairo_matrix_translate(m_pMatrix, -dWorldDeltaXSt, dWorldDeltaYSt);

	cairo_matrix_scale(m_pMatrix, m_dScale, -m_dScale);

	//init matrix for Wld2DC & DC2Wld
	cairo_matrix_init_translate (m_pDisplayMatrix, m_dFrameCenterX, m_dFrameCenterY);
	if(!IsDoubleEquil(m_dAngleRad, 0.0))
		cairo_matrix_rotate(m_pDisplayMatrix, m_dAngleRad);
	cairo_matrix_translate(m_pDisplayMatrix, -dWorldDeltaXSt, dWorldDeltaYSt);
	cairo_matrix_scale(m_pDisplayMatrix, m_dScale, -m_dScale);

	//init matrix for TransformRect
	cairo_matrix_init_translate (m_pDisplayMatrixNoRotate, m_dFrameCenterX, m_dFrameCenterY);
	cairo_matrix_translate(m_pDisplayMatrixNoRotate, -dWorldDeltaXSt, dWorldDeltaYSt);
	cairo_matrix_scale(m_pDisplayMatrixNoRotate, m_dScale, -m_dScale);

	//set matrix to all caches
	for(size_t i = 0; i < m_saLayerCaches.size(); ++i)
		cairo_set_matrix (m_saLayerCaches[i].pCairoContext, m_pMatrix);
}

void wxGISDisplay::DC2World(double* pdX, double* pdY)
{
    //set center of real window not cache
	cairo_matrix_t InvertMatrix = {m_pDisplayMatrix->xx, m_pDisplayMatrix->yx, m_pDisplayMatrix->xy, m_pDisplayMatrix->yy, m_pDisplayMatrix->x0, m_pDisplayMatrix->y0};
	cairo_matrix_invert(&InvertMatrix);
	cairo_matrix_transform_point(&InvertMatrix, pdX, pdY);
}

void wxGISDisplay::World2DC(double* pdX, double* pdY)
{
    //set center of real window not cache
	cairo_matrix_t Matrix = {m_pDisplayMatrix->xx, m_pDisplayMatrix->yx, m_pDisplayMatrix->xy, m_pDisplayMatrix->yy, m_pDisplayMatrix->x0, m_pDisplayMatrix->y0};
	cairo_matrix_transform_point(&Matrix, pdX, pdY);
}

void wxGISDisplay::DC2WorldDist(double* pdX, double* pdY, bool bRotated)
{
	cairo_matrix_t InvertMatrix;
	if(bRotated)//set center of real window not cache
		InvertMatrix = *m_pDisplayMatrix;
	else
		InvertMatrix = *m_pDisplayMatrixNoRotate;
	cairo_matrix_invert(&InvertMatrix);
	cairo_matrix_transform_distance(&InvertMatrix, pdX, pdY);
}

void wxGISDisplay::World2DCDist(double* pdX, double* pdY, bool bRotated)
{
	cairo_matrix_t Matrix;
	if(bRotated)
		Matrix = *m_pDisplayMatrix;
	else
		Matrix = *m_pDisplayMatrixNoRotate;
	cairo_matrix_transform_distance(&Matrix, pdX, pdY);
}

void wxGISDisplay::TestDraw(void)
{
/*	srand ( time(NULL) );
	SetDrawCache(0);

	m_stLineColour.dRed = 0.0;
	m_stLineColour.dGreen = 0.0;
	m_stLineColour.dBlue = 0.0;
	m_stLineColour.dAlpha = 1.0;
	m_stPointColour = m_stLineColour;

	int random_number1 = (rand() % 50);
	int random_number2 = (rand() % 50);
	int random_number3 = (rand() % 50);
	m_stFillColour.dRed = double(205 + random_number1) / 255;
	m_stFillColour.dGreen = double(205 + random_number2) / 255;
	m_stFillColour.dBlue = double(205 + random_number3) / 255;
	m_stFillColour.dAlpha = 1.0;

	SetColor(m_stLineColour);
	cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -178.0, 79.0);
	cairo_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 178.0, -79.0);
	cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 178.0, 79.0);
	cairo_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -178.0, -79.0);
	cairo_set_line_width (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 0.2);
	cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);

	cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -6, 6);
	cairo_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 6, 6);
//	cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 6, 6);
	cairo_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 6, -6);
//	cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 6, -6);
	cairo_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -6, -6);
//	cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -6, -6);
	cairo_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -6, 6);

	//cairo_move_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 64.0, 25.6);
	//cairo_rel_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, 51.2, 51.2);
	//cairo_rel_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -51.2, 51.2);
	//cairo_rel_line_to (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -51.2, -51.2);
	//cairo_close_path (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
	//cairo_rectangle (m_saLayerCaches[m_nCurrentLayer].pCairoContext, -6, 6, 12, -12);
	SetColor(m_stFillColour);
	cairo_fill_preserve (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
	SetColor(m_stLineColour);
	cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
    */
}

size_t wxGISDisplay::GetDrawCache(void) const
{
	return m_nCurrentLayer;
}

void wxGISDisplay::SetColor(double dRed, double dGreen, double dBlue, double dAlpha)
{
    wxCriticalSectionLocker locker(m_CritSect);
    cairo_set_source_rgba(m_saLayerCaches[m_nCurrentLayer].pCairoContext, dRed, dGreen, dBlue, dAlpha);
}

void wxGISDisplay::SetColor(const wxGISColor &Color)
{
	SetColor(Color.GetRed(), Color.GetGreen(), Color.GetBlue(), Color.GetAlpha());
}

void wxGISDisplay::Stroke()
{
    wxCriticalSectionLocker locker(m_CritSect);
    cairo_stroke(m_saLayerCaches[m_nCurrentLayer].pCairoContext);
}

void wxGISDisplay::FillPreserve()
{
    wxCriticalSectionLocker locker(m_CritSect);
    cairo_fill_preserve(m_saLayerCaches[m_nCurrentLayer].pCairoContext);
}

bool wxGISDisplay::DrawCircle(double dX, double dY, double dOffsetX, double dOffsetY, double dfRadius, double angle1, double angle2)
{
    wxCriticalSectionLocker locker(m_CritSect);
    cairo_arc(m_saLayerCaches[m_nCurrentLayer].pCairoContext, dX + dOffsetX, dY + dOffsetY, dfRadius, angle1, angle2);
	return true;
}

bool wxGISDisplay::DrawEllipse(double dX, double dY, double dOffsetX, double dOffsetY, double dfWidth, double dfHeight)
{
    wxCriticalSectionLocker locker(m_CritSect);
    cairo_save(m_saLayerCaches[m_nCurrentLayer].pCairoContext);
    cairo_translate(m_saLayerCaches[m_nCurrentLayer].pCairoContext, dX + dOffsetX + dfWidth / 2., dY + dOffsetY + dfHeight / 2.);
    cairo_scale(m_saLayerCaches[m_nCurrentLayer].pCairoContext, dfWidth / 2., dfHeight / 2.);
    cairo_arc(m_saLayerCaches[m_nCurrentLayer].pCairoContext, 0., 0., 1., 0., 2 * M_PI);
    cairo_restore(m_saLayerCaches[m_nCurrentLayer].pCairoContext);

	return true;
}

bool wxGISDisplay::DrawPointFast(double dX, double dY, double dOffsetX, double dOffsetY)
{
    wxCriticalSectionLocker locker(m_CritSect);
    cairo_move_to(m_saLayerCaches[m_nCurrentLayer].pCairoContext, dX + dOffsetX, dY + dOffsetY);
	cairo_close_path (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
	return true;
}

//TODO: move to cache class
void wxGISDisplay::SetLineCap(cairo_line_cap_t line_cap)
{
    wxCriticalSectionLocker locker(m_CritSect);
    cairo_set_line_cap(m_saLayerCaches[m_nCurrentLayer].pCairoContext, line_cap);
}

//TODO: move to cache class
void wxGISDisplay::SetLineJoin(cairo_line_join_t line_join)
{
    wxCriticalSectionLocker locker(m_CritSect);
    cairo_set_line_join(m_saLayerCaches[m_nCurrentLayer].pCairoContext, line_join);
}

//TODO: move to cache class
void wxGISDisplay::SetLineWidth(double dWidth)
{
    wxCriticalSectionLocker locker(m_CritSect);
    cairo_set_line_width(m_saLayerCaches[m_nCurrentLayer].pCairoContext, GetScaledWidth(dWidth));
}

//TODO: move to cache class
void wxGISDisplay::SetMiterLimit(double dMiterLimit)
{
    wxCriticalSectionLocker locker(m_CritSect);
    cairo_set_miter_limit(m_saLayerCaches[m_nCurrentLayer].pCairoContext, dMiterLimit);
}

//TODO: move to cache class
void wxGISDisplay::SetFillRule(cairo_fill_rule_t fill_rule)
{
    wxCriticalSectionLocker locker(m_CritSect);
    cairo_set_fill_rule(m_saLayerCaches[m_nCurrentLayer].pCairoContext, fill_rule);
}

bool wxGISDisplay::CanDraw(OGREnvelope &Env)
{
	if(!IsDoubleEquil(m_dAngleRad, 0.0))
	{
		RotateEnvelope(Env, -m_dAngleRad, m_dRotatedBoundsCenterX, m_dRotatedBoundsCenterY);
	}

	return Env.Contains(m_CurrentBoundsRotated) || m_CurrentBoundsRotated.Contains(Env) || Env.Intersects(m_CurrentBoundsRotated);
}

bool wxGISDisplay::DrawLine(OGRRawPoint* pOGRRawPoints, int nPointCount, bool bOwn, double dOffsetX, double dOffsetY, bool bIsRing)
{
    if (NULL == pOGRRawPoints)
    {
        if(bOwn)
            wxDELETEA(pOGRRawPoints);
        return false;
    }

	ClipGeometryByEnvelope(pOGRRawPoints, &nPointCount, m_CurrentBoundsX8, !bIsRing);

	cairo_move_to(m_saLayerCaches[m_nCurrentLayer].pCairoContext, pOGRRawPoints[0].x + dOffsetX, pOGRRawPoints[0].y + dOffsetY);

	for(int i = 1; i < nPointCount; ++i)
	{
        wxCriticalSectionLocker locker(m_CritSect);
        cairo_line_to(m_saLayerCaches[m_nCurrentLayer].pCairoContext, pOGRRawPoints[i].x + dOffsetX, pOGRRawPoints[i].y + dOffsetY);
	}

    if(bOwn)
        wxDELETEA(pOGRRawPoints);
	return true;
}

bool wxGISDisplay::CheckDrawAsPoint(const OGREnvelope &Envelope, double dfLineWidth, double dOffsetX, double dOffsetY, bool bCheckEnvelope)
{
	OGREnvelope TestEnv;
	TestEnv = Envelope;
	if(!IsDoubleEquil(m_dAngleRad, 0.0))
	{
		RotateEnvelope(TestEnv, -m_dAngleRad, m_dRotatedBoundsCenterX, m_dRotatedBoundsCenterY);
	}

	if(bCheckEnvelope && !m_CurrentBoundsRotated.Intersects(TestEnv))
		return true;

	double EnvWidth = Envelope.MaxX - Envelope.MinX;
	double EnvHeight = Envelope.MaxY - Envelope.MinY;

	World2DCDist(&EnvWidth, &EnvHeight);
	EnvWidth = fabs(EnvWidth);
	EnvHeight = fabs(EnvHeight);
	if(	EnvWidth <= MINPOLYDRAWAREA && EnvHeight <= MINPOLYDRAWAREA )
	{
		if(	EnvWidth >= MINPOLYAREA && EnvHeight >= MINPOLYAREA )
		{
            wxCriticalSectionLocker locker(m_CritSect);

			cairo_move_to(m_saLayerCaches[m_nCurrentLayer].pCairoContext, Envelope.MinX + dOffsetX, Envelope.MinY + dOffsetY);
			cairo_line_to(m_saLayerCaches[m_nCurrentLayer].pCairoContext, Envelope.MaxX + dOffsetX, Envelope.MaxY + dOffsetY);
		}
        else
        {
            SetLineCap(CAIRO_LINE_CAP_ROUND);
			SetLineWidth( dfLineWidth + dfLineWidth );
			DrawPointFast(Envelope.MinX, Envelope.MinY, dOffsetX, dOffsetY);
        }
		return true;
	}
	return false;
}

void wxGISDisplay::DrawRaster(cairo_surface_t *surface, const OGREnvelope& Envelope, bool bDrawEnvelope)
{
	wxCriticalSectionLocker locker(m_CritSect);

    cairo_pattern_t *pattern = cairo_pattern_create_for_surface (surface);
	cairo_matrix_t   matrix;
	cairo_matrix_init_scale (&matrix, m_dScale, -m_dScale);
	cairo_matrix_translate(&matrix, -Envelope.MinX, -Envelope.MaxY);
	cairo_pattern_set_matrix (pattern, &matrix);
	cairo_set_source (m_saLayerCaches[m_nCurrentLayer].pCairoContext, pattern);
	cairo_paint (m_saLayerCaches[m_nCurrentLayer].pCairoContext);

	if(bDrawEnvelope)
	{
        //TODO:
		//SetLineWidth( m_dLineWidth );
        //SetColor(m_FillColour);
		
        cairo_move_to(m_saLayerCaches[m_nCurrentLayer].pCairoContext, Envelope.MinX, Envelope.MinY);
		cairo_line_to(m_saLayerCaches[m_nCurrentLayer].pCairoContext, Envelope.MaxX, Envelope.MinY);
		cairo_line_to(m_saLayerCaches[m_nCurrentLayer].pCairoContext, Envelope.MaxX, Envelope.MaxY);
		cairo_line_to(m_saLayerCaches[m_nCurrentLayer].pCairoContext, Envelope.MinX, Envelope.MaxY);
		cairo_line_to(m_saLayerCaches[m_nCurrentLayer].pCairoContext, Envelope.MinX, Envelope.MinY);
		cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
	}

	cairo_pattern_destroy (pattern);
}

OGREnvelope wxGISDisplay::TransformRect(wxRect &rect)
{
	OGREnvelope out;
	double dX1, dX2, dY2, dY1;
	double dWHalf = double(rect.width) / 2;
	double dHHalf = double(rect.height) / 2;
	double dXCenter = rect.x + dWHalf, dYCenter = rect.y + dHHalf;
	DC2World(&dXCenter, &dYCenter);

	cairo_matrix_t InvertMatrix = {m_pDisplayMatrixNoRotate->xx, m_pDisplayMatrixNoRotate->yx, m_pDisplayMatrixNoRotate->xy, m_pDisplayMatrixNoRotate->yy, m_pDisplayMatrixNoRotate->x0, m_pDisplayMatrixNoRotate->y0};
	cairo_matrix_invert(&InvertMatrix);

	cairo_matrix_transform_distance(&InvertMatrix, &dWHalf, &dHHalf);

	dX1 = dXCenter - dWHalf;
	dX2 = dXCenter + dWHalf;
	dY1 = dYCenter - dHHalf;
	dY2 = dYCenter + dHHalf;

    out.MinX = std::min(dX1, dX2);
	out.MinY = std::min(dY1, dY2);
	out.MaxX = std::max(dX1, dX2);
	out.MaxY = std::max(dY1, dY2);
	return out;
}

void wxGISDisplay::SetRotate(double dAngleRad)
{
	m_dAngleRad = dAngleRad;
    //for rotate panning & zooming
	m_CurrentBoundsRotated = m_CurrentBounds;
	m_dRotatedBoundsCenterX = m_CurrentBoundsRotated.MinX + (m_CurrentBoundsRotated.MaxX - m_CurrentBoundsRotated.MinX) / 2;
	m_dRotatedBoundsCenterY = m_CurrentBoundsRotated.MinY + (m_CurrentBoundsRotated.MaxY - m_CurrentBoundsRotated.MinY) / 2;
	if(!IsDoubleEquil(m_dAngleRad, 0.0))
	{
		RotateEnvelope(m_CurrentBoundsRotated, m_dAngleRad, m_dRotatedBoundsCenterX, m_dRotatedBoundsCenterY);
		SetEnvelopeRatio(m_CurrentBoundsRotated, m_dFrameRatio);//test
	}
	m_CurrentBoundsX8 = m_CurrentBoundsRotated;
	IncreaseEnvelope(m_CurrentBoundsX8, 8);

	//compute current transform matrix
	InitTransformMatrix();

	SetAllCachesDerty(true);
}

wxCriticalSection &wxGISDisplay::GetLock()
{
    return m_CritSect;
}


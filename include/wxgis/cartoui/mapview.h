/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISMapView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011-2013 Bishop
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
#pragma once

#include "wxgis/cartoui/cartoui.h"
#include "wxgis/display/symbol.h"
#include "wxgis/carto/map.h"
#include "wxgis/cartoui/mxeventui.h"
#include "wxgis/carto/mxevent.h"

/** \class wxGISMapView mapview.h
    \brief The MapView class draw layers to wxWindow.
*/

class WXDLLIMPEXP_GIS_CTU wxGISMapView :
	public wxWindow,
	public wxGISExtentStack,
	public wxGISConnectionPointContainer,
    public wxThreadHelper
{
    DECLARE_CLASS(wxGISMapView)
	enum
    {
        TIMER_ID = 1015
    };
public:
    wxGISMapView(void);
	wxGISMapView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_FULL_REPAINT_ON_RESIZE);//wxSTATIC_BORDER|
	virtual ~wxGISMapView(void);
    virtual bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_FULL_REPAINT_ON_RESIZE, const wxString& name = wxT("GISMapView"));//wxSTATIC_BORDER|
	virtual void SetTrackCancel(ITrackCancel* pTrackCancel);
	virtual wxGISDisplay* GetDisplay(void){return m_pGISDisplay;};
	//wxGISExtentStack
	virtual bool AddLayer(wxGISLayer* pLayer);
	virtual void Clear(void);
	virtual void SetSpatialReference(const wxGISSpatialReference &SpatialReference);
	virtual void SetExtent(const OGREnvelope& Env);
	virtual void SetFullExtent(void);
	virtual OGREnvelope GetFullExtent(void);
    //
	virtual double GetScaleRatio(OGREnvelope& Bounds, wxDC& dc);
	virtual void PanStart(wxPoint MouseLocation);
	virtual void PanMoveTo(wxPoint MouseLocation);
	virtual void PanStop(wxPoint MouseLocation);
	virtual void RotateStart(wxPoint MouseLocation);
    virtual bool CanRotate(void);
	virtual void RotateBy(wxPoint MouseLocation);
	virtual void RotateStop(wxPoint MouseLocation);
	virtual void SetRotate(double dAngleRad);
	virtual double GetCurrentRotate(void);
    //virtual void FlashGeometry(const wxGISGeometryArray& Geoms);
    virtual void AddFlashGeometry(const wxGISGeometry& Geometry, wxGISSymbol* const pSymbol, unsigned char nPhase = 1);
    virtual void StartFlashing(wxGISEnumFlashStyle eFlashStyle = enumGISMapFlashNewColor);

    /** \struct _flash_geometry mapview.h
        \brief The geometry needed to be flashed on map.

        stFillColour and stLineColour set drawing style and phase used in multistep flashing
    */

    typedef struct _flash_geometry
    {
        wxGISGeometry Geometry;
        unsigned char nPhase;
        wxGISSymbol* pSymbol; 
    }FLASH_GEOMETRY;
protected:
	//events
	virtual void OnPaint(wxPaintEvent & event);
	virtual void OnEraseBackground(wxEraseEvent & event);
	virtual void OnSize(wxSizeEvent & event);
    virtual void OnTimer( wxTimerEvent & event);
	virtual void OnKeyDown(wxKeyEvent & event);
    virtual void OnCaptureLost(wxMouseCaptureLostEvent & event);
	virtual void OnMouseWheel(wxMouseEvent& event);
	//
	virtual void OnDraw(wxGISEnumDrawPhase nPhase);
    virtual void OnMapDrawing(wxMxMapViewUIEvent& event);
    virtual void OnLayerChanged(wxMxMapViewEvent& event);
    virtual void OnLayerLoading(wxMxMapViewEvent& event);
	//misc
	virtual void DrawToolTip(wxClientDC& dc, const wxString& sText);
	virtual OGREnvelope CreateEnvelopeFromZoomFactor(double dZoom);
	virtual void UpdateFrameCenter(void);
	//virtual void FillClipGeometry(wxRect rect, wxCoord x, wxCoord y);
    virtual void Flash(wxGISEnumFlashStyle eFlashStyle = enumGISMapFlashNewColor);
    virtual void DrawGeometry(const wxGISGeometry &Geometry, wxGISSymbol* const pSymbol);
protected:
    void Refresh(void);
    //void OnZooming( wxCommandEvent & event );
protected:
    virtual bool IsDrawing() const;
    virtual wxThread::ExitCode Entry();
    virtual bool CreateAndRunDrawThread(void);
    virtual void DestroyDrawThread(void);
protected:
	wxTimer m_timer;
	ITrackCancel *m_pTrackCancel;
	IProgressor *m_pAni;
    wxDword m_nDrawingState;
	double m_nFactor;
	wxPoint m_StartMouseLocation;
	wxPoint m_FrameCenter;
	double m_dOriginAngle;
	double m_dCurrentAngle;
    //flash
    wxVector<FLASH_GEOMETRY> m_staFlashGeoms;
    wxGISEnumFlashStyle m_eFlashStyle;

	wxCriticalSection m_CritSect, m_KeysCritSect, m_FlashCritSect;

	//wxGISPointsArray m_ClipGeometry;
    wxSize m_PrevSize;
    wxDateTime m_dtNow;
private:
	DECLARE_EVENT_TABLE()
};

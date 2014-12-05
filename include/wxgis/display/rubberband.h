/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISRubberBand class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013,2014 Dmitry Baryshnikov
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

#include "wxgis/display/display.h"
#include "wxgis/display/gisdisplay.h"
#include "wxgis/datasource/gdalinh.h"

/** @class wxGISRubberBand

    The class to drawing shape on window while mouse dragging.

    @library{display}
*/

class WXDLLIMPEXP_GIS_DSP wxGISRubberBand :
	public wxEvtHandler
{
    DECLARE_CLASS(wxGISRubberBand)
public:
	wxGISRubberBand(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef = wxNullSpatialReference);
	virtual ~wxGISRubberBand(void);
	virtual wxGISGeometry TrackNew(wxCoord x, wxCoord y);
	virtual void OnUnlock(void);
	//events
	virtual void OnKeyDown(wxKeyEvent & event);
	virtual void OnMouseMove(wxMouseEvent& event);
	virtual void OnMouseDown(wxMouseEvent& event);
	virtual void OnMouseUp(wxMouseEvent& event);
	virtual void OnMouseDoubleClick(wxMouseEvent& event);
	virtual void OnLeave(wxMouseEvent& event);
	virtual void OnEnter(wxMouseEvent& event);
    virtual void OnCaptureLost(wxMouseCaptureLostEvent & event);
protected:
	wxGISGeometry m_RetGeom;
	bool m_bLock;
	wxCoord m_StartX;
	wxCoord m_StartY;
	wxCoord m_StartXScr;
	wxCoord m_StartYScr;
	wxWindow *m_pWnd;
	wxGISDisplayUI *m_pDisp;
	wxPen m_oPen;

    //wxRect m_PrevRect;
    wxGISSpatialReference m_SpaRef;
private:
	DECLARE_EVENT_TABLE()
};

/** @class wxGISRubberEnvelope

    The class to drawing rectangle on window while mouse dragging.
    The class functionality uses in identify, ZoomIn and ZoomOut tools.

    @library{display}
*/

class WXDLLIMPEXP_GIS_DSP wxGISRubberEnvelope :
	public wxGISRubberBand
{
    DECLARE_CLASS(wxGISRubberEnvelope)
public:
	wxGISRubberEnvelope(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef = wxNullSpatialReference);
	virtual ~wxGISRubberEnvelope(void);
	virtual void OnMouseMove(wxMouseEvent& event);
	virtual void OnMouseUp(wxMouseEvent& event);
};

/** @class wxGISRubberCircle

    The class to drawing circle on window while mouse dragging.

    @library{display}
*/

class WXDLLIMPEXP_GIS_DSP wxGISRubberCircle :
    public wxGISRubberBand
{
    DECLARE_CLASS(wxGISRubberCircle)
public:
    wxGISRubberCircle(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef = wxNullSpatialReference);
    virtual ~wxGISRubberCircle(void);
    virtual void OnMouseMove(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
};

/** @class wxGISRubberEllipse

    The class to drawing ellipse on window while mouse dragging.

    @library{display}
*/

class WXDLLIMPEXP_GIS_DSP wxGISRubberEllipse :
    public wxGISRubberEnvelope
{
    DECLARE_CLASS(wxGISRubberEllipse)
public:
    wxGISRubberEllipse(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef = wxNullSpatialReference);
    virtual ~wxGISRubberEllipse(void);
    virtual void OnMouseMove(wxMouseEvent& event);
};

/** @class wxGISRubberFreeHand

    The class to drawing freehand on window while mouse dragging.

    @library{display}
*/

class WXDLLIMPEXP_GIS_DSP wxGISRubberFreeHand :
    public wxGISRubberBand
{
    DECLARE_CLASS(wxGISRubberFreeHand)
public:
    wxGISRubberFreeHand(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef = wxNullSpatialReference);
    virtual ~wxGISRubberFreeHand(void);
    virtual void OnMouseMove(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
protected:
    wxVector<wxPoint> m_aoPoints;
};

/** @class wxGISRubberMarker

    The class to drawing marker on window while mouse dragging.

    @library{display}
*/

class WXDLLIMPEXP_GIS_DSP wxGISRubberMarker :
    public wxGISRubberBand
{
    DECLARE_CLASS(wxGISRubberMarker)
public:
    wxGISRubberMarker(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef = wxNullSpatialReference);
    virtual ~wxGISRubberMarker(void);
    virtual void OnMouseUp(wxMouseEvent& event);
};

/** @class wxGISRubberLine

    The class to drawing line on window while mouse dragging.

    @library{display}
*/

class WXDLLIMPEXP_GIS_DSP wxGISRubberLine :
    public wxGISRubberBand
{
    DECLARE_CLASS(wxGISRubberLine)
public:
    wxGISRubberLine(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef = wxNullSpatialReference);
    virtual ~wxGISRubberLine(void);
    virtual void OnMouseMove(wxMouseEvent& event);
    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseDoubleClick(wxMouseEvent& event);
protected:
    wxVector<wxPoint> m_aoPoints;
};

/** @class wxGISRubberPolygon

    The class to drawing polygon on window while mouse dragging.

    @library{display}
*/

class WXDLLIMPEXP_GIS_DSP wxGISRubberPolygon :
    public wxGISRubberLine
{
    DECLARE_CLASS(wxGISRubberPolygon)
public:
    wxGISRubberPolygon(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef = wxNullSpatialReference);
    virtual ~wxGISRubberPolygon(void);
    virtual void OnMouseMove(wxMouseEvent& event);
    virtual void OnMouseDoubleClick(wxMouseEvent& event);
};

#ifdef wxGIS_USE_SPLINE

/** @class wxGISRubberSpline

    The class to drawing spline on window while mouse dragging.

    @library{display}
*/

class WXDLLIMPEXP_GIS_DSP wxGISRubberSpline :
    public wxGISRubberLine
{
    DECLARE_CLASS(wxGISRubberSpline)
public:
    wxGISRubberSpline(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef = wxNullSpatialReference);
    virtual ~wxGISRubberSpline(void);
    virtual void OnMouseMove(wxMouseEvent& event);
};
#endif // wxGIS_USE_SPLINE


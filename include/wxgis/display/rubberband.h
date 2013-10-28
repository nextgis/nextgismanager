/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISRubberBand class.
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

#pragma once

#include "wxgis/display/display.h"
#include "wxgis/display/gisdisplay.h"
#include "wxgis/datasource/gdalinh.h"

/** \class wxGISRubberBand rubberband.h
    \brief The class to drawing rectangles on display while mouse dragging

    The class functionality uses in identify, ZoomIn and ZoomOut tools
*/

class WXDLLIMPEXP_GIS_DSP wxGISRubberBand :
	public wxEvtHandler
{
    DECLARE_CLASS(wxGISRubberBand)
public:
	wxGISRubberBand(wxPen oPen, wxWindow *pWnd, wxGISDisplay *pDisp, const wxGISSpatialReference &SpaRef = wxNullSpatialReference);
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
	wxGISDisplay *m_pDisp;
	wxPen m_oPen;

    wxRect m_PrevRect;
    wxGISSpatialReference m_SpaRef;
private:
	DECLARE_EVENT_TABLE()
};

/** \class wxGISRubberEnvelope rubberband.h
    \brief The special version with clip drawing
*/

class WXDLLIMPEXP_GIS_DSP wxGISRubberEnvelope :
	public wxGISRubberBand
{
    DECLARE_CLASS(wxGISRubberEnvelope)
public:
	wxGISRubberEnvelope(wxPen oPen, wxWindow *pWnd, wxGISDisplay *pDisp, const wxGISSpatialReference &SpaRef = wxNullSpatialReference);
	virtual ~wxGISRubberEnvelope(void);
	virtual void OnMouseMove(wxMouseEvent& event);
	virtual void OnMouseUp(wxMouseEvent& event);
};

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

#include "wxgis/display/rubberband.h"

#include <wx/evtloop.h>

//----------------------------------------------------
// class wxGISRubberBand
//----------------------------------------------------

IMPLEMENT_CLASS(wxGISRubberBand, wxEvtHandler)

BEGIN_EVENT_TABLE(wxGISRubberBand, wxEvtHandler)
	EVT_KEY_DOWN(wxGISRubberBand::OnKeyDown)
	EVT_LEFT_DOWN(wxGISRubberBand::OnMouseDown)
	EVT_MIDDLE_DOWN(wxGISRubberBand::OnMouseDown)
	EVT_RIGHT_DOWN(wxGISRubberBand::OnMouseDown)
	EVT_LEFT_UP(wxGISRubberBand::OnMouseUp)
	EVT_MIDDLE_UP(wxGISRubberBand::OnMouseUp)
	EVT_RIGHT_UP(wxGISRubberBand::OnMouseUp)
	EVT_LEFT_DCLICK(wxGISRubberBand::OnMouseDoubleClick)
	EVT_MIDDLE_DCLICK(wxGISRubberBand::OnMouseDoubleClick)
	EVT_RIGHT_DCLICK(wxGISRubberBand::OnMouseDoubleClick)
	EVT_MOTION(wxGISRubberBand::OnMouseMove)
	EVT_LEAVE_WINDOW(wxGISRubberBand::OnLeave)
	EVT_ENTER_WINDOW(wxGISRubberBand::OnEnter)
	EVT_MOUSE_CAPTURE_LOST(wxGISRubberBand::OnCaptureLost)
END_EVENT_TABLE()

wxGISRubberBand::wxGISRubberBand(wxPen oPen, wxWindow *pWnd, wxGISDisplay *pDisp, const wxGISSpatialReference &SpaRef) :  m_bLock(true)
{
	m_pWnd = pWnd;
	m_pDisp = pDisp;
	m_oPen = oPen;
    m_SpaRef = SpaRef;
}

wxGISRubberBand::~wxGISRubberBand(void)
{
}

wxGISGeometry wxGISRubberBand::TrackNew(wxCoord x, wxCoord y)
{
	m_StartX = x;
	m_StartY = y;

	wxRect ScrRect = m_pWnd->GetScreenRect();
	wxRect Rect = m_pWnd->GetRect();
	m_StartXScr = ScrRect.GetLeft() + m_StartX - Rect.GetLeft();
	m_StartYScr = ScrRect.GetTop() + m_StartY - Rect.GetTop();

	m_pWnd->CaptureMouse();
	m_pWnd->PushEventHandler(this);

	wxEventLoopBase* const loop = wxEventLoop::GetActive();
	while(loop->IsRunning())
	{
		loop->Dispatch();
		if(!m_bLock) 
            break;
	}
	m_pWnd->PopEventHandler();
	m_pWnd->ReleaseMouse();
	return m_RetGeom;
}

void wxGISRubberBand::OnUnlock(void)
{
	m_bLock = false;
}

void wxGISRubberBand::OnKeyDown(wxKeyEvent & event)
{
	switch(event.GetKeyCode())
	{
	case WXK_ESCAPE:
		m_pWnd->Refresh(false);
		OnUnlock();
		break;
	default:
		break;
	}
	//event.Skip();
}

void wxGISRubberBand::OnMouseMove(wxMouseEvent& event)
{
    event.Skip();
}

void wxGISRubberBand::OnMouseDown(wxMouseEvent& event)
{
    event.Skip();
}

void wxGISRubberBand::OnMouseUp(wxMouseEvent& event)
{
    event.Skip();
}

void wxGISRubberBand::OnMouseDoubleClick(wxMouseEvent& event)
{
    event.Skip();
}

void wxGISRubberBand::OnLeave(wxMouseEvent& event)
{
    event.Skip();
}

void wxGISRubberBand::OnEnter(wxMouseEvent& event)
{
    event.Skip();
}

void wxGISRubberBand::OnCaptureLost(wxMouseCaptureLostEvent & event)
{
	event.Skip();
	if( m_pWnd->HasCapture() )
		m_pWnd->ReleaseMouse();
}

//----------------------------------------------------
// class wxGISRubberEnvelope
//----------------------------------------------------
IMPLEMENT_CLASS(wxGISRubberEnvelope, wxGISRubberBand)

wxGISRubberEnvelope::wxGISRubberEnvelope(wxPen oPen, wxWindow *pWnd, wxGISDisplay *pDisp, const wxGISSpatialReference &SpaRef) : wxGISRubberBand(oPen, pWnd, pDisp, SpaRef)
{
}

wxGISRubberEnvelope::~wxGISRubberEnvelope()
{
}

void wxGISRubberEnvelope::OnMouseMove(wxMouseEvent& event)
{
    event.Skip();

	int EvX = event.GetX(), EvY = event.GetY();
	int width, height, X, Y;
	width = abs(EvX - m_StartX);
	height = abs(EvY - m_StartY);
	X = std::min(m_StartX, EvX);
	Y = std::min(m_StartY, EvY);

	wxClientDC CDC(m_pWnd);
	//wxGISPointsArray ClipGeometry;
 //   if(!m_PrevRect.IsEmpty())
 //   {
 //       m_PrevRect.Inflate(2,2);
	//	ClipGeometry.Add(new wxRealPoint(double(m_PrevRect.GetLeft()), double(m_PrevRect.GetTop())));//top-left
	//	ClipGeometry.Add(new wxRealPoint(double(m_PrevRect.GetRight()), double(m_PrevRect.GetTop())));//top-right
	//	ClipGeometry.Add(new wxRealPoint(double(m_PrevRect.GetRight()), double(m_PrevRect.GetBottom())));//bottom-right
	//	ClipGeometry.Add(new wxRealPoint(double(m_PrevRect.GetLeft()), double(m_PrevRect.GetBottom())));//bottom-left
	//	m_pDisp->Output(&CDC);
 //   }
	//else
		m_pDisp->Output(&CDC);

	CDC.SetPen(m_oPen);
	CDC.SetBrush(wxBrush(m_oPen.GetColour(), wxTRANSPARENT));
	CDC.SetLogicalFunction(wxOR_REVERSE);
	CDC.DrawRectangle(X, Y, width, height);
    m_PrevRect = wxRect(X, Y, width, height);
}

void wxGISRubberEnvelope::OnMouseUp(wxMouseEvent& event)
{
    event.Skip();

	double dX1 = std::min(m_StartX, event.GetX());
	double dY1 = std::max(m_StartY, event.GetY());
	double dX2 = std::max(m_StartX, event.GetX());
	double dY2 = std::min(m_StartY, event.GetY());

	double dfX1(dX1), dfX2(dX2), dfX3(dX2), dfX4(dX1);
	double dfY1(dY1), dfY2(dY1), dfY3(dY2), dfY4(dY2);
	m_pDisp->DC2World(&dfX1, &dfY1);
	m_pDisp->DC2World(&dfX2, &dfY2);
	m_pDisp->DC2World(&dfX3, &dfY3);
	m_pDisp->DC2World(&dfX4, &dfY4);

	OGRLinearRing ring;
	ring.addPoint(dfX1, dfY1);
	ring.addPoint(dfX2, dfY2);
	ring.addPoint(dfX3, dfY3);
	ring.addPoint(dfX4, dfY4);
    ring.closeRings();

    OGRPolygon* pRgn = new OGRPolygon();
    pRgn->addRing(&ring);
    pRgn->flattenTo2D();
    if(m_SpaRef.IsOk())
		pRgn->assignSpatialReference(m_SpaRef);
	m_RetGeom = wxGISGeometry(static_cast<OGRGeometry*>(pRgn));

	//wxRect rc(wxPoint(dX1, dY1), wxPoint(dX2, dY2));
	//m_RetEnv = m_pDisp->TransformRect(rc);
	//if(IsDoubleEquil(dX1, dX2) || IsDoubleEquil(dY1, dY2))
	//{
	//	m_RetEnv.MaxX = m_RetEnv.MinX;
	//	m_RetEnv.MaxY = m_RetEnv.MinY;
	//}
	//m_pDisp->DC2World(&dX1, &dY1);
	//m_pDisp->DC2World(&dX2, &dY2);
	//m_RetEnv.MinX = dX1;
	//m_RetEnv.MinY = dY1;
	//m_RetEnv.MaxX = dX2;
	//m_RetEnv.MaxY = dY2;



	OnUnlock();
    m_PrevRect.width = -1;
    m_PrevRect.height = -1;
}

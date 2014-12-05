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

wxGISRubberBand::wxGISRubberBand(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef) :  m_bLock(true)
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

    if (!m_pWnd->HasCapture())
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
	if( m_pWnd->HasCapture() )
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
    event.Skip(true);
}

void wxGISRubberBand::OnMouseDown(wxMouseEvent& event)
{
    event.Skip(true);
}

void wxGISRubberBand::OnMouseUp(wxMouseEvent& event)
{
    event.Skip(true);
}

void wxGISRubberBand::OnMouseDoubleClick(wxMouseEvent& event)
{
    event.Skip(true);
}

void wxGISRubberBand::OnLeave(wxMouseEvent& event)
{
    event.Skip(true);
}

void wxGISRubberBand::OnEnter(wxMouseEvent& event)
{
    event.Skip(true);
}

void wxGISRubberBand::OnCaptureLost(wxMouseCaptureLostEvent & event)
{
	event.Skip(true);
	if( m_pWnd->HasCapture() )
		m_pWnd->ReleaseMouse();
}

//----------------------------------------------------
// class wxGISRubberEnvelope
//----------------------------------------------------
IMPLEMENT_CLASS(wxGISRubberEnvelope, wxGISRubberBand)

wxGISRubberEnvelope::wxGISRubberEnvelope(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef) : wxGISRubberBand(oPen, pWnd, pDisp, SpaRef)
{
}

wxGISRubberEnvelope::~wxGISRubberEnvelope()
{
}

void wxGISRubberEnvelope::OnMouseMove(wxMouseEvent& event)
{
    event.Skip(true);

	int EvX = event.GetX(), EvY = event.GetY();
	int width, height, X, Y;
	width = abs(EvX - m_StartX);
	height = abs(EvY - m_StartY);
    X = wxMin(m_StartX, EvX);
    Y = wxMin(m_StartY, EvY);

	wxClientDC CDC(m_pWnd);
	m_pDisp->Output(&CDC);

	CDC.SetPen(m_oPen);
	CDC.SetBrush(wxBrush(m_oPen.GetColour(), wxTRANSPARENT));
	CDC.SetLogicalFunction(wxOR_REVERSE);
	CDC.DrawRectangle(X, Y, width, height);
    //m_PrevRect = wxRect(X, Y, width, height);
}

void wxGISRubberEnvelope::OnMouseUp(wxMouseEvent& event)
{
    event.Skip(true);

    double dX1 = wxMin(m_StartX, event.GetX());
	double dY1 = wxMax(m_StartY, event.GetY());
    double dX2 = wxMax(m_StartX, event.GetX());
    double dY2 = wxMin(m_StartY, event.GetY());

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

	OnUnlock();
    //m_PrevRect.width = -1;
    //m_PrevRect.height = -1;
}

//----------------------------------------------------
// class wxGISRubberCircle
//----------------------------------------------------
IMPLEMENT_CLASS(wxGISRubberCircle, wxGISRubberBand)

wxGISRubberCircle::wxGISRubberCircle(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef) : wxGISRubberBand(oPen, pWnd, pDisp, SpaRef)
{
}

wxGISRubberCircle::~wxGISRubberCircle()
{
}

void wxGISRubberCircle::OnMouseMove(wxMouseEvent& event)
{
    event.Skip(true);

    int EvX = event.GetX(), EvY = event.GetY();
    int width, height;
    width = EvX - m_StartX;
    height = EvY - m_StartY;

    wxClientDC CDC(m_pWnd);
    m_pDisp->Output(&CDC);

    CDC.SetPen(m_oPen);
    CDC.SetBrush(wxBrush(m_oPen.GetColour(), wxTRANSPARENT));
    CDC.SetLogicalFunction(wxOR_REVERSE);

    wxCoord nRadius = sqrt(width*width + height*height);
    CDC.DrawCircle(m_StartX, m_StartY, nRadius);
    CDC.DrawLine(m_StartX, m_StartY, EvX, EvY);
}

void wxGISRubberCircle::OnMouseUp(wxMouseEvent& event)
{
    event.Skip(true);

    double dX1 = m_StartX;
    double dY1 = m_StartY;
    double dX2 = event.GetX();
    double dY2 = event.GetY();

    m_pDisp->DC2World(&dX1, &dY1);
    m_pDisp->DC2World(&dX2, &dY2);

    OGRPoint pt1(dX1, dY1);
    OGRPoint pt2(dX2, dY2);

    OGRMultiPoint * pMPt = new OGRMultiPoint();
    pMPt->addGeometry(&pt1);
    pMPt->addGeometry(&pt2);
    pMPt->flattenTo2D();
    if (m_SpaRef.IsOk())
        pMPt->assignSpatialReference(m_SpaRef);
    m_RetGeom = wxGISGeometry(static_cast<OGRGeometry*>(pMPt));

    OnUnlock();
}

//----------------------------------------------------
// class wxGISRubberEllipse
//----------------------------------------------------
IMPLEMENT_CLASS(wxGISRubberEllipse, wxGISRubberEnvelope)

wxGISRubberEllipse::wxGISRubberEllipse(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef) : wxGISRubberEnvelope(oPen, pWnd, pDisp, SpaRef)
{
}

wxGISRubberEllipse::~wxGISRubberEllipse()
{
}

void wxGISRubberEllipse::OnMouseMove(wxMouseEvent& event)
{
    event.Skip(true);

    int EvX = event.GetX(), EvY = event.GetY();
    int width, height, X, Y;
    width = abs(EvX - m_StartX);
    height = abs(EvY - m_StartY);
    X = wxMin(m_StartX, EvX);
    Y = wxMin(m_StartY, EvY);

    wxClientDC CDC(m_pWnd);
    m_pDisp->Output(&CDC);

    CDC.SetPen(m_oPen);
    CDC.SetBrush(wxBrush(m_oPen.GetColour(), wxTRANSPARENT));
    CDC.SetLogicalFunction(wxOR_REVERSE);

    wxRect rect;
    CDC.DrawEllipse(wxPoint(X,Y), wxSize(width, height));
}

//----------------------------------------------------
// class wxGISRubberFreeHand
//----------------------------------------------------
IMPLEMENT_CLASS(wxGISRubberFreeHand, wxGISRubberBand)

#define FREEHAND_STEP_PIX 7

wxGISRubberFreeHand::wxGISRubberFreeHand(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef) : wxGISRubberBand(oPen, pWnd, pDisp, SpaRef)
{
}

wxGISRubberFreeHand::~wxGISRubberFreeHand()
{
}

void wxGISRubberFreeHand::OnMouseMove(wxMouseEvent& event)
{
    event.Skip(true);

    int EvX = event.GetX(), EvY = event.GetY();
    int width, height;
    if (m_aoPoints.empty())
    {
        width = abs(EvX - m_StartX);
        height = abs(EvY - m_StartY);
    }
    else
    {
        width = abs(EvX - m_aoPoints[m_aoPoints.size() - 1].x);
        height = abs(EvY - m_aoPoints[m_aoPoints.size() - 1].y);
    }

    if (width > FREEHAND_STEP_PIX || height > FREEHAND_STEP_PIX)
    {
        m_aoPoints.push_back(wxPoint(EvX, EvY));
    }

    if (m_aoPoints.empty())
    {
        return;
    }

    wxClientDC CDC(m_pWnd);
    m_pDisp->Output(&CDC);

    CDC.SetPen(m_oPen);
    CDC.SetBrush(wxBrush(m_oPen.GetColour(), wxTRANSPARENT));
    CDC.SetLogicalFunction(wxOR_REVERSE);

#ifdef wxGIS_USE_SPLINE
    if (m_aoPoints.empty())
    {
        CDC.DrawLine(m_StartX, m_StartY, EvX, EvY);
    }
    else
    {
        wxPoint *paoPoints = new wxPoint[m_aoPoints.size() + 2];
        int nCounter = 0;
        paoPoints[nCounter++] = wxPoint(m_StartX, m_StartY);
        size_t i;
        for (i = 0; i < m_aoPoints.size(); ++i)
        {
            paoPoints[nCounter++] = m_aoPoints[i];
        }
        paoPoints[nCounter++] = wxPoint(EvX, EvY);

        CDC.DrawLines(nCounter, paoPoints);

        wxDELETEA(paoPoints);
    }
#else
    wxPoint *paoPoints = new wxPoint[m_aoPoints.size() + 1];
    int nCounter = 0;
    paoPoints[nCounter++] = wxPoint(m_StartX, m_StartY);
    size_t i;
    for (i = 0; i < m_aoPoints.size(); ++i)
    {
    paoPoints[nCounter++] = m_aoPoints[i];
    }

    CDC.DrawLines(nCounter, paoPoints);

    wxDELETEA(paoPoints);
#endif //wxGIS_USE_SPLINE

}

void wxGISRubberFreeHand::OnMouseUp(wxMouseEvent& event)
{
    event.Skip(true);

    OGRLineString* pLine = new OGRLineString();
    double dX1 = m_StartX;
    double dY1 = m_StartY;
    m_pDisp->DC2World(&dX1, &dY1);
    pLine->addPoint(dX1, dY1);

    for (size_t i = 0; i < m_aoPoints.size(); ++i)
    {
        dX1 = m_aoPoints[i].x;
        dY1 = m_aoPoints[i].y;
        m_pDisp->DC2World(&dX1, &dY1);

        pLine->addPoint(dX1, dY1);
    }

    pLine->flattenTo2D();
    if (m_SpaRef.IsOk())
        pLine->assignSpatialReference(m_SpaRef);
    m_RetGeom = wxGISGeometry(static_cast<OGRGeometry*>(pLine));

    OnUnlock();
}

//----------------------------------------------------
// class wxGISRubberMarker
//----------------------------------------------------
IMPLEMENT_CLASS(wxGISRubberMarker, wxGISRubberBand)

wxGISRubberMarker::wxGISRubberMarker(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef) : wxGISRubberBand(oPen, pWnd, pDisp, SpaRef)
{
}

wxGISRubberMarker::~wxGISRubberMarker()
{
}

void wxGISRubberMarker::OnMouseUp(wxMouseEvent& event)
{
    event.Skip(true);


    int EvX = event.GetX(), EvY = event.GetY();

    double dX1 = event.GetX();
    double dY1 = event.GetY();
    m_pDisp->DC2World(&dX1, &dY1);
    OGRPoint* pPt = new OGRPoint(dX1, dY1);

    if (m_SpaRef.IsOk())
        pPt->assignSpatialReference(m_SpaRef);
    m_RetGeom = wxGISGeometry(static_cast<OGRGeometry*>(pPt));

    OnUnlock();
}


//----------------------------------------------------
// class wxGISRubberLine
//----------------------------------------------------
IMPLEMENT_CLASS(wxGISRubberLine, wxGISRubberBand)

wxGISRubberLine::wxGISRubberLine(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef) : wxGISRubberBand(oPen, pWnd, pDisp, SpaRef)
{
}

wxGISRubberLine::~wxGISRubberLine()
{
}

void wxGISRubberLine::OnMouseMove(wxMouseEvent& event)
{
    event.Skip(true);

    int EvX = event.GetX(), EvY = event.GetY();

    wxClientDC CDC(m_pWnd);
    m_pDisp->Output(&CDC);

    CDC.SetPen(m_oPen);
    CDC.SetBrush(wxBrush(m_oPen.GetColour(), wxTRANSPARENT));
    CDC.SetLogicalFunction(wxOR_REVERSE);

    if (m_aoPoints.empty())
    {
        CDC.DrawLine(m_StartX, m_StartY, EvX, EvY);
    }
    else
    {
        wxPoint *paoPoints = new wxPoint[m_aoPoints.size() + 2];
        int nCounter = 0;
        paoPoints[nCounter++] = wxPoint(m_StartX, m_StartY);
        size_t i;
        for (i = 0; i < m_aoPoints.size(); ++i)
        {
            paoPoints[nCounter++] = m_aoPoints[i];
        }
        paoPoints[nCounter++] = wxPoint(EvX, EvY);

        CDC.DrawLines(nCounter, paoPoints);

        wxDELETEA(paoPoints);
    }
}

void wxGISRubberLine::OnMouseDown(wxMouseEvent& event)
{
    event.Skip(true);

    int EvX = event.GetX(), EvY = event.GetY();
    m_aoPoints.push_back(wxPoint(EvX, EvY));
}

void wxGISRubberLine::OnMouseDoubleClick(wxMouseEvent& event)
{
    event.Skip(true);

    OGRLineString* pLine = new OGRLineString();
    double dX1 = m_StartX;
    double dY1 = m_StartY;
    m_pDisp->DC2World(&dX1, &dY1);
    pLine->addPoint(dX1, dY1);

    for (size_t i = 0; i < m_aoPoints.size(); ++i)
    {
        dX1 = m_aoPoints[i].x;
        dY1 = m_aoPoints[i].y;
        m_pDisp->DC2World(&dX1, &dY1);
        pLine->addPoint(dX1, dY1);
    }

    pLine->flattenTo2D();
    if (m_SpaRef.IsOk())
        pLine->assignSpatialReference(m_SpaRef);
    m_RetGeom = wxGISGeometry(static_cast<OGRGeometry*>(pLine));

    OnUnlock();
}

//----------------------------------------------------
// class wxGISRubberPolygon
//----------------------------------------------------
IMPLEMENT_CLASS(wxGISRubberPolygon, wxGISRubberLine)

wxGISRubberPolygon::wxGISRubberPolygon(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef) : wxGISRubberLine(oPen, pWnd, pDisp, SpaRef)
{
}

wxGISRubberPolygon::~wxGISRubberPolygon()
{
}

void wxGISRubberPolygon::OnMouseMove(wxMouseEvent& event)
{
    event.Skip(true);

    int EvX = event.GetX(), EvY = event.GetY();

    wxClientDC CDC(m_pWnd);
    m_pDisp->Output(&CDC);

    CDC.SetPen(m_oPen);
    CDC.SetBrush(wxBrush(m_oPen.GetColour(), wxTRANSPARENT));
    CDC.SetLogicalFunction(wxOR_REVERSE);

    if (m_aoPoints.empty())
    {
        CDC.DrawLine(m_StartX, m_StartY, EvX, EvY);
    }
    else
    {
        wxPoint *paoPoints = new wxPoint[m_aoPoints.size() + 3];
        int nCounter = 0;
        paoPoints[nCounter++] = wxPoint(m_StartX, m_StartY);
        size_t i;
        for (i = 0; i < m_aoPoints.size(); ++i)
        {
            paoPoints[nCounter++] = m_aoPoints[i];
        }
        paoPoints[nCounter++] = wxPoint(EvX, EvY);
        paoPoints[nCounter++] = wxPoint(m_StartX, m_StartY);

        CDC.DrawLines(nCounter, paoPoints);

        wxDELETEA(paoPoints);
    }
}

void wxGISRubberPolygon::OnMouseDoubleClick(wxMouseEvent& event)
{
    event.Skip(true);

    OGRLinearRing ring;
    double dX1 = m_StartX;
    double dY1 = m_StartY;
    m_pDisp->DC2World(&dX1, &dY1);
    ring.addPoint(dX1, dY1);

    for (size_t i = 0; i < m_aoPoints.size(); ++i)
    {
        dX1 = m_aoPoints[i].x;
        dY1 = m_aoPoints[i].y;
        m_pDisp->DC2World(&dX1, &dY1);
        ring.addPoint(dX1, dY1);
    }
    ring.closeRings();

    OGRPolygon* pRgn = new OGRPolygon();
    pRgn->addRing(&ring);
    pRgn->flattenTo2D();
    if (m_SpaRef.IsOk())
        pRgn->assignSpatialReference(m_SpaRef);
    m_RetGeom = wxGISGeometry(static_cast<OGRGeometry*>(pRgn));

    OnUnlock();
}

#ifdef wxGIS_USE_SPLINE

//----------------------------------------------------
// class wxGISRubberSpline
//----------------------------------------------------
IMPLEMENT_CLASS(wxGISRubberSpline, wxGISRubberLine)

wxGISRubberSpline::wxGISRubberSpline(wxPen oPen, wxWindow *pWnd, wxGISDisplayUI *pDisp, const wxGISSpatialReference &SpaRef) : wxGISRubberLine(oPen, pWnd, pDisp, SpaRef)
{
}

wxGISRubberSpline::~wxGISRubberSpline()
{
}

void wxGISRubberSpline::OnMouseMove(wxMouseEvent& event)
{
    event.Skip(true);

    int EvX = event.GetX(), EvY = event.GetY();

    wxClientDC CDC(m_pWnd);
    m_pDisp->Output(&CDC);

    CDC.SetPen(m_oPen);
    CDC.SetBrush(wxBrush(m_oPen.GetColour(), wxTRANSPARENT));
    CDC.SetLogicalFunction(wxOR_REVERSE);

    if (m_aoPoints.empty())
    {
        CDC.DrawLine(m_StartX, m_StartY, EvX, EvY);
    }
    else
    {
        wxPoint *paoPoints = new wxPoint[m_aoPoints.size() + 2];
        int nCounter = 0;
        paoPoints[nCounter++] = wxPoint(m_StartX, m_StartY);
        size_t i;
        for (i = 0; i < m_aoPoints.size(); ++i)
        {
            paoPoints[nCounter++] = m_aoPoints[i];
        }
        paoPoints[nCounter++] = wxPoint(EvX, EvY);

        CDC.DrawSpline(nCounter, paoPoints);

        wxDELETEA(paoPoints);
    }
}

#endif // wxGIS_USE_SPLINE


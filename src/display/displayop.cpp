/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  display transformation. Transform from world to screen coordinates and vice versa
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
#include "wxgis/display/displayop.h"
#include "wxgis/display/color.h"

#include <cairo.h>

#ifdef __WXMSW__
	#include <cairo-win32.h>
#endif

void SetColorValue(wxXmlNode* pNode, const wxString &sAttrName, const wxGISColor &oColor)
{
    pNode->AddAttribute(sAttrName, oColor.GetAsString());
}

wxGISColor GetColorValue(const wxXmlNode* pNode, const wxString &sAttrName, const wxGISColor &oDefaultColor)
{
    if(!pNode)
        return oDefaultColor;
    const wxString sVal = pNode->GetAttribute(sAttrName, oDefaultColor.GetAsString());
    wxGISColor color;
    color.SetFromString(sVal);
    return color;
}

void RotateEnvelope(OGREnvelope &Env, double dAngle, double dX, double dY)
{
	cairo_matrix_t Matrix;
	cairo_matrix_init_translate(&Matrix, dX, dY);
	cairo_matrix_rotate(&Matrix, dAngle);
	cairo_matrix_translate(&Matrix, -dX, -dY);
	//cairo_matrix_init_rotate(&Matrix, dAngle);
	double X1 = Env.MinX;
	double Y1 = Env.MaxY;
	double X2 = Env.MaxX;
	double Y2 = Env.MaxY;
	double X3 = Env.MaxX;
	double Y3 = Env.MinY;
	double X4 = Env.MinX;
	double Y4 = Env.MinY;

	cairo_matrix_transform_point(&Matrix, &X1, &Y1);
	cairo_matrix_transform_point(&Matrix, &X2, &Y2);
	cairo_matrix_transform_point(&Matrix, &X3, &Y3);
	cairo_matrix_transform_point(&Matrix, &X4, &Y4);

    Env.MinX = wxMin(wxMin(X1, X2), wxMin(X3, X4));
    Env.MinY = wxMin(wxMin(Y1, Y2), wxMin(Y3, Y4));
    Env.MaxX = wxMax(wxMax(X1, X2), wxMax(X3, X4));
    Env.MaxY = wxMax(wxMax(Y1, Y2), wxMax(Y3, Y4));
}

//Sutherland-Hodgman Polygon Clipping
//Adopted from (C) 2005 by Gavin Macaulay QGIS Project

void ClipGeometryByEnvelope(OGRRawPoint* pOGRRawPoints, int *pnPointCount, const OGREnvelope &Env, bool shapeOpen)
{
	OGRRawPoint* pTmpOGRRawPoints = new OGRRawPoint[*pnPointCount * 2];
	int nTmpPtCount(0);
	TrimFeatureToBoundary( pOGRRawPoints, *pnPointCount, &pTmpOGRRawPoints, &nTmpPtCount, enumGISPtPosRight, Env, shapeOpen );

	*pnPointCount = 0;
	TrimFeatureToBoundary( pTmpOGRRawPoints, nTmpPtCount, &pOGRRawPoints, pnPointCount, enumGISPtPosTop, Env, shapeOpen );

	nTmpPtCount = 0;
	TrimFeatureToBoundary( pOGRRawPoints, *pnPointCount, &pTmpOGRRawPoints, &nTmpPtCount, enumGISPtPosLeft, Env, shapeOpen );

	*pnPointCount = 0;
	TrimFeatureToBoundary(  pTmpOGRRawPoints, nTmpPtCount, &pOGRRawPoints, pnPointCount, enumGISPtPosBottom, Env, shapeOpen );

	delete [] pTmpOGRRawPoints;
}

//////////////////////////////////////////////////////////////////////////////////////////
//wxGISDisplayTransformation::wxGISDisplayTransformation(void) : m_pSpatialReference(NULL)
//{
//	m_DeviceFrameRect.x = 0;
//	m_DeviceFrameRect.y = 0;
//	m_DeviceFrameRect.width = 800;
//	m_DeviceFrameRect.height = 600;
//	Reset();
//}
//
//wxGISDisplayTransformation::~wxGISDisplayTransformation(void)
//{
//	wxDELETE(m_pSpatialReference);
//}
//
//void wxGISDisplayTransformation::Reset(void)
//{
//	m_Bounds.MaxX = 84;
//	m_Bounds.MinX = -6;
//	m_Bounds.MaxY = 80;
//	m_Bounds.MinY = -80;
//
//	wxDELETE(m_pSpatialReference);
//	m_bIsBoundsSet = false;
//}
//
//void wxGISDisplayTransformation::SetDeviceFrame(wxRect rc)
//{
//	m_DeviceFrameRect = rc;
//	m_WorldXDelta = double(m_Bounds.MaxX - m_Bounds.MinX) / 2;
//	m_WorldYDelta = double(m_Bounds.MaxY - m_Bounds.MinY) / 2;
//	m_DCXDelta = double(m_DeviceFrameRect.GetWidth()) / 2;
//	m_DCYDelta = double(m_DeviceFrameRect.GetHeight()) / 2;
//	double sc1 = fabs(m_DCXDelta / m_WorldXDelta);
//	double sc2 = fabs(m_DCYDelta / m_WorldYDelta);
//    if(sc1 <= DELTA)
//        m_World2DC = sc2;
//    else if(sc2 <= DELTA)
//        m_World2DC = sc1;
//    else
//        m_World2DC	= wxMin(sc1, sc2);
//}
//
//wxRect wxGISDisplayTransformation::GetDeviceFrame(void)
//{
//	return m_DeviceFrameRect;
//}
//
//bool wxGISDisplayTransformation::IsBoundsSet(void)
//{
//	return m_bIsBoundsSet;
//}
//
//void wxGISDisplayTransformation::SetBounds(OGREnvelope bounds)
//{
//	m_Bounds = bounds;
//	m_WorldXDelta = double(m_Bounds.MaxX - m_Bounds.MinX) / 2;
//	m_WorldYDelta = double(m_Bounds.MaxY - m_Bounds.MinY) / 2;
//	m_DCXDelta = double(m_DeviceFrameRect.GetWidth()) / 2;
//	m_DCYDelta = double(m_DeviceFrameRect.GetHeight()) / 2;
//	double sc1 = fabs(m_DCXDelta / m_WorldXDelta);
//	double sc2 = fabs(m_DCYDelta / m_WorldYDelta);
//    if(sc1 <= DELTA)
//        m_World2DC = sc2;
//    else if(sc2 <= DELTA)
//        m_World2DC = sc1;
//    else
//        m_World2DC	= wxMin(sc1, sc2);
//	m_bIsBoundsSet = true;
//}
//
//OGREnvelope wxGISDisplayTransformation::GetBounds(void)
//{
//	return m_Bounds;
//}
//
//OGREnvelope wxGISDisplayTransformation::GetVisibleBounds(void)
//{
//	return TransformRect(m_DeviceFrameRect);
//}
//
//double wxGISDisplayTransformation::GetRatio(void)
//{
//	return m_World2DC;
//}
//
//double wxGISDisplayTransformation::GetScaleRatio(void)
//{
//	double screen_w = (double)m_DeviceFrameRect.GetWidth() / m_ppi.GetWidth() * 2.54; //Width in cm!!!
//	double screen_h = (double)m_DeviceFrameRect.GetHeight() / m_ppi.GetHeight() * 2.54; //Width in cm!!!
//	//double fLinearUnits = m_pSpatialReference->GetLinearUnits();
//	OGREnvelope VisBounds = GetVisibleBounds();
//
//	double w_w =fabs(VisBounds.MaxX - VisBounds.MinX);
//	double w_h = fabs(VisBounds.MaxY - VisBounds.MinY);
//	if(m_pSpatialReference && m_pSpatialReference->IsGeographic())
//	{
//		w_w = w_w * PIDEG * m_pSpatialReference->GetSemiMajor();
//		w_h = w_h * PIDEG * m_pSpatialReference->GetSemiMinor();
//	}
//
//	double screen = wxMin(screen_w, screen_h);
//	double world = wxMin(w_w, w_h);
//
//	return (world * 100) / screen;//cm!!!
//}
//
//void wxGISDisplayTransformation::SetSpatialReference(OGRSpatialReference* pSpatialReference)
//{
//    //set bounds in new spa ref
//	if(m_pSpatialReference)
//	{
//		OGRPoint *pPt1 = new OGRPoint(m_Bounds.MaxX, m_Bounds.MaxY);
//		OGRPoint *pPt2 = new OGRPoint(m_Bounds.MinX, m_Bounds.MinY);
//		pPt1->assignSpatialReference(m_pSpatialReference);
//		pPt2->assignSpatialReference(m_pSpatialReference);
//		pPt1->transformTo(pSpatialReference);
//		pPt2->transformTo(pSpatialReference);
//		OGREnvelope Env;
//		Env.MaxX = pPt1->getX();
//		Env.MaxY = pPt1->getY();
//		Env.MinX = pPt2->getX();
//		Env.MinY = pPt2->getY();
//
//        wxDELETE(pPt1);
//        wxDELETE(pPt2);
//		wxDELETE(m_pSpatialReference);
//		m_pSpatialReference = pSpatialReference->Clone();
//		SetBounds(Env);
//	}
//	else
//	{
//		m_pSpatialReference = pSpatialReference->Clone();
//	}
//}
//
//OGRSpatialReference* wxGISDisplayTransformation::GetSpatialReference(void)
//{
//	return m_pSpatialReference;
//}
//
//wxPoint* wxGISDisplayTransformation::TransformCoordWorld2DC(OGRRawPoint* pPoints, size_t nPointCount)
//{
//	wxPoint* pResult = new wxPoint[nPointCount];
//	for(size_t i = 0; i < nPointCount; ++i)
//	{
//		pResult[i].x = (int)xWorld2DC(pPoints[i].x);
//		pResult[i].y = (int)yWorld2DC(pPoints[i].y);
//	}
//	return pResult;
//}
//
//void wxGISDisplayTransformation::TransformCoordWorld2DC(OGRRawPoint* pPoints, size_t nPointCount, wxPoint* pResult)
//{
//	if(pResult == NULL)
//		return;
//	for(size_t i = 0; i < nPointCount; ++i)
//	{
//		pResult[i].x = (int)xWorld2DC(pPoints[i].x);
//		pResult[i].y = (int)yWorld2DC(pPoints[i].y);
//	}
//}
//
//OGRRawPoint* wxGISDisplayTransformation::TransformCoordDC2World(wxPoint* pPoints, size_t nPointCount)
//{
//	OGRRawPoint* pResult = new OGRRawPoint[nPointCount];
//	for(size_t i = 0; i < nPointCount; ++i)
//	{
//		pResult[i].x = xDC2World(pPoints[i].x);
//		pResult[i].y = yDC2World(pPoints[i].y);
//	}
//	return pResult;
//}
//
//void wxGISDisplayTransformation::SetPPI(wxSize new_res)
//{
//	m_ppi = new_res;
//}
//
//double wxGISDisplayTransformation::xWorld2DC(double x)
//{
//	return m_DCXDelta - (m_WorldXDelta - x + m_Bounds.MinX) * m_World2DC;
//}
//
//double wxGISDisplayTransformation::yWorld2DC(double y)
//{
//	return m_DCYDelta + (m_WorldYDelta - y + m_Bounds.MinY) * m_World2DC;
//}
//
//double wxGISDisplayTransformation::xDC2World(int x)
//{
//	return m_WorldXDelta + m_Bounds.MinX + (double(x) - m_DCXDelta) / m_World2DC;
//}
//
//double wxGISDisplayTransformation::yDC2World(int y)
//{
//	return m_WorldYDelta + m_Bounds.MinY - (double(y) - m_DCYDelta) / m_World2DC;
//}
//
//OGREnvelope wxGISDisplayTransformation::TransformRect(wxRect rect)
//{
//	OGREnvelope ResEnv;
//	wxPoint LTPoint = rect.GetLeftTop();
//	wxPoint RBPoint = rect.GetRightBottom();
//    RBPoint.x++; RBPoint.y++;
//	ResEnv.MaxX = xDC2World(RBPoint.x);
//	ResEnv.MinX = xDC2World(LTPoint.x);
//	ResEnv.MaxY = yDC2World(LTPoint.y);
//	ResEnv.MinY = yDC2World(RBPoint.y);
//	return ResEnv;
//}

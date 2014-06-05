/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  display transformation. Transform from world to screen coordinates and vice versa
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013 Dmitry Barishnikov
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

#include <wx/xml/xml.h>

#include "ogrsf_frmts.h"

class WXDLLIMPEXP_FWD_GIS_DSP wxGISColor;

WXDLLIMPEXP_GIS_DSP void SetColorValue(wxXmlNode* pNode, const wxString &sAttrName, const wxGISColor &oColor);
WXDLLIMPEXP_GIS_DSP wxGISColor GetColorValue(const wxXmlNode* pNode, const wxString &sAttrName, const wxGISColor &oDefaultColor);

WXDLLIMPEXP_GIS_DSP void RotateEnvelope(OGREnvelope &Env, double dAngle, double dX, double dY);
WXDLLIMPEXP_GIS_DSP void ClipGeometryByEnvelope(OGRRawPoint* pOGRRawPoints, int *pnPointCount, const OGREnvelope &Env, bool shapeOpen);

//Sutherland-Hodgman Polygon Clipping
inline bool IsInsideEnvelope(const OGRRawPoint &pt, wxGISEnumPointPosition nPos, const OGREnvelope &Env)
{
	switch(nPos)
	{
	case enumGISPtPosLeft://XMin
		return (pt.x > Env.MinX);
	case enumGISPtPosRight://XMax
		return (pt.x < Env.MaxX);
	case enumGISPtPosBottom://YMin
		return (pt.y > Env.MinY);
	case enumGISPtPosTop://YMax
		return (pt.y < Env.MaxY);
	}
	return false;
}

inline OGRRawPoint SolveIntersection(const OGRRawPoint &pt1, const OGRRawPoint &pt2, wxGISEnumPointPosition nPos, const OGREnvelope &Env)
{
	OGRRawPoint out;
	double r_n = EPSILON, r_d = EPSILON;

	switch ( nPos )
	{
	case enumGISPtPosRight: // x = MAX_X boundary
		r_n = -( pt1.x - Env.MaxX ) * ( Env.MaxY - Env.MinY );
		r_d = ( pt2.x - pt1.x ) * ( Env.MaxY - Env.MinY );
		break;
	case enumGISPtPosLeft: // x = MIN_X boundary
		r_n = -( pt1.x - Env.MinX ) * ( Env.MaxY - Env.MinY );
		r_d = ( pt2.x - pt1.x ) * ( Env.MaxY - Env.MinY );
		break;
	case enumGISPtPosTop: // y = MAX_Y boundary
		r_n = ( pt1.y - Env.MaxY ) * ( Env.MaxX - Env.MinX );
		r_d = -( pt2.y - pt1.y ) * ( Env.MaxX - Env.MinX );
		break;
	case enumGISPtPosBottom: // y = MIN_Y boundary
		r_n = ( pt1.y - Env.MinY ) * ( Env.MaxX - Env.MinX );
		r_d = -( pt2.y - pt1.y ) * ( Env.MaxX - Env.MinX );
		break;
	}

	if ( fabs( r_d ) > EPSILON && fabs( r_n ) > EPSILON )
	{ // they cross
		double r = r_n / r_d;
		out.x = pt1.x + r * ( pt2.x - pt1.x );
		out.y = pt1.y + r * ( pt2.y - pt1.y );
	}
	else
	{
		// Should never get here, but if we do for some reason, cause a
		// clunk because something else is wrong if we do.
		wxASSERT( fabs( r_d ) > EPSILON && fabs( r_n ) > EPSILON );
	}

	return out;
}

inline void TrimFeatureToBoundary(OGRRawPoint* pOGRRawPointsIn, int nPointCountIn, OGRRawPoint** pOGRRawPointsOut, int *pnPointCountOut, wxGISEnumPointPosition nPos, const OGREnvelope &Env, bool shapeOpen )
{
	// The shapeOpen parameter selects whether this function treats the
	// shape as open or closed. False is appropriate for polygons and
	// true for polylines.

	unsigned int i1 = nPointCountIn - 1; // start with last point

	// and compare to the first point initially.
	for ( int i2 = 0; i2 < nPointCountIn; ++i2 )
	{ // look at each edge of the polygon in turn
		if ( IsInsideEnvelope( pOGRRawPointsIn[i2], nPos, Env ) ) // end point of edge is inside boundary
		{
			if ( IsInsideEnvelope( pOGRRawPointsIn[i1], nPos, Env) )
				(*pOGRRawPointsOut)[(*pnPointCountOut)++] = pOGRRawPointsIn[i2];
			else
			{
				// edge crosses into the boundary, so trim back to the boundary, and
				// store both ends of the new edge
				if ( !( i2 == 0 && shapeOpen ) )
					(*pOGRRawPointsOut)[(*pnPointCountOut)++] = SolveIntersection( pOGRRawPointsIn[i1], pOGRRawPointsIn[i2], nPos, Env );
				(*pOGRRawPointsOut)[(*pnPointCountOut)++] = pOGRRawPointsIn[i2];
			}
		}
		else // end point of edge is outside boundary
		{
			// start point is in boundary, so need to trim back
			if ( IsInsideEnvelope( pOGRRawPointsIn[i1], nPos, Env) )
			{
				if ( !( i2 == 0 && shapeOpen ) )
					(*pOGRRawPointsOut)[(*pnPointCountOut)++] = SolveIntersection( pOGRRawPointsIn[i1], pOGRRawPointsIn[i2], nPos, Env );
			}
		}
		i1 = i2;
	}
}

//DisplayTransformation – This object defines how real-world coordinates are mapped to a output device. Three rectangles define the transformation.
//The Bounds specifies the full extent in real-world coordinates. The VisibleBounds specifies what extent is currently visible.
//And the DeviceFrame specifies where the VisibleBounds appears on the output device. Since the aspect ratio of the DeviceFrame may not always match the aspect ratio of the specified VisibleBounds, the transformation calculates the actual visible bounds that fits the DeviceFrame. This is called the FittedBounds and is in real-world coordinates. All coordinates can be rotated about the center of the visible bounds by simply setting the transformation’s Rotation property.

//class wxGISDisplayTransformation :
//	public IDisplayTransformation
//{
//public:
//	wxGISDisplayTransformation(void);
//	virtual ~wxGISDisplayTransformation(void);
//	virtual void Reset(void);
//	virtual void SetDeviceFrame(wxRect rc);
//	virtual wxRect GetDeviceFrame(void);
//	virtual bool IsBoundsSet(void);
//	virtual void SetBounds(OGREnvelope bounds);
//	virtual OGREnvelope GetBounds(void);
//	virtual OGREnvelope GetVisibleBounds(void);
//	virtual double GetRatio(void);
//	virtual double GetScaleRatio(void);
//	virtual void SetSpatialReference(OGRSpatialReference* pSpatialReference);
//	virtual OGRSpatialReference* GetSpatialReference(void);
//	virtual wxPoint* TransformCoordWorld2DC(OGRRawPoint* pPoints, size_t nPointCount);
//	virtual void TransformCoordWorld2DC(OGRRawPoint* pPoints, size_t nPointCount, wxPoint* pResult);
//	virtual OGRRawPoint* TransformCoordDC2World(wxPoint* pPoints, size_t nPointCount);
//	virtual void SetPPI(wxSize new_res);
//	virtual OGREnvelope TransformRect(wxRect rect);
//protected:
//	// World2DC
//	double xWorld2DC(double x);
//	double yWorld2DC(double y);
//	// DC2World
//	double xDC2World(int x);
//	double yDC2World(int y);
//protected:
//	double	m_World2DC, m_DC2World, m_DCXDelta, m_DCYDelta, m_WorldXDelta, m_WorldYDelta;
//	wxRect m_DeviceFrameRect;
//	OGREnvelope m_Bounds;
//	OGRSpatialReference* m_pSpatialReference;
//	bool m_bIsBoundsSet;
//	wxSize m_ppi;
//};

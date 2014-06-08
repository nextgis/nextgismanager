/******************************************************************************
* Project:  wxGIS
* Purpose:  DrawingLayer header
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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
#include "wxgis/carto/drawinglayer.h"
#include "wxgis/carto/mxevent.h"
#include "wxgis/display/displayop.h"
#include "wxgis/core/pointer.h"

//----------------------------------------------------------------------------
// wxGISShape
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISShape, wxObject)

wxGISShape::wxGISShape(const wxString &sName, const wxGISGeometry &Geom, wxGISEnumShapeType eType, wxGISSymbol* pSymbol)
{
    m_oGeom = Geom;
    m_eType = eType;
    m_pSymbol = pSymbol;
    m_eState = enumGISShapeStateNormal;
    m_sName = sName;
}

wxGISShape::~wxGISShape()
{
    wxDELETE(m_pSymbol);
}

wxGISEnumShapeType wxGISShape::GetType() const
{
    return m_eType;
}

wxGISEnumShapeState wxGISShape::GetState() const
{
    return m_eState;
}

void wxGISShape::Draw(wxGISDisplay *pDisplay)
{
    m_pSymbol->SetupDisplay(pDisplay);
    m_pSymbol->Draw( m_oGeom );
}

OGREnvelope wxGISShape::GetBounds() const
{
    OGREnvelope oReturnBounds;

    switch (m_eType)
    {
    case enumGISShapeTypeRectangle:
    case enumGISShapeTypePolygon:
    case enumGISShapeTypeLine:
    case enumGISShapeTypeEllipse:
    case enumGISShapeTypeMarker:
        oReturnBounds = m_oGeom.GetEnvelope();
        break;
    case enumGISShapeTypeCircle:
    {
        OGRGeometry *pGeom = m_oGeom;

        OGRMultiPoint* pMPT = (OGRMultiPoint*)pGeom;
        OGRPoint* pCenterPt = (OGRPoint*)pMPT->getGeometryRef(0);
        OGRPoint* pOriginPt = (OGRPoint*)pMPT->getGeometryRef(1);
        double dfRadius = sqrt((pCenterPt->getX() - pOriginPt->getX())*(pCenterPt->getX() - pOriginPt->getX()) + (pCenterPt->getY() - pOriginPt->getY())*(pCenterPt->getY() - pOriginPt->getY()));
        oReturnBounds.MaxX = pCenterPt->getX() + dfRadius;
        oReturnBounds.MinX = pCenterPt->getX() - dfRadius;
        oReturnBounds.MaxY = pCenterPt->getY() + dfRadius;
        oReturnBounds.MinY = pCenterPt->getY() - dfRadius;
        break;
    }
#ifdef wxGIS_USE_SPLINE
    case enumGISShapeTypeCurve://TODO:
    case enumGISShapeTypeFreeHand:
        break;
#endif //wxGIS_USE_SPLINE
    case enumGISShapeTypeFreeHand:
        oReturnBounds = m_oGeom.GetEnvelope();
        break;
    };

    return oReturnBounds;
}

//----------------------------------------------------------------------------
// wxGISDrawingLayer
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISDrawingLayer, wxGISLayer)

wxGISDrawingLayer::wxGISDrawingLayer(const wxString &sName, wxGISDataset* pwxGISDataset) : wxGISLayer(sName, pwxGISDataset)
{
    m_pMarkerSymbol = new wxGISSimpleMarkerSymbol(wxGISColor(51, 51, 51, 255), 4);//black point
    m_pLineSymbol = new wxGISSimpleLineSymbol(wxGISColor(51, 51, 51, 255), 0.5);
    m_pFillSymbol = new wxGISSimpleFillSymbol(wxGISColor(190, 255, 190, 255), m_pLineSymbol->Clone());
    m_pCircleSymbol = new wxGISSimpleCircleSymbol(wxGISColor(190, 255, 190, 255), m_pLineSymbol->Clone());
    m_pEllipseSymbol = new wxGISSimpleEllipseSymbol(wxGISColor(190, 255, 190, 255), m_pLineSymbol->Clone());
}

wxGISDrawingLayer::~wxGISDrawingLayer(void)
{
    for (size_t i = 0; i < m_aoShapes.size(); ++i)
    {
        wxDELETE(m_aoShapes[i]);
    }

    wxDELETE(m_pMarkerSymbol);
    wxDELETE(m_pLineSymbol);
    wxDELETE(m_pFillSymbol);
    wxDELETE(m_pCircleSymbol);
    wxDELETE(m_pEllipseSymbol);
}

bool wxGISDrawingLayer::Draw(wxGISEnumDrawPhase DrawPhase, ITrackCancel* const pTrackCancel)
{
    wxCHECK_MSG(m_pDisplay, false, wxT("Display pointer is NULL"));

    wxCriticalSectionLocker lock(m_CritSect);

    OGREnvelope stDisplayExtentRotated = m_pDisplay->GetBounds(true);
    OGREnvelope stFeatureDatasetExtentRotated = m_oLayerExtent;

    //rotate featureclass extent
    if (!IsDoubleEquil(m_pDisplay->GetRotate(), 0.0))
    {
        wxRealPoint dfCenter = m_pDisplay->GetBoundsCenter();
        RotateEnvelope(stFeatureDatasetExtentRotated, m_pDisplay->GetRotate(), dfCenter.x, dfCenter.y);//dCenterX, dCenterY);
    }

    //if envelopes don't intersect exit
    if (!stDisplayExtentRotated.Intersects(stFeatureDatasetExtentRotated))
        return false;

    //get intersect envelope to fill vector data
    OGREnvelope stDrawBounds = stDisplayExtentRotated;
    stDrawBounds.Intersect(stFeatureDatasetExtentRotated);
    if (!stDrawBounds.IsInit())
        return false;

    IProgressor* pProgress = NULL;
    if (NULL != pTrackCancel)
    {
        pProgress = pTrackCancel->GetProgressor();
    }

    if (NULL != pProgress)
    {
        pProgress->SetRange(m_aoShapes.size());
    }

    //draw shapes
    for (size_t i = 0; i < m_aoShapes.size(); ++i)
    {
        if (NULL != pProgress)
        {
            pProgress->SetValue(i);
        }

        if (pTrackCancel != NULL && !pTrackCancel->Continue())
            return true;
        if (m_aoShapes[i] != NULL)
        {
            m_aoShapes[i]->Draw(m_pDisplay);
        }
    }

    //draw enumGISShapeStateSelected

    //draw enumGISShapeStateRotated

    //draw enumGISShapeStatePoints

    return true;
}

bool wxGISDrawingLayer::AddShape(const wxGISGeometry &Geom, wxGISEnumShapeType eType)
{
    if (!Geom.IsOk())
        return false;

    wxGISSymbol* pSymbol = NULL;
    wxString sShapeName;
    switch (eType)
    {
    case enumGISShapeTypeRectangle:
        sShapeName = wxString::Format(_("Rectangle %ld"), m_aoShapes.size() + 1);
        pSymbol = wxStaticCast(m_pFillSymbol->Clone(), wxGISSymbol);
        break;
    case enumGISShapeTypePolygon:
        sShapeName = wxString::Format(_("Polygon %ld"), m_aoShapes.size() + 1);
        pSymbol = wxStaticCast(m_pFillSymbol->Clone(), wxGISSymbol);
        break;
    case enumGISShapeTypeCircle:
        sShapeName = wxString::Format(_("Circle %ld"), m_aoShapes.size() + 1);
        pSymbol = wxStaticCast(m_pCircleSymbol->Clone(), wxGISSymbol);
        break;
    case enumGISShapeTypeEllipse:
        sShapeName = wxString::Format(_("Ellipse %ld"), m_aoShapes.size() + 1);
        pSymbol = wxStaticCast(m_pEllipseSymbol->Clone(), wxGISSymbol);
        break;
    case enumGISShapeTypeLine:
        sShapeName = wxString::Format(_("Line %ld"), m_aoShapes.size() + 1);
        pSymbol = wxStaticCast(m_pLineSymbol->Clone(), wxGISSymbol);
        break;
#ifdef wxGIS_USE_SPLINE
    case enumGISShapeTypeCurve:
        sShapeName = wxString::Format(_("Curve %ld"), m_aoShapes.size() + 1);
        pSymbol = wxStaticCast(m_pLineSymbol->Clone(), wxGISSymbol);
        break;
#endif //wxGIS_USE_SPLINE
    case enumGISShapeTypeFreeHand:
        sShapeName = wxString::Format(_("FreeHand %ld"), m_aoShapes.size() + 1);
        pSymbol = wxStaticCast(m_pLineSymbol->Clone(), wxGISSymbol);
        break;
    case enumGISShapeTypeMarker:
        sShapeName = wxString::Format(_("Marker %ld"), m_aoShapes.size() + 1);
        pSymbol = wxStaticCast(m_pMarkerSymbol->Clone(), wxGISSymbol);
        break;
    default:
        sShapeName = wxString::Format(_("Shape %ld"), m_aoShapes.size() + 1);
        pSymbol = wxStaticCast(m_pFillSymbol->Clone(), wxGISSymbol);
        break;
    };

    wxGISShape * pNewShape = new wxGISShape(sShapeName, Geom, eType, pSymbol);
    m_aoShapes.push_back(pNewShape);

    if (m_oLayerExtent.IsInit())
    {
        m_oLayerExtent.Merge(pNewShape->GetBounds());
    }
    else
    {
        m_oLayerExtent = pNewShape->GetBounds();
    }


    pNewShape->Draw(m_pDisplay);

    wxMxMapViewEvent wxMxMapViewEvent_(wxMXMAP_LAYER_CHANGED, GetId());
    AddEvent(wxMxMapViewEvent_);

    return true;
}

size_t wxGISDrawingLayer::GetShapeCount(void) const
{
    return m_aoShapes.size();
}

wxGISShape* wxGISDrawingLayer::GetShape(size_t nIndex) const
{
    if (nIndex >= m_aoShapes.size())
        return NULL;
    return m_aoShapes[nIndex];
}

void wxGISDrawingLayer::Clear()
{
    for (size_t i = 0; i < m_aoShapes.size(); ++i)
    {
        wxDELETE(m_aoShapes[i]);
    }
    m_aoShapes.clear();
    //clear cache
    m_pDisplay->ClearCache(GetCacheId());

    wxMxMapViewEvent wxMxMapViewEvent_(wxMXMAP_LAYER_CHANGED, GetId());
    AddEvent(wxMxMapViewEvent_);
}

wxGISSymbol* wxGISDrawingLayer::GetSymbol(wxGISEnumShapeType eType)
{
    switch (eType)
    {
    case enumGISShapeTypeRectangle:
        return wxStaticCast(m_pFillSymbol, wxGISSymbol);
    case enumGISShapeTypePolygon:
        return wxStaticCast(m_pFillSymbol, wxGISSymbol);
    case enumGISShapeTypeCircle:
        return wxStaticCast(m_pCircleSymbol, wxGISSymbol);
    case enumGISShapeTypeEllipse:
        return wxStaticCast(m_pEllipseSymbol, wxGISSymbol);
    case enumGISShapeTypeLine:
        return wxStaticCast(m_pLineSymbol, wxGISSymbol);
#ifdef wxGIS_USE_SPLINE
    case enumGISShapeTypeCurve:
        return wxStaticCast(m_pLineSymbol, wxGISSymbol);
#endif //wxGIS_USE_SPLINE
    case enumGISShapeTypeFreeHand:
        return wxStaticCast(m_pLineSymbol, wxGISSymbol);
    case enumGISShapeTypeMarker:
        return wxStaticCast(m_pMarkerSymbol, wxGISSymbol);
    default:
        return NULL;
    };
}

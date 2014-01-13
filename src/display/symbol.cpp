/******************************************************************************
 * Project:  wxGIS
 * Purpose:  symbols for feature renderer
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013 Bishop
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
#include "wxgis/display/symbol.h"

//-------------------------------------------------------------------------------
// wxGISSymbol
//-------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISSymbol, wxObject)

wxGISSymbol::wxGISSymbol()
{
    m_pDisplay = NULL;
    m_Color = wxGISColor::MakeRandom();
}

wxGISSymbol::wxGISSymbol(const wxGISColor& Color)
{
    m_pDisplay = NULL;
    m_Color = Color;
}

wxGISSymbol::~wxGISSymbol()
{
}

wxGISColor wxGISSymbol::GetColor() const
{
    return m_Color;
}

void wxGISSymbol::SetColor(const wxGISColor& Color)
{
    m_Color = Color;
}

void wxGISSymbol::SetupDisplay(wxGISDisplay* const pDisplay)
{
    m_pDisplay = pDisplay;
    
}

//void wxGISSymbol::SetStyleToDisplay()
//{
//    //set colors and etc.
//	//pDisplay->SetColor(enumGISDrawStyleFill, m_stFillColour);
//	//pDisplay->SetColor(enumGISDrawStyleOutline, m_stLineColour);
//	//pDisplay->SetColor(enumGISDrawStylePoint, m_stPointColour);
//}

//-------------------------------------------------------------------------------
// wxGISSimpleLineSymbol
//-------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISSimpleLineSymbol, wxGISSymbol)

wxGISSimpleLineSymbol::wxGISSimpleLineSymbol() : wxGISSymbol()
{
    m_dfWidth = 1.0;
    m_eCup = enumGISLineCupButt;
    m_eJoin = enumGISLineJoinMiter;
    m_dfMiterLimit = 10.0;
}

wxGISSimpleLineSymbol::wxGISSimpleLineSymbol(const wxGISColor& Color, double dfWidth) : wxGISSymbol(Color)
{
    m_dfWidth = dfWidth;
    m_eCup = enumGISLineCupButt;
    m_eJoin = enumGISLineJoinMiter;
    m_dfMiterLimit = 10.0;
}

wxGISSimpleLineSymbol::~wxGISSimpleLineSymbol()
{
}

double wxGISSimpleLineSymbol::GetWidth() const
{
    return m_dfWidth;
}

void wxGISSimpleLineSymbol::SetWidth(double dfWidth)
{
    m_dfWidth = dfWidth;
}

wxGISEnumLineCup wxGISSimpleLineSymbol::GeCup() const
{
    return m_eCup;
}

void wxGISSimpleLineSymbol::SetCup(wxGISEnumLineCup eCup)
{
    m_eCup = eCup;
}

wxGISEnumLineJoin wxGISSimpleLineSymbol::GetJoin() const
{
    return m_eJoin;
}

void wxGISSimpleLineSymbol::SetJoin(wxGISEnumLineJoin eJoin)
{
    m_eJoin = eJoin;
}

double wxGISSimpleLineSymbol::GetMiterLimit() const
{
    return m_dfMiterLimit;
}

void wxGISSimpleLineSymbol::SetMiterLimit(double dfMiterLimit)
{
    m_dfMiterLimit = dfMiterLimit;
}

void wxGISSimpleLineSymbol::Draw(const wxGISGeometry &Geometry, int nLevel)
{
    if(!Geometry.IsOk() || !m_pDisplay)
        return;

    OGRwkbGeometryType eGeomType = wkbFlatten(Geometry.GetType());
    if(eGeomType != wkbLineString && eGeomType != wkbLinearRing && eGeomType != wkbMultiLineString)
        return;
    OGREnvelope Env = Geometry.GetEnvelope();
    if(!m_pDisplay->CanDraw(Env))
        return;

    wxCriticalSectionLocker lock(m_pDisplay->GetLock());

    OGRGeometry *pGeom = Geometry;
    if (eGeomType == wkbMultiLineString)
    {
		OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pGeom;
		for(int i = 0; i < pOGRGeometryCollection->getNumGeometries(); ++i)
			Draw(wxGISGeometry(pOGRGeometryCollection->getGeometryRef(i), false));
    }
    else
    {
        OGRLineString* pLine = (OGRLineString*)pGeom;
        if (!m_pDisplay->CheckDrawAsPoint(pLine, m_dfWidth))
        {
            if(!DrawPreserved(pLine))
		        return;
        }
        SetStyleToDisplay();
        m_pDisplay->Stroke();
    }
}

void wxGISSimpleLineSymbol::SetStyleToDisplay()
{
    //set additional style
    switch(m_eCup)
    {
    case enumGISLineCupSquare:
        m_pDisplay->SetLineCap(CAIRO_LINE_CAP_SQUARE);
        break;
    case enumGISLineCupRound:
        m_pDisplay->SetLineCap(CAIRO_LINE_CAP_ROUND);
        break;
    case enumGISLineCupButt:
	    m_pDisplay->SetLineCap(CAIRO_LINE_CAP_BUTT);
        break;
    }

    switch(m_eJoin)
    {
    case enumGISLineJoinMiter:
        m_pDisplay->SetLineJoin(CAIRO_LINE_JOIN_MITER);
        break;
    case enumGISLineJoinBevel:
        m_pDisplay->SetLineJoin(CAIRO_LINE_JOIN_ROUND);
        break;
    case enumGISLineJoinRound:
	    m_pDisplay->SetLineJoin(CAIRO_LINE_JOIN_BEVEL);
        break;
    }

    m_pDisplay->SetMiterLimit(m_dfMiterLimit);
    m_pDisplay->SetColor(m_Color);
    m_pDisplay->SetLineWidth(m_dfWidth);
}

bool wxGISSimpleLineSymbol::DrawPreserved(const OGRLineString* pLine, bool bIsRing)
{
	int nPointCount = pLine->getNumPoints();
	if(nPointCount < 2)
		return false;
    OGRRawPoint* pOGRRawPoints = new OGRRawPoint[nPointCount * 3];
	pLine->getPoints(pOGRRawPoints);

	return m_pDisplay->DrawLine(pOGRRawPoints, nPointCount, true, 0, 0, bIsRing);
}

//-------------------------------------------------------------------------------
// wxGISSimpleFillSymbol
//-------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISSimpleFillSymbol, wxGISSymbol)

wxGISSimpleFillSymbol::wxGISSimpleFillSymbol() : wxGISSymbol()
{
    m_pLineSymbol = new wxGISSimpleLineSymbol();
    m_pLineSymbol->SetColor(wxGISColor(255, 255, 255, 255)); //set black color
    m_eFillRule = enumGISFillRuleOdd;
}
    
wxGISSimpleFillSymbol::wxGISSimpleFillSymbol(const wxGISColor& Color, wxGISSimpleLineSymbol *pLineSymbol) : wxGISSymbol(Color)
{
    wsSET(m_pLineSymbol, pLineSymbol);
    m_eFillRule = enumGISFillRuleOdd;
}

wxGISSimpleFillSymbol::~wxGISSimpleFillSymbol()
{
    wsDELETE(m_pLineSymbol);
}

wxGISEnumFillRule wxGISSimpleFillSymbol::GetFillRule() const
{
    return m_eFillRule;
}

void wxGISSimpleFillSymbol::SetFillRule(wxGISEnumFillRule eFillRule)
{
    m_eFillRule = eFillRule;
}

wxGISSimpleLineSymbol *wxGISSimpleFillSymbol::GetSimpleLineSymbol() const
{
    return m_pLineSymbol;
}

void wxGISSimpleFillSymbol::SetSimpleLineSymbol(wxGISSimpleLineSymbol *pLineSymbol)
{
    m_pLineSymbol = pLineSymbol; 
}

void wxGISSimpleFillSymbol::Draw(const wxGISGeometry &Geometry, int nLevel)
{
    if(!Geometry.IsOk() || !m_pDisplay)
        return;

    OGRwkbGeometryType eGeomType = wkbFlatten(Geometry.GetType());
    if(eGeomType != wkbPolygon && eGeomType != wkbMultiPolygon)
        return;

    OGREnvelope Env = Geometry.GetEnvelope();
    if(!m_pDisplay->CanDraw(Env))
        return;

    wxCriticalSectionLocker lock(m_pDisplay->GetLock());

	if(!m_pDisplay->CheckDrawAsPoint(Env, m_pLineSymbol->GetWidth()))
    {
        OGRGeometry *pGeom = Geometry;

        if(eGeomType == wkbMultiPolygon)
        {
		    OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pGeom;
		    for(int i = 0; i < pOGRGeometryCollection->getNumGeometries(); ++i)
			    Draw(wxGISGeometry(pOGRGeometryCollection->getGeometryRef(i), false));
            return;
        }

        OGRPolygon* pPolygon = (OGRPolygon*)pGeom;
        const OGRLinearRing *pRing = pPolygon->getExteriorRing();
        if(!m_pLineSymbol->DrawPreserved((OGRLineString*)pRing, true))
        {
            return;
        }

	    int nNumInteriorRings = pPolygon->getNumInteriorRings();
	    for(int nPart = 0; nPart < nNumInteriorRings; ++nPart)
	    {
		    pRing = pPolygon->getInteriorRing(nPart);
            OGREnvelope IntEnv;
            pRing->getEnvelope(&IntEnv);
		    if(!m_pDisplay->CheckDrawAsPoint(IntEnv, m_pLineSymbol->GetWidth()))
            {
			    m_pLineSymbol->DrawPreserved((OGRLineString*)pRing, true);
            }
	    }

        if(m_Color.Alpha() > 0)
        {
            switch(m_eFillRule)
            {
            case enumGISFillRuleWinding:
	            m_pDisplay->SetFillRule( CAIRO_FILL_RULE_WINDING );
                break;
            case enumGISFillRuleOdd:
	            m_pDisplay->SetFillRule( CAIRO_FILL_RULE_EVEN_ODD );
                break;
            }
	        m_pDisplay->SetColor(m_Color);
	        m_pDisplay->FillPreserve();
        }

    }

    m_pLineSymbol->SetStyleToDisplay();

	m_pDisplay->Stroke();
}

void wxGISSimpleFillSymbol::SetupDisplay(wxGISDisplay* const pDisplay)
{
    m_pLineSymbol->SetupDisplay(pDisplay);
    wxGISSymbol::SetupDisplay(pDisplay);
}

//-------------------------------------------------------------------------------
// wxGISSimpleMarkerSymbol
//-------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISSimpleMarkerSymbol, wxGISSymbol)

wxGISSimpleMarkerSymbol::wxGISSimpleMarkerSymbol() : wxGISSymbol()
{
    m_dfSize = 10;
    m_dfOutlineSize = 0;
}

wxGISSimpleMarkerSymbol::wxGISSimpleMarkerSymbol(const wxGISColor& Color, double dfSize) : wxGISSymbol(Color)
{
    m_dfSize = dfSize;
    m_dfOutlineSize = 0;
}

wxGISSimpleMarkerSymbol::~wxGISSimpleMarkerSymbol()
{
}

void wxGISSimpleMarkerSymbol::Draw(const wxGISGeometry &Geometry, int nLevel)
{
    if(!Geometry.IsOk() ||!m_pDisplay)
        return;

    OGRwkbGeometryType eGeomType = wkbFlatten(Geometry.GetType());
    if(eGeomType != wkbPoint && eGeomType != wkbMultiPoint)
        return;

    OGREnvelope Env = Geometry.GetEnvelope();
    if(!m_pDisplay->CanDraw(Env))
        return;

    OGRGeometry *pGeom = Geometry;

    if(eGeomType == wkbMultiPoint)
    {
		OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)pGeom;
		for(int i = 0; i < pOGRGeometryCollection->getNumGeometries(); ++i)
			Draw(wxGISGeometry(pOGRGeometryCollection->getGeometryRef(i), false));
        return;
    }

    wxCriticalSectionLocker lock(m_pDisplay->GetLock());

    OGRPoint* pPoint = (OGRPoint*)pGeom;
	if(m_dfOutlineSize)
	{
		if(!m_pDisplay->DrawPointFast(pPoint->getX(), pPoint->getY()))
        {
			return;
        }
        m_pDisplay->SetColor(m_OutlineColor);
		m_pDisplay->SetLineWidth( m_dfSize + m_dfOutlineSize + m_dfOutlineSize);
        m_pDisplay->Stroke();
	}
	 
	if(!m_pDisplay->DrawPointFast(pPoint->getX(), pPoint->getY()))
    {
		return;
    }

    m_pDisplay->SetColor(m_Color);
	m_pDisplay->SetLineWidth( m_dfSize );
    m_pDisplay->SetLineCap(CAIRO_LINE_CAP_ROUND);
    m_pDisplay->Stroke();
}

void wxGISSimpleMarkerSymbol::SetSize(double dfSize)
{
    m_dfSize = dfSize;
}

double wxGISSimpleMarkerSymbol::GetSize() const
{
    return m_dfSize;
}

void wxGISSimpleMarkerSymbol::SetOutlineColor(const wxGISColor &Color)
{
    m_OutlineColor = Color;
}

wxGISColor wxGISSimpleMarkerSymbol::GetOutlineColor() const
{
    return m_OutlineColor;
}

void wxGISSimpleMarkerSymbol::SetOutlineSize(double dfSize)
{
    m_dfOutlineSize = dfSize;
}

double wxGISSimpleMarkerSymbol::GetOutlineSize() const
{
    return m_dfOutlineSize;
}

//-------------------------------------------------------------------------------
// wxGISSimpleCollectionSymbol
//-------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISSimpleCollectionSymbol, wxGISSymbol)

wxGISSimpleCollectionSymbol::wxGISSimpleCollectionSymbol() : wxGISSymbol()
{
    m_pMarkerSymbol = NULL;
    m_pLineSymbol = NULL;
    m_pFillSymbol = NULL;
}

wxGISSimpleCollectionSymbol::wxGISSimpleCollectionSymbol(const wxGISColor& Color, wxGISSimpleMarkerSymbol* pMarkerSymbol, wxGISSimpleLineSymbol* pLineSymbol, wxGISSimpleFillSymbol* pFillSymbol) : wxGISSymbol(Color)
{
    m_pMarkerSymbol = pMarkerSymbol;
    m_pLineSymbol = pLineSymbol;
    m_pFillSymbol = pFillSymbol;
}

wxGISSimpleCollectionSymbol::~wxGISSimpleCollectionSymbol()
{
}

void wxGISSimpleCollectionSymbol::Draw(const wxGISGeometry &Geometry, int nLevel)
{
    if(!Geometry.IsOk() ||!m_pDisplay)
        return;

    if (!Geometry.IsOk())
        return;

    switch (wkbFlatten(Geometry.GetType()))
    {
    case wkbMultiPoint:
    case wkbPoint:
        m_pMarkerSymbol->Draw(Geometry);
        break;
    case wkbMultiPolygon:
    case wkbPolygon:
        m_pFillSymbol->Draw(Geometry);
        break;
    case wkbMultiLineString:
    case wkbLineString:
        m_pLineSymbol->Draw(Geometry);
        break;
    case wkbGeometryCollection:
        {
            OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)Geometry.operator OGRGeometry *();
            for (int i = 0; i < pOGRGeometryCollection->getNumGeometries(); ++i)
            {
                wxGISGeometry CollectionGeom(pOGRGeometryCollection->getGeometryRef(i), false);
                Draw(CollectionGeom, nLevel);
            }
        }
        break;
    case wkbLinearRing:
    case wkbUnknown:
    case wkbNone:
    default:
        break;
    }
}

void wxGISSimpleCollectionSymbol::SetupDisplay(wxGISDisplay* const pDisplay)
{
    wxGISSymbol::SetupDisplay(pDisplay);
    if (NULL != m_pMarkerSymbol)
    {
        m_pMarkerSymbol->SetupDisplay(pDisplay);
    }
    
    if (NULL != m_pLineSymbol)
    {
        m_pLineSymbol->SetupDisplay(pDisplay);
    }
    if (NULL != m_pFillSymbol)
    {
        m_pFillSymbol->SetupDisplay(pDisplay);
    }

}
/*

			{
				wxGISGeometry Geom = pItem->GetGeometry();
                if(!Geom.IsOk() || pItem->GetOID() == wxNOT_FOUND)
					break;
				OGRwkbGeometryType type = wkbFlatten(Geom.GetType());
				switch(type)
				{
				case wkbPoint:
				case wkbMultiPoint:
					pDisplay->SetLineCap(CAIRO_LINE_CAP_ROUND);
					pDisplay->SetLineWidth(m_dWidth);
					pDisplay->SetPointRadius(m_dRadius);
					break;
				case wkbLineString:
				case wkbLinearRing:
				case wkbMultiLineString:
					pDisplay->SetLineCap(CAIRO_LINE_CAP_BUTT);
					pDisplay->SetLineWidth(m_dWidth);
					break;
				case wkbMultiPolygon:
				case wkbPolygon:
					pDisplay->SetLineCap(CAIRO_LINE_CAP_BUTT);
					pDisplay->SetLineWidth(m_dWidth);
					break;
				case wkbGeometryCollection:
					break;
				default:
				case wkbUnknown:
				case wkbNone:
					break;
				}

				pDisplay->DrawGeometry(Geom);

void wxGISDisplay::SetSymbol(const wxGISSymbol *pSymbol)
{
	OGRwkbGeometryType type = wkbFlatten(Geometry.GetType());
	switch(type)
	{
	case wkbPoint:
	case wkbMultiPoint:
	    m_pGISDisplay->SetColor(enumGISDrawStylePoint, stLineColour);
	    m_pGISDisplay->SetColor(enumGISDrawStyleOutline, stLineColour);
	    m_pGISDisplay->SetColor(enumGISDrawStyleFill, stFillColour);
		m_pGISDisplay->SetLineCap(CAIRO_LINE_CAP_ROUND);
		m_pGISDisplay->SetLineWidth(dfLineWidth);
		m_pGISDisplay->SetPointRadius(2.0);
		break;
	case wkbLineString:
	case wkbLinearRing:
	case wkbMultiLineString:
	    m_pGISDisplay->SetColor(enumGISDrawStylePoint, stFillColour);
	    m_pGISDisplay->SetColor(enumGISDrawStyleOutline, stFillColour);
	    m_pGISDisplay->SetColor(enumGISDrawStyleFill, stFillColour);
		m_pGISDisplay->SetLineCap(CAIRO_LINE_CAP_BUTT);
		m_pGISDisplay->SetLineWidth(2.0);
		break;
	case wkbMultiPolygon:
	case wkbPolygon:
	    m_pGISDisplay->SetColor(enumGISDrawStyleOutline, stLineColour);
	    m_pGISDisplay->SetColor(enumGISDrawStyleFill, stFillColour);
		m_pGISDisplay->SetLineCap(CAIRO_LINE_CAP_BUTT);
		m_pGISDisplay->SetLineWidth(dfLineWidth);
		break;
	case wkbGeometryCollection:
		break;
	default:
	case wkbUnknown:
	case wkbNone:
		break;
	}
}

void wxGISDisplay::DrawGeometry(OGRGeometry* poGeometry)
{
    if(!poGeometry)
        return;
	wxCriticalSectionLocker locker(m_CritSect);

	OGRwkbGeometryType type = wkbFlatten(poGeometry->getGeometryType());
	switch(type)
	{
	case wkbPoint:
		if(m_dLineWidth)
		{
			SetColor(m_PointColour);
			cairo_set_line_width(m_saLayerCaches[m_nCurrentLayer].pCairoContext, m_dPointRadius + m_dPointRadius + m_dLineWidth);
			if(!DrawPointFast((OGRPoint*)poGeometry))
				break;
			cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
			cairo_set_line_width(m_saLayerCaches[m_nCurrentLayer].pCairoContext, m_dPointRadius +m_dPointRadius);
		}
		SetColor(m_FillColour);
		if(!DrawPointFast((OGRPoint*)poGeometry))
			return;
		cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
		break;
	case wkbLineString:
		cairo_set_line_width(m_saLayerCaches[m_nCurrentLayer].pCairoContext, m_dLineWidth);
		//check if line is too small
		if(!CheckDrawAsPoint(poGeometry))
			if(!DrawLine((OGRLineString*)poGeometry))
				break;
		SetColor(m_LineColour);
		cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
		break;
	case wkbLinearRing:
		cairo_set_line_width(m_saLayerCaches[m_nCurrentLayer].pCairoContext, m_dLineWidth);
		//check if line is too small
		if(!CheckDrawAsPoint(poGeometry))
			if(!DrawRing((OGRLinearRing*)poGeometry))
				break;
		SetColor(m_LineColour);
		cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
		break;
	case wkbPolygon:
		cairo_set_line_width(m_saLayerCaches[m_nCurrentLayer].pCairoContext, m_dLineWidth);
		//check if line is too small
		if(!CheckDrawAsPoint(poGeometry))
			if(!DrawPolygon((OGRPolygon*)poGeometry))
				break;
		SetColor(m_LineColour);
		cairo_stroke (m_saLayerCaches[m_nCurrentLayer].pCairoContext);
		break;
	case wkbMultiPolygon:
	case wkbMultiPoint:
	case wkbMultiLineString:
	case wkbGeometryCollection:
		{
		OGRGeometryCollection* pOGRGeometryCollection = (OGRGeometryCollection*)poGeometry;
		for(int i = 0; i < pOGRGeometryCollection->getNumGeometries(); ++i)
			DrawGeometry(pOGRGeometryCollection->getGeometryRef(i));
		}
		break;
	default:
	case wkbUnknown:
	case wkbNone:
		break;
	}
}
*/
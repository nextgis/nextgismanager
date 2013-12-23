/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISSimpleRenderer class.
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
#include "wxgis/carto/featurerenderer.h"
#include "wxgis/carto/featurelayer.h"
#include "wxgis/display/displayop.h"

//-----------------------------------------------------------------------------
// wxGISFeatureRenderer
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISFeatureRenderer, wxGISRenderer)

wxGISFeatureRenderer::wxGISFeatureRenderer(wxGISLayer* pwxGISLayer) : wxGISRenderer(pwxGISLayer)
{
    m_pwxGISFeatureLayer = wxDynamicCast(m_pwxGISLayer, wxGISFeatureLayer);

    if(!m_pwxGISFeatureLayer)
        return;
    
    wxGISSymbol* pSymbol = NULL;

    OGRwkbGeometryType type = wkbFlatten(m_pwxGISFeatureLayer->GetGeometryType());
	switch(type)
	{
	case wkbMultiPoint:
	case wkbPoint:
        {
            wxGISSimpleMarkerSymbol *pMarkerSymbol = new wxGISSimpleMarkerSymbol(wxGISColor::MakeRandom(), 4);
            pMarkerSymbol->SetOutlineColor(wxGISColor(0,0,0,255));
            pMarkerSymbol->SetOutlineSize(0.5);
            pSymbol = wxStaticCast(pMarkerSymbol, wxGISSymbol);
        }
		break;
	case wkbMultiPolygon:
	case wkbPolygon:
        {
            wxGISSimpleLineSymbol *pLineSymbol = new wxGISSimpleLineSymbol(wxGISColor(0,0,0,255), 0.5);
            pSymbol = wxStaticCast(new wxGISSimpleFillSymbol(wxGISColor::MakeRandom(), pLineSymbol), wxGISSymbol);
        }
        break;
	case wkbMultiLineString:
    case wkbLineString:
        {
            wxGISSimpleLineSymbol *pLineSymbol = new wxGISSimpleLineSymbol(wxGISColor(0,0,0,255), 0.5);
            pSymbol = wxStaticCast(pLineSymbol, wxGISSymbol);
        }
        break;
	case wkbUnknown:
	case wkbGeometryCollection:
        {
            wxGISSimpleLineSymbol *pLineSymbol = new wxGISSimpleLineSymbol(wxGISColor(0,0,0,255), 0.5);
            wxGISSimpleCollectionSymbol *pCollectionSymbol = new wxGISSimpleCollectionSymbol(wxGISColor(0,0,0,0), new wxGISSimpleMarkerSymbol(wxGISColor::MakeRandom(), 4), pLineSymbol, new wxGISSimpleFillSymbol(wxGISColor::MakeRandom(), pLineSymbol));
            pSymbol = wxStaticCast(pCollectionSymbol, wxGISSymbol);
        }
        break;
	case wkbLinearRing:
	case wkbNone:
	default:
		break;
	}

    SetSymbol(pSymbol);
}

wxGISFeatureRenderer::~wxGISFeatureRenderer(void)
{
    wsDELETE(m_pSymbol);
}

void wxGISFeatureRenderer::SetSymbol(wxGISSymbol *pSymbol)
{
    wsSET(m_pSymbol, pSymbol);
}

bool wxGISFeatureRenderer::CanRender(wxGISLayer* const pwxGISLayer) const
{
    wxCHECK_MSG(pwxGISLayer, false, wxT("Input dataset pointer is NULL"));
	return pwxGISLayer->GetType() == enumGISFeatureDataset ? true : false;
}

bool wxGISFeatureRenderer::Draw(wxGISEnumDrawPhase DrawPhase, wxGISDisplay* const pDisplay, ITrackCancel* const pTrackCancel)
{
    wxCHECK_MSG(pDisplay && m_pwxGISFeatureLayer, false, wxT("Display or FeatureDataset pointer is NULL"));

    wxCriticalSectionLocker lock(m_CritSect);

    OGREnvelope stFeatureDatasetExtent = m_pwxGISFeatureLayer->GetEnvelope();
	OGREnvelope stDisplayExtentRotated = pDisplay->GetBounds(true);
	OGREnvelope stFeatureDatasetExtentRotated = stFeatureDatasetExtent;

	//rotate featureclass extent
	if(!IsDoubleEquil(pDisplay->GetRotate(), 0.0))
	{
        wxRealPoint dfCenter = pDisplay->GetBoundsCenter();
		RotateEnvelope(stFeatureDatasetExtentRotated, pDisplay->GetRotate(), dfCenter.x, dfCenter.y);//dCenterX, dCenterY);
	}

	//if envelopes don't intersect exit
    if(!stDisplayExtentRotated.Intersects(stFeatureDatasetExtentRotated))
        return false;

	//get intersect envelope to fill vector data
	OGREnvelope stDrawBounds = stDisplayExtentRotated;
	stDrawBounds.Intersect(stFeatureDatasetExtentRotated);
	if(!stDrawBounds.IsInit())
		return false;

    bool bAllFeatures = stDrawBounds.Contains(stFeatureDatasetExtent) == 0 ? false : true;
	//if(!stFeatureDatasetExtent.Contains(stDrawBounds))
	//	stDrawBounds = stFeatureDatasetExtent;

    wxGISSpatialTreeCursor Cursor;
	if(bAllFeatures)
	{
		Cursor = m_pwxGISFeatureLayer->SearchGeometry();
	}
	else
	{
		Cursor = m_pwxGISFeatureLayer->SearchGeometry(stDrawBounds);
	}

    Draw(Cursor, DrawPhase, pDisplay, pTrackCancel);

    return true;
}

void wxGISFeatureRenderer::Draw(const wxGISSpatialTreeCursor& Cursor, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel)
{
    wxCHECK_RET(m_pSymbol, wxT("No symbol set"));

    m_pSymbol->SetupDisplay(pDisplay);

    int nCounter = 0;
    IProgressor* pProgress = NULL;
    if (NULL != pTrackCancel)
    {
        pProgress = pTrackCancel->GetProgressor();
        pProgress->SetRange(Cursor.size());
    }


    wxGISSpatialTreeCursor::const_iterator iter;
    for(iter = Cursor.begin(); iter != Cursor.end(); ++iter)
    {
        if (NULL != pProgress)
            pProgress->SetValue(nCounter++);

        wxGISSpatialTreeData *current = *iter;
        if(!current)
            continue;

        switch(DrawPhase)
		{
		case wxGISDPGeography:
            m_pSymbol->Draw(current->GetGeometry());
			break;
		case wxGISDPAnnotation:
			break;
		case wxGISDPSelection:
			break;
		default:
			break;
		}

		if(pTrackCancel && !pTrackCancel->Continue())
			break;
	}
}


//-----------------------------------------------------------------------------
// wxGISUniqueValueRenderer
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISUniqueValueRenderer, wxGISFeatureRenderer)

wxGISUniqueValueRenderer::wxGISUniqueValueRenderer(wxGISLayer* pwxGISLayer) : wxGISFeatureRenderer(pwxGISLayer)
{
}

wxGISUniqueValueRenderer::~wxGISUniqueValueRenderer(void)
{
    for( wxGISUniqueValuesSymbolMap::iterator it = m_omSymbols.begin(); it != m_omSymbols.end(); ++it )
        wsDELETE( it->second );
}

void wxGISUniqueValueRenderer::AddValue(int nField, const wxString &sValue, wxGISSymbol* const Symbol)
{
    m_naCheckFields.Add(nField);
    wsSET(m_omSymbols[wxString::Format(wxT("%d_%s"), nField, sValue)], Symbol);
}

void wxGISUniqueValueRenderer::SetSymbol(wxGISSymbol *pSymbol)
{
    wsSET(m_omSymbols[wxT("default")], pSymbol);
}

void wxGISUniqueValueRenderer::Draw(const wxGISSpatialTreeCursor& Cursor, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel)
{
    //TODO: check if dataset has filter with zero attribute fields

    wxGISSymbol* pDefaultSymbol = m_omSymbols[wxT("default")];
    if(pDefaultSymbol)
        pDefaultSymbol->SetupDisplay(pDisplay);

    int nCounter = 0;
    IProgressor* pProgress = NULL;
    if (NULL != pTrackCancel)
    {
        pProgress = pTrackCancel->GetProgressor();
        pProgress->SetRange(Cursor.size());
    }

    wxGISSpatialTreeCursor::const_iterator iter;
    for(iter = Cursor.begin(); iter != Cursor.end(); ++iter)
    {    
        if (NULL != pProgress)
            pProgress->SetValue(nCounter++);

        wxGISSpatialTreeData *current = *iter;

        if(!current)
            continue;

        switch(DrawPhase)
		{
		case wxGISDPGeography:
            {
            //search style
                wxGISFeature Feature = m_pwxGISFeatureLayer->GetFeatureByID(current->GetFID());
                if(Feature.IsOk())
                {
                    wxGISSymbol *pSymbol = NULL;
                    for(size_t j = 0; j < m_naCheckFields.GetCount(); ++j)
                    {
                        wxString sCmp = wxString::Format(wxT("%d_%s"), m_naCheckFields[j], Feature.GetFieldAsString( m_naCheckFields[j] ));
                        pSymbol = m_omSymbols[sCmp];
                        if(pSymbol)
                            break;
                    }
                    //draw
                    if(pSymbol)
                    {
                        pSymbol->SetupDisplay(pDisplay);
                        pSymbol->Draw(current->GetGeometry());
                    }
                    else if(pDefaultSymbol)
                    {
                        //draw default
                        pDefaultSymbol->Draw(current->GetGeometry());
                    }
			    }
            }
			break;
		case wxGISDPAnnotation:
			break;
		case wxGISDPSelection:
			break;
		default:
			break;
		}

		if(pTrackCancel && !pTrackCancel->Continue())
			break;
	}
}

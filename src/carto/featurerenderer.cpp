/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISSimpleRenderer class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013 Bishop
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
#include "wxgis/carto/featurerenderer.h"
#include "wxgis/carto/featurelayer.h"
#include "wxgis/display/displayop.h"

//-----------------------------------------------------------------------------
// wxGISFeatureRenderer
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISFeatureRenderer, wxGISRenderer)

wxGISFeatureRenderer::wxGISFeatureRenderer(wxGISLayer* pwxGISLayer) : wxGISRenderer(pwxGISLayer)
{
    m_pSymbol = NULL;
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
    if (m_pSymbol != NULL)
        m_pSymbol->Release();
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
    }

    if (NULL != pProgress)
    {
        pProgress->SetRange(Cursor.size());
    }

    wxGISSpatialTreeCursor::const_iterator iter;
    for(iter = Cursor.begin(); iter != Cursor.end(); ++iter)
    {
        if (NULL != pProgress)
        {
            pProgress->SetValue(nCounter++);
        }

        if (m_pSymbol == NULL)
            break;

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

bool wxGISFeatureRenderer::Apply(ITrackCancel* const pTrackCancel)
{
    return true;
}

void wxGISFeatureRenderer::FeatureChanged(const wxGISFeature &Feature)
{

}

//-----------------------------------------------------------------------------
// wxGISUniqueValueRenderer
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISUniqueValueRenderer, wxGISFeatureRenderer)

wxGISUniqueValueRenderer::wxGISUniqueValueRenderer(wxGISLayer* pwxGISLayer) : wxGISFeatureRenderer(pwxGISLayer)
{
    m_pwxGISFeatureDataset = wxDynamicCast(m_pwxGISFeatureLayer->GetDataset(), wxGISFeatureDataset);
    if (m_pwxGISFeatureDataset)
    {
        m_pwxGISFeatureDataset->Reference();
    }
}

wxGISUniqueValueRenderer::~wxGISUniqueValueRenderer(void)
{
    for( wxGISUniqueValuesSymbolMap::iterator it = m_omSymbols.begin(); it != m_omSymbols.end(); ++it )
        wsDELETE( it->second );

    for (size_t i = 0; i < m_astUniqueValues.size(); ++i)
    {
        wsDELETE(m_astUniqueValues[i].Symbol);
    }
    wsDELETE(m_pwxGISFeatureDataset);
}

void wxGISUniqueValueRenderer::AddValue(int nField, const wxString &sValue, wxGISSymbol* const Symbol)
{
    UNIQ_VALUE val;
    val.sField = m_pwxGISFeatureDataset->GetFieldName(nField);
    val.sValue = sValue;

    //Check if pair field <-> value exist
    for (size_t i = 0; i < m_astUniqueValues.size(); ++i)
    {
        if (m_astUniqueValues[i].sField == val.sField && m_astUniqueValues[i].sValue == val.sValue)
            return;
    }
    wsSET(val.Symbol, Symbol);
    m_astUniqueValues.push_back(val);
}


bool wxGISUniqueValueRenderer::Apply(ITrackCancel* const pTrackCancel)
{
    for (wxGISUniqueValuesSymbolMap::iterator it = m_omSymbols.begin(); it != m_omSymbols.end(); ++it)
        wsDELETE(it->second);
    m_omSymbols.clear();

    long nFeaturesCount = m_pwxGISFeatureDataset->GetFeatureCount(TRUE, pTrackCancel);
    if (nFeaturesCount == 0)
    {
        return true;     // success
    }

    IProgressor* pProgress(NULL);
    if (pTrackCancel)
    {
        pTrackCancel->Reset();
        pTrackCancel->PutMessage(wxString(_("Apply renderer for ")) + m_pwxGISFeatureLayer->GetName(), -1, enumGISMessageInfo);
        pProgress = pTrackCancel->GetProgressor();
    }


    if (pProgress)
    {
        pProgress->SetRange(nFeaturesCount);
    } 
    
    //set needed fields
    wxArrayString saIgnoredFields = m_pwxGISFeatureDataset->GetFieldNames();
    saIgnoredFields.Add(wxT("OGR_STYLE"));
    saIgnoredFields.Add(wxT("GEOMETRY"));

    for (size_t i = 0; i < m_astUniqueValues.size(); ++i)
    {
        int nPos = saIgnoredFields.Index(m_astUniqueValues[i].sField, false);
        if (nPos != wxNOT_FOUND)
        {
            saIgnoredFields.RemoveAt(nPos);
        }
    }

    m_pwxGISFeatureDataset->SetIgnoredFields(saIgnoredFields);
    m_pwxGISFeatureDataset->Reset();

    //get all features
    wxGISFeature Feature;
    while ((Feature = m_pwxGISFeatureDataset->Next()).IsOk())
    {
        for (size_t j = 0; j < m_astUniqueValues.size(); ++j)
        {
            if (Feature.GetFieldAsString(m_astUniqueValues[j].sField) == m_astUniqueValues[j].sValue)
            {
                wsSET(m_omSymbols[Feature.GetFID()], m_astUniqueValues[j].Symbol);
                break;
            }
        }
    }

    saIgnoredFields.Clear();
    m_pwxGISFeatureDataset->SetIgnoredFields(saIgnoredFields);

    return true;
}

void wxGISUniqueValueRenderer::SetSymbol(wxGISSymbol *pSymbol)
{
    wsSET(m_pSymbol, pSymbol);//default
}

void wxGISUniqueValueRenderer::Draw(const wxGISSpatialTreeCursor& Cursor, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel)
{
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
                wxGISUniqueValuesSymbolMap::iterator it = m_omSymbols.find(current->GetFID());
                if (it != m_omSymbols.end())
                {
                    wxGISSymbol *pSymbol = it->second;
                    if (NULL != pSymbol)
                    {
                        pSymbol->SetupDisplay(pDisplay);
                        pSymbol->Draw(current->GetGeometry());
                        continue;
                    }
                }

                if (NULL != m_pSymbol)
                {
                    m_pSymbol->SetupDisplay(pDisplay);
                    m_pSymbol->Draw(current->GetGeometry());
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

void wxGISUniqueValueRenderer::FeatureChanged(const wxGISFeature &Feature)
{
    for (size_t i = 0; i < m_astUniqueValues.size(); ++i)
    {
        if (Feature.GetFieldAsString(m_astUniqueValues[i].sField) == m_astUniqueValues[i].sValue)
        {
            if (m_omSymbols[Feature.GetFID()] != NULL)
                m_omSymbols[Feature.GetFID()]->Release();

            wsSET(m_omSymbols[Feature.GetFID()], m_astUniqueValues[i].Symbol);
            break;
        }
    }
}
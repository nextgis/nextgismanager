/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISMap class.
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
#include "wxgis/carto/map.h"

#include "wxgis/datasource/vectorop.h"

//--------------------------------------------------------------------------------
// wxGISMap
//--------------------------------------------------------------------------------

wxGISMap::wxGISMap(void)
{
	m_sMapName = wxString(_("new map"));
    m_pGISDisplay = NULL;
	m_bFullExtIsInit = false;
    m_nIdCounter = 0;
}

wxGISMap::~wxGISMap(void)
{
	Clear();
}

bool wxGISMap::AddLayer(wxGISLayer* pLayer)
{
    wxCHECK_MSG(pLayer, false, wxT("Layer pointer is NULL"));
	wxCriticalSectionLocker locker(m_CritSect);

    if(!m_SpatialReference.IsOk())
    {
        m_SpatialReference = pLayer->GetSpatialReference();
        //set default map spatial reference to wgs84
		if(!m_SpatialReference.IsOk())
		{
			OGREnvelope Env = pLayer->GetEnvelope();
			if(Env.IsInit())
			{
				if(Env.MaxX <= ENVMAX_X && Env.MaxY <= ENVMAX_Y && Env.MinX >= ENVMIN_X && Env.MinY >= ENVMIN_Y)
				{
					m_SpatialReference = wxGISSpatialReference(new OGRSpatialReference(SRS_WKT_WGS84));
				}
			}
		}
    }
	else
	{
		wxGISSpatialReference InputSpatialReference = pLayer->GetSpatialReference();
		if(!m_SpatialReference.IsSame(InputSpatialReference))
			pLayer->SetSpatialReference(m_SpatialReference);
	}
	//recalc full  envelope
	OGREnvelope Env = pLayer->GetEnvelope();
	if(Env.IsInit())
	{
    	if(m_bFullExtIsInit)
	    	m_FullExtent.Merge(Env);
	    else
	    {
		    m_FullExtent = Env;
		    m_bFullExtIsInit = true;
	    }
    }

    pLayer->SetDisplay(m_pGISDisplay);
    pLayer->SetId(m_nIdCounter++);
	m_paLayers.push_back(pLayer);
	return true;
}

void wxGISMap::Clear(void)
{
	wxCriticalSectionLocker locker(m_CritSect);
	for (size_t i = 0; i < m_paLayers.size(); ++i)
		wxDELETE(m_paLayers[i]);
    m_paLayers.clear();

    m_SpatialReference = wxNullSpatialReference;
	m_FullExtent.MaxX = ENVMAX_X;
	m_FullExtent.MinX = ENVMIN_X;
	m_FullExtent.MaxY = ENVMAX_Y;
	m_FullExtent.MinY = ENVMIN_Y;
	m_bFullExtIsInit = false;
}

OGREnvelope wxGISMap::GetFullExtent(void) const
{
	OGREnvelope OutputEnv = m_FullExtent;
    //increase 10%
	IncreaseEnvelope(OutputEnv, 0.1);
	return OutputEnv;
}

void wxGISMap::SetSpatialReference(const wxGISSpatialReference &SpatialReference)
{
    if(m_SpatialReference.IsOk() && m_SpatialReference->IsSame(SpatialReference))
		return;
    
	for(size_t i = 0; i < m_paLayers.size(); ++i)
		m_paLayers[i]->SetSpatialReference(SpatialReference);
	m_SpatialReference = SpatialReference;
}

wxGISSpatialReference wxGISMap::GetSpatialReference(void) const
{
	return m_SpatialReference;
}

wxGISLayer* const wxGISMap::GetLayerByIndex(size_t nIndex)
{
	wxCriticalSectionLocker locker(m_CritSect);
	if(nIndex >= m_paLayers.size())
        return NULL;
	return m_paLayers[nIndex];
}

wxGISLayer* const wxGISMap::GetLayerById(short nId)
{
    wxCriticalSectionLocker locker(m_CritSect);
    for (size_t i = 0; i < m_paLayers.size(); ++i)
    {
        if (m_paLayers[i]->GetId() == nId)
            return m_paLayers[i];
    }
    return NULL;
}

void wxGISMap::ChangeLayerOrder(size_t nOldIndex, size_t nNewIndex)
{
    if (nOldIndex == nNewIndex)
        return;
    //bool bAddToIndex = nNewIndex > nOldIndex;
    wxVector<wxGISLayer*>::iterator IT = m_paLayers.begin();
    std::advance(IT, nOldIndex);
    //remove layer from array
    wxGISLayer* pLayer = *IT;// m_paLayers[nOldIndex];
    m_paLayers.erase(IT);

    //insert layer to new index
    //if (bAddToIndex)
    //{
    //    nNewIndex--;
    //}
    IT = m_paLayers.begin();
    std::advance(IT, nNewIndex);
    m_paLayers.insert(IT, pLayer);

    //update layer cache id
    //we assume that no layers share the same cache
    //TODO: check this issue

    for (size_t i = 0; i < m_paLayers.size(); ++i)
    {
        m_paLayers[i]->SetCacheId(i);
    }
}

bool wxGISMap::HasLayerType(wxGISEnumDatasetType eType) const
{
    for (size_t i = 0; i < m_paLayers.size(); ++i)
    {
        if (m_paLayers[i]->GetType() == eType)
            return true;
    }
    return false;
}
//The AddLayer method adds a layer to the Map. Use GetLayerCount to get the total number of layers in the map.
//AddLayer automatically attempts to set the Map's SpatialReference if a coordinate system has not yet been defined for the map.

//TODO: When the SpatialReference property is set, the Map's MapUnits and DistanceUnits properties are additionally set.

//AddLayer also sets the spatial reference of the layer.

//TODO: If no layers have a spatial reference, AddLayer checks the extent of the first layer (ILayer::AreaOfInterest) and if it has coordinates
//that look like geographic coordinates (XMin >= -180 and XMax <= 180 and YMin >= -90 and YMax <= 90), ArcMap assumes the data is in decimal
//degrees and sets the MapUnits to esriDecimalDegrees and DistanceUnits to esriMiles.
//If no spatial reference is found and the coordinates do not look like geographic coordinates, ArcMap sets no spatial reference and sets the
//MapUnits to esriMeters and the DistanceUnits to esriMeters.
//The full extent is recalculated each time a layer added.

//--------------------------------------------------------------------------------
// wxGISExtentStack
//--------------------------------------------------------------------------------

wxGISExtentStack::wxGISExtentStack() : wxGISMap()
{
	m_nPos = wxNOT_FOUND;
}

wxGISExtentStack::~wxGISExtentStack()
{
}

bool wxGISExtentStack::CanRedo()
{
	if(m_staEnvelope.empty())
		return false;
	return m_nPos < m_staEnvelope.size() - 1;
}

bool wxGISExtentStack::CanUndo()
{
	if(m_staEnvelope.empty())
		return false;
	return m_nPos > 0;
}

void wxGISExtentStack::Do(const OGREnvelope &NewEnv)
{
	m_nPos++;
	if(m_nPos == m_staEnvelope.size())
		m_staEnvelope.push_back(NewEnv);
	else
	{
		m_staEnvelope[m_nPos] = NewEnv;
		m_staEnvelope.erase(m_staEnvelope.begin() + m_nPos + 1, m_staEnvelope.end());
	}
	SetExtent(NewEnv);
}

void wxGISExtentStack::Redo()
{
	m_nPos++;
	if(m_nPos > wxNOT_FOUND && m_nPos < m_staEnvelope.size())
	{
		OGREnvelope Env = m_staEnvelope[m_nPos];
		SetExtent(Env);
	}
}

void wxGISExtentStack::Undo()
{
	m_nPos--;
    if (m_nPos > wxNOT_FOUND && m_staEnvelope.size() > m_nPos)
	{
		OGREnvelope Env = m_staEnvelope[m_nPos];
		SetExtent(Env);
	}
}

void wxGISExtentStack::SetExtent(const OGREnvelope &Env)
{
	m_CurrentExtent = Env;
}

void wxGISExtentStack::Clear()
{
	wxGISMap::Clear();
	m_staEnvelope.clear();
	m_nPos = wxNOT_FOUND;
	m_CurrentExtent = m_FullExtent;
}

size_t wxGISExtentStack::GetSize() const
{
	return m_staEnvelope.size();
}

OGREnvelope wxGISExtentStack::GetCurrentExtent(void) const
{
	return m_CurrentExtent;
}


/******************************************************************************
 * Project:  wxGIS
 * Purpose:  RasterLayer header.
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
#include "wxgis/carto/layer.h"

//---------------------------------------------------------------------------
// wxGISLayer
//---------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxGISLayer, wxEvtHandler)

wxGISLayer::wxGISLayer(const wxString &sName, wxGISDataset* pwxGISDataset)
{
    m_pwxGISDataset = pwxGISDataset;
    m_sName = sName;
    m_dMaxScale = wxNOT_FOUND;
    m_dMinScale = wxNOT_FOUND;
    m_bVisible = true;
    m_nCacheID = 0;

    if(m_pwxGISDataset)
    {
        m_SpatialReference = m_pwxGISDataset->GetSpatialReference();
    }
    else
    {
        m_SpatialReference = wxNullSpatialReference;
    }
    m_pRenderer = NULL;
}

wxGISLayer::~wxGISLayer(void)
{
    wsDELETE(m_pwxGISDataset);
    wxDELETE(m_pRenderer);
}

const wxGISSpatialReference wxGISLayer::GetSpatialReference(void) 
{
    return m_SpatialReference;
}

void wxGISLayer::SetSpatialReference(const wxGISSpatialReference &SpatialReference)
{
    if(m_SpatialReference->IsSame(SpatialReference))
        return;
    m_SpatialReference = SpatialReference;
}

OGREnvelope wxGISLayer::GetEnvelope(void) const
{
    return m_FullEnvelope;
}

bool wxGISLayer::IsValid(void) const
{
	return true;
}

void wxGISLayer::SetRenderer(wxGISRenderer* pRenderer)
{
    //Delete previous renderer
    //The only one can be exist for a layer
    if(m_pRenderer)
        wxDELETE(m_pRenderer);
    m_pRenderer = pRenderer;
}

wxGISRenderer* wxGISLayer::GetRenderer(void)
{
    return m_pRenderer;
}

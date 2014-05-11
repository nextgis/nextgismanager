/******************************************************************************
* Project:  wxGIS
* Purpose:  wxGISDrawingMapView class.
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2014 Bishop
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

#include "wxgis/cartoui/drawingmap.h"

//-----------------------------------------------
// wxGISDrawingMapView
//-----------------------------------------------

IMPLEMENT_CLASS(wxGISDrawingMapView, wxGISMapView)

wxGISDrawingMapView::wxGISDrawingMapView(void) : wxGISMapView()
{
    m_nCurrentDrawingLayer = wxNOT_FOUND;
}

wxGISDrawingMapView::wxGISDrawingMapView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style) : wxGISMapView(parent, id, pos, size, style)
{
    m_nCurrentDrawingLayer = wxNOT_FOUND;
}

wxGISDrawingMapView::~wxGISDrawingMapView()
{

}

bool wxGISDrawingMapView::AddLayer(wxGISLayer* pLayer)
{
    bool bRes = true;
    bool bAddBaseDrawingLayer = false;
    //if map is empty add base drawing layer
    if (m_paLayers.empty())
    {
        bAddBaseDrawingLayer = true;
    }

    bool bMoveDrawingLayerOnTop = false;
    //if the drawing layer is on top - stay it on top
    if (m_nCurrentDrawingLayer == m_paLayers.size() - 1)
    {
        bMoveDrawingLayerOnTop = true;
    }

    //add layer
    bRes = wxGISMapView::AddLayer(pLayer);
    if (bRes == false)
    {
        return bRes;
    }

    if (bAddBaseDrawingLayer)
    {
        wxGISLayer* pBaseDrawingLayer = new wxGISDrawingLayer(_("Base drawing layer"));
        pBaseDrawingLayer->SetSpatialReference(GetSpatialReference());
        bRes = wxGISMapView::AddLayer(pBaseDrawingLayer);
        if (bRes == false)
        {
            return bRes;
        }
        m_nCurrentDrawingLayer = m_paLayers.size() - 1;
    }


    //if layer is drawing and no drawign layers exist store layer index
    if (pLayer->GetType() == enumGISDrawing)
    {
        pLayer->SetSpatialReference(GetSpatialReference());
        if (m_nCurrentDrawingLayer == wxNOT_FOUND)
            m_nCurrentDrawingLayer = m_paLayers.size() - 1;
        return true;
    }

    //if drawing layer on top let's stay it there
    if (bMoveDrawingLayerOnTop)
    {
        ChangeLayerOrder(m_nCurrentDrawingLayer, m_paLayers.size() - 1);
    }

    return true;
}

short wxGISDrawingMapView::GetCurrentDrawingLayer(void) const
{
    return m_nCurrentDrawingLayer;
}

void wxGISDrawingMapView::SetCurrentDrawingLayer(short nCurrentDrawingLayer)
{
    m_nCurrentDrawingLayer = nCurrentDrawingLayer;
}

bool wxGISDrawingMapView::AddShape(const wxGISGeometry &Geom, wxGISEnumShapeType eType)
{
    if (m_nCurrentDrawingLayer == wxNOT_FOUND)
        return false;
    wxGISDrawingLayer *pLayer = wxDynamicCast(m_paLayers[m_nCurrentDrawingLayer], wxGISDrawingLayer);
    if (NULL == pLayer)
        return false;
    return pLayer->AddShape(Geom, eType);
}

void wxGISDrawingMapView::ChangeLayerOrder(size_t nOldIndex, size_t nNewIndex)
{
    if (nOldIndex == nNewIndex)
    {
        return;
    }

    //check if m_nCurrentDrawingLayer changed
    if (nOldIndex == m_nCurrentDrawingLayer)
    {
        m_nCurrentDrawingLayer = nNewIndex;
        return wxGISMapView::ChangeLayerOrder(nOldIndex, nNewIndex);
    }
    else if (nOldIndex < m_nCurrentDrawingLayer)
    {
        if (nNewIndex < m_nCurrentDrawingLayer)
        {
            return wxGISMapView::ChangeLayerOrder(nOldIndex, nNewIndex);
        }
        else if (nNewIndex > m_nCurrentDrawingLayer)
        {
            m_nCurrentDrawingLayer--;
            return wxGISMapView::ChangeLayerOrder(nOldIndex, nNewIndex);
        }
    }
    else if (nOldIndex > m_nCurrentDrawingLayer)
    {
        if (nNewIndex < m_nCurrentDrawingLayer)
        {
            m_nCurrentDrawingLayer++;
            return wxGISMapView::ChangeLayerOrder(nOldIndex, nNewIndex);
        }
        else if (nNewIndex > m_nCurrentDrawingLayer)
        {
            return wxGISMapView::ChangeLayerOrder(nOldIndex, nNewIndex);
        }
    }

    wxASSERT_MSG(1, wxT("Should never happened"));
}

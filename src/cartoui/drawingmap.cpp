/******************************************************************************
* Project:  wxGIS
* Purpose:  wxGISDrawingMapView class.
* Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
******************************************************************************
*   Copyright (C) 2014 Bishop
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

#include "wxgis/cartoui/drawingmap.h"

//-----------------------------------------------
// wxGISDrawingMapView
//-----------------------------------------------

IMPLEMENT_CLASS(wxGISDrawingMapView, wxGISMapView)

wxGISDrawingMapView::wxGISDrawingMapView(void) : wxGISMapView()
{
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
    //if map is empty add base drawing layer
    if (m_paLayers.empty())
    {
        wxGISLayer* pBaseDrawingLayer = new wxGISDrawingLayer(_("Base drawing layer"));
        pBaseDrawingLayer->SetSpatialReference(GetSpatialReference());
        bRes = wxGISMapView::AddLayer(pBaseDrawingLayer);
        if (bRes == false)
        {
            return bRes;
        }
    }

    //add layer
    bRes = wxGISMapView::AddLayer(pLayer);
    if (bRes == false)
    {
        return bRes;
    }

    //if layer is drawing and no drawign layers exist store layer index
    size_t nLayerIndex = m_paLayers.size() - 1;
    if (pLayer->GetType() == enumGISDrawing)
    {
        pLayer->SetSpatialReference(GetSpatialReference());
        return true;
    }
    //if layer is not drawing or drawing order changed
    //move it after drawing layers from top of the map
    for (int i = m_paLayers.size() - 1; i >= 0; --i)
    {
        if (m_paLayers[i]->GetType() != enumGISDrawing)
        {
            ChangeLayerOrder(nLayerIndex, i);
            break;
        }            
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

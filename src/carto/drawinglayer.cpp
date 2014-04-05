/******************************************************************************
* Project:  wxGIS
* Purpose:  DrawingLayer header
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
#include "wxgis/carto/drawinglayer.h"
#include "wxgis/carto/mxevent.h"


//----------------------------------------------------------------------------
// wxGISDrawingLayer
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISDrawingLayer, wxGISLayer)

wxGISDrawingLayer::wxGISDrawingLayer(const wxString &sName, wxGISDataset* pwxGISDataset) : wxGISLayer(sName, pwxGISDataset)
{
}

wxGISDrawingLayer::~wxGISDrawingLayer(void)
{
}

bool wxGISDrawingLayer::Draw(wxGISEnumDrawPhase DrawPhase, ITrackCancel* const pTrackCancel)
{
    return true;
}

bool wxGISDrawingLayer::AddShape(const wxGISGeometry &Geom, wxGISEnumShapeType eType)
{
    if (!Geom.IsOk())
        return false;
    //wxGISDrawingLayer *pLayer = wxDynamicCast(m_paLayers[m_nCurrentDrawingLayer], wxGISDrawingLayer);
    //if (NULL == pLayer)
    //    return false;
    //return pLayer->AddShape(Geom, eType);

    AddEvent(wxMxMapViewEvent(wxMXMAP_LAYER_CHANGED, GetId()));

    return true;
}

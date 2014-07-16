/******************************************************************************
* Project:  wxGIS
* Purpose:  wxGISDrawingMapView class.
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
#pragma once

#include "wxgis/cartoui/mapview.h"
#include "wxgis/carto/drawinglayer.h"


/** @class wxGISDrawingMapView

    The MapView with support of drawing layers.

    @library{cartoui}
*/

class WXDLLIMPEXP_GIS_CTU wxGISDrawingMapView :
    public wxGISMapView
{
    DECLARE_CLASS(wxGISDrawingMapView)
public:
    wxGISDrawingMapView(void);
    wxGISDrawingMapView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL | wxCLIP_CHILDREN | wxNO_FULL_REPAINT_ON_RESIZE);
    virtual ~wxGISDrawingMapView(void);
    //wxGISMap
    virtual void ChangeLayerOrder(size_t nOldIndex, size_t nNewIndex);
    //wxGISExtentStack
    virtual bool AddLayer(wxGISLayer* pLayer);
    //wxGISDrawingMapView
    virtual short GetCurrentDrawingLayer(void) const;
    virtual void SetCurrentDrawingLayer(short nCurrentDrawingLayer);
    virtual bool AddShape(const wxGISGeometry &Geom, wxGISEnumShapeType eType);
protected:
    short m_nCurrentDrawingLayer;
};

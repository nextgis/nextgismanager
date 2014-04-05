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
#pragma once

#include "wxgis/carto/layer.h"
//#include "wxgis/carto/featurerenderer.h"
//#include "wxgis/carto/rasterrenderer.h"

/** @enum wxGISEnumShapeType

    The base drawing shape types

    @library{carto}
*/

enum wxGISEnumShapeType{
    enumGISShapeTypeRectangle = 0,
    enumGISShapeTypePolygon,
    enumGISShapeTypeCircle,
    enumGISShapeTypeEllipse,
    enumGISShapeTypeLine,
    enumGISShapeTypeCurve,
    enumGISShapeTypeFreeHand,
    enumGISShapeTypeMarker,
    enumGISShapeTypeMax
};

enum wxGISEnumShapeState{
    enumGISShapeStateNormal = 0,
    enumGISShapeStateSelected,
    enumGISShapeStateRotated,
    enumGISShapeStatePoints,
    enumGISShapeStateMax
};

/** @class wxGISShape

    The base drawing shape
    
    @library{carto}
*/

/*
class wxGISShape : public wxObject
{
    DECLARE_CLASS(wxGISShape)
public:
    wxGISShape(const wxGISGeometry &Geom, wxGISEnumShapeType eType, wxGISSymbol* pSymbol);
    virtual ~wxGISShape();
    virtual wxGISEnumShapeType GetType() const;
    virtual wxGISEnumShapeState GetState() const;
protected:
    //wxGISSymbol* m_pSymbol;
};
*/

/** @class wxGISDrawingLayer

    The class represent user drawings in map.

    @library{carto}
*/

class WXDLLIMPEXP_GIS_CRT wxGISDrawingLayer :
    public wxGISLayer
{
    DECLARE_CLASS(wxGISDrawingLayer)
public:
    wxGISDrawingLayer(const wxString &sName = _("new drawing layer"), wxGISDataset* pwxGISDataset = NULL);
    virtual ~wxGISDrawingLayer(void);
    //wxGISLayer
    virtual bool Draw(wxGISEnumDrawPhase DrawPhase, ITrackCancel* const pTrackCancel = NULL);
    virtual wxGISEnumDatasetType GetType(void) const { return enumGISDrawing; };
    virtual bool AddShape(const wxGISGeometry &Geom, wxGISEnumShapeType eType);
protected:
//    wxVector<wxGISShape> m_aoShapes;
    wxGISSpatialTree *pTree;
};

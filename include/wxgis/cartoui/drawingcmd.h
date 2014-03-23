/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  Carto Drawing Commands & tools class.
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
#include "wxgis/framework/command.h"
#include "wxgis/framework/applicationbase.h"
#include "wxgis/cartoui/mapview.h"
#include "wxgis/cartoui/mxeventui.h"

enum wxGISEnumDrawingToolType{
    enumGISDrawingToolRectangle = 0,
    enumGISDrawingToolPolygon,
    enumGISDrawingToolCircle,
    enumGISDrawingToolEllipse,
    enumGISDrawingToolLine,
    enumGISDrawingToolCurve,
    enumGISDrawingToolFreeHand,
    enumGISDrawingToolMarker,
    enumGISDrawingToolMax
};

/** @class wxGISDrawingTool

    The carto drawing tools (polygon, line, point, etc)

    @library {cartoui}
*/

class WXDLLIMPEXP_GIS_CTU wxGISDrawingTool :
    public ITool
{
    DECLARE_DYNAMIC_CLASS(wxGISDrawingTool)

public:
    wxGISDrawingTool(void);
    virtual ~wxGISDrawingTool(void);
    //ICommand
    virtual wxIcon GetBitmap(void);
    virtual wxString GetCaption(void);
    virtual wxString GetCategory(void);
    virtual bool GetChecked(void);
    virtual bool GetEnabled(void);
    virtual wxString GetMessage(void);
    virtual wxGISEnumCommandKind GetKind(void);
    virtual void OnClick(void);
    virtual bool OnCreate(wxGISApplicationBase* pApp);
    virtual wxString GetTooltip(void);
    virtual unsigned char GetCount(void);
    //ITool
    virtual wxCursor GetCursor(void);
    virtual void SetChecked(bool bCheck);
    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
    virtual void OnMouseDoubleClick(wxMouseEvent& event);
    virtual void OnMouseMove(wxMouseEvent& event);
private:
    wxGISApplicationBase* m_pApp;
    wxIcon m_IconPolygon, m_IconLine, m_IconMarker, m_IconCircle, m_IconEllipse, m_IconCurve, m_IconRectangle, m_IconFreeHand;
    wxCursor m_CurDrawing;
    wxGISMapView* m_pMapView;
    WINDOWARRAY m_anMapWinIDs;
    bool m_bCheck;
};

enum wxGISEnumDrawingToolMenuType{
    enumGISDrawingToolMenu = 0,
    enumGISDrawingTextToolMenu
};


/** @class wxGISDrawingToolMenu

    The carto drawing tools menu (polygon, line, point, etc)

    @library {cartoui}
*/

class WXDLLIMPEXP_GIS_CTU wxGISDrawingToolMenu :
    public ITool,
    public IDropDownCommand
{
    DECLARE_DYNAMIC_CLASS(wxGISDrawingToolMenu)
public:
    wxGISDrawingToolMenu(void);
    virtual ~wxGISDrawingToolMenu(void);
    //ICommand
    virtual wxIcon GetBitmap(void);
    virtual wxString GetCaption(void);
    virtual wxString GetCategory(void);
    virtual bool GetChecked(void);
    virtual bool GetEnabled(void);
    virtual wxString GetMessage(void);
    virtual wxGISEnumCommandKind GetKind(void);
    virtual void OnClick(void);
    virtual bool OnCreate(wxGISApplicationBase* pApp);
    virtual wxString GetTooltip(void);
    virtual unsigned char GetCount(void);
    //ITool
    virtual wxCursor GetCursor(void);
    virtual void SetChecked(bool bCheck);
    virtual void OnMouseDown(wxMouseEvent& event);
    virtual void OnMouseUp(wxMouseEvent& event);
    virtual void OnMouseDoubleClick(wxMouseEvent& event);
    virtual void OnMouseMove(wxMouseEvent& event);
    //IDropDownCommand
    virtual wxMenu* GetDropDownMenu(void);
    virtual void OnDropDownCommand(int nID);
private:
    wxGISApplicationBase* m_pApp;
    wxGISDrawingTool* m_pCurrentTool;
};

/******************************************************************************
* Project:  wxGIS (GIS Catalog)
* Purpose:  Carto Drawing Commands & tools class.
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

#include "wxgis/cartoui/drawingcmd.h"
#include "wxgis/core/config.h"
#include "wxgis/core/format.h"
#include "wxgis/display/rubberband.h"
#include "wxgis/display/displayop.h"

#include "../../art/dr_circle.xpm"
#include "../../art/dr_rectangle.xpm"
#include "../../art/dr_polygon.xpm"
#include "../../art/dr_line.xpm"
#include "../../art/dr_ellipse.xpm"
#include "../../art/dr_marker.xpm"
#include "../../art/dr_curve.xpm"
#include "../../art/dr_freehand.xpm"
#include "../../art/edit_clear.xpm"


//--------------------------------------------------
// wxGISDrawingTool
//--------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISDrawingTool, wxGISCommand)


wxGISDrawingTool::wxGISDrawingTool(void)
{
    m_subtype = 0;
    m_pMapView = NULL;
    m_bCheck = false;
}

wxGISDrawingTool::~wxGISDrawingTool(void)
{
}

wxIcon wxGISDrawingTool::GetBitmap(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolRectangle://
        if (!m_IconRectangle.IsOk())
            m_IconRectangle = wxIcon(rectangle_xpm);
        return m_IconRectangle;
    case enumGISDrawingToolMarker://
        if (!m_IconMarker.IsOk())
            m_IconMarker = wxIcon(marker_xpm);
        return m_IconMarker;
    case enumGISDrawingToolCircle://
        if (!m_IconCircle.IsOk())
            m_IconCircle = wxIcon(circle_xpm);
        return m_IconCircle;
    case enumGISDrawingToolEllipse://
        if (!m_IconEllipse.IsOk())
            m_IconEllipse = wxIcon(ellipse_xpm);
        return m_IconEllipse;
    case enumGISDrawingToolPolygon://
        if (!m_IconPolygon.IsOk())
            m_IconPolygon = wxIcon(polygon_xpm);
        return m_IconPolygon;
    case enumGISDrawingToolLine://
        if (!m_IconLine.IsOk())
            m_IconLine = wxIcon(line_xpm);
        return m_IconLine;
#ifdef wxGIS_USE_SPLINE
    case enumGISDrawingToolCurve://
        if (!m_IconCurve.IsOk())
            m_IconCurve = wxIcon(curve_xpm);
        return m_IconCurve;
#endif // wxGIS_USE_SPLINE
    case enumGISDrawingToolFreeHand://
        if (!m_IconFreeHand.IsOk())
            m_IconFreeHand = wxIcon(freehand_xpm);
        return m_IconFreeHand;
    case enumGISDrawingToolLayerClear://
        if (!m_IconEditClear.IsOk())
            m_IconEditClear = wxIcon(edit_clear_xpm);
        return m_IconEditClear;
    default:
        return wxNullIcon;
    }
}

wxString wxGISDrawingTool::GetCaption(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolRectangle://
        return wxString(_("Rectangle"));
    case enumGISDrawingToolMarker://
        return wxString(_("Marker"));
    case enumGISDrawingToolCircle://
        return wxString(_("Circle"));
    case enumGISDrawingToolEllipse://
        return wxString(_("Ellipse"));
    case enumGISDrawingToolPolygon://
        return wxString(_("Polygon"));
    case enumGISDrawingToolLine://
        return wxString(_("Line"));
#ifdef wxGIS_USE_SPLINE
    case enumGISDrawingToolCurve://
        return wxString(_("Curve"));
#endif //wxGIS_USE_SPLINE
    case enumGISDrawingToolFreeHand://
        return wxString(_("Freehand"));
    case enumGISDrawingToolLayerSelector://
        return wxString(_("Select drawing layer"));
    case enumGISDrawingToolLayerClear://
        return wxString(_("Clear drawing layer"));
    default:
        return wxEmptyString;
    }
}

wxString wxGISDrawingTool::GetCategory(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolRectangle://
    case enumGISDrawingToolMarker://
    case enumGISDrawingToolCircle://
    case enumGISDrawingToolEllipse://
    case enumGISDrawingToolPolygon://
    case enumGISDrawingToolLine://
#ifdef wxGIS_USE_SPLINE
    case enumGISDrawingToolCurve://
#endif //wxGIS_USE_SPLINE
    case enumGISDrawingToolFreeHand://
    case enumGISDrawingToolLayerSelector://
    case enumGISDrawingToolLayerClear://
        return wxString(_("Drawing"));
    default:
        return NO_CATEGORY;
    }
}

bool wxGISDrawingTool::GetChecked(void)
{
    return m_bCheck;
}

bool wxGISDrawingTool::GetEnabled(void)
{
    if (m_anMapWinIDs.empty())
    {
        WINDOWARRAY WinIDsArr = m_pApp->GetChildWindows();
        for (size_t i = 0; i < WinIDsArr.GetCount(); ++i)
        {
            wxWindow* pWnd = wxWindow::FindWindowById(WinIDsArr[i]);
            if (pWnd && pWnd->IsKindOf(wxCLASSINFO(wxGISDrawingMapView)))
            {
                m_anMapWinIDs.Add(WinIDsArr[i]);
            }
        }
    }

    switch (m_subtype)
    {
    case enumGISDrawingToolRectangle://
    case enumGISDrawingToolMarker://
    case enumGISDrawingToolCircle://
    case enumGISDrawingToolEllipse://
    case enumGISDrawingToolPolygon://
    case enumGISDrawingToolLine://
#ifdef wxGIS_USE_SPLINE
    case enumGISDrawingToolCurve://
#endif //wxGIS_USE_SPLINE
    case enumGISDrawingToolFreeHand://
    case enumGISDrawingToolLayerClear://
    case enumGISDrawingToolLayerSelector://
        for (size_t i = 0; i < m_anMapWinIDs.GetCount(); ++i)
        {
            wxWindow* pWnd = wxWindow::FindWindowById(m_anMapWinIDs[i]);
            m_pMapView = wxDynamicCast(pWnd, wxGISDrawingMapView);
            if (m_pMapView && m_pMapView->IsShownOnScreen())
                return m_pMapView->GetCurrentDrawingLayer() != wxNOT_FOUND;//m_pMapView->HasLayerType(enumGISDrawing) &&
        }
        return false;
    default:
        return false;
    }
    return false;
}

wxGISEnumCommandKind wxGISDrawingTool::GetKind(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolRectangle://
    case enumGISDrawingToolMarker://
    case enumGISDrawingToolCircle://
    case enumGISDrawingToolEllipse://
    case enumGISDrawingToolPolygon://
    case enumGISDrawingToolLine://
#ifdef wxGIS_USE_SPLINE
    case enumGISDrawingToolCurve://
#endif // wxGIS_USE_SPLINE
    case enumGISDrawingToolFreeHand://
        return enumGISCommandCheck;
    case enumGISDrawingToolLayerSelector://
        return enumGISCommandControl;
    case enumGISDrawingToolLayerClear://
    default:
        return enumGISCommandNormal;
    }
}

wxString wxGISDrawingTool::GetMessage(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolRectangle://
        return wxString(_("The tool to draw rectangle"));
    case enumGISDrawingToolMarker://
        return wxString(_("The tool to draw marker"));
    case enumGISDrawingToolCircle://
        return wxString(_("The tool to draw circle"));
    case enumGISDrawingToolEllipse://
        return wxString(_("The tool to draw ellipse"));
    case enumGISDrawingToolPolygon://
        return wxString(_("The tool to draw polygon"));
    case enumGISDrawingToolLine://
        return wxString(_("The tool to draw line"));
#ifdef wxGIS_USE_SPLINE
    case enumGISDrawingToolCurve://
        return wxString(_("The tool to draw curve"));
#endif //wxGIS_USE_SPLINE
    case enumGISDrawingToolFreeHand://
        return wxString(_("The tool to draw freehand"));
    case enumGISDrawingToolLayerSelector://
        return wxString(_("Select layer to draw"));
    case enumGISDrawingToolLayerClear://
        return wxString(_("Clear drawing layer"));
    default:
        return wxEmptyString;
    }
}

void wxGISDrawingTool::OnClick(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolRectangle://
    case enumGISDrawingToolMarker://
    case enumGISDrawingToolCircle://
    case enumGISDrawingToolEllipse://
    case enumGISDrawingToolPolygon://
    case enumGISDrawingToolLine://
#ifdef wxGIS_USE_SPLINE
    case enumGISDrawingToolCurve://
#endif //wxGIS_USE_SPLINE
    case enumGISDrawingToolFreeHand://
    case enumGISDrawingToolLayerSelector://
        break;
    case enumGISDrawingToolLayerClear://
    {
        short nLayerNo = m_pMapView->GetCurrentDrawingLayer();
        wxGISDrawingLayer *pLayer = wxDynamicCast(m_pMapView->GetLayerByIndex(nLayerNo), wxGISDrawingLayer);
        if (NULL != pLayer)
        {
            pLayer->Clear();
        }
        break;
    }
    default:
        break;
    }
}

bool wxGISDrawingTool::OnCreate(wxGISApplicationBase* pApp)
{
    m_pApp = pApp;
    return true;
}

wxString wxGISDrawingTool::GetTooltip(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolRectangle://
        return wxString(_("Draw rectangle"));
    case enumGISDrawingToolMarker://
        return wxString(_("Draw marker"));
    case enumGISDrawingToolCircle://
        return wxString(_("Draw circle"));
    case enumGISDrawingToolEllipse://
        return wxString(_("Draw ellipse"));
    case enumGISDrawingToolPolygon://
        return wxString(_("Draw polygon"));
    case enumGISDrawingToolLine://
        return wxString(_("Draw line"));
#ifdef wxGIS_USE_SPLINE
    case enumGISDrawingToolCurve://
        return wxString(_("Draw curve"));
#endif // wxGIS_USE_SPLINE
    case enumGISDrawingToolFreeHand://
        return wxString(_("Draw freehand"));
    case enumGISDrawingToolLayerSelector://
        return wxString(_("Select drawing layer"));
    case enumGISDrawingToolLayerClear://
        return wxString(_("Clear drawing layer"));
    default:
        return wxEmptyString;
    }
}

unsigned char wxGISDrawingTool::GetCount(void)
{
    return enumGISDrawingToolMax;
}

wxCursor wxGISDrawingTool::GetCursor(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolRectangle://
    case enumGISDrawingToolMarker://
    case enumGISDrawingToolCircle://
    case enumGISDrawingToolEllipse://
    case enumGISDrawingToolPolygon://
    case enumGISDrawingToolLine://
#ifdef wxGIS_USE_SPLINE
    case enumGISDrawingToolCurve://
#endif //wxGIS_USE_SPLINE
    case enumGISDrawingToolFreeHand://
        if (!m_CurDrawing.IsOk())
        {
            m_CurDrawing = wxCursor(wxCURSOR_CROSS);
        }
        return m_CurDrawing;
    case enumGISDrawingToolLayerSelector://
    case enumGISDrawingToolLayerClear://
    default:
        return wxNullCursor;
    }
}

void wxGISDrawingTool::SetChecked(bool bCheck)
{
    m_bCheck = bCheck;
    if (m_bCheck && m_pMapView)
        m_pMapView->SetCursor(GetCursor());
}

void wxGISDrawingTool::OnMouseDown(wxMouseEvent& event)
{
    wxGISAppConfig oConfig = GetConfig();
    wxGISColor color(0, 255, 0, 255);
    int nWidth(2);
    if (oConfig.IsOk())
    {
        wxXmlNode* pNode = oConfig.GetConfigNode(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/drawing/rabberband")));
        color = GetColorValue(pNode, wxT("color"), color);
        nWidth = GetDecimalValue(pNode, wxT("width"), nWidth);
    }

    event.Skip(true);
    switch (m_subtype)
    {
    case enumGISDrawingToolRectangle://
    {
        wxGISRubberEnvelope RubberEnvelope(wxPen(color.GetColour(), nWidth), m_pMapView, m_pMapView->GetDisplay(), m_pMapView->GetSpatialReference());
        wxGISGeometry Geom = RubberEnvelope.TrackNew(event.GetX(), event.GetY());
        if (!Geom.IsOk() || m_pMapView == NULL)
            break;
        m_pMapView->AddShape(Geom, enumGISShapeTypeRectangle);
        break;
    }
    case enumGISDrawingToolMarker://
    {
        wxGISRubberMarker RubberMarker(wxPen(color.GetColour(), nWidth), m_pMapView, m_pMapView->GetDisplay(), m_pMapView->GetSpatialReference());
        wxGISGeometry Geom = RubberMarker.TrackNew(event.GetX(), event.GetY());
        if (!Geom.IsOk() || m_pMapView == NULL)
            break;
        m_pMapView->AddShape(Geom, enumGISShapeTypeMarker);
        break;
    }
    case enumGISDrawingToolCircle://
    {
        wxGISRubberCircle RubberCircle(wxPen(color.GetColour(), nWidth), m_pMapView, m_pMapView->GetDisplay(), m_pMapView->GetSpatialReference());
        wxGISGeometry Geom = RubberCircle.TrackNew(event.GetX(), event.GetY());
        if (!Geom.IsOk() || m_pMapView == NULL)
            break;
        m_pMapView->AddShape(Geom, enumGISShapeTypeCircle);
        break;
    }
    case enumGISDrawingToolEllipse://
    {
        wxGISRubberEllipse RubberEllipse(wxPen(color.GetColour(), nWidth), m_pMapView, m_pMapView->GetDisplay(), m_pMapView->GetSpatialReference());
        wxGISGeometry Geom = RubberEllipse.TrackNew(event.GetX(), event.GetY());
        if (!Geom.IsOk() || m_pMapView == NULL)
            break;
        m_pMapView->AddShape(Geom, enumGISShapeTypeEllipse);
        break;
    }
    case enumGISDrawingToolPolygon://
    {
        wxPen oPen(color.GetColour(), nWidth);
        oPen.SetCap(wxCAP_ROUND);
        oPen.SetJoin(wxJOIN_ROUND);

        wxGISRubberPolygon RubberPolygon(oPen, m_pMapView, m_pMapView->GetDisplay(), m_pMapView->GetSpatialReference());
        wxGISGeometry Geom = RubberPolygon.TrackNew(event.GetX(), event.GetY());
        if (!Geom.IsOk() || m_pMapView == NULL)
            break;
        m_pMapView->AddShape(Geom, enumGISShapeTypePolygon);
        break;
    }
    case enumGISDrawingToolLine://
    {
        wxPen oPen(color.GetColour(), nWidth);
        oPen.SetCap(wxCAP_ROUND);
        oPen.SetJoin(wxJOIN_ROUND);

        wxGISRubberLine RubberLine(oPen, m_pMapView, m_pMapView->GetDisplay(), m_pMapView->GetSpatialReference());
        wxGISGeometry Geom = RubberLine.TrackNew(event.GetX(), event.GetY());
        if (!Geom.IsOk() || m_pMapView == NULL)
            break;
        m_pMapView->AddShape(Geom, enumGISShapeTypeLine);
        break;
    }
#ifdef wxGIS_USE_SPLINE
    case enumGISDrawingToolCurve://
    {
        wxPen oPen(color.GetColour(), nWidth);
        oPen.SetCap(wxCAP_ROUND);
        oPen.SetJoin(wxJOIN_ROUND);

        wxGISRubberSpline RubberSpline(oPen, m_pMapView, m_pMapView->GetDisplay(), m_pMapView->GetSpatialReference());
        wxGISGeometry Geom = RubberSpline.TrackNew(event.GetX(), event.GetY());
        if (!Geom.IsOk() || m_pMapView == NULL)
            break;
        m_pMapView->AddShape(Geom, enumGISShapeTypeCurve);
        break;
    }
#endif //wxGIS_USE_SPLINE
    case enumGISDrawingToolFreeHand://
    {
        wxPen oPen(color.GetColour(), nWidth);
        oPen.SetCap(wxCAP_ROUND);
        oPen.SetJoin(wxJOIN_ROUND);

        wxGISRubberFreeHand RubberFreeHand(oPen, m_pMapView, m_pMapView->GetDisplay(), m_pMapView->GetSpatialReference());
        wxGISGeometry Geom = RubberFreeHand.TrackNew(event.GetX(), event.GetY());
        if (!Geom.IsOk() || m_pMapView == NULL)
            break;
        m_pMapView->AddShape(Geom, enumGISShapeTypeFreeHand);
        break;
    }
    default:
        break;
    }
}

void wxGISDrawingTool::OnMouseUp(wxMouseEvent& event)
{
        event.Skip(true);
    switch (m_subtype)
    {
    case enumGISDrawingToolMarker://
    case enumGISDrawingToolRectangle://
    case enumGISDrawingToolCircle://
    case enumGISDrawingToolEllipse://
    case enumGISDrawingToolPolygon://
    case enumGISDrawingToolLine://
#ifdef wxGIS_USE_SPLINE
    case enumGISDrawingToolCurve://
#endif //wxGIS_USE_SPLINE
    case enumGISDrawingToolFreeHand://
        break;
    default:
        break;
    }
}

void wxGISDrawingTool::OnMouseMove(wxMouseEvent& event)
{
    event.Skip(true);
    switch (m_subtype)
    {
    case enumGISDrawingToolRectangle://
    case enumGISDrawingToolMarker://
    case enumGISDrawingToolCircle://
    case enumGISDrawingToolEllipse://
    case enumGISDrawingToolPolygon://
    case enumGISDrawingToolLine://
#ifdef wxGIS_USE_SPLINE
    case enumGISDrawingToolCurve://
#endif //wxGIS_USE_SPLINE
    case enumGISDrawingToolFreeHand://
        break;
    default:
        break;
    }
}

void wxGISDrawingTool::OnMouseDoubleClick(wxMouseEvent& event)
{
    event.Skip(true);
}

IToolBarControl* wxGISDrawingTool::GetControl(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolLayerSelector:
    {
        wxArrayString ValuesArray;
        wxGISDrawingLayersComboBox* pComboBox = new wxGISDrawingLayersComboBox(dynamic_cast<wxWindow*>(m_pApp), wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(180, 22), ValuesArray);
        return static_cast<IToolBarControl*>(pComboBox);
    }
    default:
        return NULL;
    }
}

wxString wxGISDrawingTool::GetToolLabel(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolLayerSelector:
        return wxString(_("Layer:  "));
    default:
        return wxEmptyString;
    }
}

bool wxGISDrawingTool::HasToolLabel(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolLayerSelector:
        return true;
    default:
        return false;
    }
}

//--------------------------------------------------
// wxGISDrawingToolMenu
//--------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISDrawingToolMenu, wxGISCommand)


wxGISDrawingToolMenu::wxGISDrawingToolMenu(void)
{
    m_subtype = enumGISDrawingToolMenu;
    m_pCurrentTool = NULL;
}

wxGISDrawingToolMenu::~wxGISDrawingToolMenu(void)
{
}

wxIcon wxGISDrawingToolMenu::GetBitmap(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        if (m_pCurrentTool != NULL)
            return m_pCurrentTool->GetBitmap();
    default:
        return wxNullIcon;
    }
}

wxString wxGISDrawingToolMenu::GetCaption(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        return wxString(_("Drawing tools"));
    default:
        return wxEmptyString;
    }
}

wxString wxGISDrawingToolMenu::GetCategory(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        return wxString(_("Drawing"));
    default:
        return NO_CATEGORY;
    }
}

bool wxGISDrawingToolMenu::GetChecked(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        if (m_pCurrentTool != NULL)
            return m_pCurrentTool->GetChecked();
    default:
        return false;
    }
}

bool wxGISDrawingToolMenu::GetEnabled(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        if (m_pCurrentTool != NULL)
            return m_pCurrentTool->GetEnabled();
        return true;
    default:
        return false;
    }
    return false;
}

wxGISEnumCommandKind wxGISDrawingToolMenu::GetKind(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        return enumGISCommandDropDown;
    default:
        return enumGISCommandNormal;
    }
}

wxString wxGISDrawingToolMenu::GetMessage(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        if (m_pCurrentTool != NULL)
            return m_pCurrentTool->GetMessage();
    default:
        return wxEmptyString;
    }
}

void wxGISDrawingToolMenu::OnClick(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        m_pApp->Command(this);
        break;
    default:
        break;
    }
}

bool wxGISDrawingToolMenu::OnCreate(wxGISApplicationBase* pApp)
{
    m_pApp = pApp;

    wxGISCommand* pCmd = m_pApp->GetCommand(wxT("wxGISDrawingTool"), enumGISDrawingToolRectangle);
    m_pCurrentTool = wxDynamicCast(pCmd, wxGISDrawingTool);

    return true;
}

wxString wxGISDrawingToolMenu::GetTooltip(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        if (m_pCurrentTool != NULL)
            return m_pCurrentTool->GetTooltip();
    default:
        return wxEmptyString;
    }
}

unsigned char wxGISDrawingToolMenu::GetCount(void)
{
    return 1;
}

wxCursor wxGISDrawingToolMenu::GetCursor(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        if (m_pCurrentTool != NULL)
            return m_pCurrentTool->GetCursor();
    default:
        return wxNullCursor;
    }
}

void wxGISDrawingToolMenu::SetChecked(bool bCheck)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        if (m_pCurrentTool != NULL)
            m_pCurrentTool->SetChecked(bCheck);
    default:
        break;
    }
}

void wxGISDrawingToolMenu::OnMouseDown(wxMouseEvent& event)
{
    event.Skip(true);
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        if (m_pCurrentTool != NULL)
            m_pCurrentTool->OnMouseDown(event);
    default:
        break;
    }
}

void wxGISDrawingToolMenu::OnMouseUp(wxMouseEvent& event)
{
    event.Skip(true);
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        if (m_pCurrentTool != NULL)
            m_pCurrentTool->OnMouseUp(event);
    default:
        break;
    }
}

void wxGISDrawingToolMenu::OnMouseMove(wxMouseEvent& event)
{
    event.Skip(true);
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        if (m_pCurrentTool != NULL)
            m_pCurrentTool->OnMouseMove(event);
    default:
        break;
    }
}

void wxGISDrawingToolMenu::OnMouseDoubleClick(wxMouseEvent& event)
{
    event.Skip(true);
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
        if (m_pCurrentTool != NULL)
            m_pCurrentTool->OnMouseDoubleClick(event);
    default:
        break;
    }
}


wxMenu* wxGISDrawingToolMenu::GetDropDownMenu(void)
{
    switch (m_subtype)
    {
    case enumGISDrawingToolMenu:
    {
        wxMenu* pMenu = new wxMenu();
        for (size_t i = enumGISDrawingToolRectangle; i < enumGISDrawingToolMarker + 1; ++i)
        {
            wxGISCommand* pCmd = m_pApp->GetCommand(wxT("wxGISDrawingTool"), i);
            if (pCmd != NULL)
            {
                wxMenuItem *item = new wxMenuItem(pMenu, ID_MENUCMD + i, pCmd->GetCaption(), pCmd->GetMessage(), (wxItemKind)pCmd->GetKind());
                item->Enable(pCmd->GetEnabled());
                wxBitmap Bmp = pCmd->GetBitmap();
#ifdef __WIN32__
                if (Bmp.IsOk())
                {
                    wxImage Img = Bmp.ConvertToImage();                //Img.RotateHue(-0.1);
                    item->SetBitmaps(Bmp, Img.ConvertToGreyscale());
                }
#else
                if (Bmp.IsOk())
                {
                    item->SetBitmap(Bmp);
                }
#endif
                pMenu->Append(item);
            }

        }

        return pMenu;
    }
    default:
        return NULL;
    }
}

void wxGISDrawingToolMenu::OnDropDownCommand(int nID)
{
    wxGISCommand* pCmd = m_pApp->GetCommand(wxT("wxGISDrawingTool"), nID - ID_MENUCMD);
    m_pCurrentTool = wxDynamicCast(pCmd, wxGISDrawingTool);

    m_pApp->Command(this);// m_pCurrentTool);
}


//-----------------------------------------------------------------------------------------
// wxGISDrawingLayersComboBox
//-----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISDrawingLayersComboBox, wxComboBox)

BEGIN_EVENT_TABLE(wxGISDrawingLayersComboBox, wxComboBox)
EVT_MXMAP_LAYER_ADDED(wxGISDrawingLayersComboBox::OnLayerAdded)
EVT_MXMAP_LAYER_REMOVED(wxGISDrawingLayersComboBox::OnLayerRemoved)
END_EVENT_TABLE()

wxGISDrawingLayersComboBox::wxGISDrawingLayersComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name) : wxComboBox(parent, id, value, pos, size, choices, style, validator, name)
{
}

wxGISDrawingLayersComboBox::~wxGISDrawingLayersComboBox(void)
{
}

void wxGISDrawingLayersComboBox::Activate(wxGISApplicationBase* pApp)
{
    if (m_anMapWinIDs.empty())
    {
        WINDOWARRAY WinIDsArr = pApp->GetChildWindows();
        for (size_t i = 0; i < WinIDsArr.GetCount(); ++i)
        {
            wxWindow* pWnd = wxWindow::FindWindowById(WinIDsArr[i]);
            if (pWnd && pWnd->IsKindOf(wxCLASSINFO(wxGISDrawingMapView)))
            {
                wxGISDrawingMapView* pMapView = wxDynamicCast(pWnd, wxGISDrawingMapView);
                long nConnectionPointMapCookie = pMapView->Advise(this);
                m_anMapWinIDs.push_back(std::make_pair(pWnd->GetId(), nConnectionPointMapCookie));
            }
        }
    }
}

void wxGISDrawingLayersComboBox::Deactivate(void)
{
    for (size_t i = 0; i < m_anMapWinIDs.size(); ++i)
    {
        wxWindow* pWnd = wxWindow::FindWindowById(m_anMapWinIDs[i].first);
        wxGISDrawingMapView* pMapView = wxDynamicCast(pWnd, wxGISDrawingMapView);
        if (pMapView)
        {
            pMapView->Unadvise(m_anMapWinIDs[i].second);
        }
    }
}

void wxGISDrawingLayersComboBox::UpdateLayersList(int nWinId)
{
    wxWindow* pWnd = wxWindow::FindWindowById(nWinId);
    wxGISDrawingMapView* pMapView = wxDynamicCast(pWnd, wxGISDrawingMapView);
    if (pMapView)
    {
        //clear
        Clear(); //TODO: store selection in combo
        for (size_t i = 0; i < pMapView->GetLayerCount(); ++i)
        {
            wxGISLayer* pLayer = pMapView->GetLayerByIndex(i);
            if (NULL != pLayer && pLayer->GetType() == enumGISDrawing)
            {
                int nPos = Append(pLayer->GetName());
                SetClientData(nPos, (void*)pLayer->GetId());
            }
        }
        SetSelection(0);
    }
}

void wxGISDrawingLayersComboBox::OnLayerAdded(wxMxMapViewUIEvent& event)
{
    UpdateLayersList(event.GetId());
}

void wxGISDrawingLayersComboBox::OnLayerRemoved(wxMxMapViewUIEvent& event)
{
    UpdateLayersList(event.GetId());
}

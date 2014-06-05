/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Carto Main Commands class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011-2013 Dmitry Baryshnikov
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
#include "wxgis/cartoui/cartocmd.h"
#include "wxgis/cartoui/mxeventui.h"
#include "wxgis/display/rubberband.h"
#include "wxgis/core/config.h"
#include "wxgis/display/displayop.h"

#include "../../art/fullext.xpm"
#include "../../art/prevext.xpm"
#include "../../art/nextext.xpm"
#include "../../art/zoomin.xpm"
#include "../../art/zoomout.xpm"
#include "../../art/info.xpm"
#include "../../art/pan1_cur.xpm"
#include "../../art/pan2_cur.xpm"
#include "../../art/cancel_map_rotate.xpm"
#include "../../art/get_info_cur.xpm"
#include "../../art/map_rotate.xpm"
#include "../../art/rotate_cur.xpm"
#include "../../art/zoom_in_cur.xpm"
#include "../../art/zoom_out_cur.xpm"

//#include "../../art/geography16.xpm"
//#include "../../art/cursors_16.xpm"

//	0	Full Extent
//	1	Prev Extent
//	2	Next Extent
//	3	?

//--------------------------------------------------
// wxGISCartoMainCmd
//--------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISCartoMainCmd, wxGISCommand)


wxGISCartoMainCmd::wxGISCartoMainCmd(void) : wxGISCommand()
{
    m_pMapView = NULL;
}

wxGISCartoMainCmd::~wxGISCartoMainCmd(void)
{
}

wxIcon wxGISCartoMainCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainCmdFullExtent:
		if(!m_IconFullExt.IsOk())
			m_IconFullExt = wxIcon(fullext_xpm);
		return m_IconFullExt;
    case enumGISCartoMainCmdPrevExtent:
		if(!m_IconPrevExt.IsOk())
			m_IconPrevExt = wxIcon(prevext_xpm);
		return m_IconPrevExt;
    case enumGISCartoMainCmdNextExtent:
		if(!m_IconNextExt.IsOk())
			m_IconNextExt = wxIcon(nextext_xpm);
		return m_IconNextExt;
	default:
		return wxNullIcon;
	}
}

wxString wxGISCartoMainCmd::GetCaption(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainCmdFullExtent:
		return wxString(_("&Full Extent"));
    case enumGISCartoMainCmdPrevExtent:
		return wxString(_("&Previous Extent"));
    case enumGISCartoMainCmdNextExtent:
		return wxString(_("&Next Extent"));
	default:
		return wxEmptyString;
	}
}

wxString wxGISCartoMainCmd::GetCategory(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainCmdFullExtent:
    case enumGISCartoMainCmdPrevExtent:
    case enumGISCartoMainCmdNextExtent:
		return wxString(_("Geography"));
	default:
		return NO_CATEGORY;
	}
}

bool wxGISCartoMainCmd::GetChecked(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainCmdFullExtent:
    case enumGISCartoMainCmdPrevExtent:
    case enumGISCartoMainCmdNextExtent:
		return false;
	default:
		return false;
	}
}

bool wxGISCartoMainCmd::GetEnabled(void)
{
    wxCHECK_MSG(m_pApp, false, wxT("Application pointer is null"));

	if(m_anMapWinIDs.empty())
	{
		WINDOWARRAY WinIDsArr = m_pApp->GetChildWindows();
        for(size_t i = 0; i < WinIDsArr.GetCount(); ++i)
		{
            wxWindow* pWnd = wxWindow::FindWindowById(WinIDsArr[i]);
            if(pWnd && pWnd->IsKindOf(wxCLASSINFO(wxGISMapView)))
            {
                m_anMapWinIDs.Add(WinIDsArr[i]);
            }
		}
	}

    //if(!m_pMapView)
    //{
    //    wxWindow* pWnd = m_pApp->GetRegisteredWindowByType(wxCLASSINFO(wxGISMapView));
    //    m_pMapView = wxDynamicCast(pWnd, wxGISMapView);
    //}

    //if(!m_pMapView)
    //    return false;

	switch(m_subtype)
	{
    case enumGISCartoMainCmdFullExtent:
        for(size_t i = 0; i < m_anMapWinIDs.GetCount(); ++i)
        {
            wxWindow* pWnd = wxWindow::FindWindowById(m_anMapWinIDs[i]);
            m_pMapView = wxDynamicCast(pWnd, wxGISMapView);
            if(m_pMapView && m_pMapView->IsShownOnScreen())// && pWnd->HasFocus()
                return true;
        }
		return false;
    case enumGISCartoMainCmdPrevExtent:
        for(size_t i = 0; i < m_anMapWinIDs.GetCount(); ++i)
        {
            wxWindow* pWnd = wxWindow::FindWindowById(m_anMapWinIDs[i]);
            m_pMapView = wxDynamicCast(pWnd, wxGISMapView);
            if(m_pMapView && m_pMapView->IsShownOnScreen() && m_pMapView->CanUndo())// && pWnd->HasFocus()
                return true;
        }			
        return false;
    case enumGISCartoMainCmdNextExtent:
        for(size_t i = 0; i < m_anMapWinIDs.GetCount(); ++i)
        {
            wxWindow* pWnd = wxWindow::FindWindowById(m_anMapWinIDs[i]);
            m_pMapView = wxDynamicCast(pWnd, wxGISMapView);
            if(m_pMapView && m_pMapView->IsShownOnScreen() && m_pMapView->CanUndo())// && pWnd->HasFocus()
                return true;
        }			
        return false;
    default:
		return false;
	}
    return false;
}

wxGISEnumCommandKind wxGISCartoMainCmd::GetKind(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainCmdFullExtent://Full Extent
    case enumGISCartoMainCmdPrevExtent://Previous Extent
    case enumGISCartoMainCmdNextExtent://Next Extent
		return enumGISCommandNormal;
	default:
		return enumGISCommandNormal;
	}
}

wxString wxGISCartoMainCmd::GetMessage(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainCmdFullExtent:
		return wxString(_("Full extent of the map"));
    case enumGISCartoMainCmdPrevExtent:
		return wxString(_("Previous extent of the map"));
    case enumGISCartoMainCmdNextExtent:
		return wxString(_("Next extent of the map"));
	default:
		return wxEmptyString;
	}
}

void wxGISCartoMainCmd::OnClick(void)
{
    wxCHECK_RET(m_pMapView, wxT("MapView pointer is null"));

	switch(m_subtype)
	{
    case enumGISCartoMainCmdFullExtent:
        if(m_pMapView)// && pWnd->HasFocus()
            return m_pMapView->SetFullExtent();
    case enumGISCartoMainCmdPrevExtent:
        if(m_pMapView)// && m_pMapView->IsShown() && pMapView->CanUndo())// && pWnd->HasFocus()
            return m_pMapView->Undo();
    case enumGISCartoMainCmdNextExtent:
        if(m_pMapView)// && m_pMapView->IsShown() && m_pMapView->CanRedo())// && pWnd->HasFocus()
            return m_pMapView->Redo();
	default:
		break;
	}
}

bool wxGISCartoMainCmd::OnCreate(wxGISApplicationBase* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISCartoMainCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainCmdFullExtent:
		return wxString(_("Full Extent"));
    case enumGISCartoMainCmdPrevExtent:
		return wxString(_("Previous Extent"));
    case enumGISCartoMainCmdNextExtent:
		return wxString(_("Next Extent"));
	default:
		return wxEmptyString;
	}
}

unsigned char wxGISCartoMainCmd::GetCount(void)
{
    return enumGISCartoMainCmdMax;
}

//--------------------------------------------------
// wxGISCartoMainTool
//--------------------------------------------------

//	0	//Zoom In
//	1	//Zoom Out
//	2	//Pan
//	3	//Info
//	4	//?

IMPLEMENT_DYNAMIC_CLASS(wxGISCartoMainTool, wxGISCommand)


wxGISCartoMainTool::wxGISCartoMainTool(void)
{
    m_subtype = 0;
    m_pMapView = NULL;
    m_pIdentifyView = NULL;
    m_bCheck = false;
}

wxGISCartoMainTool::~wxGISCartoMainTool(void)
{
}

wxIcon wxGISCartoMainTool::GetBitmap(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainToolZoomIn:
		if(!m_IconZoomIn.IsOk())
			m_IconZoomIn = wxIcon(zoomin_xpm);
		return m_IconZoomIn;
    case enumGISCartoMainToolZoomOut:
		if(!m_IconZoomOut.IsOk())
			m_IconZoomOut = wxIcon(zoomout_xpm);
		return m_IconZoomOut;
    case enumGISCartoMainToolPan:
		if(!m_IconPan.IsOk())
			m_IconPan = wxIcon(pan1_cur_xpm);
		return m_IconPan;
    case enumGISCartoMainToolIdentify:
		if(!m_IconInfo.IsOk())
			m_IconInfo = wxIcon(info_xpm);
		return m_IconInfo;
	default:
		return wxNullIcon;
	}
}

wxString wxGISCartoMainTool::GetCaption(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainToolZoomIn:
		return wxString(_("Zoom &In"));
    case enumGISCartoMainToolZoomOut:
		return wxString(_("Zoom &Out"));
    case enumGISCartoMainToolPan:
		return wxString(_("&Pan"));
    case enumGISCartoMainToolIdentify:
		return wxString(_("&Information"));
	default:
		return wxEmptyString;
	}
}

wxString wxGISCartoMainTool::GetCategory(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainToolZoomIn:
    case enumGISCartoMainToolZoomOut:
    case enumGISCartoMainToolPan:
    case enumGISCartoMainToolIdentify:
		return wxString(_("Geography"));
	default:
		return NO_CATEGORY;
	}
}

bool wxGISCartoMainTool::GetChecked(void)
{
	return m_bCheck;
}

bool wxGISCartoMainTool::GetEnabled(void)
{
	if(m_anMapWinIDs.empty())
	{
		WINDOWARRAY WinIDsArr = m_pApp->GetChildWindows();
        for(size_t i = 0; i < WinIDsArr.GetCount(); ++i)
		{
            wxWindow* pWnd = wxWindow::FindWindowById(WinIDsArr[i]);
            if(pWnd && pWnd->IsKindOf(wxCLASSINFO(wxGISMapView)))
            {
                m_anMapWinIDs.Add(WinIDsArr[i]);
            }
		}
	}
    
    //if(NULL == m_pMapView)
	//{
 //       wxWindow* pWnd = m_pApp->GetRegisteredWindowByType(wxCLASSINFO(wxGISMapView));
 //       m_pMapView = dynamic_cast<wxGISMapView*>(pWnd);
	//}

	if(NULL == m_pIdentifyView)
	{
        wxWindow* pWnd = m_pApp->GetRegisteredWindowByType(wxCLASSINFO(wxAxIdentifyView));
        m_pIdentifyView = dynamic_cast<wxAxIdentifyView*>(pWnd);
	}

	switch(m_subtype)
	{
    case enumGISCartoMainToolZoomIn://zoom in
    case enumGISCartoMainToolZoomOut://zoom out
    case enumGISCartoMainToolPan://pan
        for(size_t i = 0; i < m_anMapWinIDs.GetCount(); ++i)
        {
            wxWindow* pWnd = wxWindow::FindWindowById(m_anMapWinIDs[i]);
            m_pMapView = wxDynamicCast(pWnd, wxGISMapView);
            if(m_pMapView && m_pMapView->IsShownOnScreen())// && pWnd->HasFocus()
                return true;
        }
        return false;
    case enumGISCartoMainToolIdentify://info
            for(size_t i = 0; i < m_anMapWinIDs.GetCount(); ++i)
        {
            wxWindow* pWnd = wxWindow::FindWindowById(m_anMapWinIDs[i]);
            m_pMapView = wxDynamicCast(pWnd, wxGISMapView);
            if(m_pMapView && m_pMapView->IsShownOnScreen() && m_pIdentifyView)// && pWnd->HasFocus()
                return true;
        }
        return false;          
	default:
		return false;
	}
    return false;
}

wxGISEnumCommandKind wxGISCartoMainTool::GetKind(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainToolZoomIn://ZoomIn
    case enumGISCartoMainToolZoomOut://ZoomOut
    case enumGISCartoMainToolPan://Pan
    case enumGISCartoMainToolIdentify://Information
		return enumGISCommandCheck;
	default:
		return enumGISCommandNormal;
	}
}

wxString wxGISCartoMainTool::GetMessage(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainToolZoomIn:
		return wxString(_("Zoom map in"));
    case enumGISCartoMainToolZoomOut:
		return wxString(_("Zoom map out"));
    case enumGISCartoMainToolPan:
		return wxString(_("Pan map"));
    case enumGISCartoMainToolIdentify:
		return wxString(_("Feature information"));
	default:
		return wxEmptyString;
	}
}

void wxGISCartoMainTool::OnClick(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainToolZoomIn:
    case enumGISCartoMainToolZoomOut:
    case enumGISCartoMainToolPan:
    case enumGISCartoMainToolIdentify:
		break;
	default:
		break;
	}
}

bool wxGISCartoMainTool::OnCreate(wxGISApplicationBase* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISCartoMainTool::GetTooltip(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainToolZoomIn:
		return wxString(_("Zoom In"));
    case enumGISCartoMainToolZoomOut:
		return wxString(_("Zoom Out"));
    case enumGISCartoMainToolPan:
		return wxString(_("Pan"));
    case enumGISCartoMainToolIdentify:
		return wxString(_("Identify"));
	default:
		return wxEmptyString;
	}
}

unsigned char wxGISCartoMainTool::GetCount(void)
{
    return enumGISCartoMainToolMax;
}

wxCursor wxGISCartoMainTool::GetCursor(void)
{
	switch(m_subtype)
	{
    case enumGISCartoMainToolZoomIn:	//z_in
		if(!m_CurZoomIn.IsOk())
		{
			wxImage CursorImage = wxBitmap(zoom_in_cur_xpm).ConvertToImage();
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 6);
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 6);
			m_CurZoomIn = wxCursor(CursorImage);
		}
		return m_CurZoomIn;
    case enumGISCartoMainToolZoomOut:	//z_out
		if(!m_CurZoomIn.IsOk())
		{
			wxImage CursorImage = wxBitmap(zoom_out_cur_xpm).ConvertToImage();
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 6);
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 6);
			m_CurZoomOut = wxCursor(CursorImage);
		}
		return m_CurZoomOut;
    case enumGISCartoMainToolPan:	//pan
		if(!m_CurPan1.IsOk())
		{
			wxImage CursorImage = wxBitmap(pan1_cur_xpm).ConvertToImage();
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
			m_CurPan1 = wxCursor(CursorImage);
		}
		return m_CurPan1;
    case enumGISCartoMainToolIdentify:	//inf
		if(!m_CurInfo.IsOk())
		{
			wxImage CursorImage = wxBitmap(get_info_cur_xpm).ConvertToImage();
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 0);
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 0);
			m_CurInfo = wxCursor(CursorImage);
		}
		return m_CurInfo;
	default:
		return wxNullCursor;
	}
}

void wxGISCartoMainTool::SetChecked(bool bCheck)
{
	m_bCheck = bCheck;
	if(m_bCheck && m_pMapView)
		m_pMapView->SetCursor(GetCursor());
}

void wxGISCartoMainTool::OnMouseDown(wxMouseEvent& event)
{
	wxGISAppConfig oConfig = GetConfig();
    wxGISColor color(0, 0, 255, 255);
    int nWidth(2);

    event.Skip();
	switch(m_subtype)
	{
    case enumGISCartoMainToolZoomIn:	//z_in
		{
            if(oConfig.IsOk())
            {
                wxXmlNode* pNode = oConfig.GetConfigNode(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband")));                
                color = GetColorValue( pNode, wxT("color"),color);
                nWidth = GetDecimalValue( pNode, wxT("width"), nWidth);
            }

            wxGISRubberEnvelope RubberEnvelope(wxPen(color.GetColour(), nWidth), m_pMapView, m_pMapView->GetDisplay(), m_pMapView->GetSpatialReference());			
			wxGISGeometry Geom = RubberEnvelope.TrackNew( event.GetX(), event.GetY() );
            if(!Geom.IsOk())
				break;
            OGREnvelope Env = Geom.GetEnvelope();
			if(IsDoubleEquil(Env.MaxX, Env.MinX) || IsDoubleEquil(Env.MaxY, Env.MinY))
			{
				OGREnvelope CurrentEnv = m_pMapView->GetCurrentExtent();
				double widthdiv4 = (CurrentEnv.MaxX - CurrentEnv.MinX) / 4;
				double heightdiv4 = (CurrentEnv.MaxY - CurrentEnv.MinY) / 4;

				Env.MinX -= widthdiv4;
				Env.MinY -= heightdiv4;
				Env.MaxX += widthdiv4;
				Env.MaxY += heightdiv4;
			}

            wxDC* pDC = new wxClientDC(m_pMapView);
			if(m_pMapView->GetScaleRatio(Env, *pDC) > 1.0)
                m_pMapView->Do(Env);
            wxDELETE(pDC);
		}
		break;
    case enumGISCartoMainToolZoomOut:	//z_out
		{
            if(oConfig.IsOk())
            {
                wxXmlNode* pNode = oConfig.GetConfigNode(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband")));                
                color = GetColorValue( pNode, wxT("color"),color);
                nWidth = GetDecimalValue( pNode, wxT("width"), nWidth);
            }

            wxGISRubberEnvelope RubberEnvelope(wxPen(color.GetColour(), nWidth), m_pMapView, m_pMapView->GetDisplay(), m_pMapView->GetSpatialReference());			
			wxGISGeometry Geom = RubberEnvelope.TrackNew( event.GetX(), event.GetY() );
            if(!Geom.IsOk())
				break;
            OGREnvelope Env = Geom.GetEnvelope();
			OGREnvelope CurrentEnv = m_pMapView->GetCurrentExtent();
			OGREnvelope NewEnv;
			NewEnv.MinX = CurrentEnv.MinX + CurrentEnv.MinX - Env.MinX;
			NewEnv.MinY = CurrentEnv.MinY + CurrentEnv.MinY - Env.MinY;
			NewEnv.MaxX = CurrentEnv.MaxX + CurrentEnv.MaxX - Env.MaxX;
			NewEnv.MaxY = CurrentEnv.MaxY + CurrentEnv.MaxY - Env.MaxY;
            m_pMapView->Do(NewEnv);
		}
		break;
    case enumGISCartoMainToolPan:	//pan
			if(!m_CurPan2.IsOk())
			{
				wxImage CursorImage = wxBitmap(pan2_cur_xpm).ConvertToImage();
				CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
				CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
				m_CurPan2 = wxCursor(CursorImage);
			}
			m_pMapView->SetCursor(m_CurPan2);
			m_pMapView->PanStart(event.GetPosition());
			break;
    case enumGISCartoMainToolIdentify:	//inf
		{
            if(oConfig.IsOk())
            {
                wxXmlNode* pNode = oConfig.GetConfigNode(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/rabberband")));                
                color = GetColorValue( pNode, wxT("color"),color);
                nWidth = GetDecimalValue( pNode, wxT("width"), nWidth);
            }

            wxGISRubberEnvelope RubberEnvelope(wxPen(color.GetColour(), nWidth), m_pMapView, m_pMapView->GetDisplay(), m_pMapView->GetSpatialReference());			
			wxGISGeometry Geom = RubberEnvelope.TrackNew( event.GetX(), event.GetY() );
            wxWindow* pWnd = wxStaticCast(m_pIdentifyView, wxWindow);
            if(!m_pApp->IsApplicationWindowShown(pWnd))
            {
                m_pApp->ShowApplicationWindow(pWnd);
            }
			if(m_pIdentifyView)
				m_pIdentifyView->Identify(m_pMapView, Geom);
		}
		break;
	default:
		break;
	}
}

void wxGISCartoMainTool::OnMouseUp(wxMouseEvent& event)
{
//    event.Skip();
	switch(m_subtype)
	{
    case enumGISCartoMainToolZoomIn:	//z_in
		break;
    case enumGISCartoMainToolZoomOut:	//z_out
		break;
    case enumGISCartoMainToolPan:	//pan
		m_pMapView->PanStop(event.GetPosition());
		m_pMapView->SetCursor(GetCursor());
		break;
    case enumGISCartoMainToolIdentify:	//inf
		break;
	default:
		break;
	}
}

void wxGISCartoMainTool::OnMouseMove(wxMouseEvent& event)
{
    //event.Skip();
	switch(m_subtype)
	{
    case enumGISCartoMainToolZoomIn:	//z_in
		break;
    case enumGISCartoMainToolZoomOut:	//z_out
		break;
    case enumGISCartoMainToolPan:	//pan
		if(event.Dragging())
			m_pMapView->PanMoveTo(event.GetPosition());
		break;
    case enumGISCartoMainToolIdentify:	//inf
		break;
	default:
		break;
	}
}

void wxGISCartoMainTool::OnMouseDoubleClick(wxMouseEvent& event)
{
    event.Skip();
}

//--------------------------------------------------
// wxGISCartoFrameTool
//--------------------------------------------------

//	0	//Rotate
//	1	//Cancel Rotate
//	2	//Input Rotate Angle
//	3	//?

IMPLEMENT_DYNAMIC_CLASS(wxGISCartoFrameTool, wxGISCommand)


wxGISCartoFrameTool::wxGISCartoFrameTool(void) : m_pMapView(NULL), m_bCheck(false)
{
}

wxGISCartoFrameTool::~wxGISCartoFrameTool(void)
{
}

wxIcon wxGISCartoFrameTool::GetBitmap(void)
{
	switch(m_subtype)
	{
    case enumGISCartoFrameToolRotate:
		if(!m_IconRotate.IsOk())
			m_IconRotate = wxIcon(map_rotate_xpm);
		return m_IconRotate;
    case enumGISCartoFrameToolCancelRotate:
		if(!m_IconCancelRotate.IsOk())
			m_IconCancelRotate = wxIcon(cancel_map_rotate_xpm);
		return m_IconCancelRotate;
    case enumGISCartoFrameToolInputRotateAngle:
	default:
		return wxNullIcon;
	}
}

wxString wxGISCartoFrameTool::GetCaption(void)
{
	switch(m_subtype)
	{
    case enumGISCartoFrameToolRotate:
		return wxString(_("&Rotate"));
    case enumGISCartoFrameToolCancelRotate:
		return wxString(_("Cancel rotate"));
    case enumGISCartoFrameToolInputRotateAngle:
		return wxString(_("Input rotate angle"));
	default:
		return wxEmptyString;
	}
}

wxString wxGISCartoFrameTool::GetCategory(void)
{
	switch(m_subtype)
	{
    case enumGISCartoFrameToolRotate:
    case enumGISCartoFrameToolCancelRotate:
    case enumGISCartoFrameToolInputRotateAngle:
		return wxString(_("View"));
	default:
		return NO_CATEGORY;
	}
}

bool wxGISCartoFrameTool::GetChecked(void)
{
	return m_bCheck;
}

bool wxGISCartoFrameTool::GetEnabled(void)
{
	if(m_anMapWinIDs.empty())
	{
		WINDOWARRAY WinIDsArr = m_pApp->GetChildWindows();
        for(size_t i = 0; i < WinIDsArr.GetCount(); ++i)
		{
            wxWindow* pWnd = wxWindow::FindWindowById(WinIDsArr[i]);
            if(pWnd && pWnd->IsKindOf(wxCLASSINFO(wxGISMapView)))
            {
                m_anMapWinIDs.Add(WinIDsArr[i]);
            }
		}
	}



	switch(m_subtype)
	{
    case enumGISCartoFrameToolRotate:
    case enumGISCartoFrameToolCancelRotate:
			//check if angle == 0
    case enumGISCartoFrameToolInputRotateAngle:
        for(size_t i = 0; i < m_anMapWinIDs.GetCount(); ++i)
        {
            wxWindow* pWnd = wxWindow::FindWindowById(m_anMapWinIDs[i]);
            m_pMapView = wxDynamicCast(pWnd, wxGISMapView);
            if(m_pMapView && m_pMapView->IsShownOnScreen() && m_pMapView->CanRotate())
                return true;
        }
        return false;
	default:
		return false;
	}
}

wxGISEnumCommandKind wxGISCartoFrameTool::GetKind(void)
{
	switch(m_subtype)
	{
    case enumGISCartoFrameToolRotate://Rotate
		return enumGISCommandCheck;
    case enumGISCartoFrameToolCancelRotate://Cancel rotate
		return enumGISCommandNormal;
    case enumGISCartoFrameToolInputRotateAngle://Input rotate
		return enumGISCommandControl;
	default:
		return enumGISCommandNormal;
	}
}

wxString wxGISCartoFrameTool::GetMessage(void)
{
	switch(m_subtype)
	{
    case enumGISCartoFrameToolRotate:
		return wxString(_("Rotate map frame"));
    case enumGISCartoFrameToolCancelRotate:
		return wxString(_("Cancel rotate map frame"));
    case enumGISCartoFrameToolInputRotateAngle:
		return wxString(_("Input map frame rotate angle"));
	default:
		return wxEmptyString;
	}
}

void wxGISCartoFrameTool::OnClick(void)
{
	switch(m_subtype)
	{
    case enumGISCartoFrameToolRotate:
		break;
    case enumGISCartoFrameToolCancelRotate:
		if(m_pMapView)
		{
			m_pMapView->SetRotate(0.0);
		}
		break;
    case enumGISCartoFrameToolInputRotateAngle:
		break;
	default:
		break;
	}
}

bool wxGISCartoFrameTool::OnCreate(wxGISApplicationBase* pApp)
{
	m_pApp = pApp;

	return true;
}

wxString wxGISCartoFrameTool::GetTooltip(void)
{
	switch(m_subtype)
	{
    case enumGISCartoFrameToolRotate:
		return wxString(_("Rotate map frame"));
    case enumGISCartoFrameToolCancelRotate:
		return wxString(_("Cancel rotate map frame"));
    case enumGISCartoFrameToolInputRotateAngle:
		return wxString(_("Input map frame rotate angle"));
	default:
		return wxEmptyString;
	}
}

unsigned char wxGISCartoFrameTool::GetCount(void)
{
    return enumGISCartoFrameToolMax;
}

wxCursor wxGISCartoFrameTool::GetCursor(void)
{
	switch(m_subtype)
	{
    case enumGISCartoFrameToolRotate:	//rotate
		if(!m_CurRotate.IsOk())
		{
			wxImage CursorImage = wxBitmap(rotate_cur_xpm).ConvertToImage();
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_X, 7);
			CursorImage.SetOption(wxIMAGE_OPTION_CUR_HOTSPOT_Y, 7);
			m_CurRotate = wxCursor(CursorImage);
		}
		return m_CurRotate;
	default:
		return wxNullCursor;
	}
}

void wxGISCartoFrameTool::SetChecked(bool bCheck)
{
	m_bCheck = bCheck;
	if(m_bCheck)
		m_pMapView->SetCursor(GetCursor());
}

void wxGISCartoFrameTool::OnMouseDown(wxMouseEvent& event)
{
    event.Skip();
	switch(m_subtype)
	{
    case enumGISCartoFrameToolRotate:	//rotate
		m_pMapView->RotateStart(event.GetPosition());
		break;
	default:
		break;
	}
}

void wxGISCartoFrameTool::OnMouseUp(wxMouseEvent& event)
{
//    event.Skip();
	switch(m_subtype)
	{
    case enumGISCartoFrameToolRotate:	//rotate
		m_pMapView->RotateStop(event.GetPosition());
		m_pMapView->SetCursor(GetCursor());
		break;
	default:
		break;
	}
}

void wxGISCartoFrameTool::OnMouseMove(wxMouseEvent& event)
{
    //event.Skip();
	switch(m_subtype)
	{
    case enumGISCartoFrameToolRotate:	//rotate
		if(event.Dragging())
		{
			m_pMapView->RotateBy(event.GetPosition());
		}
		break;
	default:
		break;
	}
}

void wxGISCartoFrameTool::OnMouseDoubleClick(wxMouseEvent& event)
{
    event.Skip();
}

IToolBarControl* wxGISCartoFrameTool::GetControl(void)
{
	switch(m_subtype)
	{
    case enumGISCartoFrameToolInputRotateAngle:
		//if(!m_pRotationComboBox)
		{
			wxArrayString ValuesArray;
			wxGISRotationComboBox* pRotationComboBox = new wxGISRotationComboBox(dynamic_cast<wxWindow*>(m_pApp), wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 65, 22 ), ValuesArray);
			return static_cast<IToolBarControl*>(pRotationComboBox);
		}
	default:
		return NULL;
	}
}

wxString wxGISCartoFrameTool::GetToolLabel(void)
{
	switch(m_subtype)
	{
    case enumGISCartoFrameToolInputRotateAngle:
		return wxEmptyString;
	default:
		return wxEmptyString;
	}
}

bool wxGISCartoFrameTool::HasToolLabel(void)
{
	switch(m_subtype)
	{
    case enumGISCartoFrameToolInputRotateAngle:
		return false;
	default:
		return false;
	}
}


//-----------------------------------------------------------------------------------------
// wxGISRotationComboBox
//-----------------------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISRotationComboBox, wxComboBox)

BEGIN_EVENT_TABLE(wxGISRotationComboBox, wxComboBox)
	EVT_TEXT_ENTER(wxID_ANY, wxGISRotationComboBox::OnTextEnter)
	EVT_COMBOBOX(wxID_ANY, wxGISRotationComboBox::OnTextEnter)
	EVT_MXMAP_ROTATED(wxGISRotationComboBox::OnMapRotated)
    //EVT_SET_FOCUS//EVT_KILL_FOCUS
END_EVENT_TABLE()

wxGISRotationComboBox::wxGISRotationComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, const wxArrayString& choices, long style, const wxValidator& validator, const wxString& name) : wxComboBox(parent, id, value, pos, size, choices, style, validator, name)
{
	AppendString(wxT("0.00"));
	AppendString(wxT("45.00"));
	AppendString(wxT("90.00"));
	AppendString(wxT("135.00"));
	AppendString(wxT("180.00"));
	AppendString(wxT("225.00"));
	AppendString(wxT("270.00"));
	AppendString(wxT("315.00"));

	SetValue(wxT("0.00"));
}

wxGISRotationComboBox::~wxGISRotationComboBox(void)
{
}

void wxGISRotationComboBox::OnTextEnter(wxCommandEvent& event)
{
    wxGISMapView* pMapView = NULL;
    for(size_t i = 0; i < m_anMapWinIDs.size(); ++i)
    {
        wxWindow* pWnd = wxWindow::FindWindowById(m_anMapWinIDs[i].first);
        pMapView = wxDynamicCast(pWnd, wxGISMapView);
        if(pMapView->IsShownOnScreen())
        {
		    wxString sVal = GetValue();
		    double dAngleGrad = wxAtof(sVal);
		    wxString sFloatVal = wxString::Format(wxT("%.2f"), dAngleGrad);
		    double dAngleRad = dAngleGrad * PIDEG;
		    pMapView->SetRotate(-dAngleRad);
		    //check uniq
		    if(FindString(sFloatVal) == wxNOT_FOUND)
			    AppendString(sFloatVal);
		    SetStringSelection(sFloatVal);
        }
    }
}

void wxGISRotationComboBox::OnMapRotated(wxMxMapViewUIEvent& event)
{
	UpdateAngle(event.GetId());
}

void wxGISRotationComboBox::Activate(wxGISApplicationBase* pApp)
{
  	if(m_anMapWinIDs.empty())
	{
		WINDOWARRAY WinIDsArr = pApp->GetChildWindows();
        for(size_t i = 0; i < WinIDsArr.GetCount(); ++i)
		{
            wxWindow* pWnd = wxWindow::FindWindowById(WinIDsArr[i]);
            if(pWnd && pWnd->IsKindOf(wxCLASSINFO(wxGISMapView)))
            {
                wxGISMapView* pMapView = wxDynamicCast(pWnd, wxGISMapView);
                long nConnectionPointMapCookie = pMapView->Advise(this);
                m_anMapWinIDs.push_back(std::make_pair(pWnd->GetId(), nConnectionPointMapCookie));
            }
		}
	}
}

void wxGISRotationComboBox::Deactivate(void)
{
    for(size_t i = 0; i < m_anMapWinIDs.size(); ++i)
    {
        wxWindow* pWnd = wxWindow::FindWindowById(m_anMapWinIDs[i].first);
        wxGISMapView* pMapView = wxDynamicCast(pWnd, wxGISMapView);
        if(pMapView)
        {
		    pMapView->Unadvise(m_anMapWinIDs[i].second);
        }
    }
}

void wxGISRotationComboBox::UpdateAngle(int nWinId)
{
    wxWindow* pWnd = wxWindow::FindWindowById(nWinId);
    wxGISMapView* pMapView = wxDynamicCast(pWnd, wxGISMapView);
    if(pMapView)
	{
		double dRotate = pMapView->GetCurrentRotate();
		wxString sVal = wxString::Format(wxT("%.2f"), dRotate * DEGPI);
		SetValue(sVal);
	}
}


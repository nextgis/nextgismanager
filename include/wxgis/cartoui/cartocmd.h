/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Carto Main Commands & tools class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011-2013 Bishop
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
#include "wxgis/cartoui/identifydlg.h"

/** @class wxGISRotationComboBox 

    The frame angle rotation input combobox.

    @library {cartoui}
*/

class wxGISRotationComboBox : 
	public wxComboBox,
	public IToolBarControl
{
    DECLARE_CLASS(wxGISRotationComboBox)
public:
	wxGISRotationComboBox(wxWindow* parent, wxWindowID id, const wxString& value, const wxPoint& pos, const wxSize& size, const wxArrayString& choices, long style = wxTE_PROCESS_ENTER, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT("LocationComboBox"));
	virtual ~wxGISRotationComboBox(void);
	virtual void UpdateAngle(int nWinId);
	//events
	void OnTextEnter(wxCommandEvent& event);
	void OnMapRotated(wxMxMapViewUIEvent& event);
	//IToolBarControl
	virtual void Activate(wxGISApplicationBase* pApp);
	virtual void Deactivate(void);
protected:
	//wxGISMapView* m_pMapView;
	//long m_nConnectionPointMapCookie;
    wxVector<std::pair<wxWindowID, long>> m_anMapWinIDs;
private:
    DECLARE_EVENT_TABLE()
};

enum wxGISEnumCartoMainCmdType{
    enumGISCartoMainCmdFullExtent = 0,
    enumGISCartoMainCmdPrevExtent,
    enumGISCartoMainCmdNextExtent,
    enumGISCartoMainCmdMax
};


/** @class wxGISCartoMainCmd
    
    The carto main commands.

    @library{cartoui}
*/

class WXDLLIMPEXP_GIS_CTU wxGISCartoMainCmd :
	public wxGISCommand
{
	DECLARE_DYNAMIC_CLASS(wxGISCartoMainCmd)
public:
	wxGISCartoMainCmd(void);
	virtual ~wxGISCartoMainCmd(void);
	//wxGISCommand
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
private:
	wxGISApplicationBase* m_pApp;
	wxIcon m_IconFullExt, m_IconNextExt, m_IconPrevExt;
	wxGISMapView* m_pMapView;    
    WINDOWARRAY m_anMapWinIDs;
};

enum wxGISEnumCartoMainToolType{
    enumGISCartoMainToolZoomIn = 0,
    enumGISCartoMainToolZoomOut,
    enumGISCartoMainToolPan,
    enumGISCartoMainToolIdentify,
    enumGISCartoMainToolMax
};

/** @class wxGISCartoMainTool
    
    The carto main tool.

    @library{cartoui}
*/

class WXDLLIMPEXP_GIS_CTU wxGISCartoMainTool :
	public ITool
{
	DECLARE_DYNAMIC_CLASS(wxGISCartoMainTool)
public:
	wxGISCartoMainTool(void);
	virtual ~wxGISCartoMainTool(void);
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
	wxIcon m_IconZoomIn, m_IconZoomOut, m_IconPan, m_IconInfo;
	wxCursor m_CurZoomIn, m_CurZoomOut, m_CurPan1, m_CurPan2, m_CurInfo;
	wxGISMapView* m_pMapView;
    WINDOWARRAY m_anMapWinIDs;
    wxAxIdentifyView* m_pIdentifyView;
	bool m_bCheck;
};

enum wxGISEnumCartoFrameToolType{
    enumGISCartoFrameToolRotate = 0,
    enumGISCartoFrameToolCancelRotate,
    enumGISCartoFrameToolInputRotateAngle,
    enumGISCartoFrameToolMax
};


/** @class wxGISCartoFrameTool

    The carto frame tools and commands.

    @library{cartoui}
*/

class WXDLLIMPEXP_GIS_CTU wxGISCartoFrameTool :
	public ITool,
	public IToolControl
{
	DECLARE_DYNAMIC_CLASS(wxGISCartoFrameTool)
public:
	wxGISCartoFrameTool(void);
	virtual ~wxGISCartoFrameTool(void);
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
	//IToolControl
	virtual IToolBarControl* GetControl(void);
	virtual wxString GetToolLabel(void);
	virtual bool HasToolLabel(void);
private:
	wxGISApplicationBase* m_pApp;
	wxIcon m_IconRotate, m_IconCancelRotate;
	wxCursor m_CurRotate;
	wxGISMapView* m_pMapView;
    WINDOWARRAY m_anMapWinIDs;
	bool m_bCheck;
};


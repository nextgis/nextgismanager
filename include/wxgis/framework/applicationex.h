/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISApplicationEx class. Add AUI managed frames & etc.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2012 Bishop
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

#include "wxgis/framework/application.h"

#include "wx/aui/aui.h"
#include "wx/artprov.h"

/** \class wxGISApplicationEx applicationex.h
 *   \brief An application framework class with aui mngr.
 */
class WXDLLIMPEXP_GIS_FRW wxGISApplicationEx :	public wxGISApplication
{
    DECLARE_CLASS(wxGISApplicationEx)
public:	
	wxGISApplicationEx(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);
	virtual ~wxGISApplicationEx(void);
	virtual wxAuiManager* GetAuiManager(void) const{return (wxAuiManager*)&m_mgr;};
	virtual void ShowPane(wxWindow* pWnd, bool bShow = true);
	virtual void ShowPane(const wxString& sName, bool bShow = true);
	virtual bool IsPaneShown(const wxString& sName);
	//wxGISApplication
	virtual void RemoveCommandBar(wxGISCommandBar* pBar);
	virtual bool AddCommandBar(wxGISCommandBar* pBar);
    //events 
    virtual void OnClose(wxCloseEvent& event);
	//wxGISApplication
	virtual void Customize(void);
	virtual void ShowStatusBar(bool bShow);
	virtual void ShowApplicationWindow(wxWindow* pWnd, bool bShow = true);
	virtual bool IsApplicationWindowShown(wxWindow* pWnd);
    virtual wxString GetAppVersionString(void) const;
    virtual bool CreateApp(void);
    virtual bool SetupSys(const wxString &sSysPath);
    virtual void SetDebugMode(bool bDebugMode);
    virtual bool SetupLog(const wxString &sLogPath, const wxString &sNamePrefix = wxEmptyString);
protected:
	virtual void SerializeFramePosEx(bool bSave = false);
protected:
	wxAuiManager m_mgr;
};

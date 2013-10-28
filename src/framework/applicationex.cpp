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
#include "wxgis/framework/applicationex.h"
#include "wxgis/framework/customizedlg.h"
#include "wxgis/framework/toolbarmenu.h"
#include "wxgis/version.h"
#include "wxgisdefs.h"

#ifdef wxGIS_USE_PROJ
    #include "proj_api.h"
#endif

#include "cpl_string.h"

//-----------------------------------------------
// wxGISApplicationEx
//-----------------------------------------------
IMPLEMENT_CLASS(wxGISApplicationEx, wxGISApplication)

wxGISApplicationEx::wxGISApplicationEx(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxGISApplication(parent, id, title, pos, size, style)
{
}

wxGISApplicationEx::~wxGISApplicationEx(void)
{
}

void wxGISApplicationEx::SerializeFramePosEx(bool bSave)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return;

	wxXmlNode *pPerspectiveXmlNode = oConfig.GetConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/frame/perspective")));
	if(bSave)
		oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/frame/perspective/data")), m_mgr.SavePerspective());
	else
	{
		wxString wxPerspective = oConfig.Read(enumGISHKCU, GetAppName() + wxString(wxT("/frame/perspective/data")), wxEmptyString);
		m_mgr.LoadPerspective(wxPerspective);
	}
}

void wxGISApplicationEx::Customize(void)
{
	wxGISCustomizeDlg dialog(this);
	if(dialog.ShowModal() == wxID_OK )
	{
		if(m_pGISAcceleratorTable->HasChanges())
			SetAcceleratorTable(m_pGISAcceleratorTable->GetAcceleratorTable());
		wxGISToolBarMenu* pToolBarMenu =  static_cast<wxGISToolBarMenu*>(GetCommandBar(TOOLBARMENUNAME));
		if(pToolBarMenu)
			pToolBarMenu->Update();
	}
}

void wxGISApplicationEx::RemoveCommandBar(wxGISCommandBar* pBar) 
{
	for(size_t i = 0; i < m_CommandBarArray.GetCount(); ++i)
	{
		if(m_CommandBarArray[i] == pBar)
		{
			switch(m_CommandBarArray[i]->GetType())
			{
			case enumGISCBMenubar:
				m_pMenuBar->RemoveMenu(pBar);
				break;
			case enumGISCBToolbar:
				m_mgr.DetachPane(dynamic_cast<wxWindow*>(pBar));
				m_mgr.Update();
				break;
			case enumGISCBContextmenu:
			case enumGISCBSubMenu:
			case enumGISCBNone:
				break;
			}
			wxDELETE(pBar);
            m_CommandBarArray.RemoveAt(i);
			break;
		}
	}
}

void wxGISApplicationEx::ShowPane(wxWindow* pWnd, bool bShow)
{
	m_mgr.GetPane(pWnd).Show(bShow);
	m_mgr.Update();
}

void wxGISApplicationEx::ShowPane(const wxString& sName, bool bShow)
{
	m_mgr.GetPane(sName).Show(bShow);
	m_mgr.Update();
}

bool wxGISApplicationEx::IsPaneShown(const wxString& sName)
{
	return m_mgr.GetPane(sName).IsShown();
}

bool wxGISApplicationEx::IsApplicationWindowShown(wxWindow* pWnd)
{
	return m_mgr.GetPane(pWnd).IsShown();
}

bool wxGISApplicationEx::AddCommandBar(wxGISCommandBar* pBar)
{
	if(!pBar)
		return false;
	m_CommandBarArray.Add(pBar);
	switch(pBar->GetType())
	{
	case enumGISCBMenubar:
		m_pMenuBar->AddMenu(pBar);
		break;
	case enumGISCBToolbar:
		{
			wxGISToolBar* pToolBar = static_cast<wxGISToolBar*>(pBar);
			if(pToolBar)
			{
				m_mgr.AddPane(pToolBar, wxAuiPaneInfo().Name(pToolBar->GetName()).Caption(pToolBar->GetCaption()).ToolbarPane().Top().Position(100).LeftDockable(pToolBar->GetLeftDockable()).RightDockable(pToolBar->GetRightDockable()));
				m_mgr.Update();
			}
		}
		break;
	case enumGISCBContextmenu:
	case enumGISCBSubMenu:
	case enumGISCBNone:
		break;
	}
	return true;
}

void wxGISApplicationEx::ShowStatusBar(bool bShow)
{
	wxGISApplication::ShowStatusBar(bShow);
	m_mgr.Update();
}

void wxGISApplicationEx::ShowApplicationWindow(wxWindow* pWnd, bool bShow)
{
	ShowPane(pWnd, bShow);
}

bool wxGISApplicationEx::CreateApp(void)
{
	m_mgr.SetManagedWindow(this);

    wxGISApplication::CreateApp();

	wxLogMessage(_("wxGISApplicationEx: Start. Creating main application frame..."));

	for(size_t i = 0; i < m_CommandBarArray.GetCount(); ++i)
	{
		if(m_CommandBarArray[i]->GetType() == enumGISCBToolbar)
		{
			wxGISToolBar* pToolBar = static_cast<wxGISToolBar*>(m_CommandBarArray[i]);
			if(pToolBar)
			{
				m_mgr.AddPane(pToolBar, wxAuiPaneInfo().Name(pToolBar->GetName()).Caption(pToolBar->GetCaption()).ToolbarPane().Top().Position(i).LeftDockable(pToolBar->GetLeftDockable()).RightDockable(pToolBar->GetRightDockable()).BestSize(-1,-1));
				//pToolBar->Activate(this);
			}
		}
	}

	//SerializeFramePosEx(false);

	//m_mgr.Update();

	wxLogMessage(_("wxGISApplicationEx: Creation complete"));

    return true;
}

void wxGISApplicationEx::OnClose(wxCloseEvent& event)
{
    event.Skip();

 	for(size_t i = 0; i < m_CommandBarArray.GetCount(); ++i)
	{
		if(m_CommandBarArray[i]->GetType() == enumGISCBToolbar)
		{
			wxGISToolBar* pToolBar = static_cast<wxGISToolBar*>(m_CommandBarArray[i]);
			if(pToolBar)
				pToolBar->Deactivate();
		}
	}

    for(size_t i = 0; i < m_anWindowsIDs.GetCount(); ++i)
    {
        wxWindow* pWnd = wxWindow::FindWindowById(m_anWindowsIDs[i]);
        IView* pView = dynamic_cast<IView*>(pWnd);
        if(pView)
            pView->Deactivate();
    }

	wxGISApplication::OnClose(event);

 	SerializeFramePosEx(true);
    m_mgr.UnInit();
}

bool wxGISApplicationEx::SetupSys(const wxString &sSysPath)
{
    if(!wxGISApplication::SetupSys(sSysPath))
        return false;
	wxString sGdalDataDir = sSysPath + wxFileName::GetPathSeparator() + wxString(wxT("gdal")) + wxFileName::GetPathSeparator();
	CPLSetConfigOption("GDAL_DATA", sGdalDataDir.mb_str(wxConvUTF8) );
#ifdef wxGIS_USE_PROJ
	sGdalDataDir = sSysPath + wxFileName::GetPathSeparator() + wxString(wxT("proj")) + wxFileName::GetPathSeparator();
	//CPLSetConfigOption("PROJ_LIB", sGdalDataDir.mb_str(wxConvUTF8) );
    CPLString pszPROJ_LIB(sGdalDataDir.mb_str(wxConvUTF8));
    const char *path = pszPROJ_LIB.c_str();
    pj_set_searchpath(1, &path);
#endif

#ifndef CPL_RECODE_ICONV
    //the gdal compiled without iconv support
    //we should recode string by ourselthes
    CPLSetConfigOption("SHAPE_ENCODING", CPL_ENC_ASCII);
    CPLSetConfigOption("DXF_ENCODING", CPL_ENC_ASCII);
#endif //CPL_RECODE_ICONV
    return true;
}

void wxGISApplicationEx::SetDebugMode(bool bDebugMode)
{
	CPLSetConfigOption("CPL_DEBUG", bDebugMode == true ? "ON" : "OFF");
	CPLSetConfigOption("CPL_TIMESTAMP", "ON");
	CPLSetConfigOption("CPL_LOG_ERRORS", bDebugMode == true ? "ON" : "OFF");
}

bool wxGISApplicationEx::SetupLog(const wxString &sLogPath, const wxString &sNamePrefix)
{
    if(!wxGISApplication::SetupLog(sLogPath, sNamePrefix))
        return false;
	wxString sCPLLogPath = sLogPath + wxFileName::GetPathSeparator() + wxString(wxT("gdal_log_cat.txt"));
	CPLString szCPLLogPath(sCPLLogPath.mb_str(wxConvUTF8));
	CPLSetConfigOption("CPL_LOG", szCPLLogPath );
    return true;
}

wxString wxGISApplicationEx::GetAppVersionString(void) const
{
    return wxString(wxGIS_VERSION_NUM_DOT_STRING_T);
}
/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISApplicationEx class. Add AUI managed frames & etc.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2012 Dmitry Baryshnikov
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

#include "wxgis/framework/applicationex.h"
#include "wxgis/framework/customizedlg.h"
#include "wxgis/framework/toolbarmenu.h"
#include "wxgis/version.h"

#ifdef wxGIS_USE_PROJ
    #include "proj_api.h"
#endif

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

        //update captions for current locale
        wxAuiPaneInfoArray arr = m_mgr.GetAllPanes();
        for (size_t i = 0; i < arr.GetCount(); ++i)
        {
			IView *pView = dynamic_cast<IView*>(arr[i].window);
            if(pView)
			{
				m_mgr.GetPane(arr[i].name).Caption(pView->GetViewName());
			}
			else
			{
				wxGISCommandBar* pCommandBar = dynamic_cast<wxGISCommandBar*>(arr[i].window);
				if(pCommandBar)
				{
					m_mgr.GetPane(arr[i].name).Caption(pCommandBar->GetCaption());
				}
			}	
        }
	}
}

void wxGISApplicationEx::Customize(void)
{
	wxGISCustomizeDlg dialog(this);
	if(dialog.ShowModal() == wxID_OK )
	{
		if(m_pGISAcceleratorTable->HasChanges())
        {
            SetAcceleratorTable(m_pGISAcceleratorTable->GetAcceleratorTable());
            UpdateAccelerators();
        }
        wxGISToolBarMenu* pToolBarMenu =  static_cast<wxGISToolBarMenu*>(GetCommandBar(TOOLBARMENUNAME));
		if(pToolBarMenu)
			pToolBarMenu->Update();
	}
}

void wxGISApplicationEx::CollectToolbars(std::set<ToolbarFitInfo> &result)
{
    int nMaxPosX = 0;
	for(size_t i = 0; i < m_CommandBarArray.GetCount(); ++i)
	{
		if(m_CommandBarArray[i] && m_CommandBarArray[i]->GetType() == enumGISCBToolbar)
        {
            wxLogDebug(m_CommandBarArray[i]->GetName());
            const wxAuiPaneInfo &info = m_mgr.GetPane(m_CommandBarArray[i]->GetName());
            if (info.IsShown())
            {
                ToolbarFitInfo f;
                f.row = info.dock_row;
                f.rect = info.rect;
                if(f.rect.GetX() > nMaxPosX)
                    nMaxPosX = f.rect.GetX();
                if(info.rect.width == 0)
                {
                    wxSize oSize = info.window->GetBestSize();
                    f.rect.width = oSize.GetWidth() + 1;
                    f.rect.height = oSize.GetHeight() + 1;
                    f.rect.SetX(nMaxPosX + 1);
                    nMaxPosX += f.rect.width;
                }

                f.window = info.window;
                result.insert(f);
                wxLogDebug(wxT("%s x %d, y %d, h %d, w %d"), m_CommandBarArray[i]->GetName().c_str(), f.rect.x, f.rect.y, f.rect.height, f.rect.width);
            }
        }
	}
}


void wxGISApplicationEx::FitToolbars()
{
    std::set<ToolbarFitInfo> sorted;
    CollectToolbars(sorted);
    if (sorted.empty())
        return;

    int maxWidth = GetSize().x;

    // move all toolbars to the left as possible and add the non-fitting to a list
    wxVector<ToolbarRowInfo> rows;
    wxVector<wxWindow*> nonFitingToolbars;
    for (std::set<ToolbarFitInfo>::const_iterator it = sorted.begin(); it != sorted.end(); ++it)
    {
        wxAuiPaneInfo &pane = m_mgr.GetPane(it->window);
        int row = pane.dock_row;
        while (static_cast<int>(rows.size()) <= row)
            rows.push_back(ToolbarRowInfo(0, 0));

        int maxX = rows[row].width + it->rect.width;
        if (maxX > maxWidth)
            nonFitingToolbars.push_back(it->window);
        else
        {
            rows[row].width = maxX;
            pane.Position(rows[row].position++);
        }
    }

    // move the non-fitting toolbars at the bottom
    int lastRow = rows.empty() ? 0 : (rows.size() - 1);
    int position = rows.back().position, maxX = rows.back().width;
    for (wxVector<wxWindow*>::iterator it = nonFitingToolbars.begin(); it != nonFitingToolbars.end(); ++it)
    {
        maxX += (*it)->GetBestSize().x;
        if (maxX > maxWidth)
        {
            position = 0;
            lastRow++;
            maxX = (*it)->GetBestSize().x;
        }
        m_mgr.GetPane(*it).Position(position++).Row(lastRow);
    }

    m_mgr.Update();
}

void wxGISApplicationEx::OptimizeToolbars()
{
    std::set<ToolbarFitInfo> sorted;
    CollectToolbars(sorted);
    if (sorted.empty())
        return;

    int maxWidth = GetSize().x;
    int lastRow = 0, position = 0, maxX = 0;
    for (std::set<ToolbarFitInfo>::const_iterator it = sorted.begin(); it != sorted.end(); ++it)
    {
        maxX += it->window->GetBestSize().x;
        if (maxX > maxWidth)
        {
            position = 0;
            lastRow++;
            maxX = it->window->GetBestSize().x;
        }
        m_mgr.GetPane(it->window).Position(position++).Row(lastRow);
    }

    m_mgr.Update();
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
	m_mgr.SetFlags(wxAUI_MGR_DEFAULT | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_TRANSPARENT_DRAG);

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

 	SerializeFramePosEx(true);

	wxGISApplication::OnClose(event);

    m_mgr.UnInit();
}

bool wxGISApplicationEx::SetupSys(const wxString &sSysPath)
{
    if(!wxGISApplication::SetupSys(sSysPath))
        return false;
#ifdef __WINDOWS__
	wxString sGdalDataDir = sSysPath + wxFileName::GetPathSeparator() + wxString(wxT("gdal")) + wxFileName::GetPathSeparator();
	CPLSetConfigOption("GDAL_DATA", sGdalDataDir.ToUTF8() );
    #ifdef wxGIS_USE_PROJ
        sGdalDataDir = sSysPath + wxFileName::GetPathSeparator() + wxString(wxT("proj")) + wxFileName::GetPathSeparator();
        //CPLSetConfigOption("PROJ_LIB", sGdalDataDir.mb_str(wxConvUTF8) );
        CPLString pszPROJ_LIB(sGdalDataDir.mb_str(wxConvUTF8));
        const char *path = pszPROJ_LIB.c_str();
        pj_set_searchpath(1, &path);
    #endif // wxGIS_USE_PROJ
#endif // __WINDOWS__

#ifdef __UNIX__
    //check config option for gdal directory
    wxString sGDALPath;
    wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
    {
        sGDALPath = oConfig.Read(enumGISHKCU, wxString(wxT("wxGISCommon/GDAL/path")), wxEmptyString);
    }

    if(sGDALPath.IsEmpty())
    {
        int nVerNum = atoi(GDALVersionInfo("VERSION_NUM"));
        wxString sSubDir;
        if(nVerNum > 1900)
        {
            int nMajorVer = nVerNum / 1000000;
            int nMinorVer = nVerNum / 10000 - nMajorVer * 100;
            sSubDir = wxString::Format(wxT("%d.%d"), nMajorVer, nMinorVer);
        }
        else
        {
            int nMajorVer = nVerNum / 1000;
            int nMinorVer = nVerNum / 100 - nMajorVer * 10;
            sSubDir = wxString::Format(wxT("%d.%d"), nMajorVer, nMinorVer);
        }

        wxString sGdalDataGCS = wxString::Format(wxT("/usr/share/gdal/%s/gcs.csv"), sSubDir.c_str());
        if(!wxFileName::FileExists(sGdalDataGCS))
        {
            wxLogWarning(_("Failed to find gdal sys dir in path %s"), sGdalDataGCS.c_str());
            sGdalDataGCS = wxString(wxT("/usr/share/gdal/gcs.csv"));
            if(!wxFileName::FileExists(sGdalDataGCS))
            {
                wxLogWarning(_("Failed to find gdal sys dir in path %s"), sGdalDataGCS.c_str());
                sGdalDataGCS = wxString::Format(wxT("/usr/local/share/gdal/%s/gcs.csv"), sSubDir.c_str());
                if(!wxFileName::FileExists(sGdalDataGCS))
                {
                    wxLogWarning(_("Failed to find gdal sys dir in path %s"), sGdalDataGCS.c_str());
                    sGdalDataGCS = wxString(wxT("/usr/local/share/gdal/gcs.csv"));
                    if(!wxFileName::FileExists(sGdalDataGCS))
                    {
                        wxLogError(_("Failed to find gdal sys dir in path %s"), sGdalDataGCS.c_str());
                        return false;
                    }
                }
            }
        }

        wxFileName Name(sGdalDataGCS);
        sGDALPath =  Name.GetPath();
    }

    CPLSetConfigOption("GDAL_DATA", sGDALPath.ToUTF8() );

    //TODO: set path to proj lib
    #ifdef wxGIS_USE_PROJ
    #endif // wxGIS_USE_PROJ
#endif //__UNIX__

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



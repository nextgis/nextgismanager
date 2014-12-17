/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Catalog Views Commands class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012 Dmitry Baryshnikov
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
#include "wxgis/catalogui/viewscmd.h"

#include "wxgis/catalogui/gxtreeview.h"
#include "wxgis/catalogui/finddlg.h"

#include "../../art/views.xpm"
#include "../../art/treeview.xpm"
#include "../../art/select_all.xpm"
#include "../../art/system_search.xpm"

//------------------------------------------------------------------------
// wxGISCatalogViewsCmd
//------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGISCatalogViewsCmd, wxGISCommand)

wxGISCatalogViewsCmd::wxGISCatalogViewsCmd(void) : wxGISCommand()
{
     m_pTreeView = NULL;
	 m_pFindView = NULL;
}

wxGISCatalogViewsCmd::~wxGISCatalogViewsCmd(void)
{
}

wxIcon wxGISCatalogViewsCmd::GetBitmap(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogViewsCmdView:
			if(!m_IconViews.IsOk())
				m_IconViews = wxIcon(views_xpm);
			return m_IconViews;
		case enumGISCatalogViewsCmdSelectAll:
			if(!m_IconSelAll.IsOk())
				m_IconSelAll = wxIcon(select_all_xpm);
			return m_IconSelAll;
		case enumGISCatalogViewsCmdTreePanel:
			if(!m_IconTreeView.IsOk())
				m_IconTreeView = wxIcon(treeview_xpm);
			return m_IconTreeView;
		case enumGISCatalogViewsCmdFind:
			if(!m_IconFind.IsOk())
				m_IconFind = wxIcon(system_search_xpm);
			return m_IconFind;
		default:
			return wxNullIcon;
	}
}

wxString wxGISCatalogViewsCmd::GetCaption(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogViewsCmdView:
			return wxString(_("View"));
		case enumGISCatalogViewsCmdSelectAll:
			return wxString(_("Select All"));
		case enumGISCatalogViewsCmdTreePanel:
			return wxString(_("Show/Hide Tree panel"));
		case enumGISCatalogViewsCmdFind:
			return wxString(_("Find"));		
		default:
			return wxEmptyString;
	}
}

wxString wxGISCatalogViewsCmd::GetCategory(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogViewsCmdView:
		case enumGISCatalogViewsCmdTreePanel:
		case enumGISCatalogViewsCmdFind:
			return wxString(_("View"));
		case enumGISCatalogViewsCmdSelectAll:
			return wxString(_("Edit"));
		default:
			return NO_CATEGORY;
	}
}

bool wxGISCatalogViewsCmd::GetChecked(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogViewsCmdTreePanel:
            if(m_pTreeView)
                return m_pApp->IsApplicationWindowShown(m_pTreeView);
		case enumGISCatalogViewsCmdFind:
            if(m_pFindView)
                return m_pApp->IsApplicationWindowShown(m_pFindView);
		case enumGISCatalogViewsCmdView:
		case enumGISCatalogViewsCmdSelectAll:
		default:
	        return false;
	}
	return false;
}

bool wxGISCatalogViewsCmd::GetEnabled(void)
{
    if(NULL == m_pTreeView)
	{
        m_pTreeView = m_pApp->GetRegisteredWindowByType(wxCLASSINFO(wxGxTreeView));
	}
    if(NULL == m_pFindView)
	{
        m_pFindView = m_pApp->GetRegisteredWindowByType(wxCLASSINFO(wxAxFindView));
	}
	
	if(m_anContentsWinIDs.empty())
	{
		WINDOWARRAY WinIDsArr = m_pApp->GetChildWindows();
        for(size_t i = 0; i < WinIDsArr.GetCount(); ++i)
		{
            wxWindow* pWnd = wxWindow::FindWindowById(WinIDsArr[i]);
			IGxContentsView* pGxContentsView = dynamic_cast<IGxContentsView*>(pWnd);
            if(pGxContentsView)
                m_anContentsWinIDs.Add(WinIDsArr[i]);
		}
	}

    switch(m_subtype)
	{
		case enumGISCatalogViewsCmdTreePanel:
            return NULL != m_pTreeView;
		case enumGISCatalogViewsCmdFind:
            return NULL != m_pFindView;
		case enumGISCatalogViewsCmdView:
		case enumGISCatalogViewsCmdSelectAll:
            for(size_t i = 0; i < m_anContentsWinIDs.GetCount(); ++i)
            {
                wxWindow* pWnd = wxWindow::FindWindowById(m_anContentsWinIDs[i]);
                if(pWnd && pWnd->IsShownOnScreen())// && pWnd->HasFocus()
                    return true;
            }
 		default:
			return false;
	}
}

wxGISEnumCommandKind wxGISCatalogViewsCmd::GetKind(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogViewsCmdView:
			return enumGISCommandDropDown;
		case enumGISCatalogViewsCmdFind:	
		case enumGISCatalogViewsCmdTreePanel:
            return enumGISCommandCheck;
        case enumGISCatalogViewsCmdSelectAll:
		default:
			return enumGISCommandNormal;
	}
}

wxString wxGISCatalogViewsCmd::GetMessage(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogViewsCmdView:
			return wxString(_("Select view"));
		case enumGISCatalogViewsCmdSelectAll:
			return wxString(_("Select All objects"));
		case enumGISCatalogViewsCmdTreePanel:
			return wxString(_("Show/Hide Tree panel"));
		case enumGISCatalogViewsCmdFind:
			return wxString(_("Show/Hide Find panel"));
		default:
			return wxEmptyString;
	}
}

void wxGISCatalogViewsCmd::OnClick(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogViewsCmdView:
            for(size_t i = 0; i < m_anContentsWinIDs.GetCount(); ++i)
            {
                wxWindow* pWnd = wxWindow::FindWindowById(m_anContentsWinIDs[i]);
                if(pWnd && pWnd->IsShownOnScreen())
                {
                    IGxContentsView* pGxContentsView = dynamic_cast<IGxContentsView*>(pWnd);
                    if(pGxContentsView)
                    {
                        if(pGxContentsView->CanSetStyle())
                        {
                            int nStyle = pGxContentsView->GetStyle() + 1;
                            if(nStyle > enumGISCVList)
                                nStyle = enumGISCVReport;
                            pGxContentsView->SetStyle((wxGISEnumContentsViewStyle)(nStyle));
                        }
                    }
                }
            }
			break;
		case enumGISCatalogViewsCmdSelectAll:
            for(size_t i = 0; i < m_anContentsWinIDs.GetCount(); ++i)
            {
                wxWindow* pWnd = wxWindow::FindWindowById(m_anContentsWinIDs[i]);
                if(pWnd && pWnd->IsShownOnScreen() && pWnd->HasFocus())
                {
                    IGxContentsView* pGxContentsView = dynamic_cast<IGxContentsView*>(pWnd);                    
                    if(pGxContentsView)
                    {
                        pGxContentsView->SelectAll();
                        pWnd->SetFocus();
                    }
                }
            }
			break;
		case enumGISCatalogViewsCmdTreePanel:
            m_pApp->ShowApplicationWindow(m_pTreeView, !m_pApp->IsApplicationWindowShown(m_pTreeView));
			return;
		case enumGISCatalogViewsCmdFind:
            m_pApp->ShowApplicationWindow(m_pFindView, !m_pApp->IsApplicationWindowShown(m_pFindView));
			return;
		default:
			return;
	}
}

bool wxGISCatalogViewsCmd::OnCreate(wxGISApplicationBase* pApp)
{
	m_pApp = pApp;
	return true;
}

wxString wxGISCatalogViewsCmd::GetTooltip(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogViewsCmdView:
			return wxString(_("Select view"));
		case enumGISCatalogViewsCmdSelectAll:
			return wxString(_("Select All"));
		case enumGISCatalogViewsCmdTreePanel:
			return wxString(_("Show/Hide Tree panel"));
		case enumGISCatalogViewsCmdFind:
			return wxString(_("Show/Hide Find panel"));
		default:
			return wxEmptyString;
	}
}

unsigned char wxGISCatalogViewsCmd::GetCount(void)
{
	return enumGISCatalogViewsCmdMax;
}

wxMenu* wxGISCatalogViewsCmd::GetDropDownMenu(void)
{
	switch(m_subtype)
	{
		case enumGISCatalogViewsCmdView:
        {
			if(GetEnabled())
			{
                wxMenu* pMenu = new wxMenu();
                pMenu->AppendCheckItem(ID_MENUCMD + (int)enumGISCVList, wxString(_("List")));
                pMenu->AppendCheckItem(ID_MENUCMD + (int)enumGISCVLarge, wxString(_("Icons")));
                pMenu->AppendCheckItem(ID_MENUCMD + (int)enumGISCVSmall, wxString(_("Smal Icons")));
                pMenu->AppendCheckItem(ID_MENUCMD + (int)enumGISCVReport, wxString(_("Details")));
                for(size_t i = 0; i < m_anContentsWinIDs.GetCount(); ++i)
                {
                    wxWindow* pWnd = wxWindow::FindWindowById(m_anContentsWinIDs[i]);
                    if(pWnd && pWnd->IsShownOnScreen())
                    {
                        IGxContentsView* pGxContentsView = dynamic_cast<IGxContentsView*>(pWnd);
                        if(pGxContentsView)
                        {
                            wxGISEnumContentsViewStyle nStyle = pGxContentsView->GetStyle();
                            pMenu->Check(ID_MENUCMD + (int)nStyle, true);
                        }
                    }
                }
                return pMenu;
            }
            return NULL;
        }
		default:
			return NULL;
	}
}

void wxGISCatalogViewsCmd::OnDropDownCommand(int nID)
{
    if(GetEnabled())
    {
        for(size_t i = 0; i < m_anContentsWinIDs.GetCount(); ++i)
        {
            wxWindow* pWnd = wxWindow::FindWindowById(m_anContentsWinIDs[i]);
            if(pWnd && pWnd->IsShownOnScreen())
            {
                IGxContentsView* pGxContentsView = dynamic_cast<IGxContentsView*>(pWnd);
                if(pGxContentsView)
                    pGxContentsView->SetStyle((wxGISEnumContentsViewStyle)(nID - ID_MENUCMD));
            }
        }
    }
}


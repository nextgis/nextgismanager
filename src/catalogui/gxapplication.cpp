/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxApplication class.
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
#include "wxgis/catalogui/gxapplication.h"
#include "wxgis/framework/toolbarmenu.h"
#include "wxgis/catalogui/gxtreeview.h"


#include "../../art/mainframecat.xpm"

//-----------------------------------------------
// wxGxApplication
//-----------------------------------------------
IMPLEMENT_CLASS(wxGxApplication, wxGISApplicationEx)

wxGxApplication::wxGxApplication(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxGxApplicationBase(), wxGISApplicationEx(parent, id, title, pos, size, style)
{
    m_pTreeView = NULL;
    m_pNewMenu = NULL;
    m_pCatalog = NULL;
    m_pTabView = NULL;
}

wxGxApplication::~wxGxApplication(void)
{
}

bool wxGxApplication::CreateApp(void)
{

    m_pCatalog = new wxGxCatalogUI();
    SetGxCatalog(m_pCatalog);

    if(!m_pCatalog->Init())
    {
        wxDELETE(m_pCatalog);
        return false;
    }

    m_pNewMenu = new wxGISNewMenu();
    m_pNewMenu->OnCreate(this);
    m_pNewMenu->Reference();
    m_CommandBarArray.Add(m_pNewMenu);
    m_pNewMenu->Reference();
    m_CommandArray.Add(m_pNewMenu);

    wxGISApplicationEx::CreateApp();

	wxLogMessage(_("wxGxApplication: Start. Creating main application frame..."));

	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return false;

	wxXmlNode* pViewsNode = oConfig.GetConfigNode(enumGISHKCU, GetAppName() + wxString(wxT("/frame/views")));

	if(!pViewsNode)
	{
		wxLogError(_("wxGxApplication: Error finding <views> XML Node"));
		return false;
	}

    int nPaneCount(0);
    wxXmlNode* pViewsChildNode = pViewsNode->GetChildren();
    while(pViewsChildNode)
    {

        if(pViewsChildNode->GetName().CmpNoCase(wxT("treeview")) == 0)
        {
            int nOSMajorVer(0);
            wxGetOsVersion(&nOSMajorVer);
	        m_pTreeView = new wxGxTreeView(this, TREECTRLID, wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS | wxBORDER_NONE | wxTR_EDIT_LABELS | (nOSMajorVer > 5 ? wxTR_NO_LINES : 0) );
	        if(m_pTreeView->Activate(this, pViewsChildNode))
	        {
		        m_mgr.AddPane(m_pTreeView, wxAuiPaneInfo().Name(wxT("tree_window")).Caption(_("Tree Pane")).BestSize(wxSize(280,128)).MinSize(wxSize(200,64)).Left().Layer(1).Position(1).CloseButton(true));//.MinimizeButton(true).MaximizeButton(true)
		        RegisterChildWindow(m_pTreeView->GetId());
	        }
	        else
            {
		        wxDELETE(m_pTreeView);
            }
        }
        else if(pViewsChildNode->GetName().CmpNoCase(wxT("tabview")) == 0)
        {
	        m_pTabView = new wxGxTabView(this);
	        if(m_pTabView->Activate(this, pViewsChildNode))
	        {
		        m_mgr.AddPane(m_pTabView, wxAuiPaneInfo().Name(wxT("main_window")).CenterPane());//.PaneBorder(true)
		        RegisterChildWindow(m_pTabView->GetId());
	        }
	        else
            {
		        wxDELETE(m_pTabView);
            }
        }
        else
        {
 		    wxString sClass = pViewsChildNode->GetAttribute(wxT("class"), ERR);

		    wxObject *obj = wxCreateDynamicObject(sClass);
		    IView *pView = dynamic_cast<IView*>(obj);
            if(pView)
            {
                pView->Create(this);
			    wxWindow* pWnd = dynamic_cast<wxWindow*>(pView);
			    if(pWnd != NULL)
			    {
				    pWnd->Hide();
				    if(pView->Activate(this, pViewsChildNode))
                    {
                        nPaneCount++;
                        m_mgr.AddPane(pWnd, wxAuiPaneInfo().Name(wxString::Format(wxT("window_%d"), nPaneCount)).Caption(pView->GetViewName()).BestSize(wxSize(280,128)).MinSize(wxSize(200,64)).Right().Layer(1).Position(nPaneCount).CloseButton(true));
                        RegisterChildWindow(pWnd->GetId());
                        wxLogMessage(_("wxGxApplication: View class %s initialise"), sClass.c_str());
                    }
				    else
				    {
					    wxLogError(_("wxGxApplication: Error activation IGxView class (%s)"), sClass.c_str());
					    wxDELETE(obj);
				    }
			    }
		    }
		    else
		    {
			    wxLogError(_("wxGxApplication: Error creating view %s"), sClass.c_str());
			    wxDELETE(obj);
		    }
        }
        pViewsChildNode = pViewsChildNode->GetNext();
    }

	for(size_t i = 0; i < m_CommandBarArray.GetCount(); ++i)
	{
		if(m_CommandBarArray[i]->GetType() == enumGISCBToolbar)
		{
			wxGISToolBar* pToolBar = static_cast<wxGISToolBar*>(m_CommandBarArray[i]);
			if(pToolBar)
			{
				pToolBar->Activate(this);
			}
		}
	}

    SerializeFramePosEx(false);
    m_mgr.Update();

	wxString sLastPath;

	if(m_pCatalog)
	{
		if(m_pCatalog->GetOpenLastPath())
        {
            sLastPath = oConfig.Read(enumGISHKCU, GetAppName() + wxString(wxT("/lastpath/path")), m_pCatalog->GetName());
			SetLocation(sLastPath);
        }
		else
        {
			SetLocation(wxT(""));
        }
	}

	wxLogMessage(_("wxGxApplication: Creation complete"));

	return true;
}

void wxGxApplication::OnClose(wxCloseEvent& event)
{
	wxGISAppConfig oConfig = GetConfig();
	//should remove toolbars from commandbar array as m_mgr manage toolbars by itself
	if(m_pCatalog)
	{
		wxGxSelection* pSel = GetGxSelection();

		if(pSel && pSel->GetCount(TREECTRLID) > 0)
		{
			wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(pSel->GetSelectedObjectId(TREECTRLID, 0));
			if(pGxObject != static_cast<wxGxObject*>(m_pCatalog))
			{
				wxString sLastPath = pGxObject->GetFullName();
				if(sLastPath.IsEmpty())
					sLastPath = pGxObject->GetName();

				if(oConfig.IsOk())
					oConfig.Write(enumGISHKCU, GetAppName() + wxString(wxT("/lastpath/path")), sLastPath);
			}
		}
	}

    wxGISApplicationEx::OnClose(event);

    //if(m_pTreeView)
    //{
    //    UnRegisterChildWindow(m_pTreeView->GetId());
    //    m_pTreeView->Deactivate();
    //}

    //prevent doubling deactivation of tabview and it's tabs
    //if(m_pTabView)
    //{
    //    UnRegisterChildWindow(m_pTabView->GetId());
    //    m_pTabView->Deactivate();
    //}

    if(m_pCatalog)
    {
        SetGxCatalog(NULL);
        m_pCatalog = NULL;
    }
}

void wxGxApplication::OnIdle(wxIdleEvent & event)
{
	if(m_pCatalog)
    {
		m_pCatalog->OnIdle();
	}
	
	wxGISApplicationEx::OnIdle(event);
}

wxIcon wxGxApplication::GetAppIcon(void)
{
    if(!m_pAppIcon.IsOk())
        m_pAppIcon = wxIcon(mainframecat_xpm);
    return m_pAppIcon;
}

wxString wxGxApplication::GetAppDisplayName(void) const 
{
	if(m_sAppDisplayName.IsEmpty())
		return wxString(_("NextGIS Manager"));
	return m_sAppDisplayName;
}

wxString wxGxApplication::GetAppDisplayNameShort(void) const 
{
	if(m_sAppDisplayNameShort.IsEmpty())
		return wxString(_("Manager"));
	return 	m_sAppDisplayNameShort;
}

void wxGxApplication::UpdateNewMenu(wxGxSelection* Selection)
{
    if (NULL != m_pNewMenu)
        m_pNewMenu->Update(Selection);
    UpdateNewMenuInCommands();
}

void wxGxApplication::UpdateNewMenuInCommands()
{
    for (size_t i = 0; i < m_CommandBarArray.size(); ++i)
    {
        switch (m_CommandBarArray[i]->GetType())
        {
        case enumGISCBContextmenu:
        case enumGISCBSubMenu:
        case enumGISCBMenubar:
        {
            wxMenu* pMenu = dynamic_cast<wxMenu*>(m_CommandBarArray[i]);

            size_t nPos = wxNOT_FOUND;
            wxMenuItem * this_item = pMenu->FindChildItem(NEWMENUID, &nPos);
            if (nPos != wxNOT_FOUND && this_item != NULL)
            {
                //nthis_item->GetSubMenu()->UpdateUI(this);
                pMenu->Remove(NEWMENUID);
                pMenu->Insert(nPos, this_item);
                return;
            }
        }
            break;
        case enumGISCBToolbar:
        case enumGISCBNone:
        default:
            break;
        }
    }
}


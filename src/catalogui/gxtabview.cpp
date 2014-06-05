/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISTabView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2012 Dmitry Barishnikov
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
#include "wxgis/catalogui/gxtabview.h"
#include "wxgis/catalogui/gxapplication.h"
#include "wxgis/catalogui/droptarget.h"


//-------------------------------------------------------------------
// wxGxTab
//-------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxTab, wxPanel)

BEGIN_EVENT_TABLE(wxGxTab, wxPanel)
	EVT_CHOICE(ID_WNDCHOICE, wxGxTab::OnChoice)
	EVT_GXSELECTION_CHANGED(wxGxTab::OnSelectionChanged)
END_EVENT_TABLE()

wxGxTab::wxGxTab(wxGxApplication* application, wxXmlNode* pTabDesc, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style), m_bShowChoices(false), m_pCurrentWnd(NULL), m_pNoWnd(NULL)
{
	m_tabwnd = NULL;
    SetDropTarget(new wxGISDropTarget(static_cast<IViewDropTarget*>(this)));

	m_sName = wxGetTranslation( pTabDesc->GetAttribute(wxT("name"), NONAME) );
	m_bShowChoices = pTabDesc->GetAttribute(wxT("show_choices"), wxT("f")) == wxT("f") ? false : true;
    m_pApp = application;

	wxXmlNode* pChild = pTabDesc->GetChildren();
	while(pChild)
	{
		wxString sClass = pChild->GetAttribute(wxT("class"), ERR);
		int nPriority = wxAtoi(pChild->GetAttribute(wxT("priority"), wxT("0")));

		wxObject *obj = wxCreateDynamicObject(sClass);
		IView *pGxView = dynamic_cast<IView*>(obj);
		wxWindow* pWnd = dynamic_cast<wxWindow*>(pGxView);
		if(pGxView && pWnd)
		{
            if(!pGxView->Create(this))
		    {
			    wxLogError(_("wxGxTab: Error creating view %s"), sClass.c_str());
		        wxDELETE(obj);
		    }
            else
            {
			    pWnd->Hide();			

			    if(pGxView->Activate(m_pApp, pChild))
                    m_pApp->RegisterChildWindow(pWnd->GetId());

			    if(m_pWindows.size() <= nPriority)
			    for(size_t i = 0; i < nPriority + 1; ++i)
				    m_pWindows.push_back(NULL);

			    m_pWindows[nPriority] = pWnd;
			    wxLogMessage(_("wxGxTab: View class %s in '%s' tab initialise"), sClass.c_str(), m_sName.c_str());
			    //store NoView
			    if(sClass.CmpNoCase(wxT("wxGxNoView")) == 0)
				    m_pNoWnd = pWnd;
            }
		}
		else
		{
			wxLogError(_("wxGxTab: Error creating view %s"), sClass.c_str());
		    wxDELETE(obj);
		}

		pChild = pChild->GetNext();
	}

	m_bSizerMain = new wxBoxSizer( wxVERTICAL );

	if(m_pNoWnd)
	{
		m_bSizerMain->Add( m_pNoWnd, 1, m_bShowChoices == true ? wxEXPAND | wxALL : wxEXPAND, 5 );
		m_pNoWnd->Show();
		m_pCurrentWnd = m_pNoWnd;
	}
	else
	{
		m_tabwnd = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
		m_bSizerMain->Add( m_tabwnd, 1, m_bShowChoices == true ? wxEXPAND | wxALL : wxEXPAND, 5 );
		m_pCurrentWnd = m_tabwnd;
	}

	if(m_bShowChoices)
	{

		m_tabselector = new wxPanel( this, wxID_ANY, wxDefaultPosition, wxSize( -1,-1 ), wxTAB_TRAVERSAL );
		m_tabselector->SetMaxSize( wxSize( -1,30 ) );

		wxBoxSizer* bSizerMinor = new wxBoxSizer( wxHORIZONTAL );

		m_staticText = new wxStaticText( m_tabselector, wxID_ANY, _("Preview: ") + wxT("   "), wxDefaultPosition, wxDefaultSize, 0 );
		m_staticText->Wrap( -1 );
		bSizerMinor->Add( m_staticText, 0, wxALIGN_CENTER_VERTICAL, 5 );

		m_choice = new wxChoice( m_tabselector, ID_WNDCHOICE, wxDefaultPosition, wxDefaultSize, 0, 0 );
		m_choice->Disable();
		m_choice->SetMinSize( wxSize( 170,-1 ) );

		bSizerMinor->Add( m_choice, 0, wxALIGN_CENTER_VERTICAL, 5 );

		m_tabselector->SetSizer( bSizerMinor );
		m_tabselector->Layout();
		bSizerMinor->Fit( m_tabselector );
		m_bSizerMain->Add( m_tabselector, 0, wxEXPAND|wxBOTTOM|wxRIGHT|wxLEFT, 5 );
	}
	this->SetSizer( m_bSizerMain );
	this->Layout();
}

wxGxTab::~wxGxTab(void)
{
}

wxString wxGxTab::GetName(void)
{
	return m_sName;
}

wxWindow* wxGxTab::GetWindow(int iIndex)
{
	wxCHECK(iIndex >= 0 && iIndex < m_pWindows.size(), NULL);
	return m_pWindows[iIndex];
}

void wxGxTab::OnSelectionChanged(wxGxSelectionEvent& event)
{
    //wxLogDebug(wxT("wxGxTab::OnSelectionChanged"));
	if(event.GetInitiator() == GetId())
		return;

    //if(!IsShown())
    //    return;

    this->Layout();

	//select in tree ctrl
	//if(event.GetInitiator() != TREECTRLID) // && event.GetInitiator() != LISTCTRLID&& event.GetInitiator() != IGxSelection::INIT_ALL
	//{
	//	long nSelectedObjectID = m_pSelection->GetLastSelectedObjectID();
	//	m_pSelection->Select(nSelectedObjectID, false, GetId());

	////	//event.GetSelection()->Select(event.GetSelection()->GetLastSelectedObjectID() , false, GetId());
	//	return;
	//}


	if(m_bShowChoices)
	{
		m_choice->Clear();

		//check all windows if applies selection and fill m_choice
		for(size_t i = 0; i < m_pWindows.size(); ++i)
		{
			wxWindow* pWnd = m_pWindows[i];
			wxGxView* pView = dynamic_cast<wxGxView*>(pWnd);
			if(pView == NULL)
				continue;
			if(pView->Applies(event.GetSelection()))
			{
				if(pWnd != m_pNoWnd)
					m_choice->Append(pView->GetViewName(), pWnd);
			}
		}
		if(m_choice->GetCount() > 0)
		{
			m_choice->Enable(true);
			bool bWndPresent(false);

			for(size_t i = 0; i < m_choice->GetCount(); ++i)
			{
				wxWindow* pWnd = (wxWindow*) m_choice->GetClientData(i);
				if(m_pCurrentWnd == pWnd)
				{
					m_choice->Select(i);
					bWndPresent = true;
					break;
				}
			}

			if(!bWndPresent)
			{
				m_choice->Select(0);
				wxWindow* pWnd = (wxWindow*) m_choice->GetClientData(0);
				if(pWnd != NULL)
				{
					m_pCurrentWnd->Hide();
					m_bSizerMain->Replace(m_pCurrentWnd, pWnd);
					pWnd->Show();
					m_pCurrentWnd = pWnd;

                    wxGxView* pView = dynamic_cast<wxGxView*>(m_pCurrentWnd);
                    if (pView != NULL)
                    {
                        pView->OnShow(true);
                    }
				}
			}
		}
		else
		{
			m_choice->Enable(false);
			if(m_pNoWnd != NULL)
			{
				m_pCurrentWnd->Hide();
				m_bSizerMain->Replace(m_pCurrentWnd, m_pNoWnd);
				m_pNoWnd->Show();
				m_pCurrentWnd = m_pNoWnd;
			}
		}
		goto END;
	}
	else
	{
		//get first apply window
		wxGxView* pCurrView = dynamic_cast<wxGxView*>(m_pCurrentWnd);
		for(size_t i = 0; i < m_pWindows.size(); ++i)
		{
			wxWindow* pWnd = m_pWindows[i];
			wxGxView* pView = dynamic_cast<wxGxView*>(pWnd);
			if(pView == NULL)
				continue;
			if(pView->Applies(event.GetSelection()))
			{
				if(pCurrView != pView)
				{
					m_pCurrentWnd->Hide();
					m_bSizerMain->Replace(m_pCurrentWnd, pWnd);
					pWnd->Show();
					m_pCurrentWnd = pWnd;

                    wxGxView* pView = dynamic_cast<wxGxView*>(m_pCurrentWnd);
                    if (pView != NULL)
                    {
                        pView->OnShow(true);
                    }

				}
				goto END;
			}
		}
	}
END:
	this->Layout();
	if(m_pCurrentWnd)
		m_pCurrentWnd->ProcessWindowEventLocally(event);
}

void wxGxTab::OnChoice(wxCommandEvent& event)
{
    wxLogDebug(wxT("wxGxTab::OnChoice"));
	event.Skip();
	int pos = event.GetSelection();
	if(pos < 0)
		return;

	wxWindow* pWnd = (wxWindow*) m_choice->GetClientData(pos);
	if(pWnd && m_pCurrentWnd != pWnd)
	{
		m_pCurrentWnd->Hide();
		m_bSizerMain->Replace(m_pCurrentWnd, pWnd);
		pWnd->Show();
		m_pCurrentWnd = pWnd;

		this->Layout();

        wxGxView* pView = dynamic_cast<wxGxView*>(m_pCurrentWnd);
        if (pView != NULL)
        {
            pView->OnShow(true);
        }


		wxGxSelectionEvent sel_event(wxGXSELECTION_CHANGED, m_pApp->GetGxSelection(), GetId());
		if(m_pCurrentWnd)
			m_pCurrentWnd->ProcessWindowEventLocally(sel_event);
	}
}

bool wxGxTab::Show(bool bShow)
{
	//if(m_pCurrentWnd && !m_pCurrentWnd->IsBeingDeleted() )
	//	m_pCurrentWnd->Show(bShow);

	//if(m_pCurrentWnd != m_pNoWnd)
	//{
	//	m_pCurrentWnd->Hide();
	//	m_bSizerMain->Replace(m_pCurrentWnd, m_pNoWnd);
	//	m_pNoWnd->Show();
	//	m_pCurrentWnd = m_pNoWnd;
	//}

    if (bShow)
    {
        this->Layout();

    }
    wxGxView* pView = dynamic_cast<wxGxView*>(m_pCurrentWnd);
    if (pView != NULL)
    {
        pView->OnShow(bShow);
    }

	return wxPanel::Show(bShow);
}

void wxGxTab::Deactivate(void)
{
	m_pCurrentWnd->Hide();
	if(m_pNoWnd)
	{
		m_bSizerMain->Replace(m_pCurrentWnd, m_pNoWnd);
		m_pNoWnd->Show();
		m_pCurrentWnd = m_pNoWnd;
	}
	else
	{
		m_bSizerMain->Replace(m_pCurrentWnd, m_tabwnd);
		m_tabwnd->Show();
		m_pCurrentWnd = m_tabwnd;
	}

	for(size_t i = 0; i < m_pWindows.size(); ++i)
	{
		wxGxView* pView = dynamic_cast<wxGxView*>(m_pWindows[i]);
		if(pView != NULL)
		{
			pView->Deactivate();
            m_pApp->UnRegisterChildWindow(m_pWindows[i]->GetId());
		}
	}
	m_pWindows.clear();
}

wxDragResult wxGxTab::OnDragOver(wxCoord x, wxCoord y, wxDragResult def)
{
	IViewDropTarget* pViewDropTarget = dynamic_cast<IViewDropTarget*>(m_pCurrentWnd);
	if(!pViewDropTarget)
		return wxDragNone;
	return pViewDropTarget->OnDragOver(x, y, def);
}

bool wxGxTab::OnDropObjects(wxCoord x, wxCoord y, const wxArrayString& GxObjects)
{
	IViewDropTarget* pViewDropTarget = dynamic_cast<IViewDropTarget*>(m_pCurrentWnd);
	if(!pViewDropTarget)
		return wxDragNone;
	return pViewDropTarget->OnDropObjects(x, y, GxObjects);
}

void wxGxTab::OnLeave()
{
	IViewDropTarget* pViewDropTarget = dynamic_cast<IViewDropTarget*>(m_pCurrentWnd);
	if(!pViewDropTarget)
		return;
	return pViewDropTarget->OnLeave();
}

bool wxGxTab::CanPaste()
{
	IViewDropTarget* pViewDropTarget = dynamic_cast<IViewDropTarget*>(m_pCurrentWnd);
	if(!pViewDropTarget)
		return false;
	return pViewDropTarget->CanPaste();
}

//-------------------------------------------------------------------
// wxGxTabView
//-------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxTabView, wxAuiNotebook)

BEGIN_EVENT_TABLE(wxGxTabView, wxAuiNotebook)
	EVT_AUINOTEBOOK_PAGE_CHANGED(TABCTRLID, wxGxTabView::OnAUINotebookPageChanged)
	EVT_GXSELECTION_CHANGED(wxGxTabView::OnSelectionChanged)
END_EVENT_TABLE()

wxGxTabView::wxGxTabView(void) : wxAuiNotebook()
{
    m_pSelection = NULL;
}

wxGxTabView::wxGxTabView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxAuiNotebook(parent, id, pos, size, wxAUI_NB_TOP | wxNO_BORDER | wxAUI_NB_TAB_MOVE)
{
    m_pSelection = NULL;
}

wxGxTabView::~wxGxTabView(void)
{
}

bool wxGxTabView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    return wxAuiNotebook::Create(parent, TABCTRLID, pos, size, wxAUI_NB_TOP | wxNO_BORDER | wxAUI_NB_TAB_MOVE);
}

bool wxGxTabView::Activate(IApplication* const application, wxXmlNode* const pConf)
{
    m_pApp = dynamic_cast<wxGxApplication*>(application);
    if(!m_pApp)
        return false;
	if(!wxGxView::Activate(application, pConf))
		return false;

	wxXmlNode* pChild = m_pXmlConf->GetChildren();
	wxUint8 count(0);
	while(pChild)
	{
		wxGxTab* pGxTab = new wxGxTab(m_pApp, pChild, this);
        m_Tabs.push_back(pGxTab);

		AddPage(static_cast<wxWindow*>(pGxTab), pGxTab->GetName(), count == 0 ? true : false);//, m_ImageListSmall.GetBitmap(9)

		count++;

		pChild = pChild->GetNext();
	}

    m_pSelection = m_pApp->GetGxSelection();
	if(m_pSelection)
        m_ConnectionPointSelectionCookie = m_pSelection->Advise(this);

	return true;
}

void wxGxTabView::Deactivate(void)
{
	if(m_ConnectionPointSelectionCookie != wxNOT_FOUND && m_pSelection)
    {
		m_pSelection->Unadvise(m_ConnectionPointSelectionCookie);
        m_ConnectionPointSelectionCookie = wxNOT_FOUND;
    }
    
    for(size_t i = 0; i < m_Tabs.size(); ++i)
    {
        m_Tabs[i]->Deactivate();
	}

	while(GetPageCount()  > 0)
		DeletePage(0);

	wxGxView::Deactivate();
}

void wxGxTabView::OnSelectionChanged(wxGxSelectionEvent& event)
{   
    //wxLogDebug(wxT("wxGxTabView::OnSelectionChanged"));
    if(event.GetInitiator() == GetId() || event.GetInitiator() == NOTFIRESELID)
		return;

    int nSelTab = GetSelection();
	wxCHECK_RET(nSelTab >= 0 && nSelTab < m_Tabs.size(), "wrong tab index");

	wxEvtHandler* pCurrEvtHandler = dynamic_cast<wxEvtHandler*>(m_Tabs[nSelTab]);
	if(!pCurrEvtHandler)
		return;
	pCurrEvtHandler->AddPendingEvent(event);
}

void wxGxTabView::OnAUINotebookPageChanged(wxAuiNotebookEvent& event)
{
	event.Skip();

	if(!m_pSelection)
		return;

    SetFocus();
	////update view while changing focus of tabs
	//int nSelTab = event.GetSelection();
	////if(GetSelection() == event.GetOldSelection())
	////	return;
	//wxCHECK_RET(nSelTab >= 0 && nSelTab < m_Tabs.size(), "wrong tab index");

	//wxEvtHandler* pCurrEvtHandler = dynamic_cast<wxEvtHandler*>(m_Tabs[nSelTab]);
	//if(!pCurrEvtHandler)
	//	return;

	//wxGxSelectionEvent sel_event(wxGXSELECTION_CHANGED, m_pSelection, IGxSelection::INIT_ALL);
	//OnSelectionChanged(sel_event);
	//pCurrEvtHandler->AddPendingEvent(sel_event);

	
	//wxGxTab* pCurrTab = m_Tabs[event.GetOldSelection()];
	//wxWindow* pWnd = pCurrTab->GetCurrentWindow();
	//m_pSelection->SetInitiator(	pWnd->GetId() );
	long nSelectedObjectId = m_pSelection->GetLastSelectedObjectId();
	if(nSelectedObjectId != wxNOT_FOUND)
		m_pSelection->Select(nSelectedObjectId, false, GetId());//WXGISHIGHEST pWnd->
}

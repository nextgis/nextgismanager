/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  customize dialog class. Customize menues & toolbars
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011-2014 Dmitry Baryshnikov
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
#include "wxgis/framework/customizedlg.h"
#include "wxgis/framework/keycodedlg.h"
#include "wxgis/framework/createcbdlg.h"
#include "wxgis/framework/addcommanddlg.h"
#include "wxgis/framework/tabstyle.h"

#include "wx/accel.h"
#include "wx/utils.h"

#include "../../art/check_marks.xpm"

//------------------------------------------------------------------------------
/// Class wxGISToolBarPanel
//------------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGISToolBarPanel, wxPanel)
	EVT_LEFT_DOWN( wxGISToolBarPanel::OnLeftDown )
	EVT_TREE_SEL_CHANGED(wxGISToolBarPanel::ID_TREECTRL, wxGISToolBarPanel::OnSelChanged)
	EVT_SPLITTER_DCLICK(wxID_ANY, wxGISToolBarPanel::OnDoubleClickSash)
	EVT_LIST_ITEM_RIGHT_CLICK(wxGISToolBarPanel::ID_BUTTONSLST, wxGISToolBarPanel::OnListctrlRClick)
	EVT_LIST_ITEM_ACTIVATED(wxGISToolBarPanel::ID_BUTTONSLST, wxGISToolBarPanel::OnListctrlActivated)
	EVT_MENU(wxGISToolBarPanel::ID_ONSETKEYCODE, wxGISToolBarPanel::OnSetKeyCode)
	EVT_BUTTON(wxGISToolBarPanel::ID_CREATECB,	wxGISToolBarPanel::OnCreateCommandBar)
	EVT_BUTTON(wxGISToolBarPanel::ID_DELETECB,	wxGISToolBarPanel::OnDeleteCommandBar)
	EVT_BUTTON(wxGISToolBarPanel::ID_ADDBUTTON,		wxGISToolBarPanel::OnAddButton)
	EVT_BUTTON(wxGISToolBarPanel::ID_REMOVEBUTTON,	wxGISToolBarPanel::OnRemoveButton)
	EVT_BUTTON(wxGISToolBarPanel::ID_MOVECONTROLUP,	wxGISToolBarPanel::OnMoveUp)
	EVT_BUTTON(wxGISToolBarPanel::ID_MOVECONTROLDOWN,	wxGISToolBarPanel::OnMoveDown)
	EVT_UPDATE_UI_RANGE(wxGISToolBarPanel::ID_CREATECB, wxGISToolBarPanel::ID_MOVECONTROLDOWN, wxGISToolBarPanel::OnUpdateUI)
END_EVENT_TABLE()

wxGISToolBarPanel::wxGISToolBarPanel(wxGISApplicationEx* pApp, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style ), m_bToolsFocus(false), m_bCmdFocus(false)
{
    m_pApp = pApp;

    m_nSashPos = 150;
    SerializePanel(false);

	wxBoxSizer* bSizer = new wxBoxSizer( wxHORIZONTAL );

	m_Splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D  | wxNO_BORDER);
	//m_Splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( wxGISToolBarPanel::SplitterOnIdle ), NULL, this );
	m_Splitter->Bind( wxEVT_IDLE, &wxGISToolBarPanel::SplitterOnIdle, this );
	bSizer->Add( m_Splitter, 1, wxEXPAND, 5 );

	//wxArrayString m_commandbarlistChoices;
	//m_commandbarlist = new wxCheckListBox( m_Splitter, wxGISToolBarPanel::ID_CHKLSTBX, wxDefaultPosition, wxDefaultSize, m_commandbarlistChoices,  wxNO_BORDER );
	m_TreeImageList.Create(16, 16);
	m_TreeImageList.Add(wxBitmap(check_marks_xpm));
	m_pTreeCtrl = new wxTreeCtrl( m_Splitter, wxGISToolBarPanel::ID_TREECTRL, wxDefaultPosition, wxDefaultSize, wxTR_NO_LINES | wxTR_HIDE_ROOT | wxBORDER_NONE );//wxTR_NO_BUTTONS |
	m_pTreeCtrl->SetImageList(&m_TreeImageList);

	m_buttonslist = new wxListView( m_Splitter, wxGISToolBarPanel::ID_BUTTONSLST, wxDefaultPosition, wxDefaultSize,wxLC_REPORT|wxLC_SINGLE_SEL  | wxNO_BORDER);// wxLC_NO_SORT_HEADER||wxLC_SORT_ASCENDING
	m_buttonslist->InsertColumn(0, _("Command Name"), wxLIST_FORMAT_LEFT, 90);
	m_buttonslist->InsertColumn(1, _("Description"), wxLIST_FORMAT_LEFT, 120);
	m_buttonslist->InsertColumn(2, _("KeyCode"), wxLIST_FORMAT_LEFT, 60);
	m_ImageList.Create(16, 16);
	m_buttonslist->SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);

	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );

	m_createbutton = new wxButton( this, wxGISToolBarPanel::ID_CREATECB, _("Create toolbar/menu"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_createbutton, 0, wxALL|wxEXPAND, 5 );

	m_deletebutton = new wxButton( this, wxGISToolBarPanel::ID_DELETECB, _("Delete toolbar/menu"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_deletebutton, 0, wxALL|wxEXPAND, 5 );

	m_addbutton = new wxButton( this, wxGISToolBarPanel::ID_ADDBUTTON, _("Add button"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_addbutton, 0, wxALL|wxEXPAND, 5 );

	m_rembutton = new wxButton( this, wxGISToolBarPanel::ID_REMOVEBUTTON, _("Remove button"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_rembutton, 0, wxALL|wxEXPAND, 5 );

	m_moveup = new wxButton( this, wxGISToolBarPanel::ID_MOVECONTROLUP, _("Move up"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_moveup, 0, wxALL|wxEXPAND, 5 );
	m_movedown = new wxButton( this, wxGISToolBarPanel::ID_MOVECONTROLDOWN, _("Move down"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer3->Add( m_movedown, 0, wxALL|wxEXPAND, 5 );

	bSizer->Add( bSizer3, 0, wxEXPAND, 5 );

	m_Splitter->SetSashGravity(0.5);
//	m_Splitter->SplitVertically(m_commandbarlist, m_buttonslist, 100);
	m_Splitter->SplitVertically(m_pTreeCtrl, m_buttonslist, 100);
//	m_pTreeCtrl->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxGISToolBarPanel::OnLeftDown ), NULL, this );
	m_pTreeCtrl->Bind( wxEVT_LEFT_DOWN, &wxGISToolBarPanel::OnLeftDown, this );


	m_pContextMenu = new wxMenu();//_("Command menu")
	wxMenuItem *item = new wxMenuItem(m_pContextMenu, ID_ONSETKEYCODE, _("Set keycode"));
#ifdef __WIN32__
	item->SetBitmap(wxKeyCodeDlg::GetBitmap());
#endif
	m_pContextMenu->Append(item);

	this->SetSizerAndFit( bSizer );
	this->Layout();

	//load toolbars & menues

	//add root
	wxTreeItemId nRootId = m_pTreeCtrl->AddRoot(wxT("Commands"));
	//add sets
	m_nMenubarId = m_pTreeCtrl->AppendItem(nRootId, _("Menubar"), 3, 3);
	m_pTreeCtrl->SetItemBold(m_nMenubarId);
	m_nContextMenuesId = m_pTreeCtrl->AppendItem(nRootId, _("Context Menus"), 3, 3);
	m_pTreeCtrl->SetItemBold(m_nContextMenuesId);
	m_nNewMenuesId = m_pTreeCtrl->AppendItem(nRootId, _("New Menus"), 3, 3);
	m_pTreeCtrl->SetItemBold(m_nNewMenuesId);
	m_nToolBarsId = m_pTreeCtrl->AppendItem(nRootId, _("ToolBars"), 3, 3);
	m_pTreeCtrl->SetItemBold(m_nToolBarsId);

	wxGISMenuBar* pwxGISMenuBar = m_pApp->GetGISMenuBar();
	wxGISCommandBarPtrArray MenuBarArray = pwxGISMenuBar->GetMenuBarArray();
	for(size_t i = 0; i < MenuBarArray.GetCount(); ++i)
	{
		wxGISCommandBar* pBar = MenuBarArray[i];
		wxString sCaption = pBar->GetCaption();
		sCaption.Replace(wxT("&"), wxT(""));

		m_pTreeCtrl->AppendItem(m_nMenubarId, sCaption, -1, -1, new wxBarTreeItemData(pBar));
	}

	wxGISCommandBarPtrArray MenuArray = m_pApp->GetCommandBars();
	for(size_t i = 0; i < MenuArray.GetCount(); ++i)
	{
		wxGISCommandBar* pBar = MenuArray[i];
		if(pBar->GetType() == enumGISCBContextmenu)
		{
			wxString sCaption = pBar->GetCaption();
			if(pBar->GetName().Find(wxT(".NewMenu")) != wxNOT_FOUND)
			{				
				m_pTreeCtrl->AppendItem(m_nNewMenuesId, sCaption, -1, -1, new wxBarTreeItemData(pBar));
			}
			else
			{				
				m_pTreeCtrl->AppendItem(m_nContextMenuesId, sCaption, -1, -1, new wxBarTreeItemData(pBar));
			}
		}
		else if(pBar->GetType() == enumGISCBToolbar)
		{
			wxString sCaption = pBar->GetCaption();
			int nImgIndex = m_pApp->IsPaneShown(pBar->GetName()) == true ? 1 : 0;
			m_pTreeCtrl->AppendItem(m_nToolBarsId, sCaption, nImgIndex, nImgIndex, new wxBarTreeItemData(pBar));
		}
	}

	m_pTreeCtrl->ExpandAll();
}

wxGISToolBarPanel::~wxGISToolBarPanel()
{
	wxDELETE(m_pContextMenu);
    SerializePanel(true);
}


void wxGISToolBarPanel::SerializePanel(bool bSave)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return;

	if(bSave)
	{
        oConfig.Write(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/sashpos1")), m_Splitter->GetSashPosition());
	}
	else
	{
		//load
		m_nSashPos = oConfig.ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/sashpos1")), m_nSashPos);
	}
}

void wxGISToolBarPanel::OnDoubleClickSash(wxSplitterEvent& event)
{
	event.Veto();
}

void wxGISToolBarPanel::OnSelChanged(wxTreeEvent& event)
{
	LoadCommands();
}

void wxGISToolBarPanel::LoadCommands(void)
{
	m_buttonslist->DeleteAllItems();
	m_ImageList.RemoveAll();
	wxTreeItemId nSelId = m_pTreeCtrl->GetSelection();
    if(nSelId.IsOk())
    {
		wxBarTreeItemData* pData = (wxBarTreeItemData*)m_pTreeCtrl->GetItemData(nSelId);
		if(pData == NULL)
			return;
		wxGISCommandBar* pBar = pData->m_pBar;

		for(size_t i = 0; i < pBar->GetCommandCount(); ++i)
		{
			wxGISCommand* pCommand = pBar->GetCommand(i);
			wxString sName = wxStripMenuCodes(pCommand->GetCaption());
			wxString sMessage = pCommand->GetMessage();

			wxString sKeyCode = m_pApp->GetGISAcceleratorTable()->GetText(pCommand->GetId());
			long pos = m_buttonslist->InsertItem(i, sName);
			wxIcon oIcon = pCommand->GetBitmap();
			if(oIcon.IsOk())
			{
			    int nIndex = m_ImageList.Add(oIcon);
                m_buttonslist->SetItemImage(pos, nIndex);
			}
            else
            {
                m_buttonslist->SetItemImage(pos, wxNOT_FOUND);
            }
			m_buttonslist->SetItem(pos, 1, sMessage);
			m_buttonslist->SetItem(pos, 2, sKeyCode);
			m_buttonslist->SetItemData(pos, pCommand->GetId());
		}
		m_buttonslist->Update();
	}
}


void wxGISToolBarPanel::OnListctrlActivated(wxListEvent& event)
{
	SetKeyCode(event.GetIndex());
}

void wxGISToolBarPanel::OnListctrlRClick(wxListEvent& event)
{
	PopupMenu(m_pContextMenu);
}

void wxGISToolBarPanel::SetKeyCode(int pos)
{
	wxKeyCodeDlg dlg(this);
	wxListItem item;
	item.SetColumn(2);
	item.SetId(pos);
	item.SetMask(wxLIST_MASK_TEXT);
	if(m_buttonslist->GetItem(item))
		dlg.m_sKeyCode = item.GetText();
	if(dlg.ShowModal() == wxID_OK)
	{
        long cmd(wxID_ANY);
		long CmdID = m_buttonslist->GetItemData(pos);
		wxGISAcceleratorTable *pAccTab = m_pApp->GetGISAcceleratorTable();
        if(dlg.m_Key == wxNOT_FOUND) //remove acc
        {
            m_buttonslist->SetItem(pos, 2, wxEmptyString);
            pAccTab->Remove(pAccTab->GetEntry(CmdID));
        }
        else
        {
    		cmd = pAccTab->Add(wxAcceleratorEntry(dlg.m_Flags, dlg.m_Key, CmdID));
	    	wxString sKeyCode = pAccTab->GetText(CmdID);
    		m_buttonslist->SetItem(pos, 2, sKeyCode);
        }
		if(cmd != wxID_ANY)
		{
			for(int i = 0; i < m_buttonslist->GetItemCount(); ++i)
			{
				long CahgedCmdID = m_buttonslist->GetItemData(i);
				if(CahgedCmdID == cmd)
                    m_buttonslist->SetItem(pos, 2, wxEmptyString);
			}
		}
	}
}

void wxGISToolBarPanel::OnSetKeyCode(wxCommandEvent& event)
{
	long item = GetSelectedCommandItem();
	if(item != wxNOT_FOUND)
		SetKeyCode(item);
}

void wxGISToolBarPanel::OnUpdateUI(wxUpdateUIEvent& event)
{
	if(wxWindow::FindFocus() == m_buttonslist)
	{
		m_bCmdFocus = true;
		m_bToolsFocus = false;
	}
	if(wxWindow::FindFocus() == m_pTreeCtrl)//m_commandbarlist
	{
		m_bCmdFocus = false;
		m_bToolsFocus = true;
	}

	wxGISCommandBar* pBar(NULL);
	wxTreeItemId nSelId = m_pTreeCtrl->GetSelection();
	if(nSelId.IsOk())
	{
		wxBarTreeItemData* pData = (wxBarTreeItemData*)m_pTreeCtrl->GetItemData(nSelId);
		if(pData != NULL)
			pBar = pData->m_pBar;
	}

	switch(event.GetId())
	{
	case wxGISToolBarPanel::ID_CREATECB:
		break;
	case wxGISToolBarPanel::ID_DELETECB:
		if(pBar == NULL)
			event.Enable(false);
		else
			event.Enable(true);
		break;
	case wxGISToolBarPanel::ID_ADDBUTTON:
		if(pBar == NULL)
			event.Enable(false);
		else
			event.Enable(true);
		break;
	case wxGISToolBarPanel::ID_REMOVEBUTTON:
		if(m_buttonslist->GetSelectedItemCount() == 1)
			event.Enable(true);
		else
			event.Enable(false);
		break;
	case wxGISToolBarPanel::ID_MOVECONTROLUP:
		if(m_bCmdFocus)
		{
			int nCurrentButtonSel = GetSelectedCommandItem();
			if(nCurrentButtonSel > 0 && nCurrentButtonSel <= m_buttonslist->GetItemCount() - 1)
				event.Enable(true);
			else
				event.Enable(false);
			break;
		}
		if(m_bToolsFocus)
		{
			if(pBar != NULL && pBar->GetType() == enumGISCBMenubar)
			{
				wxGISMenuBar* pwxGISMenuBar = m_pApp->GetGISMenuBar();
				int nPos = pwxGISMenuBar->GetMenuPos(pBar);
				if(nPos > 0)
				{
					event.Enable(true);
					break;
				}
			}
		}
		event.Enable(false);
		break;
	case wxGISToolBarPanel::ID_MOVECONTROLDOWN:
		if(m_bCmdFocus)
		{
			int nCurrentButtonSel = GetSelectedCommandItem();
			if(nCurrentButtonSel >= 0 && nCurrentButtonSel < m_buttonslist->GetItemCount() - 1)
				event.Enable(true);
			else
				event.Enable(false);
			break;
		}
		if(m_bToolsFocus)
		{
			if(pBar != NULL && pBar->GetType() == enumGISCBMenubar)
			{
				wxGISMenuBar* pwxGISMenuBar = m_pApp->GetGISMenuBar();
				int nPos = pwxGISMenuBar->GetMenuPos(pBar);
				int nCount = m_pTreeCtrl->GetChildrenCount(m_nMenubarId);
				if(nPos < nCount - 1)
				{
					event.Enable(true);
					break;
				}
			}
		}
		event.Enable(false);
		break;
	}
}

void wxGISToolBarPanel::OnCreateCommandBar(wxCommandEvent& event)
{
	wxGISCreateCommandBarDlg dlg(this);
	if(dlg.ShowModal() == wxID_OK)
	{
		switch(dlg.m_CommandbarType)
		{
		case enumGISCBMenubar:
			{
			wxGISMenu* pMenu = new wxGISMenu(dlg.m_sCommandbarName, dlg.m_sCommandbarCaption, enumGISCBMenubar);
			if(m_pApp->AddCommandBar(static_cast<wxGISCommandBar*>(pMenu)))
			{
				wxString sCaption = pMenu->GetCaption();
				sCaption.Replace(wxT("&"), wxT(""));

				wxGISCommandBar* pBar = static_cast<wxGISCommandBar*>(pMenu);
				m_pTreeCtrl->AppendItem(m_nMenubarId, sCaption, -1, -1, new wxBarTreeItemData(pBar));
			}
			}
			break;
		case enumGISCBContextmenu:
			{
			wxGISMenu* pMenu = new wxGISMenu(dlg.m_sCommandbarName, dlg.m_sCommandbarCaption, enumGISCBContextmenu);
			if(m_pApp->AddCommandBar(static_cast<wxGISCommandBar*>(pMenu)))
			{
				wxString sCaption = pMenu->GetCaption();
				wxGISCommandBar* pBar = static_cast<wxGISCommandBar*>(pMenu);
				if(pMenu->GetName().Find(wxT(".NewMenu")) != wxNOT_FOUND)
				{
					m_pTreeCtrl->AppendItem(m_nNewMenuesId, sCaption, -1, -1, new wxBarTreeItemData(pBar));
				}
				else
				{
					m_pTreeCtrl->AppendItem(m_nContextMenuesId, sCaption, -1, -1, new wxBarTreeItemData(pBar));
				}
			}
			}
			break;
		case enumGISCBToolbar:
			{

			wxGISToolBar* pGISToolBar = new wxGISToolBar(m_pApp, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW, dlg.m_sCommandbarName, dlg.m_sCommandbarCaption, enumGISCBToolbar);
			pGISToolBar->SetLeftDockable(dlg.m_bLeftDockable);
			pGISToolBar->SetRightDockable(dlg.m_bRightDockable);
			if(m_pApp->AddCommandBar(static_cast<wxGISCommandBar*>(pGISToolBar)))
			{
				wxAuiToolBarItemArray prepend_items;
				wxAuiToolBarItemArray append_items;
				wxGISCommand* pCmd = m_pApp->GetCommand(wxT("wxGISCommonCmd"), enumGISCommonCmdCustomize);
				if(pCmd)
				{
					wxAuiToolBarItem item;
					item.SetKind(wxITEM_SEPARATOR);
					append_items.Add(item);
					item.SetKind(pCmd->GetKind());
					item.SetId(pCmd->GetId());
					item.SetLabel(pCmd->GetCaption());
					append_items.Add(item);
				}
				pGISToolBar->SetCustomOverflowItems(prepend_items, append_items);

				wxString sCaption = pGISToolBar->GetCaption();

				wxGISCommandBar* pBar = static_cast<wxGISCommandBar*>(pGISToolBar);
				int nImgIndex = m_pApp->IsPaneShown(pBar->GetName()) == true ? 1 : 0;
				m_pTreeCtrl->AppendItem(m_nToolBarsId, sCaption, nImgIndex, nImgIndex, new wxBarTreeItemData(pBar));
			}
			}
			break;
		case enumGISCBSubMenu:
		case enumGISCBNone:
		default:
			break;
		}
	}
}

void wxGISToolBarPanel::OnDeleteCommandBar(wxCommandEvent& event)
{
	wxTreeItemId nSelId = m_pTreeCtrl->GetSelection();
	if(!nSelId.IsOk())
		return;
	wxBarTreeItemData* pData = (wxBarTreeItemData*)m_pTreeCtrl->GetItemData(nSelId);
	if(pData == NULL)
		return;
	wxGISCommandBar* pBar = pData->m_pBar;
	m_pTreeCtrl->Delete(nSelId);
	m_pApp->RemoveCommandBar(pBar);
}

void wxGISToolBarPanel::OnAddButton(wxCommandEvent& event)
{
	wxGISAddCommandDlg dlg(m_pApp, this);
	if(dlg.ShowModal() == wxID_OK)
	{
		wxTreeItemId nSelId = m_pTreeCtrl->GetSelection();
		if(!nSelId.IsOk())
			return;
		wxBarTreeItemData* pData = (wxBarTreeItemData*)m_pTreeCtrl->GetItemData(nSelId);
		if(pData == NULL)
			return;
		wxGISCommandBar* pBar = pData->m_pBar;
		wxWindow* pWnd = dynamic_cast<wxWindow*>(pBar);

		for(size_t i = 0; i < dlg.m_IDArray.size(); ++i)
		{
			pBar->AddCommand(m_pApp->GetCommand(dlg.m_IDArray[i]));
			if(pWnd)
			{
				wxSize sz = pWnd->GetSize();
				m_pApp->GetAuiManager()->GetPane(pWnd).BestSize(sz);
			}
		}
		m_pApp->GetAuiManager()->Update();
		LoadCommands();
	}
}

void wxGISToolBarPanel::OnRemoveButton(wxCommandEvent& event)
{
	long item = GetSelectedCommandItem();
	if(item != wxNOT_FOUND)
	{
		wxTreeItemId nSelId = m_pTreeCtrl->GetSelection();
		if(nSelId.IsOk())
		{
			m_buttonslist->DeleteItem(item);
			wxBarTreeItemData* pData = (wxBarTreeItemData*)m_pTreeCtrl->GetItemData(nSelId);
			if(pData != NULL)
			{
				wxGISCommandBar* pBar = pData->m_pBar;
				pBar->RemoveCommand(item);
				wxWindow* pWnd = dynamic_cast<wxWindow*>(pBar);
				if(pWnd)
				{
					wxSize sz = pWnd->GetSize();
					m_pApp->GetAuiManager()->GetPane(pWnd).BestSize(sz);
				}
				m_pApp->GetAuiManager()->Update();
			}
		}
	}
}

void wxGISToolBarPanel::OnMoveUp(wxCommandEvent& event)
{
	wxTreeItemId nSelId = m_pTreeCtrl->GetSelection();
	if(!nSelId.IsOk())
		return;
	wxBarTreeItemData* pData = (wxBarTreeItemData*)m_pTreeCtrl->GetItemData(nSelId);
	if(pData == NULL)
		return;
	wxGISCommandBar* pBar = pData->m_pBar;
	if(m_bToolsFocus)
	{
		wxString sCaption = pBar->GetCaption();
		sCaption.Replace(wxT("&"), wxT(""));

		wxGISMenuBar* pwxGISMenuBar = m_pApp->GetGISMenuBar();
		int nPos = pwxGISMenuBar->GetMenuPos(pBar);
		if(nPos == wxNOT_FOUND)
			return;

		wxTreeItemId nNewItemId = m_pTreeCtrl->InsertItem(m_nMenubarId, nPos - 1, sCaption, -1, -1, new wxBarTreeItemData(pBar));
		m_pTreeCtrl->Delete(nSelId);
		m_pTreeCtrl->SelectItem(nNewItemId);

		pwxGISMenuBar->MoveLeft(nPos);
		pwxGISMenuBar->Update();

		m_pTreeCtrl->SetFocus();
	}
	if(m_bCmdFocus)
	{
		long item = GetSelectedCommandItem();
		if(item != wxNOT_FOUND)
		{
			//IGISCommandBar* pBar = m_CategoryArray[selpos];
			pBar->MoveCommandLeft(item);
			m_pApp->GetAuiManager()->Update();

			LoadCommands();
			m_buttonslist->Select(item - 1);
			m_buttonslist->SetFocus();
		}
	}
}

void wxGISToolBarPanel::OnMoveDown(wxCommandEvent& event)
{
	wxTreeItemId nSelId = m_pTreeCtrl->GetSelection();
	if(!nSelId.IsOk())
		return;
	wxBarTreeItemData* pData = (wxBarTreeItemData*)m_pTreeCtrl->GetItemData(nSelId);
	if(pData == NULL)
		return;
	wxGISCommandBar* pBar = pData->m_pBar;
	if(m_bToolsFocus)
	{
		wxString sCaption = pBar->GetCaption();
		sCaption.Replace(wxT("&"), wxT(""));

		wxGISMenuBar* pwxGISMenuBar = m_pApp->GetGISMenuBar();
		int nPos = pwxGISMenuBar->GetMenuPos(pBar);
		if(nPos == wxNOT_FOUND)
			return;

		wxTreeItemId nNewItemId = m_pTreeCtrl->InsertItem(m_nMenubarId, nPos + 2, sCaption, -1, -1, new wxBarTreeItemData(pBar));
		m_pTreeCtrl->Delete(nSelId);
		m_pTreeCtrl->SelectItem(nNewItemId);

		pwxGISMenuBar->MoveRight(nPos);
		pwxGISMenuBar->Update();

		m_pTreeCtrl->SetFocus();
	}
	if(m_bCmdFocus)
	{
		long item = GetSelectedCommandItem();
		if(item != wxNOT_FOUND)
		{
			pBar->MoveCommandRight(item);
			m_pApp->GetAuiManager()->Update();

			LoadCommands();
			m_buttonslist->Select(item + 1);
			m_buttonslist->SetFocus();
		}
	}
}

void wxGISToolBarPanel::OnLeftDown(wxMouseEvent& event)
{
	event.Skip(true);
	wxPoint pt = event.GetPosition();
	int nFlags;
	wxTreeItemId nItemId = m_pTreeCtrl->HitTest(pt, nFlags);
	if(nItemId.IsOk() && (nFlags & wxTREE_HITTEST_ONITEMICON))
	{
		wxBarTreeItemData* pData = (wxBarTreeItemData*)m_pTreeCtrl->GetItemData(nItemId);
		if(pData == NULL)
			return;
		wxGISCommandBar* pBar = pData->m_pBar;
		if(pBar->GetType() ==  enumGISCBToolbar)
		{
			bool bCheck = m_pApp->IsPaneShown(pBar->GetName());
			bCheck = !bCheck;
			m_pApp->ShowPane(pBar->GetName(), bCheck);
			m_pTreeCtrl->SetItemImage(nItemId, bCheck == true ? 1 : 0);
			m_pTreeCtrl->SetItemImage(nItemId, bCheck == true ? 1 : 0, wxTreeItemIcon_Selected);
		}
	}
}

long wxGISToolBarPanel::GetSelectedCommandItem(void)
{
	return m_buttonslist->GetFirstSelected();
}


//--------------------------------------------------------------------------
/// Class wxGISCommandPanel
//--------------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGISCommandPanel, wxPanel)
	EVT_LISTBOX(wxGISCommandPanel::ID_LSTBX, wxGISCommandPanel::OnListboxSelect)
	EVT_SPLITTER_DCLICK(wxID_ANY, wxGISCommandPanel::OnDoubleClickSash)
	EVT_LIST_ITEM_RIGHT_CLICK(wxGISCommandPanel::ID_LSTCTRL, wxGISCommandPanel::OnListctrlRClick)
	EVT_LIST_ITEM_ACTIVATED(wxGISCommandPanel::ID_LSTCTRL, wxGISCommandPanel::OnListctrlActivated)
	EVT_MENU(ID_ONSETKEYCODE, wxGISCommandPanel::OnSetKeyCode)
END_EVENT_TABLE()

wxGISCommandPanel::wxGISCommandPanel( wxGISApplicationEx* pApp, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	m_pApp = pApp;

    m_nSashPos = 150;
    SerializePanel(false);

	wxBoxSizer* bSizer5;
	bSizer5 = new wxBoxSizer( wxHORIZONTAL );

	m_splitter2 = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxNO_BORDER );
	//m_splitter2->Connect( wxEVT_IDLE, wxIdleEventHandler( wxGISCommandPanel::m_splitter2OnIdle ), NULL, this );
	m_splitter2->Bind( wxEVT_IDLE, &wxGISCommandPanel::m_splitter2OnIdle, this );
	bSizer5->Add( m_splitter2, 1, wxEXPAND, 5 );

	//fill m_listBox1
	wxCommandPtrArray CommandArr = m_pApp->GetCommands();
	for(size_t i = 0; i < CommandArr.GetCount(); ++i)
	{
		wxString sCat = CommandArr[i]->GetCategory();
        m_CategoryMap[sCat].Add(CommandArr[i]);
	}

	wxArrayString CatArray;
	for(CATEGORYMAP::iterator IT = m_CategoryMap.begin(); IT != m_CategoryMap.end(); ++IT)
	{
		CatArray.Add(IT->first);
	}

	m_listBox1 = new wxListBox( m_splitter2, wxGISCommandPanel::ID_LSTBX, wxDefaultPosition, wxDefaultSize, CatArray, wxLC_SINGLE_SEL|wxLC_SORT_ASCENDING| wxNO_BORDER );
//	m_listBox1->InsertColumn(0, _("Category"));
	//bSizer5->Add( m_listBox1, 0, wxALL, 5 );

	m_listCtrl3 = new wxListCtrl( m_splitter2, wxGISCommandPanel::ID_LSTCTRL, wxDefaultPosition, wxDefaultSize, wxLC_NO_SORT_HEADER|wxLC_REPORT|wxLC_SINGLE_SEL|wxLC_SORT_ASCENDING|wxNO_BORDER );//wxLC_EDIT_LABELS|wxLC_NO_HEADER|wxLC_VRULES
	m_listCtrl3->InsertColumn(0, _("Command Name"), wxLIST_FORMAT_LEFT, 90);
	m_listCtrl3->InsertColumn(1, _("Description"), wxLIST_FORMAT_LEFT, 120);
	m_listCtrl3->InsertColumn(2, _("KeyCode"), wxLIST_FORMAT_LEFT, 60);
	m_ImageList.Create(16, 16);
	m_listCtrl3->SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);
	//bSizer5->Add( m_listCtrl3, 0, wxALL, 5 );
	m_splitter2->SetSashGravity(0.5);
	m_splitter2->SplitVertically(m_listBox1, m_listCtrl3, 150);

	m_pContextMenu = new wxMenu();//_("Command menu")
	wxMenuItem *item = new wxMenuItem(m_pContextMenu, ID_ONSETKEYCODE, _("Set keycode"));
#ifdef __WIN32__
	item->SetBitmap(wxKeyCodeDlg::GetBitmap());
#endif
	m_pContextMenu->Append(item);

    this->SetSizerAndFit(bSizer5);
	this->Layout();

	m_listBox1->Select(0);
	wxCommandEvent event;
	event.SetString(m_listBox1->GetString(0));//CatArray[0]
	OnListboxSelect(event);
}

wxGISCommandPanel::~wxGISCommandPanel()
{
	wxDELETE(m_pContextMenu);
    SerializePanel(true);
}

void wxGISCommandPanel::SerializePanel(bool bSave)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return;

	if(bSave)
	{
        oConfig.Write(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/sashpos2")), m_splitter2->GetSashPosition());
	}
	else
	{
		//load
		m_nSashPos = oConfig.ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/sashpos2")), m_nSashPos);
	}
}

void wxGISCommandPanel::OnListboxSelect(wxCommandEvent& event)
{
	int selpos = m_listBox1->GetSelection();
	if(selpos == wxNOT_FOUND)
		return;

	wxString selName = m_listBox1->GetString(selpos);
	wxCommandPtrArray CommandArray = m_CategoryMap[selName];
	m_listCtrl3->DeleteAllItems();
	m_ImageList.RemoveAll();
	for(size_t i = 0; i < CommandArray.GetCount(); ++i)
	{
		wxString sName = wxStripMenuCodes(CommandArray[i]->GetCaption());
		wxString sMessage = CommandArray[i]->GetMessage();
		wxString sKeyCode = m_pApp->GetGISAcceleratorTable()->GetText(CommandArray[i]->GetId());
		long pos = m_listCtrl3->InsertItem(i, sName);
		wxIcon oIcon = CommandArray[i]->GetBitmap();
		if(oIcon.IsOk())
		{
            int nIndex = m_ImageList.Add(oIcon);
            m_listCtrl3->SetItemImage(pos, nIndex);
		}
        else
        {
            m_listCtrl3->SetItemImage(pos, wxNOT_FOUND);
        }
		m_listCtrl3->SetItem(pos, 1, sMessage);
		m_listCtrl3->SetItem(pos, 2, sKeyCode);
		m_listCtrl3->SetItemData(pos,  CommandArray[i]->GetId());
	}
	m_listCtrl3->Update();
}

void wxGISCommandPanel::OnDoubleClickSash(wxSplitterEvent& event)
{
	event.Veto();
}

void wxGISCommandPanel::OnListctrlActivated(wxListEvent& event)
{
	m_CurSelection = event.GetIndex();
	SetKeyCode(event.GetIndex());
}

void wxGISCommandPanel::OnListctrlRClick(wxListEvent& event)
{
	m_CurSelection = event.GetIndex();
	PopupMenu(m_pContextMenu);
}

void wxGISCommandPanel::SetKeyCode(int pos)
{
	wxKeyCodeDlg dlg(this);
	wxListItem item;
	item.SetColumn(2);
	item.SetId(pos);
	item.SetMask(wxLIST_MASK_TEXT);
	if(m_listCtrl3->GetItem(item))
		dlg.m_sKeyCode = item.GetText();
	if(dlg.ShowModal() == wxID_OK)
	{
        long cmd(wxID_ANY);
		long CmdID = m_listCtrl3->GetItemData(pos);
		wxGISAcceleratorTable *pAccTab = m_pApp->GetGISAcceleratorTable();
        if(dlg.m_Key == wxNOT_FOUND) //remove acc
        {
            m_listCtrl3->SetItem(pos, 2, wxEmptyString);
            pAccTab->Remove(pAccTab->GetEntry(CmdID));
        }
        else
        {
		    cmd = pAccTab->Add(wxAcceleratorEntry(dlg.m_Flags, dlg.m_Key, CmdID));
		    wxString sKeyCode = pAccTab->GetText(CmdID);
		    m_listCtrl3->SetItem(pos, 2, sKeyCode);
        }
		if(cmd != wxID_ANY)
		{
			for(int i = 0; i < m_listCtrl3->GetItemCount(); ++i)
			{
				long CahgedCmdID = m_listCtrl3->GetItemData(i);
				if(CahgedCmdID == cmd)
				m_listCtrl3->SetItem(pos, 2, wxT(""));
			}
		}
	}
}

void wxGISCommandPanel::OnSetKeyCode(wxCommandEvent& event)
{
	SetKeyCode(m_CurSelection);
}

//-----------------------------------------------------------------------------------
/// Class wxGISCustomizeDlg
//-----------------------------------------------------------------------------------

wxGISCustomizeDlg::wxGISCustomizeDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	m_pApp = dynamic_cast<wxGISApplicationEx*>(parent);
	//this->SetSizeHints( wxSize( 540,400 ), wxDefaultSize );
	if(NULL == m_pApp)
		return;

	wxBoxSizer* bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_AuiNotebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxNO_BORDER | wxAUI_NB_TAB_MOVE );
	m_AuiNotebook->AddPage(new wxGISToolBarPanel(m_pApp, m_AuiNotebook), _("ToolBars & Menues"));
	m_AuiNotebook->AddPage(new wxGISCommandPanel(m_pApp, m_AuiNotebook), _("Commands"));
#ifdef __WXGTK__
	m_AuiNotebook->SetArtProvider(new wxGISTabArt());
#endif // __WXGTK__
	bSizerMain->Add( m_AuiNotebook, 1, wxEXPAND | wxALL, 5 );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK, wxString(_("Close")) );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizer->Realize();
	bSizerMain->Add( m_sdbSizer, 0, wxALL|wxEXPAND, 5 );

    this->SetSizerAndFit(bSizerMain);
	this->Layout();

    SerializeFramePos(false);
}

wxGISCustomizeDlg::~wxGISCustomizeDlg()
{
}

void wxGISCustomizeDlg::SerializeFramePos(bool bSave)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return;

	if(bSave)
	{
		if( IsMaximized() )
			oConfig.Write(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/maxi")), true);
		else
		{
			int x, y, w, h;
			GetClientSize(&w, &h);
			GetPosition(&x, &y);
			oConfig.Write(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/maxi")), false);
			oConfig.Write(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/width")), w);
			oConfig.Write(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/height")), h);
			oConfig.Write(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/xpos")), x);
			oConfig.Write(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/ypos")), y);
		}
	}
	else
	{
		//load
		bool bMaxi = oConfig.ReadBool(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/maxi")), false);
		if(!bMaxi)
		{
			int x = oConfig.ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/xpos")), 50);
			int y = oConfig.ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/ypos")), 50);
			int w = oConfig.ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/width")), 850);
			int h = oConfig.ReadInt(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/customizedlg/height")), 530);
			Move(x, y);
			SetClientSize(w, h);
		}
		else
		{
			Maximize();
		}
	}
}

void wxGISCustomizeDlg::EndModal(int retCode)
{
    SerializeFramePos(true);
    wxDialog::EndModal(retCode);
}

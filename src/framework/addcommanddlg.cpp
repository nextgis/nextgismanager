/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  add command in command bar dialog.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010,2013  Bishop
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
#include "wxgis/framework/addcommanddlg.h"
#include <wx/accel.h>
#include <wx/utils.h>


BEGIN_EVENT_TABLE(wxGISAddCommandDlg, wxDialog)
	EVT_LISTBOX(wxGISAddCommandDlg::ID_LSTBX, wxGISAddCommandDlg::OnListboxSelect)
	EVT_SPLITTER_DCLICK(wxID_ANY, wxGISAddCommandDlg::OnDoubleClickSash)
	EVT_LIST_ITEM_ACTIVATED(wxGISAddCommandDlg::ID_LSTCTRL, wxGISAddCommandDlg::OnListctrlActivated)
	EVT_UPDATE_UI(wxID_OK, wxGISAddCommandDlg::OnUpdateOKUI)
	EVT_BUTTON(wxID_OK, wxGISAddCommandDlg::OnOk)
END_EVENT_TABLE()

wxGISAddCommandDlg::wxGISAddCommandDlg( wxGISApplication* pGxApp, wxWindow* parent,  wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	m_pGxApp = pGxApp;
	if(!m_pGxApp)
		return;

    m_nSashPos = 150;
    SerializeDialogPos(false);

	this->SetSizeHints( wxSize( 540,400 ), wxDefaultSize );
	wxBoxSizer* bSizer5 = new wxBoxSizer( wxVERTICAL );

	m_Splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D | wxNO_BORDER );
	//m_Splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( wxGISAddCommandDlg::SplitterOnIdle ), NULL, this );
	m_Splitter->Bind( wxEVT_IDLE, &wxGISAddCommandDlg::SplitterOnIdle, this ); 
	bSizer5->Add( m_Splitter, 1, wxEXPAND, 5 );

	//fill m_listBox1
	wxCommandPtrArray CommandArr = m_pGxApp->GetCommands();
	for(size_t i = 0; i < CommandArr.size(); ++i)
	{
		wxString sCat = CommandArr[i]->GetCategory();
		m_CategoryMap[sCat].Add(CommandArr[i]);
	}

	wxArrayString CatArray;
	for(CATEGORYMAP::iterator IT = m_CategoryMap.begin(); IT != m_CategoryMap.end(); ++IT)
	{
		CatArray.Add(IT->first);
	}

	m_ListBox = new wxListBox( m_Splitter, wxGISAddCommandDlg::ID_LSTBX, wxDefaultPosition, wxDefaultSize, CatArray, wxLC_SINGLE_SEL|wxLC_SORT_ASCENDING  |wxNO_BORDER);

	m_ListCtrl = new wxListView( m_Splitter, wxGISAddCommandDlg::ID_LSTCTRL, wxDefaultPosition, wxDefaultSize, wxLC_NO_SORT_HEADER|wxLC_REPORT|wxLC_SORT_ASCENDING | wxNO_BORDER );
	m_ListCtrl->InsertColumn(0, _("Command Name"), wxLIST_FORMAT_LEFT, 90);
	m_ListCtrl->InsertColumn(1, _("Description"), wxLIST_FORMAT_LEFT, 120);
	m_ListCtrl->InsertColumn(2, _("KeyCode"), wxLIST_FORMAT_LEFT, 60);
	m_ImageList.Create(16, 16);
	m_ListCtrl->SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);
	m_Splitter->SetSashGravity(0.5);
	m_Splitter->SplitVertically(m_ListBox, m_ListCtrl, 150);

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK, wxString(_("OK")) );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL, wxString(_("Cancel")));
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();

	bSizer5->Add( m_sdbSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( bSizer5 );
	this->Layout();

	m_ListBox->Select(0);
	wxCommandEvent event;
	event.SetString(m_ListBox->GetString(0));//CatArray[0]
	OnListboxSelect(event);
}

wxGISAddCommandDlg::~wxGISAddCommandDlg()
{
}


void wxGISAddCommandDlg::SerializeDialogPos(bool bSave)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
		return;

	if(bSave)
	{
		if( IsMaximized() )
			oConfig.Write(enumGISHKCU, m_pGxApp->GetAppName() + wxString(wxT("/addcommanddlg/maxi")), true);
		else
		{
			int x, y, w, h;
			GetClientSize(&w, &h);
			GetPosition(&x, &y);
			oConfig.Write(enumGISHKCU, m_pGxApp->GetAppName() + wxString(wxT("/addcommanddlg/maxi")), false);
			oConfig.Write(enumGISHKCU, m_pGxApp->GetAppName() + wxString(wxT("/addcommanddlg/width")), w);
			oConfig.Write(enumGISHKCU, m_pGxApp->GetAppName() + wxString(wxT("/addcommanddlg/height")), h);
			oConfig.Write(enumGISHKCU, m_pGxApp->GetAppName() + wxString(wxT("/addcommanddlg/xpos")), x);
			oConfig.Write(enumGISHKCU, m_pGxApp->GetAppName() + wxString(wxT("/addcommanddlg/ypos")), y);
		}
        oConfig.Write(enumGISHKCU, m_pGxApp->GetAppName() + wxString(wxT("/addcommanddlg/sashpos1")), m_Splitter->GetSashPosition());
	}
	else
	{
		//load
		bool bMaxi = oConfig.ReadBool(enumGISHKCU, m_pGxApp->GetAppName() + wxString(wxT("/addcommanddlg/maxi")), false);
		if(!bMaxi)
		{
			int x = oConfig.ReadInt(enumGISHKCU, m_pGxApp->GetAppName() + wxString(wxT("/addcommanddlg/xpos")), 50);
			int y = oConfig.ReadInt(enumGISHKCU, m_pGxApp->GetAppName() + wxString(wxT("/addcommanddlg/ypos")), 50);
			int w = oConfig.ReadInt(enumGISHKCU, m_pGxApp->GetAppName() + wxString(wxT("/addcommanddlg/width")), 850);
			int h = oConfig.ReadInt(enumGISHKCU, m_pGxApp->GetAppName() + wxString(wxT("/addcommanddlg/height")), 530);
			Move(x, y);
			SetClientSize(w, h);
		}
		else
		{
			Maximize();
		}
		m_nSashPos = oConfig.ReadInt(enumGISHKCU, m_pGxApp->GetAppName() + wxString(wxT("/addcommanddlg/sashpos1")), m_nSashPos);
	}
}


void wxGISAddCommandDlg::OnListboxSelect(wxCommandEvent& event)
{
	int selpos = m_ListBox->GetSelection();
	if(selpos == wxNOT_FOUND)
		return;

	wxString selName = m_ListBox->GetString(selpos);
	wxCommandPtrArray CommandArr = m_CategoryMap[selName];
	m_ListCtrl->DeleteAllItems();
	m_ImageList.RemoveAll();
	for(size_t i = 0; i < CommandArr.GetCount(); ++i)
	{
		wxString sName = wxStripMenuCodes(CommandArr[i]->GetCaption());
		wxString sMessage = CommandArr[i]->GetMessage();
		wxString sKeyCode = m_pGxApp->GetGISAcceleratorTable()->GetText(CommandArr[i]->GetID());
		long pos = m_ListCtrl->InsertItem(i, sName);
		m_ListCtrl->SetItem(pos, 1, sMessage);
		m_ListCtrl->SetItem(pos, 2, sKeyCode);
		m_ListCtrl->SetItemData(pos,  CommandArr[i]->GetID());
		wxIcon oIcon = CommandArr[i]->GetBitmap();
		if(oIcon.IsOk())
		{
			int nIndex = m_ImageList.Add(oIcon);
            m_ListCtrl->SetItemImage(pos, nIndex);
		}
        else
        {
            m_ListCtrl->SetItemImage(pos, wxNOT_FOUND);
        }	
    }
	m_ListCtrl->Update();
}

void wxGISAddCommandDlg::OnDoubleClickSash(wxSplitterEvent& event)
{
	event.Veto();
}

void wxGISAddCommandDlg::OnListctrlActivated(wxListEvent& event)
{
	FillIDArray();
	EndModal(wxID_OK);
}

void wxGISAddCommandDlg::OnUpdateOKUI(wxUpdateUIEvent& event)
{
	if(m_ListCtrl->GetSelectedItemCount() > 0)
		event.Enable(true);
	else
		event.Enable(false);
}

void wxGISAddCommandDlg::OnOk(wxCommandEvent& event)
{
	FillIDArray();
	EndModal(wxID_OK);
}

void wxGISAddCommandDlg::FillIDArray(void)
{
	long selpos = m_ListCtrl->GetFirstSelected();
	while(selpos != wxNOT_FOUND)
	{
		long ID = m_ListCtrl->GetItemData(selpos);
		m_IDArray.push_back(ID);
		selpos = m_ListCtrl->GetNextSelected(selpos);
	}
}


void wxGISAddCommandDlg::EndModal(int retCode)
{
    SerializeDialogPos(true);
    wxDialog::EndModal(retCode);
}
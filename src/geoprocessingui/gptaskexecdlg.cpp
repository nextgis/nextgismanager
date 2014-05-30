/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  wxGxTaskExecDlg class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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

#include "wxgis/geoprocessingui/gptaskexecdlg.h"
#include "wxgis/geoprocessingui/gptoolbox.h"

#include "../../art/process_stop.xpm"
#include "../../art/state.xpm"
/*
//////////////////////////////////////////////////////////////////
// wxGxTaskPanel
//////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGxTaskExecDlg, wxDialog)
    EVT_BUTTON(wxID_MORE, wxGxTaskExecDlg::OnExpand)
    EVT_BUTTON(ID_CANCEL_PROCESS, wxGxTaskExecDlg::OnCancelTask)
    EVT_BUTTON(wxID_CANCEL, wxGxTaskExecDlg::OnCancel)
	EVT_CLOSE(wxGxTaskExecDlg::OnClose)
	EVT_PROCESS_FINISH(wxGxTaskExecDlg::OnFinish)
END_EVENT_TABLE()

wxGxTaskExecDlg::wxGxTaskExecDlg(wxGISGPToolManager* pToolManager, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style), m_bExpand(false), m_nState(enumGISMessageUnk), m_nTaskID(wxNOT_FOUND)
{
    m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(state_xpm));

    m_pToolManager = pToolManager;

    m_bMainSizer = new wxBoxSizer( wxVERTICAL );
 	wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer( 1, 4, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxHORIZONTAL );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

    m_pStateBitmap = new wxStaticBitmap( this, wxID_ANY, m_ImageList.GetIcon(4), wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_pStateBitmap, 0, wxALL, 5 );

    wxStaticText * soTitle = new wxStaticText(this, wxID_ANY, title);
    wxFont titleFont = this->GetFont();
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    soTitle->SetFont(titleFont);
    soTitle->Wrap( -1 );
	fgSizer1->Add( soTitle, 1, wxALL | wxEXPAND, 5 );


    m_ExpandBitmap = wxBitmap(m_ImageList.GetIcon(4));
    wxImage bwImage = m_ExpandBitmap.ConvertToImage();
    bwImage = bwImage.Rotate90();
    m_ExpandBitmap = wxBitmap(bwImage);
    m_ExpandBitmapRotated = wxBitmap(bwImage.Mirror(false));
    m_ExpandBitmapBW = wxBitmap(bwImage.ConvertToGreyscale());
    m_ExpandBitmapBWRotated = wxBitmap(bwImage.ConvertToGreyscale().Mirror(false));
    m_bpExpandButton = new wxBitmapButton( this, wxID_MORE, m_ExpandBitmapBW, wxDefaultPosition, wxDefaultSize, 0 );
//    m_bpExpandButton->SetBackgroundColour(bgColour);
    m_bpExpandButton->SetBitmapDisabled(m_ExpandBitmapBW);
    m_bpExpandButton->SetBitmapLabel(m_ExpandBitmapBW);
    m_bpExpandButton->SetBitmapSelected(m_ExpandBitmap);
    m_bpExpandButton->SetBitmapHover(m_ExpandBitmap);
    //wxBitmapButton* bpExpandButton = new wxBitmapButton( this, wxID_MORE, wxBitmap(expand_16_xpm), wxDefaultPosition, wxDefaultSize, 0 );
    m_bpExpandButton->SetToolTip(_("Expand"));
	fgSizer1->Add( m_bpExpandButton, 0, wxALL, 5 );

    wxBitmap NormalCBitmap = wxBitmap(process_stop_xpm);
    bwImage = NormalCBitmap.ConvertToImage();

    wxBitmap bwCBitmap = bwImage.ConvertToGreyscale();
    m_bpCloseButton = new wxBitmapButton( this, ID_CANCEL_PROCESS, bwCBitmap, wxDefaultPosition, wxDefaultSize, 0 );
    //m_bpCloseButton->SetBackgroundColour(bgColour);
    m_bpCloseButton->SetBitmapDisabled(bwCBitmap);
    m_bpCloseButton->SetBitmapLabel(bwCBitmap);
    m_bpCloseButton->SetBitmapSelected(NormalCBitmap);
    m_bpCloseButton->SetBitmapHover(NormalCBitmap);
    //wxBitmapButton* bpCloseButton = new wxBitmapButton( this, ID_CANCEL_PROCESS, wxBitmap(process_stop_xpm), wxDefaultPosition, wxDefaultSize, 0 );
    m_bpCloseButton->SetToolTip(_("Cancel"));
    fgSizer1->Add( m_bpCloseButton, 0, wxALL, 5 );

    m_bMainSizer->Add(fgSizer1, 0 ,wxEXPAND | wxLEFT | wxRIGHT, 5);// | wxBOTTOM 

    m_Text = new wxStaticText(this, wxID_ANY, wxEmptyString);
    m_Text->Wrap( -1 );
    m_bMainSizer->Add(m_Text, 0, wxEXPAND | wxALL, 5);

    wxGISProgressor* pProgressor = new wxGISProgressor(this);
    pProgressor->SetInitialSize(wxSize(-1, 18));
    pProgressor->SetYield();
    m_bMainSizer->Add(pProgressor, 0, wxALL|wxEXPAND, 5);
    m_pProgressor = static_cast<IProgressor*>(pProgressor);

    m_pHtmlWindow = new wxHtmlWindow(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_DEFAULT_STYLE | wxBORDER_THEME);
    m_pHtmlWindow->SetInitialSize(wxSize(-1, 255));
    m_bMainSizer->Add(m_pHtmlWindow, 1, wxEXPAND | wxALL, 5);
    m_pHtmlWindow->Show(false);

   	this->SetSizerAndFit( m_bMainSizer );
	this->Layout();

    Fit();
}

wxGxTaskExecDlg::~wxGxTaskExecDlg(void)
{
}

void wxGxTaskExecDlg::OnExpand(wxCommandEvent & event)
{
    m_bExpand = !m_bExpand;

    if(m_bExpand)
    {
        FillHtmlWindow();
    }

    m_bpExpandButton->SetBitmapDisabled(m_bExpand == false ? m_ExpandBitmapBW : m_ExpandBitmapBWRotated);
    m_bpExpandButton->SetBitmapLabel(m_bExpand == false ? m_ExpandBitmapBW : m_ExpandBitmapBWRotated);
    m_bpExpandButton->SetBitmapSelected(m_bExpand == false ? m_ExpandBitmap : m_ExpandBitmapRotated);
    m_bpExpandButton->SetBitmapHover(m_bExpand == false ? m_ExpandBitmap : m_ExpandBitmapRotated);

    m_pHtmlWindow->Show(m_bExpand);
    wxSize Size = GetSize();
    Fit();
    wxSize NewSize = GetSize();
    SetSize(wxSize(Size.x, NewSize.y));
}

void wxGxTaskExecDlg::FillHtmlWindow()
{
    wxString sText(wxT("<html><body>"));
    for(size_t i = 0 ; i < m_MessageArray.size(); ++i)
    {
        switch(m_MessageArray[i].nType)
        {
        case enumGISMessageErr:
            sText += wxString(wxT("<FONT COLOR=red>"));
            sText += m_MessageArray[i].sMessage;
            sText += wxString(wxT("</FONT>"));
            break;
        case enumGISMessageQuestion:
            sText += wxString(wxT("<FONT color=blue>"));
            sText += m_MessageArray[i].sMessage;
            sText += wxString(wxT("</FONT>"));
            break;
        case enumGISMessageInfo:
            sText += wxString(wxT("<b>"));
            sText += m_MessageArray[i].sMessage;
            sText += wxString(wxT("</b>"));
            break;
        case enumGISMessageWarning:
            sText += wxString(wxT("<FONT color=green>"));
            sText += m_MessageArray[i].sMessage;
            sText += wxString(wxT("</FONT>"));
            break;
        case enumGISMessageNorm:
        case enumGISMessageUnk:
        default:
            sText += m_MessageArray[i].sMessage;
        }
        sText += wxString(wxT("<br>"));
    }
    sText += wxString(wxT("</body></html>"));
    m_pHtmlWindow->SetPage(sText);//bool bTest = 
    m_pHtmlWindow->Scroll(-1, 5000);
}

void wxGxTaskExecDlg::OnCancel(wxCommandEvent & event)
{
    //if exec
    //cancel m_nTaskID
    if(m_pToolManager)
		m_pToolManager->CancelProcess(m_nTaskID);

    //else close dialog
    Show(false);
    //destroy
    this->Destroy();

}

void wxGxTaskExecDlg::OnCancelTask(wxCommandEvent & event)
{
    if(m_pToolManager)
		m_pToolManager->CancelProcess(m_nTaskID);

    //else close dialog
    Show(false);
    //destroy
    this->Destroy();
}

void wxGxTaskExecDlg::OnClose(wxCloseEvent& event)
{
    if(m_pToolManager)
	{
		if(m_pToolManager->GetProcessState(m_nTaskID) == enumGISTaskWork)
		{
			wxMessageBox(wxString(_("You must stop task before closing dialog!")), wxString(_("Warning")), wxCENTRE | wxICON_WARNING | wxOK );
			event.Veto();
			return;
		}
	}
	event.Skip();
}

void wxGxTaskExecDlg::PutMessage(wxString sMessage, size_t nIndex, wxGISEnumMessageType nType)
{
    switch(nType)
    {
    case enumGISMessageTitle:
        m_Text->SetLabel(sMessage);
        return;
    case enumGISMessageErr:
        m_Text->SetLabel(sMessage);
        m_Icon = m_ImageList.GetIcon(2);
        m_pStateBitmap->SetIcon(m_Icon);
        break;
    case enumGISMessageQuestion:
    case enumGISMessageInfo:
    case enumGISMessageWarning:
    case enumGISMessageNorm:
    case enumGISMessageUnk:
    default:
        break;
    }


    TASKMESSAGE msg = {nType, sMessage};
    if(nIndex == -1 || m_MessageArray.size() < nIndex)
        m_MessageArray.push_back(msg);
    else
        m_MessageArray.insert(m_MessageArray.begin() + nIndex, msg);
    if(!m_bExpand)
        return;
    FillHtmlWindow();
}

void wxGxTaskExecDlg::SetTaskID(int nTaskID)
{
    m_nTaskID = nTaskID;
}

void wxGxTaskExecDlg::OnFinish(wxGISProcessEvent & event)
{
	m_bpCloseButton->Enable(false);
}

//////////////////////////////////////////////////////////////////
// wxGxTaskObjectExecDlg
//////////////////////////////////////////////////////////////////

wxGxTaskObjectExecDlg::wxGxTaskObjectExecDlg(wxGxTaskObject* pGxTaskObject, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxGxTaskExecDlg(NULL, parent, id, title, pos, size, style)
{
    m_pGxTaskObject = pGxTaskObject;
    switch(pGxTaskObject->GetState())
    {
    case enumGISTaskQuered:
    case enumGISTaskWork:
        m_bpCloseButton->Enable(true);
        break;
    default:
        m_bpCloseButton->Enable(false);
        break;
    }
}

wxGxTaskObjectExecDlg::~wxGxTaskObjectExecDlg()
{
}

void wxGxTaskObjectExecDlg::OnCancel(wxCommandEvent & event)
{
    EndModal(wxID_CANCEL);
}

void wxGxTaskObjectExecDlg::OnCancelTask(wxCommandEvent & event)
{
    if(m_pGxTaskObject)
		m_pGxTaskObject->StopTask();
}

void wxGxTaskObjectExecDlg::OnClose(wxCloseEvent& event)
{
	event.Skip();
}

//////////////////////////////////////////////////////////////////
// wxGxTaskObject
//////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGxTaskObject, wxEvtHandler)
	EVT_PROCESS_START(wxGxTaskObject::OnTaskStateChanged)
	EVT_PROCESS_FINISH(wxGxTaskObject::OnFinish)
	EVT_PROCESS_CANCELED(wxGxTaskObject::OnTaskStateChanged)
	EVT_PROCESS_STATE_CHANGED(wxGxTaskObject::OnTaskStateChanged)
END_EVENT_TABLE()

wxGxTaskObject::wxGxTaskObject(wxGISGPToolManager* pToolManager, wxString sName, wxIcon LargeToolIcon, wxIcon SmallToolIcon) : m_nDonePercent(0), m_pTaskExecDlg(NULL)
{
    m_sName = sName;
    m_LargeToolIcon = LargeToolIcon;
    m_SmallToolIcon = SmallToolIcon;
    m_pProgressor = this;
    m_pCatalog = NULL;
    m_pToolManager = pToolManager;
	m_nCookie = wxNOT_FOUND;
}

wxGxTaskObject::~wxGxTaskObject(void)
{
}

void wxGxTaskObject::SetValue(int value)
{
    m_nDonePercent = value;
    if(m_pTaskExecDlg)
    {
        ITrackCancel* pTrackCancel = static_cast<ITrackCancel*>(m_pTaskExecDlg);
        if(pTrackCancel)
        {
            IProgressor* pProgressor = pTrackCancel->GetProgressor();
            if(pProgressor)
                pProgressor->SetValue(value);
        }
    }
    if(m_pCatalog)
        m_pCatalog->ObjectChanged(GetID());
}

bool wxGxTaskObject::Attach(IGxObject* pParent, IGxCatalog* pCatalog)
{
    bool bRes = IGxObject::Attach(pParent, pCatalog);
	if(bRes)
	{
		if(m_pToolManager)
			m_nCookie = m_pToolManager->Advise(this);
	}
	return bRes;
}

void wxGxTaskObject::Detach(void)
{
	if(m_nCookie != wxNOT_FOUND && m_pToolManager)
		m_pToolManager->Unadvise(m_nCookie);

    IGxObject::Detach();
}

wxIcon wxGxTaskObject::GetLargeImage(void)
{
	return m_LargeToolIcon;
}

wxIcon wxGxTaskObject::GetSmallImage(void)
{
	return m_SmallToolIcon;
}

bool wxGxTaskObject::Invoke(wxWindow* pParentWnd)
{
    if(GetState() == enumGISTaskError)//show tool dlg
		ShowToolConfig(pParentWnd);
    else
		ShowProcess(pParentWnd);
    return true;
}

wxGISEnumTaskStateType wxGxTaskObject::GetState(void)
{
    if(m_pToolManager)
        return m_pToolManager->GetProcessState(m_nTaskID);
    else
        return enumGISTaskUnk;
}

wxDateTime wxGxTaskObject::GetStart()
{
    if(m_pToolManager)
        return m_pToolManager->GetProcessStart(m_nTaskID);
    else
        return wxDateTime();
}

wxDateTime wxGxTaskObject::GetFinish()
{
    if(m_pToolManager)
        return m_pToolManager->GetProcessFinish(m_nTaskID);
    else
        return wxDateTime();
}

wxString wxGxTaskObject::GetLastMessage()
{
    if(m_MessageArray.size())    
        return m_MessageArray[m_MessageArray.size() - 1].sMessage;
    else
        return wxEmptyString;
}

bool wxGxTaskObject::StartTask()
{
    if(m_pToolManager)
        m_pToolManager->StartProcess(m_nTaskID);
    if(m_pCatalog)
        m_pCatalog->ObjectChanged(GetID());
    return true;
}

bool wxGxTaskObject::StopTask()
{
    if(m_pToolManager)
        m_pToolManager->CancelProcess(m_nTaskID);
    if(m_pCatalog)
        m_pCatalog->ObjectChanged(GetID());
    return true;
}

bool wxGxTaskObject::PauseTask()
{
    return StopTask();
    //m_pToolManager->PauseProcess(m_nTaskID);
    //return true;
}

void wxGxTaskObject::OnTaskStateChanged(wxGISProcessEvent & event)
{
	if(event.GetProcessID() != m_nTaskID)
		return;
    if(m_pCatalog)
        m_pCatalog->ObjectChanged(GetID());
}

void wxGxTaskObject::OnFinish(wxGISProcessEvent & event)
{
	if(event.GetProcessID() != m_nTaskID)
		return;
    if(m_pCatalog)
        m_pCatalog->ObjectChanged(GetID());
	if(m_pTaskExecDlg)
		m_pTaskExecDlg->GetEventHandler()->ProcessEvent(event);
}

void wxGxTaskObject::PutMessage(wxString sMessage, size_t nIndex, wxGISEnumMessageType nType)
{
    TASKMESSAGE msg = {nType, sMessage};
    if(nIndex == wxNOT_FOUND || m_MessageArray.size() < nIndex)
        m_MessageArray.push_back(msg);
    else
        m_MessageArray.insert(m_MessageArray.begin() + nIndex, msg);
    if(m_pTaskExecDlg)
        m_pTaskExecDlg->PutMessage(sMessage, nIndex, nType);
    if(m_pCatalog)
        m_pCatalog->ObjectChanged(GetID());
}

void wxGxTaskObject::SetTaskID(int nTaskID)
{
    m_nTaskID = nTaskID;
}

void wxGxTaskObject::ShowProcess(wxWindow* pParentWnd)
{
    m_pTaskExecDlg = new wxGxTaskObjectExecDlg(this, pParentWnd, wxID_ANY, m_sName);
    m_pTaskExecDlg->SetIcon(m_SmallToolIcon);
    SetValue(m_nDonePercent);
    for(size_t i = 0; i < m_MessageArray.size(); ++i)
        m_pTaskExecDlg->PutMessage(m_MessageArray[i].sMessage, i, m_MessageArray[i].nType);
    m_pTaskExecDlg->ShowModal();
    wxDELETE(m_pTaskExecDlg);
}

void wxGxTaskObject::ShowToolConfig(wxWindow* pParentWnd)
{
    wxGxToolExecute* pToolManager = dynamic_cast<wxGxToolExecute*>(m_pToolManager);
    if(pToolManager)
        pToolManager->PrepareTool(pParentWnd, m_pToolManager->GetProcessTool(m_nTaskID), false);
}

int wxGxTaskObject::GetPriority(void)
{
    if(m_pToolManager)
        return m_pToolManager->GetProcessPriority(m_nTaskID);
    return wxNOT_FOUND;
}

void wxGxTaskObject::SetPriority(int nNewPriority)
{
    if(m_pToolManager)
	{
		m_pToolManager->SetProcessPriority(m_nTaskID, nNewPriority);
		m_pCatalog->ObjectRefreshed(m_pParent->GetID());
	}
}
*/
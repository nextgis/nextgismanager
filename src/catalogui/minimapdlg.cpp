/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Mini map dialog
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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
 
#include "wxgis/catalogui/minimapdlg.h"
#include "wxgis/cartoui/cartocmd.h"

#include <wx/stream.h>
#include <wx/sstream.h>

//----------------------------------------------------------------------------
// wxGISMiniMapDlg
//----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxGISMiniMapDlg, wxDialog)

BEGIN_EVENT_TABLE(wxGISMiniMapDlg, wxDialog)
    EVT_MENU_RANGE(ID_PLUGINCMD, ID_PLUGINCMDMAX, wxGISMiniMapDlg::OnCommand)
    EVT_MENU_RANGE(ID_MENUCMD, ID_MENUCMDMAX, wxGISMiniMapDlg::OnDropDownCommand)
    EVT_UPDATE_UI_RANGE(ID_PLUGINCMD, ID_PLUGINCMDMAX, wxGISMiniMapDlg::OnCommandUI)
    EVT_AUITOOLBAR_TOOL_DROPDOWN(wxID_ANY, wxGISMiniMapDlg::OnToolDropDown)
    EVT_BUTTON(wxID_OK, wxGISMiniMapDlg::OnOK)
    EVT_UPDATE_UI(wxID_OK, wxGISMiniMapDlg::OnOKUI)
    EVT_LEFT_DOWN(wxGISMiniMapDlg::OnMouseDown)
    EVT_MIDDLE_DOWN(wxGISMiniMapDlg::OnMouseDown)
    EVT_RIGHT_DOWN(wxGISMiniMapDlg::OnMouseDown)
    EVT_LEFT_UP(wxGISMiniMapDlg::OnMouseUp)
    EVT_MIDDLE_UP(wxGISMiniMapDlg::OnMouseUp)
    EVT_RIGHT_UP(wxGISMiniMapDlg::OnMouseUp)
    EVT_LEFT_DCLICK(wxGISMiniMapDlg::OnMouseDoubleClick)
    EVT_MIDDLE_DCLICK(wxGISMiniMapDlg::OnMouseDoubleClick)
    EVT_RIGHT_DCLICK(wxGISMiniMapDlg::OnMouseDoubleClick)
    EVT_MOTION(wxGISMiniMapDlg::OnMouseMove)
END_EVENT_TABLE()

wxGISMiniMapDlg::wxGISMiniMapDlg(wxDword nButtons, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxDialog(parent, id, title, pos, size, style), wxGISApplicationBase()
{
    m_pDropDownCommand = NULL;
    this->SetSizeHints(wxDefaultSize, wxDefaultSize);

    wxBoxSizer* bSizer1;
    bSizer1 = new wxBoxSizer(wxVERTICAL);

    m_toolBar = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_HORZ_LAYOUT | wxAUI_TB_GRIPPER );
    m_toolBar->SetToolBitmapSize(wxSize(16, 16));


//add map tools
    int nCmdCounter(0);

    wxGISCartoMainTool* pwxGISCartoMainTool(NULL);
    for (size_t i = enumGISCartoMainToolZoomIn; i < enumGISCartoMainToolIdentify; ++i)
    {
        pwxGISCartoMainTool = new wxGISCartoMainTool();
        pwxGISCartoMainTool->OnCreate(this);
        pwxGISCartoMainTool->SetSubType(i);
        pwxGISCartoMainTool->SetId(ID_PLUGINCMD + nCmdCounter);
        nCmdCounter++;

        m_CommandArray.push_back(pwxGISCartoMainTool);

        wxGISEnumCommandKind kind = pwxGISCartoMainTool->GetKind();
        if (kind == enumGISCommandDropDown)
            kind = enumGISCommandNormal;
        m_toolBar->AddTool(pwxGISCartoMainTool->GetId(), pwxGISCartoMainTool->GetCaption(), pwxGISCartoMainTool->GetBitmap(), wxNullBitmap, (wxItemKind)kind, pwxGISCartoMainTool->GetTooltip(), pwxGISCartoMainTool->GetMessage(), NULL);
        if (pwxGISCartoMainTool->GetKind() == enumGISCommandDropDown)
            m_toolBar->SetToolDropDown(pwxGISCartoMainTool->GetId(), true);
    }

    wxGISCartoMainCmd* pwxGISCartoMainCmd(NULL);
    for (size_t i = enumGISCartoMainCmdFullExtent; i < enumGISCartoMainCmdMax; ++i)
    {
        pwxGISCartoMainCmd = new wxGISCartoMainCmd();
        pwxGISCartoMainCmd->OnCreate(this);
        pwxGISCartoMainCmd->SetSubType(i);
        pwxGISCartoMainCmd->SetId(ID_PLUGINCMD + nCmdCounter);
        nCmdCounter++;

        m_CommandArray.push_back(pwxGISCartoMainCmd);

        wxGISEnumCommandKind kind = pwxGISCartoMainCmd->GetKind();
        if (kind == enumGISCommandDropDown)
            kind = enumGISCommandNormal;
        m_toolBar->AddTool(pwxGISCartoMainCmd->GetId(), pwxGISCartoMainCmd->GetCaption(), pwxGISCartoMainCmd->GetBitmap(), wxNullBitmap, (wxItemKind)kind, pwxGISCartoMainCmd->GetTooltip(), pwxGISCartoMainCmd->GetMessage(), NULL);
        if (pwxGISCartoMainCmd->GetKind() == enumGISCommandDropDown)
            m_toolBar->SetToolDropDown(pwxGISCartoMainCmd->GetId(), true);
    }
	
    m_toolBar->Realize();

    bSizer1->Add(m_toolBar, 0, wxEXPAND, 5);

    m_pGISMapView = new wxGISDrawingMapView(this);
    RegisterChildWindow(m_pGISMapView->GetId());

    m_pGISMapView->Bind(wxEVT_LEFT_DOWN, &wxGISMiniMapDlg::OnMouseDown, this);
    m_pGISMapView->Bind(wxEVT_MIDDLE_DOWN, &wxGISMiniMapDlg::OnMouseDown, this);
    m_pGISMapView->Bind(wxEVT_RIGHT_DOWN, &wxGISMiniMapDlg::OnMouseDown, this);
    m_pGISMapView->Bind(wxEVT_LEFT_UP, &wxGISMiniMapDlg::OnMouseUp, this);
    m_pGISMapView->Bind(wxEVT_LEFT_UP, &wxGISMiniMapDlg::OnMouseUp, this);
    m_pGISMapView->Bind(wxEVT_MIDDLE_UP, &wxGISMiniMapDlg::OnMouseUp, this);
    m_pGISMapView->Bind(wxEVT_RIGHT_UP, &wxGISMiniMapDlg::OnMouseUp, this);
    m_pGISMapView->Bind(wxEVT_LEFT_UP, &wxGISMiniMapDlg::OnMouseUp, this);
    m_pGISMapView->Bind(wxEVT_MOTION, &wxGISMiniMapDlg::OnMouseMove, this);
    m_pGISMapView->Bind(wxEVT_LEFT_DCLICK, &wxGISMiniMapDlg::OnMouseDoubleClick, this);
    m_pGISMapView->Bind(wxEVT_MIDDLE_DCLICK, &wxGISMiniMapDlg::OnMouseDoubleClick, this);
    m_pGISMapView->Bind(wxEVT_RIGHT_DCLICK, &wxGISMiniMapDlg::OnMouseDoubleClick, this);
	 
    bSizer1->Add(m_pGISMapView, 1, wxEXPAND | wxALL, 0);
	
	wxButton* m_sdbSizer1OK;
    wxButton* m_sdbSizer1Cancel;

    wxStaticLine* staticline = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
    bSizer1->Add(staticline, 0, wxEXPAND | wxALL, 5);

    wxStdDialogButtonSizer* m_sdbSizer = new wxStdDialogButtonSizer();
	if(nButtons & wxOK)
    {
		wxButton* sdbSizer1OK = new wxButton(this, wxID_OK);
		m_sdbSizer->AddButton(sdbSizer1OK);
	}
	
	if(nButtons & wxCANCEL)
	{
		wxButton* sdbSizer1Cancel = new wxButton(this, wxID_CANCEL);
		m_sdbSizer->AddButton(sdbSizer1Cancel);
	}
	
    m_sdbSizer->Realize();
    bSizer1->Add(m_sdbSizer, 0, wxEXPAND | wxALL, 5);

    this->SetSizer(bSizer1);
    this->Layout();

    this->Centre(wxBOTH);

    SerializeFramePos(false);

}

wxGISMiniMapDlg::~wxGISMiniMapDlg()
{
    SerializeFramePos(true);
}


void wxGISMiniMapDlg::SerializeFramePos(bool bSave)
{
    wxGISAppConfig oConfig = GetConfig();
    if (!oConfig.IsOk())
        return;
		
	wxString sAppName = GetApplication()->GetAppName();	

    if (bSave)
    {
        if (IsMaximized())
            oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/minimapdlg/frame/maxi")), true);
        else
        {
            int x, y, w, h;
            GetClientSize(&w, &h);
            GetPosition(&x, &y);
            oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/minimapdlg/frame/maxi")), false);
            oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/minimapdlg/frame/width")), w);
            oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/minimapdlg/frame/height")), h);
            oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/minimapdlg/frame/xpos")), x);
            oConfig.Write(enumGISHKCU, sAppName + wxString(wxT("/minimapdlg/frame/ypos")), y);
        }
    }
    else
    {
        //load
        bool bMaxi = oConfig.ReadBool(enumGISHKCU, sAppName + wxString(wxT("/minimapdlg/frame/maxi")), false);
        if (!bMaxi)
        {
            int x = oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/minimapdlg/frame/xpos")), 50);
            int y = oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/minimapdlg/frame/ypos")), 50);
            int w = oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/minimapdlg/frame/width")), 400);
            int h = oConfig.ReadInt(enumGISHKCU, sAppName + wxString(wxT("/minimapdlg/frame/height")), 300);
            Move(x, y);
            SetClientSize(w, h);
        }
        else
        {
            Maximize();
        }
    }
}

void wxGISMiniMapDlg::OnCommand(wxCommandEvent& event)
{
    event.Skip();
    Command(GetCommand(event.GetId()));
}

void wxGISMiniMapDlg::OnCommandUI(wxUpdateUIEvent& event)
{
    wxGISCommand* pCmd = GetCommand(event.GetId());
    if (pCmd)
    {
        if (pCmd->GetKind() == enumGISCommandCheck)
            event.Check(pCmd->GetChecked());
        event.Enable(pCmd->GetEnabled());
        m_toolBar->SetToolShortHelp(event.GetId(), pCmd->GetTooltip());
    }
}

void wxGISMiniMapDlg::OnDropDownCommand(wxCommandEvent& event)
{
    event.Skip();
    if (m_pDropDownCommand)
        m_pDropDownCommand->OnDropDownCommand(event.GetId());
}

void wxGISMiniMapDlg::OnToolDropDown(wxAuiToolBarEvent& event)
{
    if (event.IsDropDownClicked())
    {
        wxGISCommand* pCmd = GetCommand(event.GetToolId());
        m_pDropDownCommand = dynamic_cast<IDropDownCommand*>(pCmd);
        if (m_pDropDownCommand)
        {
            wxMenu* pMenu = m_pDropDownCommand->GetDropDownMenu();
            if (pMenu)
            {
                PushEventHandler(pMenu);
                m_toolBar->PopupMenu(pMenu, event.GetItemRect().GetBottomLeft());
                PopEventHandler();
                delete pMenu;
                return;
            }
        }
    }
    event.Skip();
}

void wxGISMiniMapDlg::OnOK(wxCommandEvent& event)
{
    EndModal(wxID_OK);
}

void wxGISMiniMapDlg::OnOKUI(wxUpdateUIEvent& event)
{
	
}

void wxGISMiniMapDlg::OnMouseMove(wxMouseEvent& event)
{
    wxGISApplicationBase::OnMouseMove(event);
}

void wxGISMiniMapDlg::OnMouseDown(wxMouseEvent& event)
{
    wxGISApplicationBase::OnMouseDown(event);
}

void wxGISMiniMapDlg::OnMouseUp(wxMouseEvent& event)
{
    wxGISApplicationBase::OnMouseUp(event);
}

void wxGISMiniMapDlg::OnMouseDoubleClick(wxMouseEvent& event)
{
    wxGISApplicationBase::OnMouseDoubleClick(event);
}

void wxGISMiniMapDlg::AddLayer(wxGISLayer* pLayer)
{
	if(NULL != m_pGISMapView)
	{
		m_pGISMapView->AddLayer(pLayer);
	}
}

void wxGISMiniMapDlg::SetFullExtent()
{
	if(NULL != m_pGISMapView)
		m_pGISMapView->SetFullExtent();
}

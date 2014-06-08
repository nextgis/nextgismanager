/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  tool dialog class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Dmitry Barishnikov
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
/*
#include "wxgis/geoprocessingui/gptooldlg.h"
#include "wxgis/core/globalfn.h"

#include "../../art/tool_16.xpm"

#include "wx/icon.h"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGISGPToolDlg, wxFrame)
	EVT_BUTTON(wxID_HELP, wxGISGPToolDlg::OnHelp)
	EVT_UPDATE_UI(wxID_HELP, wxGISGPToolDlg::OnHelpUI)
	EVT_BUTTON(wxID_CANCEL, wxGISGPToolDlg::OnCancel)
	EVT_BUTTON(wxID_OK, wxGISGPToolDlg::OnOk)
	EVT_UPDATE_UI(wxID_OK, wxGISGPToolDlg::OnOkUI)
END_EVENT_TABLE()

wxGISGPToolDlg::wxGISGPToolDlg(wxGxRootToolbox* pGxRootToolbox, IGPToolSPtr pTool, bool bSync, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxFrame( parent, id, title, pos, size, style )
{
    IFrameApplication* pApp = dynamic_cast<IFrameApplication*>(GetApplication());
    pApp->RegisterChildWindow(this);

    m_pTool = pTool;
    m_pGxRootToolbox = pGxRootToolbox;
    m_bSync = bSync;


#ifdef __WXMSW__
	SetIcon( wxIcon(tool_16_xpm) );
	this->SetSizeHints( wxSize( 350,500 ) );
//	SetLabel(m_pTool->GetDisplayName());
#endif

	//this->SetSizeHints( wxDefaultSize, wxDefaultSize );
    m_DataWidth = 350;
    m_HtmlWidth = 150;

	wxBoxSizer* bMainSizer = new wxBoxSizer( wxVERTICAL );

	m_splitter = new wxSplitterWindow( this, SASHCTRLID, wxDefaultPosition, wxDefaultSize, 0 );
	m_splitter->Bind Connect( wxEVT_IDLE, wxIdleEventHandler( wxGISGPToolDlg::m_splitterOnIdle ), NULL, this );

	m_toolpanel = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxSize(m_DataWidth, size.y), wxTAB_TRAVERSAL );//wxDefaultSize
	m_bSizer2 = new wxBoxSizer( wxVERTICAL );
	m_pInfoBar = new wxInfoBar(m_toolpanel);
    m_pInfoBar->SetOwnBackgroundColour(0xc8ffff);
    m_pInfoBar->SetFont(GetFont().Bold());//.Larger()
    //m_pInfoBar->SetShowHideEffects(wxSHOW_EFFECT_EXPAND, wxSHOW_EFFECT_EXPAND);
    //m_pInfoBar->SetEffectDuration(1500);
	m_bSizer2->Add(m_pInfoBar, wxSizerFlags().Expand());//, 0, wxEXPAND, 5 );//

    wxScrolledWindow* pToolsWnd = new wxScrolledWindow(m_toolpanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxBORDER_SUNKEN|wxVSCROLL );
    pToolsWnd->SetScrollbars(1, 1, size.x, size.y);

	wxBoxSizer* bSizer4 = new wxBoxSizer( wxVERTICAL );

    GPParameters Params = m_pTool->GetParameterInfo();
	for(size_t i = 0; i < Params.GetCount(); ++i)
    {
		IGPParameter* pParam = Params[i];
        if(!pParam)
        {
            m_pControlsArray.push_back(NULL);
            continue;
        }
		//TODO: check if group created
        switch(pParam->GetDataType())
        {
        case enumGISGPParamDTPath:
            {
                wxGISDTPath* pPath = new wxGISDTPath(pParam, m_pTool->GetCatalog(), pToolsWnd);
                bSizer4->Add( pPath, 0, wxEXPAND, 5 );
                m_pControlsArray.push_back(pPath);
            }
            break;
        case enumGISGPParamDTFolderPath:
            {
                wxGISDTFolderPath* pPath = new wxGISDTFolderPath(pParam, m_pTool->GetCatalog(), pToolsWnd);
                bSizer4->Add( pPath, 0, wxEXPAND, 5 );
                m_pControlsArray.push_back(pPath);
            }
            break;
        case enumGISGPParamDTSpatRef:
            {
                wxGISDTSpatRef* pSpatRef = new wxGISDTSpatRef(pParam, m_pTool->GetCatalog(), pToolsWnd);
                bSizer4->Add( pSpatRef, 0, wxEXPAND, 5 );
                m_pControlsArray.push_back(pSpatRef);
            }
            break;
        case enumGISGPParamDTString:
        case enumGISGPParamDTInteger:
        case enumGISGPParamDTDouble:
            {
                wxGISDTDigit* poDigit = new wxGISDTDigit(pParam, m_pTool->GetCatalog(), pToolsWnd);
                bSizer4->Add( poDigit, 0, wxEXPAND, 5 );
                m_pControlsArray.push_back(poDigit);
            }
            break;
        case enumGISGPParamDTStringChoice:
        case enumGISGPParamDTIntegerChoice:
        case enumGISGPParamDTDoubleChoice:
            {
                wxGISDTChoice* poChoice = new wxGISDTChoice(pParam, m_pTool->GetCatalog(), pToolsWnd);
                bSizer4->Add( poChoice, 0, wxEXPAND, 5 );
                m_pControlsArray.push_back(poChoice);
            }
            break;
        case enumGISGPParamDTStringChoiceEditable:
            {
                wxGISDTChoiceEditable* poChoice = new wxGISDTChoiceEditable(pParam, m_pTool->GetCatalog(), pToolsWnd);
                bSizer4->Add( poChoice, 0, wxEXPAND, 5 );
                m_pControlsArray.push_back(poChoice);
            }
            break;
	   case enumGISGPParamDTStringList:
	   case enumGISGPParamDTIntegerList:
	   case enumGISGPParamDTDoubleList:
            {
                wxGISDTList* poList = new wxGISDTList(pParam, m_pTool->GetCatalog(), pToolsWnd);
                bSizer4->Add( poList, 0, wxEXPAND, 5 );
                m_pControlsArray.push_back(poList);
            }
            break;
       case enumGISGPParamDTBool:
            {
                wxGISDTBool* poCheck = new wxGISDTBool(pParam, m_pTool->GetCatalog(), pToolsWnd);
                bSizer4->Add( poCheck, 0, wxEXPAND, 5 );
                m_pControlsArray.push_back(poCheck);
            }
            break;
        case enumGISGPParamDTParamArray:
            {
                wxGISDTMultiParam* poMultiParam = new wxGISDTMultiParam(pParam, m_pTool->GetCatalog(), pToolsWnd);
                bSizer4->Add( poMultiParam, 0, wxEXPAND, 5 );
                m_pControlsArray.push_back(poMultiParam);
            }
            break;
        case enumGISGPParamDTQuery:
            {
				wxGISSQLQueryCtrl* poSQLQueryParam = new wxGISSQLQueryCtrl(pParam, m_pTool->GetCatalog(), pToolsWnd);
                bSizer4->Add( poSQLQueryParam, 0, wxEXPAND, 5 );
                m_pControlsArray.push_back(poSQLQueryParam);
            }
            break;
        default:
            m_pControlsArray.push_back(NULL);
            break;
        }
    }

    pToolsWnd->SetSizer( bSizer4 );
	pToolsWnd->Layout();
	bSizer4->Fit( pToolsWnd );

	m_bSizer2->Add( pToolsWnd, 1, wxEXPAND, 5 );

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( m_toolpanel, wxID_OK, wxString(_("OK")) );
    m_sdbSizer1OK->Enable(false);
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( m_toolpanel, wxID_CANCEL, wxString(_("Cancel")) );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1Help = new wxButton( m_toolpanel, wxID_HELP, wxString(_("Show Help")) );
    m_sdbSizer1Help->Enable(false);
	m_sdbSizer1->AddButton( m_sdbSizer1Help );
	m_sdbSizer1->Realize();
	m_bSizer2->Add( m_sdbSizer1, 0, wxEXPAND|wxALL, 5 );

	m_toolpanel->SetSizer( m_bSizer2 );
	m_toolpanel->Layout();
	m_bSizer2->Fit( m_toolpanel );

	m_htmlWin = new wxHtmlWindow( m_splitter, wxID_ANY, wxDefaultPosition, wxSize(m_HtmlWidth, size.y), wxHW_SCROLLBAR_AUTO | wxBORDER_THEME  );
	m_splitter->SetSashGravity(1.0);
	//m_splitter1->SplitVertically(m_commandbarlist, m_buttonslist, 100);


	m_splitter->SplitVertically( m_toolpanel, m_htmlWin, m_DataWidth );//m_helppanel

	bMainSizer->Add( m_splitter, 1, wxEXPAND, 5 );

	this->SetSizer( bMainSizer );
	this->Layout();


    m_splitter->Unsplit(m_htmlWin);
    wxSize DlgSize = size;
    DlgSize.x = m_DataWidth;
    SetSize(DlgSize);

    SerializeFramePos(false);
}

wxGISGPToolDlg::~wxGISGPToolDlg()
{
    SerializeFramePos(true);
}

void wxGISGPToolDlg::OnHelp(wxCommandEvent& event)
{
    wxSize DlgSize = GetSize();
    wxSize DataSize = m_toolpanel->GetSize();
    if(m_splitter->IsSplit())
    {
        wxSize HtmlSize = m_htmlWin->GetSize();
        m_HtmlWidth = HtmlSize.x;
        DlgSize.x = DataSize.x;
        SetSize(DlgSize);
        m_splitter->Unsplit(m_htmlWin);
    }
    else
    {
        DlgSize.x += m_HtmlWidth;
        SetSize(DlgSize);
        m_splitter->SetSashGravity(1.0);
        m_splitter->SplitVertically( m_toolpanel, m_htmlWin, DataSize.x);
    }
}

void wxGISGPToolDlg::OnHelpUI(wxUpdateUIEvent& event)
{
    event.SetText(m_splitter->IsSplit() == true ? _("Hide Help") : _("Show Help"));
}

void wxGISGPToolDlg::OnOk(wxCommandEvent& event)
{
    IFrameApplication* pApp = dynamic_cast<IFrameApplication*>(GetApplication());
    pApp->UnRegisterChildWindow(this);

    wxWindow* pParentWnd = this->GetParent();
    pParentWnd->SetFocus();
    this->Destroy();

    //begin execution
    m_pGxRootToolbox->ExecuteTool(pParentWnd, m_pTool, m_bSync);
}

void wxGISGPToolDlg::OnCancel(wxCommandEvent& event)
{
    IFrameApplication* pApp = dynamic_cast<IFrameApplication*>(GetApplication());
    pApp->UnRegisterChildWindow(this);

    wxWindow* pParentWnd = this->GetParent();
    pParentWnd->SetFocus();
    this->Destroy();
}

void wxGISGPToolDlg::OnOkUI(wxUpdateUIEvent& event)
{
	//event.Skip();

	//return;
 //   //TODO: Fix tools state
    event.Enable(false);

	////internal control validate
 //   for(size_t i = 0; i < m_pControlsArray.size(); ++i)
 //   {
 //       if(m_pControlsArray[i])
 //           m_pControlsArray[i]->Validate();
 //   }

	GPParameters Params = m_pTool->GetParameterInfo();
	int nErrCount(0);
	for(size_t i = 0; i < Params.Count(); ++i)
	{
		wxGISEnumGPMessageType eType = Params[i]->GetMessageType();
		int nIco;
		switch(eType)
		{
		case wxGISEnumGPMessageRequired:
		case wxGISEnumGPMessageOk:
		case wxGISEnumGPMessageInformation:
		case wxGISEnumGPMessageUnknown:
		case wxGISEnumGPMessageNone:
		default:
			continue;
		case wxGISEnumGPMessageError:
			nIco = wxICON_ERROR;
		    nErrCount++;
			break;
		case wxGISEnumGPMessageWarning:
			nIco = wxICON_WARNING;
		    nErrCount++;
			break;
		};

        if(nErrCount > 0)
        {
		    wxString sMsg = Params[i]->GetMessage();
		    if(sMsg == m_sCurrentErrMsg && m_nCurrentErrField == i)
			    break;//continue;

		    m_pInfoBar->ShowMessage(sMsg + wxString(wxT(" ")) + wxString::Format(_("(Field No %ld)"), i + 1), nIco);
		    m_sCurrentErrMsg = sMsg;
		    m_nCurrentErrField = i;
		    break;
        }
	}
	//update elements values set while validate user tool
    for(size_t i = 0; i < m_pControlsArray.size(); ++i)
        if(m_pControlsArray[i])
            m_pControlsArray[i]->UpdateControls();

    //tool validate
    bool bIsValid = m_pTool->Validate();//validate user tool
	if(!bIsValid)
		return;

	//if any param not valid return
    for(size_t i = 0; i < m_pControlsArray.size(); ++i)
        if(m_pControlsArray[i])
            if(!m_pControlsArray[i]->Validate())
				return;

	//set OK button enabled
    event.Enable(true);

	if(nErrCount == 0)
	{
        if(m_pInfoBar->IsShown())
        {
    		m_pInfoBar->Hide();
	    	m_bSizer2->Layout();
        }
	}

    //short nNonValid(0);
    //GPParameters* pParams = m_pTool->GetParameterInfo();
    //if(!pParams)
    //    return;
    ////update controls state
    //for(size_t i = 0; i < pParams->size(); ++i)
    //{
    //    IGPParameter* pParam = pParams->operator[](i);
    //    if(!pParam)
    //        continue;
    //    if(!pParam->GetIsValid())
    //        nNonValid++;
    //    if(pParam->GetHasBeenValidated())
    //        continue;
    //    if(i < m_pControlsArray.size() && m_pControlsArray[i])
    //    {
    //        if(bIsValid)
    //            m_pControlsArray[i]->Validate();
    //        m_pControlsArray[i]->Update();
    //    }
    //    pParam->SetHasBeenValidated(true);
    //}
    //if(nNonValid > 0)
    //    return;
    //event.Enable(true);
}

void wxGISGPToolDlg::SerializeFramePos(bool bSave)
{
	wxGISAppConfig oConfig = GetConfig();
	if(!oConfig.IsOk())
        return;

	if(bSave)
	{
		if( IsMaximized() )
		{
			oConfig.Write(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tooldlg/maxi")), true);
		}
		else
		{
			int x, y, w, h;
			GetClientSize(&w, &h);
			GetPosition(&x, &y);

			oConfig.Write(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tooldlg/maxi")), false);
			oConfig.Write(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tooldlg/width")), w);
			oConfig.Write(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tooldlg/height")), h);
			oConfig.Write(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tooldlg/xpos")), x);
			oConfig.Write(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tooldlg/ypos")), y);
		}
	}
	else
	{
		//load
		bool bMaxi = oConfig.ReadBool(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tooldlg/maxi")), false);
		if(!bMaxi)
		{
			int x = oConfig.ReadInt(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tooldlg/xpos")), 50);
			int y = oConfig.ReadInt(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tooldlg/ypos")), 50);
			int w = oConfig.ReadInt(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tooldlg/width")), 450);
			int h = oConfig.ReadInt(enumGISHKCU, TOOLBX_NAME + wxString(wxT("/tooldlg/height")), 650);

			Move(x, y);
			SetClientSize(w, h);
		}
		else
		{
			Maximize();
		}
	}
}
*/

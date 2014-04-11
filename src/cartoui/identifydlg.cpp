/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISIdentifyDlg class - dialog/dock window with the results of identify.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011-2013 Bishop
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
#include "wxgis/cartoui/identifydlg.h"
#include "wxgis/core/config.h"

#include <wx/clipbrd.h>
#include <wx/fontmap.h>

#include <cmath>

#include "../../art/splitter_switch.xpm"
#include "../../art/layers_16.xpm"
#include "../../art/layer_16.xpm"
#include "../../art/id.xpm"
#include "../../art/small_arrow.xpm"
#include "../../art/field_16.xpm"

int wxCALLBACK FieldValueCompareFunction(wxIntPtr item1, wxIntPtr item2, wxIntPtr sortData)
{
    LPFIELDSORTDATA psortdata = (LPFIELDSORTDATA)sortData;
    if(psortdata->nSortAsc == 0)
    {
        return item1 - item2;
    }
    else
    {
        wxString str1, str2;
        if(psortdata->currentSortCol == 0)
        {
            str1 = psortdata->Feature.GetFieldName(item1);
            str2 = psortdata->Feature.GetFieldName(item2);
        }
        else
        {
            str1 = psortdata->Feature.GetFieldAsString(item1);
            str2 = psortdata->Feature.GetFieldAsString(item2);
        }
        return str1.CmpNoCase(str2) * psortdata->nSortAsc;
    }
}
//-------------------------------------------------------------------
// wxGISFeatureDetailsPanel
//-------------------------------------------------------------------

BEGIN_EVENT_TABLE(wxGISFeatureDetailsPanel, wxPanel)
    EVT_CONTEXT_MENU(wxGISFeatureDetailsPanel::OnContextMenu)
    EVT_MENU_RANGE(ID_WG_COPY_NAME, ID_WG_ENCODING_START + wxFONTENCODING_MAX, wxGISFeatureDetailsPanel::OnMenu)
    EVT_UPDATE_UI_RANGE(ID_WG_COPY_NAME, ID_WG_ENCODING_START + wxFONTENCODING_MAX, wxGISFeatureDetailsPanel::OnMenuUpdateUI)
    EVT_LIST_COL_CLICK(ID_LISTCTRL, wxGISFeatureDetailsPanel::OnColClick)
	EVT_BUTTON(ID_MASKBTN, wxGISFeatureDetailsPanel::OnMaskMenu)
END_EVENT_TABLE()

wxGISFeatureDetailsPanel::wxGISFeatureDetailsPanel( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxImage img = wxBitmap(small_arrow_xpm).ConvertToImage();
	wxBitmap oDownArrow = img.Rotate90();
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 1, 3, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Location:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 1, wxALIGN_CENTER_VERTICAL|wxLEFT, 5 );

	m_textCtrl = new wxTextCtrl(  this, wxID_ANY, wxT("..."), wxDefaultPosition, wxDefaultSize, wxTE_READONLY, wxGenericValidator( &m_sLocation ));
	//m_textCtrl->Enable( false );

	fgSizer1->Add( m_textCtrl, 1, wxALIGN_CENTER|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL|wxEXPAND|wxRIGHT|wxLEFT, 5 );

	m_bpSelStyleButton = new wxBitmapButton( this, ID_MASKBTN, oDownArrow, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_bpSelStyleButton->SetToolTip( _("Select location text style") );

	fgSizer1->Add( m_bpSelStyleButton, 0, wxRIGHT, 5 );

	bSizer1->Add( fgSizer1, 0, wxEXPAND, 5 );

	m_listCtrl = new wxListCtrl( this, ID_LISTCTRL, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_HRULES | wxLC_VRULES );
	m_listCtrl->InsertColumn(0, _("Field"), wxLIST_FORMAT_LEFT, 90);
	m_listCtrl->InsertColumn(1, _("Value"), wxLIST_FORMAT_LEFT, 120);

	m_ImageListSmall.Create(16, 16);

    wxBitmap SmallA(small_arrow_xpm);// > arrow
    wxImage SmallImg = SmallA.ConvertToImage();
    SmallImg = SmallImg.Rotate90();
    wxBitmap SmallDown(SmallImg);
    SmallImg = SmallImg.Mirror(false);
    wxBitmap SmallUp(SmallImg);

	m_ImageListSmall.Add(wxBitmap(SmallDown));
	m_ImageListSmall.Add(wxBitmap(SmallUp));
    m_ImageListSmall.Add(wxBitmap(field_16_xpm));

	m_listCtrl->SetImageList(&m_ImageListSmall, wxIMAGE_LIST_SMALL);

    m_listCtrl->Bind(wxEVT_LEFT_DOWN, &wxGISFeatureDetailsPanel::OnMouseLeftUp, this);

    //not work on GTK
    //m_listCtrl->Bind(wxEVT_SET_CURSOR, &wxGISFeatureDetailsPanel::OnSetCursor, this);

    m_listCtrl->Bind(wxEVT_MOTION, &wxGISFeatureDetailsPanel::OnMouseMove, this);

	bSizer1->Add( m_listCtrl, 1, wxALL|wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
	m_pCFormat = new wxGISCoordinatesFormatMenu();
    wxString sMask( wxT("X: dd.dddd[ ]Y: dd.dddd") );
	wxGISAppConfig oConfig = GetConfig();
    m_sAppName = GetApplication()->GetAppName();
	if(oConfig.IsOk())
    {
        sMask = oConfig.Read(enumGISHKCU, m_sAppName + wxString(wxT("/identifydlg/format_mask")), sMask);
    }

	m_pCFormat->Create(sMask);

	m_pMenu = new wxMenu;
	m_pMenu->Append(ID_WG_COPY_NAME, wxString::Format(_("Copy %s"), _("Field")), wxString::Format(_("Copy '%s' value"), _("Field")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WG_COPY_VALUE, wxString::Format(_("Copy %s"), _("Value")), wxString::Format(_("Copy '%s' value"), _("Value")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WG_COPY, _("Copy"), _("Copy rows"), wxITEM_NORMAL);
	m_pMenu->Append(ID_WG_HIDE, _("Hide"), _("Hide rows"), wxITEM_NORMAL);
	m_pMenu->AppendSeparator();
	m_pMenu->Append(ID_WG_RESET_SORT, wxString(_("Remove sort")), wxString(_("Remove sort")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WG_RESET_HIDE, wxString(_("Show all rows")), wxString(_("Show all rows")), wxITEM_NORMAL);
	m_pMenu->AppendSeparator();
    //encoding
    wxMenu *pEncMenu = new wxMenu;
    //add encodings
    wxString sDefault;
    for (int i = wxFONTENCODING_DEFAULT; i < wxFONTENCODING_MAX; i++)
    {
        wxString sDesc = wxFontMapper::GetEncodingDescription((wxFontEncoding)i);
        if (!sDesc.StartsWith(_("Unknown")))
        {
            wxMenuItem* pMItem = pEncMenu->AppendRadioItem(ID_WG_ENCODING_START + i, sDesc);
            if (i == wxFONTENCODING_DEFAULT)
                pMItem->Check();
        }
    }

    m_pMenu->AppendSubMenu(pEncMenu, _("Encodings"), _("Change text encoding"));

    m_currentSortCol = 0;
    m_nSortAsc = 0;
	m_dfX = 0;
	m_dfY =	0;
}

wxGISFeatureDetailsPanel::~wxGISFeatureDetailsPanel()
{
	wxDELETE(m_pMenu);
	wxDELETE(m_pCFormat);
}

void wxGISFeatureDetailsPanel::FillPanel(const OGRPoint *pPt)
{
    if(!pPt)
        return;
    //TODO: OGRPoint has spatial reference. This should be used in m->deg and vice versa transform
	m_dfX = pPt->getX();
	m_dfY = pPt->getY();
	m_sLocation = m_pCFormat->Format(m_dfX, m_dfY);
	TransferDataToWindow();
}

void wxGISFeatureDetailsPanel::FillPanel(wxGISFeature &Feature)
{
	m_Feature = Feature;
	Clear();
	for(int i = 0; i < Feature.GetFieldCount(); ++i)
	{
		if(m_anExcludeFields.Index(i) != wxNOT_FOUND)
			continue;

        wxString sName = Feature.GetFieldName(i);

		long pos = m_listCtrl->InsertItem(i, sName, 2);

        wxListItem item_val;
        item_val.SetColumn(1);
        wxString sValue = Feature.GetFieldAsString(i);
        item_val.SetText(sValue);
        item_val.SetId(pos);
		m_listCtrl->SetItem(item_val);

        if(IsURL(sValue))
        {
            wxFont Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
            Font.SetUnderlined(true);
            m_listCtrl->SetItemFont(pos, Font);
            m_listCtrl->SetItemTextColour(pos, wxSystemSettings::GetColour(wxSYS_COLOUR_HOTLIGHT));
        }
        else if(IsLocalURL(sValue))
        {
            wxFont Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
            Font.SetUnderlined(true);
            m_listCtrl->SetItemFont(pos, Font);
            m_listCtrl->SetItemTextColour(pos, wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
        }
		m_listCtrl->SetItemData(pos, (long)i);
	}
	//m_listCtrl->Update();
    FIELDSORTDATA sortdata = {m_nSortAsc, m_currentSortCol, m_Feature};
	m_listCtrl->SortItems(FieldValueCompareFunction, (long)&sortdata);
}

void wxGISFeatureDetailsPanel::Clear(bool bFull)
{
	m_listCtrl->DeleteAllItems();
	if(bFull)
		m_anExcludeFields.Clear();
}

void wxGISFeatureDetailsPanel::OnContextMenu(wxContextMenuEvent& event)
{
	wxRect rc = m_listCtrl->GetRect();
	wxPoint point = event.GetPosition();
    // If from keyboard
    if (point.x == -1 && point.y == -1)
	{
        wxSize size = GetSize();
		point.x = rc.GetLeft() + rc.GetWidth() / 2;
		point.y = rc.GetTop() + rc.GetHeight() / 2;
    }
	else
	{
        point = m_listCtrl->ScreenToClient(point);
    }
	if(!rc.Contains(point))
	{
		event.Skip();
		return;
	}
    PopupMenu(m_pMenu, point.x, point.y);
}

void wxGISFeatureDetailsPanel::OnMaskMenu(wxCommandEvent& event)
{
	wxRect rc = m_bpSelStyleButton->GetRect();
	m_pCFormat->PrepareMenu();//fill new masks from config
	PopupMenu(m_pCFormat, rc.GetBottomLeft());
	m_sLocation = m_pCFormat->Format(m_dfX, m_dfY);
	TransferDataToWindow();

    wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk() && m_pCFormat)
    {
        oConfig.Write(enumGISHKCU, m_sAppName + wxString(wxT("/identifydlg/format_mask")), m_pCFormat->GetMask());
    }
}

void wxGISFeatureDetailsPanel::WriteStringToClipboard(const wxString &sData)
{
	// Write some text to the clipboard
    if (wxTheClipboard->Open())
    {
        // This data objects are held by the clipboard,
        // so do not delete them in the app.
        wxTheClipboard->SetData( new wxTextDataObject(sData) );
        wxTheClipboard->Close();
    }
}

void wxGISFeatureDetailsPanel::OnMenu(wxCommandEvent& event)
{
    if (event.GetId() >= ID_WG_ENCODING_START)
    {
        wxFontEncoding eEnc = (wxFontEncoding)(event.GetId() - ID_WG_ENCODING_START);
        wxGISIdentifyDlg* pParentDlg = wxDynamicCast(GetParent()->GetParent(), wxGISIdentifyDlg);
        if (pParentDlg != NULL)
        {
            pParentDlg->SetEncoding(eEnc);
        }
        return;
    }

    long nItem = wxNOT_FOUND;
	wxString sOutput;
	switch(event.GetId())
	{
	case ID_WG_COPY_NAME:
		for ( ;; )
		{
			nItem = m_listCtrl->GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if ( nItem == wxNOT_FOUND )
				break;
			sOutput += m_listCtrl->GetItemText(nItem, 0);
			sOutput += wxT("\n");
		}
		WriteStringToClipboard(sOutput);
		break;
	case ID_WG_COPY_VALUE:
		for ( ;; )
		{
			nItem = m_listCtrl->GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if ( nItem == wxNOT_FOUND )
				break;
			sOutput += m_listCtrl->GetItemText(nItem, 1);
			sOutput += wxT("\n");
		}
		WriteStringToClipboard(sOutput);
		break;
	case ID_WG_COPY:
		for ( ;; )
		{
			nItem = m_listCtrl->GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if ( nItem == wxNOT_FOUND )
				break;
			sOutput += m_listCtrl->GetItemText(nItem, 0);
			sOutput += wxT("\t");
			sOutput += m_listCtrl->GetItemText(nItem, 1);
			sOutput += wxT("\n");
		}
		WriteStringToClipboard(sOutput);
		break;
	case ID_WG_HIDE:
		for ( ;; )
		{
			nItem = m_listCtrl->GetNextItem(nItem, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
			if ( nItem == wxNOT_FOUND )
				break;
	        int nFieldNo = (int)m_listCtrl->GetItemData(nItem);
			m_anExcludeFields.Add(nFieldNo);
		}
		FillPanel(m_Feature);
		break;
	case ID_WG_RESET_SORT:
        {
            m_nSortAsc = 0;

            wxListItem item;
            item.SetMask(wxLIST_MASK_IMAGE);

            //reset image
            item.SetImage(wxNOT_FOUND);
            for(int i = 0; i < m_listCtrl->GetColumnCount(); ++i)
                m_listCtrl->SetColumn(i, item);

            FIELDSORTDATA sortdata = {m_nSortAsc, m_currentSortCol, m_Feature};
	        m_listCtrl->SortItems(FieldValueCompareFunction, (long)&sortdata);
        }
		break;
	case ID_WG_RESET_HIDE:
		m_anExcludeFields.Clear();
		FillPanel(m_Feature);
		break;
	default:
		break;
	}
}

void wxGISFeatureDetailsPanel::OnMenuUpdateUI(wxUpdateUIEvent& event)
{
    if (event.GetId() == ID_WG_RESET_SORT || event.GetId() >= ID_WG_ENCODING_START)
		return;
	if(m_listCtrl->GetSelectedItemCount() == 0)
		event.Enable(false);
	else
		event.Enable(true);
}

void wxGISFeatureDetailsPanel::OnColClick(wxListEvent& event)
{
    //event.Skip();
    m_currentSortCol = event.GetColumn();
    if(m_nSortAsc == 0)
        m_nSortAsc = 1;
    else
        m_nSortAsc *= -1;

    FIELDSORTDATA sortdata = {m_nSortAsc, m_currentSortCol, m_Feature};
	m_listCtrl->SortItems(FieldValueCompareFunction, (long)&sortdata);

    wxListItem item;
    item.SetMask(wxLIST_MASK_IMAGE);

    ////reset image
    //item.SetImage(wxNOT_FOUND);
    //for(size_t i = 0; i < m_listCtrl->GetColumnCount(); ++i)
    //    m_listCtrl->SetColumn(i, item);

    item.SetImage(m_nSortAsc == 1 ? 0 : 1);
    m_listCtrl->SetColumn(m_currentSortCol, item);
}

void wxGISFeatureDetailsPanel::OnMouseMove(wxMouseEvent& event)
{
    event.Skip();
    wxPoint pt(event.GetX(), event.GetY());
    //pt = ClientToScreen(pt);
    //pt = m_listCtrl->ScreenToClient(pt);
    int flags;
    long item = m_listCtrl->HitTest(pt, flags);
    if ((item > -1) && (flags & wxLIST_HITTEST_ONITEM))
    {
        wxListItem row_info;
        row_info.m_itemId = item;
        row_info.m_col = 1;
        row_info.m_mask = wxLIST_MASK_TEXT;
        m_listCtrl->GetItem( row_info );
        if(IsURL(row_info.m_text) || IsLocalURL(row_info.m_text))
        {
            m_listCtrl->SetCursor(wxCursor(wxCURSOR_HAND));
            return;
        }
    }
    m_listCtrl->SetCursor(wxCursor(wxCURSOR_ARROW));
}

//not work on GTK
//void wxGISFeatureDetailsPanel::OnSetCursor(wxSetCursorEvent& event)
//{
//    //event.Skip();
//    wxPoint pt(event.GetX(), event.GetY());
//    //pt = ClientToScreen(pt);
//    //pt = m_listCtrl->ScreenToClient(pt);
//    int flags;
//    long item = m_listCtrl->HitTest(pt, flags);
//    if ((item > -1) && (flags & wxLIST_HITTEST_ONITEM))
//    {
//        wxListItem row_info;
//        row_info.m_itemId = item;
//        row_info.m_col = 1;
//        row_info.m_mask = wxLIST_MASK_TEXT;
//        m_listCtrl->GetItem( row_info );
//    wxLogDebug(wxT("OnSetCursor %s flags %d item %d"), row_info.m_text, flags, item);
//        if(IsURL(row_info.m_text) || IsLocalURL(row_info.m_text))
//        {
//            event.SetCursor(wxCursor(wxCURSOR_HAND));
//            m_listCtrl->SetFocus();
//        }
//    }
//}

void wxGISFeatureDetailsPanel::OnMouseLeftUp(wxMouseEvent& event)
{
    event.Skip();
    wxPoint pt(event.GetX(), event.GetY());
    //pt = ClientToScreen(pt);
    //pt = m_listCtrl->ScreenToClient(pt);
    int flags;
    long item = m_listCtrl->HitTest(pt, flags);
    if (item > -1 && (flags & wxLIST_HITTEST_ONITEM))
    {
        wxListItem row_info;
        row_info.m_itemId = item;
        row_info.m_col = 1;
        row_info.m_mask = wxLIST_MASK_TEXT;
        m_listCtrl->GetItem( row_info );
        if(IsURL(row_info.m_text))
        {
            wxLaunchDefaultBrowser( row_info.m_text );
        }
        else if(IsLocalURL(row_info.m_text))
        {
            wxLaunchDefaultApplication( row_info.m_text );
        }
    }
}

bool wxGISFeatureDetailsPanel::IsURL(const wxString &sText)
{
    return sText.StartsWith(wxT("http:")) || sText.StartsWith(wxT("www.")) || sText.StartsWith(wxT("https:")) || sText.StartsWith(wxT("ftp:")) || sText.StartsWith(wxT("ftp.")) || sText.StartsWith(wxT("www2."));

}

bool wxGISFeatureDetailsPanel::IsLocalURL(const wxString &sText)
{
    return sText.StartsWith(wxT("file:"));
}

//-------------------------------------------------------------------
// wxGISIdentifyDlg
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxGISIdentifyDlg, wxPanel)

BEGIN_EVENT_TABLE(wxGISIdentifyDlg, wxPanel)
	EVT_BUTTON(wxGISIdentifyDlg::ID_SWITCHSPLIT, wxGISIdentifyDlg::OnSwitchSplit)
	EVT_TREE_SEL_CHANGED(wxGISIdentifyDlg::ID_WXGISTREECTRL, wxGISIdentifyDlg::OnSelChanged)
	EVT_MENU_RANGE(ID_WGMENU_FLASH, ID_WGMENU_ZOOM, wxGISIdentifyDlg::OnMenu)
    EVT_TREE_ITEM_RIGHT_CLICK(ID_WXGISTREECTRL, wxGISIdentifyDlg::OnItemRightClick)
	EVT_SPLITTER_DCLICK(wxID_ANY, wxGISIdentifyDlg::OnDoubleClickSash)
END_EVENT_TABLE()


wxGISIdentifyDlg::wxGISIdentifyDlg( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
	Create(parent, id, pos, size, style);
}

wxGISIdentifyDlg::wxGISIdentifyDlg(void)
{
}

bool wxGISIdentifyDlg::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	if(!wxPanel::Create( parent, id, pos, size, style, name ))
		return false;

    m_nSashPos = 150;
    wxSplitMode eMode = wxSPLIT_VERTICAL;
	wxGISAppConfig oConfig = GetConfig();
    m_sAppName = GetApplication()->GetAppName();
	if(oConfig.IsOk())
    {
        m_nSashPos = oConfig.ReadInt(enumGISHKCU, m_sAppName + wxString(wxT("/identifydlg/sashpos1")), m_nSashPos);
        eMode = (wxSplitMode)oConfig.ReadInt(enumGISHKCU, m_sAppName + wxString(wxT("/identifydlg/mode")), eMode);
    }

	m_bMainSizer = new wxBoxSizer( wxVERTICAL );

	m_fgTopSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
	m_fgTopSizer->AddGrowableCol( 1 );
	m_fgTopSizer->SetFlexibleDirection( wxBOTH );
	m_fgTopSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Identify from:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_fgTopSizer->Add( m_staticText1, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );

	wxArrayString m_LayerChoiceChoices;
	m_LayerChoiceChoices.Add(_("<Top layer>"));
	m_LayerChoiceChoices.Add(_("<All layers>"));
	m_LayerChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_LayerChoiceChoices, 0 );
	m_LayerChoice->SetSelection( 0 );
	//m_LayerChoice->Enable(false);
	m_fgTopSizer->Add( m_LayerChoice, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );

	m_BmpVert = wxBitmap(splitter_switch_xpm);
	wxImage oImg = m_BmpVert.ConvertToImage().Rotate90();
	m_BmpHorz = wxBitmap(oImg);
	//get splitter from conf
	m_bpSplitButton = new wxBitmapButton( this, ID_SWITCHSPLIT, m_BmpVert, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_fgTopSizer->Add( m_bpSplitButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

	m_bMainSizer->Add( m_fgTopSizer, 0, wxEXPAND, 5 );

	m_splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	//m_splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( wxGISIdentifyDlg::SplitterOnIdle ), NULL, this );
    m_splitter->Bind(wxEVT_IDLE, &wxGISIdentifyDlg::SplitterOnIdle, this);

	m_bMainSizer->Add( m_splitter, 1, wxEXPAND, 5 );

	m_TreeImageList.Create(16, 16);
	m_TreeImageList.Add(wxBitmap(layers_16_xpm));
	m_TreeImageList.Add(wxBitmap(layer_16_xpm));
	m_TreeImageList.Add(wxBitmap(id_xpm));

    int nOSMajorVer(0);
    wxGetOsVersion(&nOSMajorVer);
	m_pTreeCtrl = new wxTreeCtrl( m_splitter, ID_WXGISTREECTRL, wxDefaultPosition, wxDefaultSize, wxTR_HAS_BUTTONS | wxTR_TWIST_BUTTONS | wxTR_HIDE_ROOT | wxTR_LINES_AT_ROOT | wxSTATIC_BORDER | (nOSMajorVer > 5 ? wxTR_NO_LINES : wxTR_LINES_AT_ROOT) );
	m_pTreeCtrl->SetImageList(&m_TreeImageList);
	//m_pTreeCtrl->Connect( wxEVT_LEFT_DOWN, wxMouseEventHandler( wxGISIdentifyDlg::OnLeftDown ), NULL, this );
    m_pTreeCtrl->Bind(wxEVT_LEFT_DOWN, &wxGISIdentifyDlg::OnLeftDown, this);


	m_splitter->SetSashGravity(0.5);
	m_pFeatureDetailsPanel = new wxGISFeatureDetailsPanel(m_splitter);

	switch(eMode)
	{
	case wxSPLIT_HORIZONTAL:
		m_bpSplitButton->SetBitmap(m_BmpHorz);
		m_splitter->SplitHorizontally(m_pTreeCtrl, m_pFeatureDetailsPanel, 150);
		break;
    default:
	case wxSPLIT_VERTICAL:
		m_bpSplitButton->SetBitmap(m_BmpVert);
		m_splitter->SplitVertically(m_pTreeCtrl, m_pFeatureDetailsPanel, 150);
		break;
	};
    //m_splitter->SplitVertically(m_pTreeCtrl, m_pFeatureDetailsPanel, 150);

	this->SetSizer( m_bMainSizer );
	this->Layout();

	m_pMenu = new wxMenu;
	m_pMenu->Append(ID_WGMENU_FLASH, wxString(_("Flash")), wxString(_("Flash geometry")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WGMENU_PAN, wxString(_("Pan")), wxString(_("Pan to geometry center")), wxITEM_NORMAL);
	m_pMenu->Append(ID_WGMENU_ZOOM, wxString(_("Zoom")), wxString(_("Zoom to geometry")), wxITEM_NORMAL);

    return true;
}

wxGISIdentifyDlg::~wxGISIdentifyDlg()
{
	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
    {
        oConfig.Write(enumGISHKCU, m_sAppName + wxString(wxT("/identifydlg/sashpos1")), m_splitter->GetSashPosition());
        oConfig.Write(enumGISHKCU, m_sAppName + wxString(wxT("/identifydlg/mode")), (int)m_splitter->GetSplitMode());
    }
    wxDELETE(m_pMenu);
}

void wxGISIdentifyDlg::OnSwitchSplit(wxCommandEvent& event)
{
	wxSplitMode eMode = m_splitter->GetSplitMode();
	int nSplitPos = m_splitter->GetSashPosition();
	m_splitter->Unsplit(m_pTreeCtrl);
	m_splitter->Unsplit(m_pFeatureDetailsPanel);
	if(eMode == wxSPLIT_HORIZONTAL)
		eMode = wxSPLIT_VERTICAL;
	else
		eMode = wxSPLIT_HORIZONTAL;

	switch(eMode)
	{
	case wxSPLIT_HORIZONTAL:
		m_bpSplitButton->SetBitmap(m_BmpHorz);
		m_splitter->SplitHorizontally(m_pTreeCtrl, m_pFeatureDetailsPanel, nSplitPos);
		break;
    default:
	case wxSPLIT_VERTICAL:
		m_bpSplitButton->SetBitmap(m_BmpVert);
		m_splitter->SplitVertically(m_pTreeCtrl, m_pFeatureDetailsPanel, nSplitPos);
		break;
	};
}

void wxGISIdentifyDlg::OnSelChanged(wxTreeEvent& event)
{
    wxTreeItemId TreeItemId = event.GetItem();
    if(TreeItemId.IsOk())
    {
        wxIdentifyTreeItemData* pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(TreeItemId);
        if(pData == NULL)
		{
			m_pFeatureDetailsPanel->Clear();
			return;
		}
		if(pData->m_nOID == wxNOT_FOUND)
		{
			m_pFeatureDetailsPanel->Clear();
			return;
		}
		wxGISFeature Feature = pData->m_pDataset->GetFeature(pData->m_nOID);
		m_pFeatureDetailsPanel->FillPanel(Feature);
    }
}

void wxGISIdentifyDlg::SetEncoding(const wxFontEncoding& eEnc)
{
    wxIdentifyTreeItemData* pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(m_pTreeCtrl->GetSelection());
    if (pData != NULL)
    {
        pData->m_pDataset->SetEncoding(eEnc);
        wxGISFeature Feature = pData->m_pDataset->GetFeature(pData->m_nOID);
        m_pFeatureDetailsPanel->FillPanel(Feature);
    }
}

void wxGISIdentifyDlg::OnLeftDown(wxMouseEvent& event)
{
}

void wxGISIdentifyDlg::OnMenu(wxCommandEvent& event)
{
}

void wxGISIdentifyDlg::OnItemRightClick(wxTreeEvent& event)
{
 	wxTreeItemId item = event.GetItem();
	if(!item.IsOk())
		return;
    m_pTreeCtrl->SelectItem(item);
    PopupMenu(m_pMenu, event.GetPoint());
}


void wxGISIdentifyDlg::OnDoubleClickSash(wxSplitterEvent& event)
{
	event.Veto();
}

//-------------------------------------------------------------------
// wxAxToolboxView
//-------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxAxIdentifyView, wxGISIdentifyDlg)

wxAxIdentifyView::wxAxIdentifyView(void)
{
}

wxAxIdentifyView::wxAxIdentifyView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISIdentifyDlg(parent, id, pos, size, wxNO_BORDER | wxTAB_TRAVERSAL)
{
    Create(parent, id, pos, size);
}

wxAxIdentifyView::~wxAxIdentifyView(void)
{
}

bool wxAxIdentifyView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	m_pMapView = NULL;
    m_sViewName = wxString(_("Identify"));
    return wxGISIdentifyDlg::Create(parent, id, pos, size, style, name);
}

bool wxAxIdentifyView::Activate(IApplication* const pApplication, wxXmlNode* const pConf)
{
    m_pApp = dynamic_cast<wxGISApplicationBase*>(pApplication);
    if(NULL == m_pApp)
        return false;

	////get split from config and apply it
	//m_pConf = pConf;
	//wxSplitMode eMode = (wxSplitMode)wxAtoi(m_pConf->GetAttribute(wxT("split_mode"), wxT("2")));//wxSPLIT_VERTICAL = 2
	//m_nSashPos = wxAtoi(m_pConf->GetAttribute(wxT("split_pos"), wxT("100")));

	////int w = wxAtoi(m_pConf->GetAttribute(wxT("width"), wxT("-1")));
	////int h = wxAtoi(m_pConf->GetAttribute(wxT("height"), wxT("-1")));
	////SetClientSize(w, h);

	//m_splitter->Unsplit(m_pTreeCtrl);
	//m_splitter->Unsplit(m_pFeatureDetailsPanel);
	//switch(eMode)
	//{
	//case wxSPLIT_HORIZONTAL:
	//	m_bpSplitButton->SetBitmap(m_BmpHorz);
	//	m_splitter->SplitHorizontally(m_pTreeCtrl, m_pFeatureDetailsPanel, 150);
	//	break;
	//case wxSPLIT_VERTICAL:
	//	m_bpSplitButton->SetBitmap(m_BmpVert);
	//	m_splitter->SplitVertically(m_pTreeCtrl, m_pFeatureDetailsPanel, 150);
	//	break;
	//};
	return true;
}

void wxAxIdentifyView::Deactivate(void)
{
	//set split to config
	//int w, h;
	//GetClientSize(&w, &h);
	//if(m_pConf->HasAttribute(wxT("width")))
	//	m_pConf->DeleteAttribute(wxT("width"));
	//m_pConf->AddAttribute(wxT("width"), wxString::Format(wxT("%d"), w));
	//if(m_pConf->HasAttribute(wxT("height")))
	//	m_pConf->DeleteAttribute(wxT("height"));
	//m_pConf->AddAttribute(wxT("height"), wxString::Format(wxT("%d"), h));

	//wxSplitMode eMode = m_splitter->GetSplitMode();
	//int nSplitPos = m_splitter->GetSashPosition();
	//if(m_pConf->HasAttribute(wxT("split_mode")))
	//	m_pConf->DeleteAttribute(wxT("split_mode"));
	//m_pConf->AddAttribute(wxT("split_mode"), wxString::Format(wxT("%d"), eMode));
	//if(m_pConf->HasAttribute(wxT("split_pos")))
	//	m_pConf->DeleteAttribute(wxT("split_pos"));
	//m_pConf->AddAttribute(wxT("split_pos"), wxString::Format(wxT("%d"), nSplitPos));
}

wxGISSymbol* wxAxIdentifyView::GetDrawSymbol(OGRwkbGeometryType eType) const
{

    wxGISAppConfig oConfig = GetConfig();

    //TODO: store and restore symbol in config - wxGISSymbol::Serialase(
    wxGISColor color_fill(0, 0, 0, 255);
    wxGISColor color_outline(255, 255, 255, 255);

    if(oConfig.IsOk())
    {
	    int nRed = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/identify/flash_line/red")), 0);
	    int nGreen = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/identify/flash_line/green")), 210);
	    int nBlue = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/identify/flash_line/blue")), 255);
        color_outline.Set(nRed, nGreen, nBlue);

	    nRed = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/identify/flash_fill/red")), 0);
	    nGreen = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/identify/flash_fill/green")), 255);
	    nBlue = oConfig.ReadInt(enumGISHKCU, wxString(wxT("wxGISCommon/identify/flash_fill/blue")), 255);
        color_fill.Set(nRed, nGreen, nBlue);
    }
    else
    {
        color_outline.Set(0, 210, 255);
        color_fill.Set(0, 255, 255);
    }

    wxGISSymbol* pSymbol = NULL;
	switch(eType)
	{
	case wkbMultiPoint:
	case wkbPoint:
        {
        wxGISSimpleMarkerSymbol *pMarkerSymbol = new wxGISSimpleMarkerSymbol(color_fill, 5);
        pMarkerSymbol->SetOutlineColor(color_outline);
        pMarkerSymbol->SetOutlineSize(1);
        pSymbol = wxStaticCast(pMarkerSymbol, wxGISSymbol);
        }
		break;
	case wkbMultiPolygon:
	case wkbPolygon:
        {
        wxGISSimpleLineSymbol *pLineSymbol = new wxGISSimpleLineSymbol(color_outline, 1.5);
        pSymbol = wxStaticCast(new wxGISSimpleFillSymbol(color_fill, pLineSymbol), wxGISSymbol);
        }
        break;
	case wkbMultiLineString:
    case wkbLineString:
        {
        wxGISSimpleLineSymbol *pLineSymbol = new wxGISSimpleLineSymbol(color_outline, 2.5);
        pSymbol = wxStaticCast(pLineSymbol, wxGISSymbol);
        }
        break;
	case wkbGeometryCollection:
	case wkbLinearRing:
	case wkbUnknown:
	case wkbNone:
	default:
		break;
	}
    return pSymbol;
}

void wxAxIdentifyView::Identify(wxGISMapView* pMapView, wxGISGeometry &GeometryBounds)
{
    m_pMapView = pMapView;
	//if(!m_pMapView)//TODO: add/remove layer map events connection point
	//{
 //       wxWindow* pWnd = m_pApp->GetRegisteredWindowByType(wxCLASSINFO(wxGISMapView));
 //       m_pMapView = dynamic_cast<wxGISMapView*>(pWnd);
	//}
	if(!m_pMapView)
        return;

	wxBusyCursor wait;
	wxGISSpatialReference SpaRef = m_pMapView->GetSpatialReference();
    double dfWidth(3), dfHeight(3);

    wxGISAppConfig oConfig = GetConfig();
    if(oConfig.IsOk())
    {
        dfWidth = oConfig.ReadDouble(enumGISHKCU, wxString(wxT("wxGISCommon/identify/search_width")), dfWidth);
        dfHeight = oConfig.ReadDouble(enumGISHKCU, wxString(wxT("wxGISCommon/identify/search_height")), dfHeight);
    }

    if(m_pMapView->GetDisplay())
    {
        m_pMapView->GetDisplay()->DC2WorldDist(&dfWidth, &dfHeight);
        dfWidth = std::fabs(dfWidth);
        dfHeight = std::fabs(dfHeight);
    }

    OGREnvelope Env = GeometryBounds.GetEnvelope();
    bool bChanged(false);
    //if we got a small envelope or it's a point
    if(Env.MaxX - Env.MinX < dfWidth)
    {
        Env.MinX -= dfWidth;
        Env.MaxX += dfWidth;
        bChanged = true;
    }

    if(Env.MaxY - Env.MinY < dfHeight)
    {
        Env.MinY -= dfHeight;
        Env.MaxY += dfHeight;
        bChanged = true;
    }

    if(bChanged)
    {
        GeometryBounds = EnvelopeToGeometry(Env, SpaRef);
    }

    OGRPoint *pt = GeometryBounds.GetCentroid();

    int nSelection = m_LayerChoice->GetSelection();
    wxVector<FILLTREEDATA> data;

    switch(nSelection)
    {
    case 0://get top layer
        //TODO: check group layer 	
        for (size_t i = m_pMapView->GetLayerCount() - 1; i >= 0; --i)
        {
            wxGISLayer* const pLayer = m_pMapView->GetLayerByIndex(i);
            if (NULL == pLayer)
            {
                continue;
            }
            else if (pLayer->GetType() == enumGISFeatureDataset || pLayer->GetType() == enumGISRasterDataset)
            {
                FILLTREEDATA stdata = { pLayer, wxNullSpatialTreeCursor };
                data.push_back(stdata);
                break;
            }
        }
        break;
    case 1://get all layers
        //TODO: check group layer
        for(size_t i = 0; i < m_pMapView->GetLayerCount(); ++i)
        {
            wxGISLayer* const pLayer = m_pMapView->GetLayerByIndex(i);
            if (NULL == pLayer)
            {
                continue;
            }
            else if (pLayer->GetType() == enumGISFeatureDataset || pLayer->GetType() == enumGISRasterDataset)
            {
                FILLTREEDATA stdata = { pLayer, wxNullSpatialTreeCursor };
                data.push_back(stdata);
            }
        }
        break;
    default:
        return;
    };

    for(size_t i = 0; i < data.size(); ++i)
    {
	    wxGISEnumDatasetType eType = data[i].pLayer->GetType();
	    switch(eType)
	    {
	    case enumGISFeatureDataset:
		    {
			    wxGISFeatureLayer* pFLayer = dynamic_cast<wxGISFeatureLayer*>(data[i].pLayer);
			    if(!pFLayer)
				    return;

			    wxGISSpatialTreeCursor Cursor = pFLayer->Idetify(GeometryBounds);

                wxGISSpatialTreeCursor::const_iterator iter;
                for(iter = Cursor.begin(); iter != Cursor.end(); ++iter)
                {    
                    wxGISSpatialTreeData *current = *iter;
                    if(current)
                    {
                        //flash on map
                        wxGISGeometry Geom = current->GetGeometry();
                        if (Geom.IsOk())
                        {
                            wxGISSymbol* pSymbol = GetDrawSymbol(Geom.GetType());
                            m_pMapView->AddFlashGeometry(Geom, pSymbol);
                        }
                    }
                }
                m_pMapView->StartFlashing();

                data[i].Cursor = Cursor;
		    }
		    break;
	    default:
		    break;
	    };
    }

    //fill IdentifyDlg
	m_pFeatureDetailsPanel->Clear(true);
	m_pFeatureDetailsPanel->FillPanel(pt);
	FillTree(data);

    OGRGeometryFactory::destroyGeometry(pt);
}

void wxAxIdentifyView::FillTree(const wxVector<FILLTREEDATA> &data)
{
	m_pTreeCtrl->DeleteAllItems();
	m_pFeatureDetailsPanel->Clear();

    //add root
	wxTreeItemId nRootId = m_pTreeCtrl->AddRoot(wxT("Layers"), 0);
	m_pTreeCtrl->SetItemBold(nRootId);
	//add layers
    for(size_t i = 0; i < data.size(); ++i)
    {
        if(data[i].Cursor.size() == 0)
            continue;

	    wxGISEnumDatasetType eType = data[i].pLayer->GetType();
	    wxTreeItemId nLayerId = m_pTreeCtrl->AppendItem(nRootId, data[i].pLayer->GetName(), 1);
	    switch(eType)
	    {
	    case enumGISFeatureDataset:
		    {
                wxGISFeatureDataset* pDataset = wxDynamicCast(data[i].pLayer->GetDataset(), wxGISFeatureDataset);
	            m_pTreeCtrl->SetItemData(nLayerId, new wxIdentifyTreeItemData(pDataset));
	            //wxTreeItemId nFirstFeatureId = nLayerId;

                wxGISSpatialTreeCursor::const_iterator iter;
                for(iter = data[i].Cursor.begin(); iter != data[i].Cursor.end(); ++iter)
                {    
                    wxGISSpatialTreeData *current = *iter;
                    if(current)
                    {
                        wxTreeItemId nFeatureId = m_pTreeCtrl->AppendItem(nLayerId, wxString::Format(wxT("%d"), current->GetFID()), 2);
		                m_pTreeCtrl->SetItemData(nFeatureId, new wxIdentifyTreeItemData(pDataset, current->GetFID(), current->GetGeometry()));
        //		        if(iter == data[i].Cursor.begin())
        //			        nFirstFeatureId = nFeatureId;
                    }
                }
            }
            break;
	    default:
		    break;
        };
	    m_pTreeCtrl->ExpandAllChildren(nLayerId);
    }
//	m_pTreeCtrl->SelectItem(nFirstFeatureId);
}

void wxAxIdentifyView::OnSelChanged(wxTreeEvent& event)
{
    event.Skip();
    wxTreeItemId TreeItemId = event.GetItem();
    if(TreeItemId.IsOk())
    {
        wxIdentifyTreeItemData* pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(TreeItemId);
        if(pData == NULL)
		{
			m_pFeatureDetailsPanel->Clear();
			return;
		}
		if(pData->m_nOID == wxNOT_FOUND)
		{
			m_pFeatureDetailsPanel->Clear();
			return;
		}

        m_pMapView->AddFlashGeometry(pData->m_Geometry, GetDrawSymbol(pData->m_Geometry.GetType()));
        m_pMapView->StartFlashing();
        wxGISFeature Feature = pData->m_pDataset->GetFeatureByID(pData->m_nOID);
		m_pFeatureDetailsPanel->FillPanel(Feature);
    }
}

void wxAxIdentifyView::OnLeftDown(wxMouseEvent& event)
{
	event.Skip();
	wxPoint pt = event.GetPosition();
	unsigned long nFlags(0);
	wxTreeItemId TreeItemId = m_pTreeCtrl->HitTest(pt, (int &)nFlags);
	if(TreeItemId.IsOk() && ((nFlags & wxTREE_HITTEST_ONITEMLABEL) || (nFlags & wxTREE_HITTEST_ONITEMICON)))
	{
        wxIdentifyTreeItemData* pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(TreeItemId);
        if(NULL != pData && pData->m_Geometry.IsOk())
        {
            m_pMapView->AddFlashGeometry(pData->m_Geometry, GetDrawSymbol(pData->m_Geometry.GetType()));
            m_pMapView->StartFlashing();
        }
    }
}

void wxAxIdentifyView::OnMenu(wxCommandEvent& event)
{
	wxTreeItemId TreeItemId = m_pTreeCtrl->GetSelection();
    wxIdentifyTreeItemData* pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(TreeItemId);
    if(pData == NULL)
		return;

	switch(event.GetId())
	{
	case ID_WGMENU_FLASH:
	{
        if (pData->m_Geometry.IsOk())
        {
            m_pMapView->AddFlashGeometry(pData->m_Geometry, GetDrawSymbol(pData->m_Geometry.GetType()));
        }
		else
		{
            wxTreeItemIdValue cookie;
			for ( wxTreeItemId item = m_pTreeCtrl->GetFirstChild(TreeItemId, cookie); item.IsOk(); item = m_pTreeCtrl->GetNextChild(TreeItemId, cookie) )
			{
				pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(item);
                if (pData)
                {
                    m_pMapView->AddFlashGeometry(pData->m_Geometry, GetDrawSymbol(pData->m_Geometry.GetType()));
                }
			}
		}
        m_pMapView->StartFlashing();
	}
	break;
	case ID_WGMENU_PAN:
	{
        wxGISGeometryArray Arr;
		if(pData->m_Geometry.IsOk())
        {
            m_pMapView->AddFlashGeometry(pData->m_Geometry, GetDrawSymbol(pData->m_Geometry.GetType()));
            Arr.Add(pData->m_Geometry);
        }
		else
		{
            wxTreeItemIdValue cookie;
			for ( wxTreeItemId item = m_pTreeCtrl->GetFirstChild(TreeItemId, cookie); item.IsOk(); item = m_pTreeCtrl->GetNextChild(TreeItemId, cookie) )
			{
				pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(item);
				if(pData)
                {
                    m_pMapView->AddFlashGeometry(pData->m_Geometry, GetDrawSymbol(pData->m_Geometry.GetType()));
                    Arr.Add(pData->m_Geometry);
                }
			}
		}

        if(Arr.IsEmpty())
            return;

		OGREnvelope Env;
		for(size_t i = 0; i < Arr.GetCount(); ++i)
		{
            OGREnvelope TempEnv = Arr[i].GetEnvelope();
			Env.Merge(TempEnv);
		}
		OGREnvelope CurrentEnv = m_pMapView->GetCurrentExtent();
		MoveEnvelope(CurrentEnv, Env);
		m_pMapView->Do(CurrentEnv);
        m_pMapView->StartFlashing();
	}
	break;
	case ID_WGMENU_ZOOM:
	{
        wxGISGeometryArray Arr;

		if(pData->m_Geometry.IsOk())
        {
            m_pMapView->AddFlashGeometry(pData->m_Geometry, GetDrawSymbol(pData->m_Geometry.GetType()));
            Arr.Add(pData->m_Geometry);
        }
		else
		{
			wxTreeItemIdValue cookie;
			for ( wxTreeItemId item = m_pTreeCtrl->GetFirstChild(TreeItemId, cookie); item.IsOk(); item = m_pTreeCtrl->GetNextChild(TreeItemId, cookie) )
			{
				pData = (wxIdentifyTreeItemData*)m_pTreeCtrl->GetItemData(item);
				if(pData)
                {
                    m_pMapView->AddFlashGeometry(pData->m_Geometry, GetDrawSymbol(pData->m_Geometry.GetType()));
                    Arr.Add(pData->m_Geometry);
                }
			}
		}

        if(Arr.IsEmpty())
            return;

		OGREnvelope Env;
		for(size_t i = 0; i < Arr.GetCount(); ++i)
		{
			OGREnvelope TempEnv = Arr[i].GetEnvelope();
			Env.Merge(TempEnv);
		}
		m_pMapView->Do(Env);
        m_pMapView->StartFlashing();
	}
	break;
	default:
	break;
	}
}

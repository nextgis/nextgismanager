/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISTableView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013,2014 Dmitry Baryshnikov
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
#include "wxgis/cartoui/tableview.h"
#include "wxgis/cartoui/cartoui.h"

#include "../../art/full_arrow.xpm"
#include "../../art/arrow.xpm"
#include "../../art/small_arrow.xpm"
#include "../../art/delete.xpm"

#include "wx/renderer.h"
#include <wx/fontmap.h>

#define FILL_STEP 1000

//------------------------------------------------------------------
// wxGISGridTable
//------------------------------------------------------------------
IMPLEMENT_CLASS(wxGISGridTable, wxGridTableBase)

wxGISGridTable::wxGISGridTable(wxGISDataset* pGISDataset)
{
    m_nRows = m_nCols = 0;
    m_pGISDataset = wxDynamicCast(pGISDataset, wxGISTable);
	OGRFeatureDefn* pOGRFeatureDefn = m_pGISDataset->GetDefinition();
	if(pOGRFeatureDefn)
	{
		m_nCols = pOGRFeatureDefn->GetFieldCount();
		m_nRows = m_pGISDataset->GetFeatureCount();
	}

    FillForPos(0);
}

wxGISGridTable::~wxGISGridTable()
{
    wsDELETE(m_pGISDataset);
}

int wxGISGridTable::GetNumberCols()
{
	return m_nCols;
}


int wxGISGridTable::GetNumberRows()
{
    return m_nRows;
}

wxString wxGISGridTable::GetValue(int row, int col)
{
    if(row < 0 || col < 0)
		return wxEmptyString;

	if(GetNumberCols() <= col || GetNumberRows() <= row)
		return wxEmptyString;

	//fetch more data
    wxGISFeature Feature;
    if (m_moFeatures[row].IsOk())
    {
        Feature = m_moFeatures[row];
    }
    else
    {
     //   Feature = m_pGISDataset->GetFeature(row);
     //   m_moFeatures[row] = Feature;
        FillForPos(row);
        return GetValue(row, col);
    }

    if (Feature.IsOk())
    {
        return Feature.GetFieldAsString(col);
    }
	return wxEmptyString;
}

void wxGISGridTable::SetValue(int row, int col, const wxString &value)
{
}

wxString wxGISGridTable::GetColLabelValue(int col)
{
    wxString label;
    OGRFeatureDefn* pOGRFeatureDefn = m_pGISDataset->GetDefinition();
    if(!pOGRFeatureDefn)
        return wxEmptyString;

	OGRFieldDefn* pOGRFieldDefn = pOGRFeatureDefn->GetFieldDefn(col);
    if (pOGRFieldDefn)
    {
		label = wxString(pOGRFieldDefn->GetNameRef(), wxConvUTF8);
        switch (pOGRFieldDefn->GetType())
        {
        case OFTInteger:
        case OFTReal:
            m_mnAlign[col] = wxALIGN_RIGHT;
            break;
        default:
            m_mnAlign[col] = wxALIGN_LEFT;
            break;
        }
    }

    if(label.IsSameAs(m_pGISDataset->GetFIDColumn(), false))
    {
        label.Append(_(" [*]"));
	}
    return label;
}

wxString wxGISGridTable::GetRowLabelValue(int row)
{
	return wxEmptyString;
}

wxGISTable* wxGISGridTable::GetDataset() const
{
    wsGET(m_pGISDataset);
}

bool wxGISGridTable::IsEmptyCell(int row, int col)
{
    return false;
}

void wxGISGridTable::FillForPos(int nRow)
{
    wxBusyCursor wait;

    long nBeg = floor(double(nRow) / FILL_STEP) * FILL_STEP;

    wxGISFeature Feature = m_pGISDataset->GetFeature(nBeg);
    if (Feature.IsOk())
        m_moFeatures[nBeg] = Feature;
    else
        m_nRows--;

    int nEndPos = nBeg + FILL_STEP;
    if (nEndPos > m_nRows)
        nEndPos = m_nRows;

    for (long i = nBeg + 1; i < nEndPos; ++i)
    {
        //if (m_moFeatures[i].IsOk())
        //    continue;
        Feature = m_pGISDataset->Next();
        if (Feature.IsOk())
            m_moFeatures[i] = Feature;
        else
        {
            m_nRows--;
            //TODO: Check if this is working
            wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_DELETED, m_nRows, 1);
            GetView()->ProcessTableMessage(msg);
        }
    }
}

void wxGISGridTable::ClearFeatures(void)
{
    m_moFeatures.clear();
    m_mnAlign.clear();
}

bool wxGISGridTable::DeleteCols(size_t pos, size_t numCols)
{
    int i = pos + numCols - 1;
    if (i > m_nCols)
        return false;
    int nEnd = pos;
    ClearFeatures();

    //delete all fields
    do
    {
        OGRErr eResult = m_pGISDataset->DeleteField(i);
        if (eResult != OGRERR_NONE)
        {
            const char* err = CPLGetLastErrorMsg();
            wxString sErr = wxString::Format(_("Operation '%s' failed! OGR error: %s"), _("Delete field"), wxString(err, wxConvUTF8).c_str());
            wxMessageBox(sErr, _("Error"), wxOK | wxICON_ERROR);

            return false;
        }
        i--;
        m_nCols--;
    } while (i>= nEnd);


    wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_COLS_DELETED, pos, numCols);
    GetView()->ProcessTableMessage(msg);

    return true;
}

void wxGISGridTable::SetEncoding(const wxFontEncoding &oEncoding)
{
    m_pGISDataset->SetEncoding(oEncoding);
    ClearFeatures();
}

bool wxGISGridTable::CanDeleteField(void) const
{
    return m_pGISDataset->CanDeleteField();
}


wxGridCellAttr *wxGISGridTable::GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind)
{
    wxGridCellAttr *pRet = wxGridTableBase::GetAttr(row, col, kind);
    if (pRet == NULL)
    {
        pRet = new wxGridCellAttr();
    }
    pRet->SetAlignment(m_mnAlign[col], wxALIGN_TOP);

    if(kind == wxGridCellAttr::Cell || kind == wxGridCellAttr::Any || kind == wxGridCellAttr::Default)
    {
        wxString sValue = GetValue(row, col);
        if(IsURL(sValue))
        {
            wxFont Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
            Font.SetUnderlined(true);
            pRet->SetFont(Font);
            pRet->SetTextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_HIGHLIGHT));
        }
        else if(IsLocalURL(sValue))
        {
            wxFont Font = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);
            Font.SetUnderlined(true);
            pRet->SetFont(Font);
            pRet->SetTextColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
        }
    }
    return pRet;
}

//-------------------------------------
// wxGridCtrl
//-------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGridCtrl, wxGrid);

BEGIN_EVENT_TABLE(wxGridCtrl, wxGrid)
    EVT_GRID_LABEL_LEFT_CLICK(wxGridCtrl::OnLabelLeftClick)
    EVT_GRID_LABEL_RIGHT_CLICK(wxGridCtrl::OnLabelRightClick)
    EVT_GRID_SELECT_CELL(wxGridCtrl::OnSelectCell)
    EVT_GRID_CELL_LEFT_CLICK(wxGridCtrl::OnCellClick)
    EVT_MENU_RANGE(ID_DELETE, ID_MAX, wxGridCtrl::OnMenu)
    EVT_UPDATE_UI_RANGE(ID_DELETE, ID_MAX, wxGridCtrl::OnMenuUpdateUI)
    //EVT_MOTION(wxGridCtrl::OnMouseMove)
END_EVENT_TABLE();

wxGridCtrl::wxGridCtrl()
{
}

wxGridCtrl::wxGridCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
    wxGrid(parent, id, pos, size, style, name)
{
    m_pMenu = new wxMenu;
    wxMenuItem *pItem = new wxMenuItem(m_pMenu, ID_SORT_ASC, _("Sort ascending"));
    m_pMenu->Append(pItem);
    pItem = new wxMenuItem(m_pMenu, ID_SORT_DESC, _("Sort descending"));
    m_pMenu->Append(pItem);
    pItem = new wxMenuItem(m_pMenu, ID_ADVANCED_SORTING, _("Advanced sorting..."));
    m_pMenu->Append(pItem);
    m_pMenu->AppendSeparator();
    pItem = new wxMenuItem(m_pMenu, ID_STATISTICS, _("Statistics..."));
    m_pMenu->Append(pItem);
    pItem = new wxMenuItem(m_pMenu, ID_FIELD_CALCULATOR, _("Field calculator..."));
    m_pMenu->Append(pItem);
    pItem = new wxMenuItem(m_pMenu, ID_CALCULATE_GEOMETRY, _("Calculate geometry..."));
    m_pMenu->Append(pItem);
    m_pMenu->AppendSeparator();
    pItem = new wxMenuItem(m_pMenu, ID_TURN_FIELD_OFF, _("Turn field off"));
    m_pMenu->Append(pItem);
    pItem = new wxMenuItem(m_pMenu, ID_FREESE_COLUMN, _("Freeze/Unfreeze column"));
    m_pMenu->Append(pItem);

    pItem = new wxMenuItem(m_pMenu, ID_DELETE, _("Delete field"));
    wxBitmap Bmp(delete_xpm);
#ifdef __WIN32__
    if (Bmp.IsOk())
    {
        wxImage Img = Bmp.ConvertToImage();                //Img.RotateHue(-0.1);
        pItem->SetBitmaps(Bmp, Img.ConvertToGreyscale());
    }
#else
    if (Bmp.IsOk())
    {
        pItem->SetBitmap(Bmp);
    }
#endif
    m_pMenu->Append(pItem);
    m_pMenu->AppendSeparator();
    pItem = new wxMenuItem(m_pMenu, ID_PROPERTIES, _("Properties"));
    m_pMenu->Append(pItem);

    GetGridWindow ()->Bind(wxEVT_MOTION, &wxGridCtrl::OnMouseMove, this);
    GetGridWindow ()->Bind(wxEVT_MOUSEWHEEL, &wxGridCtrl::OnMouseWheel, this);
}

wxGridCtrl::~wxGridCtrl(void)
{
}

void wxGridCtrl::DrawRowLabel(wxDC& dc, int row)
{
    if (GetRowHeight(row) <= 0 || m_rowLabelWidth <= 0)
        return;
    wxRect rect;
    int rowTop = GetRowTop(row), rowBottom = GetRowBottom(row) - 1;
    dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DSHADOW), 1, wxSOLID));
    dc.DrawLine(m_rowLabelWidth - 1, rowTop, m_rowLabelWidth - 1, rowBottom);
    dc.DrawLine(0, rowTop, 0, rowBottom);
    dc.DrawLine(0, rowBottom, m_rowLabelWidth, rowBottom);
    dc.SetPen(*wxWHITE_PEN);
    dc.DrawLine(1, rowTop, 1, rowBottom);
    dc.DrawLine(1, rowTop, m_rowLabelWidth - 1, rowTop);
    if (row == GetGridCursorRow())
	{
		dc.DrawBitmap(wxBitmap(small_arrow_xpm), 0, GetRowTop(row), true);
    }
}

void wxGridCtrl::OnLabelLeftClick(wxGridEvent& event)
{
    event.Skip();
    if (event.GetRow() != wxNOT_FOUND)
	{
        SetGridCursor(event.GetRow(),0);
    }
}

void wxGridCtrl::OnLabelRightClick(wxGridEvent& event)
{
    wxPoint point = event.GetPosition();
    // If from keyboard exit
    if (point.x == -1 && point.y == -1)
    {
        event.Skip();
        return;
    }

    wxArrayInt anCols = GetSelectedCols();
    if (anCols.Index(event.GetCol()) == wxNOT_FOUND)
    {
        ClearSelection();
        SelectCol(event.GetCol());
    }

    PopupMenu(m_pMenu, point.x, point.y);
}

void wxGridCtrl::OnMenu(wxCommandEvent& event)
{
    wxGISGridTable* pTable = NULL;
    switch (event.GetId())
    {
    case ID_DELETE:
        pTable = wxDynamicCast(GetTable(), wxGISGridTable);
        if (pTable)
        {
            wxArrayInt anCols = GetSelectedCols();
            for (int i = anCols.GetCount() - 1; i >= 0; --i)
            {
                if (!DeleteCols(anCols[i]))
                {
                    break;
                }
            }
        }
        break;
    default:
        break;
    }
}

void wxGridCtrl::OnMenuUpdateUI(wxUpdateUIEvent& event)
{
    wxGISGridTable* pTable = NULL;
    switch (event.GetId())
    {
    case ID_DELETE:
        pTable = wxDynamicCast(GetTable(), wxGISGridTable);
        if (pTable)
        {
            event.Enable(pTable->CanDeleteField());
            break;
        }
    default:
        event.Enable(false);
        break;
    }
}

void wxGridCtrl::OnSelectCell(wxGridEvent& event)
{
    event.Skip(true);
    GetGridRowLabelWindow()->Refresh();
}

void wxGridCtrl::SetEncoding(const wxFontEncoding &eEnc)
{
    wxGISGridTable* pTable = wxDynamicCast(GetTable(), wxGISGridTable);
    if (pTable)
    {
        wxBusyCursor wait;

        pTable->SetEncoding(eEnc);
        ClearGrid();
    }
}

void wxGridCtrl::OnCellClick(wxGridEvent& event)
{
    event.Skip(true);
    int nCol = event.GetCol();
    int nRow = event.GetRow();
    wxString sText = GetCellValue(nRow, nCol);

    if(IsURL(sText))
    {
        wxLaunchDefaultBrowser( sText );
    }
    else if(IsLocalURL(sText))
    {
        wxLaunchDefaultApplication( sText );
    }
}

void wxGridCtrl::OnMouseMove(wxMouseEvent& event)
{
    event.Skip(true);
    wxGridCellCoords oCoords = XYToCell(event.GetPosition());
    wxString sValue = GetCellValue(oCoords);

    if(IsURL(sValue) || IsLocalURL(sValue))
    {
        GetGridWindow ()->SetCursor(wxCursor(wxCURSOR_HAND));
    }
    else
    {
        GetGridWindow ()->SetCursor(wxCursor(wxCURSOR_ARROW));
    }
}

void wxGridCtrl::OnMouseWheel(wxMouseEvent& event)
{
    event.Skip(true);
	int nDirection = event.GetWheelRotation();
	int nDelta = event.GetWheelDelta();
    ScrollLines(-nDirection/nDelta);
}

//-------------------------------------
// wxGISTableView
//-------------------------------------

IMPLEMENT_CLASS(wxGISTableView, wxPanel)

BEGIN_EVENT_TABLE(wxGISTableView, wxPanel)
	EVT_BUTTON(wxGISTableView::ID_FIRST, wxGISTableView::OnBtnFirst)
	EVT_BUTTON(wxGISTableView::ID_NEXT, wxGISTableView::OnBtnNext)
	EVT_BUTTON(wxGISTableView::ID_PREV, wxGISTableView::OnBtnPrev)
	EVT_BUTTON(wxGISTableView::ID_LAST, wxGISTableView::OnBtnLast)
	EVT_TEXT_ENTER(wxGISTableView::ID_POS, wxGISTableView::OnSetPos)
    EVT_COMBOBOX(wxGISTableView::ID_ENCODING, wxGISTableView::OnEncodingSelect)
END_EVENT_TABLE();

wxGISTableView::wxGISTableView(void)
{
}

wxGISTableView::wxGISTableView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
{
    Create( parent, id, pos, size, style );
}

wxGISTableView::~wxGISTableView(void)
{
}

bool wxGISTableView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    if(!wxPanel::Create( parent, id, pos, size, style, name ))
		return false;

	wxBoxSizer* bSizerMain;
	bSizerMain = new wxBoxSizer( wxVERTICAL );

	m_grid = new wxGridCtrl( this, wxID_ANY);

	// Grid
	m_grid->CreateGrid( 5, 5 );
	m_grid->EnableEditing( false );
	m_grid->EnableGridLines( true );
	m_grid->EnableDragGridSize( false );
	m_grid->SetMargins( 0, 0 );

	// Columns
	m_grid->EnableDragColMove( false );
	m_grid->EnableDragColSize( true );
	m_grid->SetColLabelSize( GRID_COL_SIZE );
	m_grid->SetColLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );

	// Rows
	m_grid->EnableDragRowSize( true );
	m_grid->SetRowLabelSize( GRID_ROW_SIZE );
	m_grid->SetRowLabelAlignment( wxALIGN_CENTRE, wxALIGN_CENTRE );

//    EVT_GRID_SELECT_CELL(wxGISTableView::OnSelectCell)
    m_grid->Bind(wxEVT_GRID_SELECT_CELL, &wxGISTableView::OnSelectCell, this);
	// Label Appearance

	// Cell Defaults
	m_grid->SetDefaultCellAlignment( wxALIGN_LEFT, wxALIGN_TOP );
	//m_grid->SetDefaultCellBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE));

	bSizerMain->Add( m_grid, 1, wxALL|wxEXPAND, 0 );

	wxBoxSizer* bSizerLow;
	bSizerLow = new wxBoxSizer( wxHORIZONTAL );

	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Record:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizerLow->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    wxBitmap FullArrowRight(full_arrow_xpm);
    wxBitmap ArrowRight(arrow_xpm);
    wxBitmap FullArrowLeft = FullArrowRight.ConvertToImage().Mirror();
    wxBitmap ArrowLeft = ArrowRight.ConvertToImage().Mirror();

	m_bpFirst = new wxBitmapButton( this, wxGISTableView::ID_FIRST, FullArrowLeft, wxDefaultPosition, wxSize( WXGISBITBUTTONSIZE, WXGISBITBUTTONSIZE ), wxBU_AUTODRAW );
	m_bpFirst->SetMinSize( wxSize( WXGISBITBUTTONSIZE,WXGISBITBUTTONSIZE ) );
	m_bpFirst->SetMaxSize( wxSize( WXGISBITBUTTONSIZE,WXGISBITBUTTONSIZE ) );

	bSizerLow->Add( m_bpFirst, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );

	m_bpPrev = new wxBitmapButton( this, wxGISTableView::ID_PREV, ArrowLeft, wxDefaultPosition, wxSize( WXGISBITBUTTONSIZE,WXGISBITBUTTONSIZE ), wxBU_AUTODRAW );
	m_bpPrev->SetMinSize( wxSize( WXGISBITBUTTONSIZE,WXGISBITBUTTONSIZE ) );
	m_bpPrev->SetMaxSize( wxSize( WXGISBITBUTTONSIZE,WXGISBITBUTTONSIZE ) );

	bSizerLow->Add( m_bpPrev, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );

	m_position = new wxTextCtrl( this, wxGISTableView::ID_POS, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_CENTRE|wxTE_PROCESS_ENTER  );
	bSizerLow->Add( m_position, 0, wxALL, 5 );

	m_bpNext = new wxBitmapButton( this, wxGISTableView::ID_NEXT, ArrowRight, wxDefaultPosition, wxSize( WXGISBITBUTTONSIZE,WXGISBITBUTTONSIZE ), wxBU_AUTODRAW );
	m_bpNext->SetMinSize( wxSize( WXGISBITBUTTONSIZE,WXGISBITBUTTONSIZE ) );
	m_bpNext->SetMaxSize( wxSize( WXGISBITBUTTONSIZE,WXGISBITBUTTONSIZE ) );

	bSizerLow->Add( m_bpNext, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );

	m_bpLast = new wxBitmapButton( this, wxGISTableView::ID_LAST, FullArrowRight, wxDefaultPosition, wxSize( WXGISBITBUTTONSIZE,WXGISBITBUTTONSIZE ), wxBU_AUTODRAW );
	m_bpLast->SetMinSize( wxSize( WXGISBITBUTTONSIZE,WXGISBITBUTTONSIZE ) );
	m_bpLast->SetMaxSize( wxSize( WXGISBITBUTTONSIZE,WXGISBITBUTTONSIZE ) );

	bSizerLow->Add( m_bpLast, 0, wxALL|wxALIGN_CENTER_VERTICAL, 0 );

	m_staticText2 = new wxStaticText( this, wxID_ANY, wxT("of"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizerLow->Add( m_staticText2, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );

    //m_staticline1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_VERTICAL);
    //bSizerLow->Add(m_staticline1, 0, wxRIGHT | wxLEFT, 5);
    m_staticText4 = new wxStaticText(this, wxID_ANY, wxT("|"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText4->Enable(false);
    bSizerLow->Add(m_staticText4, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    m_staticText3 = new wxStaticText(this, wxID_ANY, _("Encoding:"), wxDefaultPosition, wxDefaultSize, 0);
    m_staticText3->Wrap(-1);
    bSizerLow->Add(m_staticText3, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);

    //add encodings combo
    wxArrayString asEnc;
    wxString sDefault;
    for (int i = wxFONTENCODING_DEFAULT; i < wxFONTENCODING_MAX; i++)
    {
        wxString sDesc = wxFontMapper::GetEncodingDescription((wxFontEncoding)i);
        if (!sDesc.StartsWith(_("Unknown")))
        {
            if (i == wxFONTENCODING_DEFAULT)
                sDefault = sDesc;
            asEnc.Add(sDesc);
            m_mnEnc[sDesc] = (wxFontEncoding)i;
        }
    }

    m_pEncodingsCombo = new wxComboBox(this, ID_ENCODING, sDefault, wxDefaultPosition, wxDefaultSize, asEnc, wxCB_DROPDOWN | wxCB_READONLY | wxCB_SORT);
    bSizerLow->Add(m_pEncodingsCombo, 0, wxALL | wxALIGN_CENTER_VERTICAL, 5);


	bSizerMain->Add( bSizerLow, 0, wxEXPAND, 5 );

	this->SetSizer( bSizerMain );

	(*m_position) << 1;

	this->Layout();

    return true;
}

void wxGISTableView::OnSelectCell(wxGridEvent& event)
{
    event.Skip(true);
	m_position->Clear();
	(*m_position) << event.GetRow() + 1;

}

void wxGISTableView::OnBtnFirst(wxCommandEvent& event)
{
	m_grid->SetGridCursor(0,0);
	m_grid->MakeCellVisible(0,0);
	m_position->Clear();
	(*m_position) << 1;
}

void wxGISTableView::OnBtnNext(wxCommandEvent& event)
{
	m_grid->MoveCursorDown(false);
	m_position->Clear();
	(*m_position) << m_grid->GetGridCursorRow() + 1;
}

void wxGISTableView::OnBtnPrev(wxCommandEvent& event)
{
	m_grid->MoveCursorUp(false);
	m_position->Clear();
	(*m_position) << m_grid->GetGridCursorRow() + 1;
}

void wxGISTableView::OnBtnLast(wxCommandEvent& event)
{
	m_grid->SetGridCursor(m_grid->GetNumberRows() - 1,0);
	m_grid->MakeCellVisible(m_grid->GetNumberRows() - 1,0);
	m_position->Clear();
	(*m_position) << m_grid->GetNumberRows();
}

void wxGISTableView::OnSetPos(wxCommandEvent& event)
{
	long pos = wxAtol(event.GetString());
	m_grid->SetGridCursor(pos - 1,0);
	m_grid->MakeCellVisible(pos - 1,0);
}

void wxGISTableView::SetTable(wxGridTableBase* table, bool takeOwnership, wxGrid::wxGridSelectionModes selmode)
{
	if(m_grid)
	{
		m_grid->SetTable(table, takeOwnership, selmode);
		m_grid->SetGridCursor(0,0);
		m_grid->MakeCellVisible(0,0);
		m_position->Clear();

        if (m_grid->GetNumberRows() > 0)
        {
		    (*m_position) << 1;

		    m_staticText2->SetLabel(wxString::Format(_("of %u"), m_grid->GetNumberRows()));
        }

        //TODO: Set grid encoding selection in combobox

        this->Layout();
	}
}

wxGridTableBase* wxGISTableView::GetTable(void) const
{
	if(m_grid)
	{
        return m_grid->GetTable();
    }
    return NULL;
}

void wxGISTableView::OnEncodingSelect(wxCommandEvent& event)
{
    wxString sSel = m_pEncodingsCombo->GetStringSelection();
    wxFontEncoding eEnc = m_mnEnc[sSel];
    m_grid->SetEncoding(eEnc);
}



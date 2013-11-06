/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISTableView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013 Bishop
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
#include "wxgis/cartoui/tableview.h"

#include "../../art/full_arrow.xpm"
#include "../../art/arrow.xpm"
#include "../../art/small_arrow.xpm"

#include "wx/renderer.h"

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
	if(GetNumberCols() <= col || GetNumberRows() <= row)
		return wxEmptyString;

	//fetch more data
    wxGISFeature Feature = m_pGISDataset->GetFeature(row);
    if(Feature.IsOk())
        return Feature.GetFieldAsString(col);
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
	if(pOGRFieldDefn)
		label = wxString(pOGRFieldDefn->GetNameRef(), wxConvUTF8);

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


//-------------------------------------
// wxGridCtrl
//-------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGridCtrl, wxGrid);

BEGIN_EVENT_TABLE(wxGridCtrl, wxGrid)
    EVT_GRID_LABEL_LEFT_CLICK(wxGridCtrl::OnLabelLeftClick)
    EVT_GRID_SELECT_CELL(wxGridCtrl::OnSelectCell)
END_EVENT_TABLE();

wxGridCtrl::wxGridCtrl()
{
}

wxGridCtrl::wxGridCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name) :
    wxGrid(parent, id, pos, size, style, name)
{
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

void wxGridCtrl::OnSelectCell(wxGridEvent& event)
{
    event.Skip();
    GetGridRowLabelWindow()->Refresh();
}

//-------------------------------------
// wxGISTableView
//-------------------------------------

IMPLEMENT_CLASS(wxGISTableView, wxPanel)

BEGIN_EVENT_TABLE(wxGISTableView, wxPanel)
    EVT_GRID_LABEL_LEFT_CLICK(wxGISTableView::OnLabelLeftClick)
    EVT_GRID_SELECT_CELL(wxGISTableView::OnSelectCell)
	EVT_BUTTON(wxGISTableView::ID_FIRST, wxGISTableView::OnBtnFirst)
	EVT_BUTTON(wxGISTableView::ID_NEXT, wxGISTableView::OnBtnNext)
	EVT_BUTTON(wxGISTableView::ID_PREV, wxGISTableView::OnBtnPrev)
	EVT_BUTTON(wxGISTableView::ID_LAST, wxGISTableView::OnBtnLast)
	EVT_TEXT_ENTER(wxGISTableView::ID_POS, wxGISTableView::OnSetPos)
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


	bSizerMain->Add( bSizerLow, 0, wxEXPAND, 5 );

	this->SetSizer( bSizerMain );

	(*m_position) << 1;

	this->Layout();

    return true;
}

void wxGISTableView::OnLabelLeftClick(wxGridEvent& event)
{
    event.Skip();
}

void wxGISTableView::OnSelectCell(wxGridEvent& event)
{
    event.Skip();
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
		(*m_position) << 1;

		m_staticText2->SetLabel(wxString::Format(_("of %u"), m_grid->GetNumberRows()));
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

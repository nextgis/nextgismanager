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
#pragma once

#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/font.h>
#include <wx/grid.h>
#include <wx/gdicmn.h>
#include <wx/stattext.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/button.h>
#include <wx/textctrl.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/statline.h>

#include "wxgis/carto/carto.h"
#include "wxgis/datasource/table.h"

#define GRID_ROW_SIZE 15
#define GRID_COL_SIZE 20

WX_DECLARE_HASH_MAP(long, wxGISFeature, wxIntegerHash, wxIntegerEqual, wxGISFeatureMap);

/**
    @class wxGISTable

    The grid of values for dataset

    @library {cartoui}
*/

class WXDLLIMPEXP_GIS_CTU wxGISGridTable :
	public wxGridTableBase
{
    DECLARE_CLASS(wxGISGridTable)
public:
    wxGISGridTable(wxGISDataset* pGISDataset);
    virtual ~wxGISGridTable();

	//overrides
    virtual int GetNumberRows();
    virtual int GetNumberCols();
    virtual bool IsEmptyCell(int row, int col);
    virtual wxString GetValue(int row, int col);
    virtual void SetValue(int row, int col, const wxString &value);
    virtual wxString GetColLabelValue(int col);
	virtual wxString GetRowLabelValue(int row);
    virtual wxGISTable* GetDataset() const;
    virtual void ClearFeatures(void);
    virtual bool DeleteCols(size_t  pos = 0, size_t numCols = 1);
    virtual void SetEncoding(const wxFontEncoding &oEncoding);
    virtual bool CanDeleteField(void) const;
    virtual wxGridCellAttr *GetAttr(int row, int col, wxGridCellAttr::wxAttrKind kind);
protected:
    virtual void FillForPos(int nRow);
private:
	wxGISTable* m_pGISDataset;
	//wxString m_sFIDKeyName;
	int m_nCols;          // columns from dataSet
    int m_nRows;          // rows initially returned by dataSet
    wxGISFeatureMap m_moFeatures;
    std::map<int, int> m_mnAlign;
};

/**
    @class wxGridCtrl

    The control with grid and buttons (first, last, next, prev) recor count field and etc.

    @library {cartoui}
*/

class WXDLLIMPEXP_GIS_CTU wxGridCtrl:
	public wxGrid
{
    enum
    {
        ID_DELETE = wxID_HIGHEST + 5001,
        ID_SORT_ASC,
        ID_SORT_DESC,
        ID_ADVANCED_SORTING,
        ID_FIELD_CALCULATOR,
        ID_CALCULATE_GEOMETRY,
        ID_TURN_FIELD_OFF,
        ID_FREESE_COLUMN,
        ID_PROPERTIES,
        ID_STATISTICS,
        ID_MAX
    };
	DECLARE_DYNAMIC_CLASS(wxGridCtrl)
public:
	wxGridCtrl();
	virtual ~wxGridCtrl(void);
	wxGridCtrl(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxWANTS_CHARS, const wxString& name = wxPanelNameStr);
    virtual void SetEncoding(const wxFontEncoding &eEnc);
protected:
	virtual void DrawRowLabel(wxDC& dc, int row);
    virtual void OnLabelLeftClick(wxGridEvent& event);
    virtual void OnLabelRightClick(wxGridEvent& event);
    virtual void OnSelectCell(wxGridEvent& event);
    virtual void OnMenu(wxCommandEvent& event);
    virtual void OnMenuUpdateUI(wxUpdateUIEvent& event);
    virtual void OnMouseMove(wxMouseEvent& event);
    virtual void OnMouseWheel(wxMouseEvent& event);
	virtual void OnCellClick(wxGridEvent& event);
protected:
    wxMenu *m_pMenu;
private:
    DECLARE_EVENT_TABLE();
};

#ifdef __WXMSW__
    #define WXGISBITBUTTONSIZE 18
#else
    #define WXGISBITBUTTONSIZE 24
#endif

class WXDLLIMPEXP_GIS_CTU wxGISTableView :
	public wxPanel
{
    enum
    {
        ID_FIRST = wxID_HIGHEST + 4001,
        ID_PREV,
        ID_NEXT,
        ID_LAST,
	    ID_POS,
        ID_ENCODING
    };
    DECLARE_CLASS(wxGISTableView)
public:
    wxGISTableView(void);
	wxGISTableView(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSTATIC_BORDER|wxTAB_TRAVERSAL);
	virtual ~wxGISTableView(void);
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxSTATIC_BORDER|wxTAB_TRAVERSAL, const wxString& name = wxT("GISTableView"));
    virtual void SetTable(wxGridTableBase* table, bool takeOwnership = false, wxGrid::wxGridSelectionModes selmode = wxGrid::wxGridSelectCells);
    virtual wxGridTableBase* GetTable(void) const;
	virtual void SetReadOnly(bool bIsReadOnly){if(m_grid) m_grid->EnableEditing( bIsReadOnly );};
	//events
	void OnSelectCell(wxGridEvent& event);
	void OnBtnFirst(wxCommandEvent& event);
	void OnBtnNext(wxCommandEvent& event);
	void OnBtnPrev(wxCommandEvent& event);
	void OnBtnLast(wxCommandEvent& event);
	void OnSetPos(wxCommandEvent& event);
    void OnEncodingSelect(wxCommandEvent& event);
protected:
	wxGridCtrl* m_grid;
    wxStaticText* m_staticText1, *m_staticText2, *m_staticText3, *m_staticText4;
	wxBitmapButton* m_bpFirst;
	wxBitmapButton* m_bpPrev;
	wxTextCtrl* m_position;
	wxBitmapButton* m_bpNext;
	wxBitmapButton* m_bpLast;    
    wxChoice *m_pEncodingsCombo;
    wxStaticLine *m_staticline1;
    std::map<wxString, wxFontEncoding> m_mnEnc;
private:
    DECLARE_EVENT_TABLE()
};

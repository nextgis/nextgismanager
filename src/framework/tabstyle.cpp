/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  AUI Tab art styles.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Barishnikov
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

#include "wxgis/framework/tabstyle.h"

#ifdef __WXGTK__

#define VERT_PADDING 4

wxGISTabArt::wxGISTabArt() : wxAuiDefaultTabArt()
{
    //ctor
}

wxGISTabArt::~wxGISTabArt()
{
    //dtor
}

wxAuiTabArt* wxGISTabArt::Clone()
{
    wxGISTabArt* clone = new wxGISTabArt();

    clone->SetNormalFont(m_normalFont);
    clone->SetSelectedFont(m_selectedFont);
    clone->SetMeasuringFont(m_measuringFont);
    return clone;
}

void wxGISTabArt::DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page, const wxRect& in_rect, int close_button_state, wxRect* out_tab_rect, wxRect* out_button_rect, int* x_extent)
{
    wxSize tab_size = GetTabSize(dc, wnd, page.caption, page.bitmap, page.active, close_button_state, x_extent);
    wxCoord tab_height = m_tabCtrlHeight - 2;
    wxCoord tab_width = tab_size.x;
    wxCoord tab_x = in_rect.x;
    wxCoord tab_y = in_rect.y + in_rect.height - tab_height;

    int clip_width = tab_width;
    if (tab_x + clip_width > in_rect.x + in_rect.width - 4)
        clip_width = (in_rect.x + in_rect.width) - tab_x - 4;
    dc.SetClippingRegion(tab_x, tab_y, clip_width + 1, tab_height - 3);

	wxPoint tabPoints[7];
	int adjust = 0;
    if (!page.active)
    {
        adjust = 1;
    }

    tabPoints[0].x = tab_x + 3;
    tabPoints[0].y = (m_flags & wxAUI_NB_BOTTOM) ? 3 : tab_height - 2;

    tabPoints[1].x = tabPoints[0].x;
    tabPoints[1].y = (m_flags & wxAUI_NB_BOTTOM) ? tab_height - (VERT_PADDING + 2) - adjust : (VERT_PADDING + 2) + adjust;

    tabPoints[2].x = tabPoints[1].x+2;
    tabPoints[2].y = (m_flags & wxAUI_NB_BOTTOM) ? tab_height - VERT_PADDING - adjust: VERT_PADDING + adjust;

    tabPoints[3].x = tab_x +tab_width - 2;
    tabPoints[3].y = tabPoints[2].y;

    tabPoints[4].x = tabPoints[3].x + 2;
    tabPoints[4].y = tabPoints[1].y;

    tabPoints[5].x = tabPoints[4].x;
    tabPoints[5].y = tabPoints[0].y;

    tabPoints[6].x = tabPoints[0].x;
    tabPoints[6].y = tabPoints[0].y;

    dc.SetBrush((page.active) ? wxBrush(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)) : wxBrush(*wxTRANSPARENT_BRUSH));

	dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNSHADOW)));

	dc.DrawPolygon(7, tabPoints);

    dc.SetPen(wxPen(wxSystemSettings::GetColour(wxSYS_COLOUR_3DFACE)));
    if (page.active)
    {
        dc.DrawLine(tabPoints[0].x + 1, tabPoints[0].y, tabPoints[5].x , tabPoints[0].y);
    }

    int drawn_tab_yoff = tabPoints[1].y;
    int drawn_tab_height = tabPoints[0].y - tabPoints[2].y;

    int text_offset = tab_x + 8;

    int bitmap_offset = 0;
    if (page.bitmap.IsOk())
    {
        bitmap_offset = tab_x + 8;

        // draw bitmap
        dc.DrawBitmap(page.bitmap,
                      bitmap_offset,
                      drawn_tab_yoff + (drawn_tab_height/2) - (page.bitmap.GetHeight()/2),
                      true);

        text_offset = bitmap_offset + page.bitmap.GetWidth();
        text_offset += 3; // bitmap padding
    }
    else
    {
        text_offset = tab_x + 8;
    }


    // if the caption is empty, measure some temporary text
    wxString caption = page.caption;
    if (caption.empty())
        caption = wxT("Xj");

    wxCoord textx;
    wxCoord texty;
    if (page.active)
        dc.SetFont(m_selectedFont);
    else
        dc.SetFont(m_normalFont);
    dc.GetTextExtent(caption, &textx, &texty);
    // draw tab text
    dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
    dc.DrawText(page.caption, text_offset,
                drawn_tab_yoff + drawn_tab_height / 2 - texty / 2 - 1);

    // draw 'x' on tab (if enabled)
    if (close_button_state != wxAUI_BUTTON_STATE_HIDDEN)
    {
        int close_button_width = m_activeCloseBmp.GetWidth();
        wxBitmap bmp = m_disabledCloseBmp;

        if ((close_button_state == wxAUI_BUTTON_STATE_HOVER) ||
                    (close_button_state == wxAUI_BUTTON_STATE_PRESSED))
            bmp = m_activeCloseBmp;

        wxRect rect(tab_x + tab_width - close_button_width - 3,
                    drawn_tab_yoff + (drawn_tab_height / 2) - (bmp.GetHeight() / 2),
                    close_button_width, tab_height);

        // Indent the button if it is pressed down:
        if (close_button_state == wxAUI_BUTTON_STATE_PRESSED)
        {
            rect.x++;
            rect.y++;
        }
        dc.DrawBitmap(bmp, rect.x, rect.y, true);
        *out_button_rect = rect;
    }

    *out_tab_rect = wxRect(tab_x, tab_y, tab_width, tab_height);
    dc.DestroyClippingRegion();
}

int wxGISTabArt::GetBestTabCtrlSize(wxWindow* wnd, const wxAuiNotebookPageArray& WXUNUSED(pages), const wxSize& WXUNUSED(required_bmp_size))
{

    wxClientDC dc(wnd);
    dc.SetFont(m_measuringFont);
    int x_ext = 0;
    wxSize s = GetTabSize(dc, wnd, wxT("ABCDEFGHIj"), wxNullBitmap, true,
                            wxAUI_BUTTON_STATE_HIDDEN, &x_ext);
    return s.y + 6;
}

#endif // __WXGTK__

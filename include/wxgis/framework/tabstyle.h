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
#pragma once

#ifdef __WXGTK__

#include "wxgis/framework/framework.h"

#include <wx/artprov.h>
#include <wx/aui/auibook.h>

class wxGISTabArt : public wxAuiGenericTabArt
{
public:
    /** Default constructor */
    wxGISTabArt();
    /** Default destructor */
    virtual ~wxGISTabArt();
    wxAuiTabArt* Clone();
    void DrawTab(wxDC& dc, wxWindow* wnd, const wxAuiNotebookPage& page, const wxRect& in_rect, int close_button_state, wxRect* out_tab_rect, wxRect* out_button_rect, int* x_extent);
    int GetBestTabCtrlSize(wxWindow* wnd, const wxAuiNotebookPageArray& pages, const wxSize& required_bmp_size);
};

#endif // __WXGTK__

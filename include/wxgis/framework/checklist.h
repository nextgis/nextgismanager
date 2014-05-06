/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  List View with check box
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010  Bishop
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

#include "wxgis/framework/framework.h"
#include <wx/listctrl.h>
#include <wx/imaglist.h>

/** @class wxGISCheckList

    A List View with check boxes

    @library{framework}
 */
class WXDLLIMPEXP_GIS_FRW wxGISCheckList : public wxListView
{
public:
    wxGISCheckList(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxLC_NO_HEADER | wxLC_LIST /*wxLC_REPORT | wxLC_ALIGN_LEFT*/ | wxLC_SINGLE_SEL | wxLC_SORT_ASCENDING, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxT("GISCheckList"));
    ~wxGISCheckList(void);
    long InsertItem(const wxString& label, int nChecked);
    bool SetItemData(long item, long data);
    long GetItemData(long item) const;
    bool IsItemChanged(long item);
    int GetItemCheckState(long item);
    void SetItemCheckState(long item, int nState);
    //events
    void OnLeftDown(wxMouseEvent& event);

    typedef struct _item_data
    {
        int nCheckState;
        bool bChanged;
        long pUserData;
    }ITEM_DATA, *LPITEM_DATA;
protected:
    wxImageList m_CheckImageList;
private:
    DECLARE_EVENT_TABLE()
};


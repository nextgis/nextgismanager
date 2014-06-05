/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  List View with check box
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Dmitry Baryshnikov
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

#include "wxgis/framework/checklist.h"
#include "../../art/check_marks.xpm"

BEGIN_EVENT_TABLE(wxGISCheckList, wxListView)
	EVT_LEFT_DOWN( wxGISCheckList::OnLeftDown )
END_EVENT_TABLE()


wxGISCheckList::wxGISCheckList(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name) : wxListView(parent, id, pos, size, style, validator, name)
{
    m_CheckImageList.Create(16, 16);
	m_CheckImageList.Add(wxBitmap(check_marks_xpm));
    SetImageList(&m_CheckImageList, wxIMAGE_LIST_SMALL);
}

wxGISCheckList::~wxGISCheckList(void)
{
    for(size_t i = 0; i < GetItemCount(); ++i)
    {
        LPITEM_DATA pdata = (LPITEM_DATA)wxListView::GetItemData(i);
        wxDELETE(pdata);
    }
}

long wxGISCheckList::InsertItem(const wxString& label, int nChecked)
{
    long nItem = wxListView::InsertItem(wxListView::GetItemCount(), label, nChecked);//some state data
    LPITEM_DATA pdata = new ITEM_DATA;
    pdata->nCheckState = nChecked;
    pdata->bChanged = false;
    pdata->pUserData = 0;
    wxListView::SetItemData(nItem, (long)pdata);
    return nItem;
}

bool wxGISCheckList::SetItemData(long item, long data)
{
    LPITEM_DATA pdata = (LPITEM_DATA)wxListView::GetItemData(item);
    if(pdata)
        pdata->pUserData = data;
    return true;
}

long wxGISCheckList::GetItemData(long item) const
{
    LPITEM_DATA pdata = (LPITEM_DATA)wxListView::GetItemData(item);
    if(pdata)
        return pdata->pUserData;
    else
        return 0;
}

void wxGISCheckList::CheckAll()
{
    for (size_t i = 0; i < GetItemCount(); ++i)
    {
        SetItemCheckState(i, 1);
    }
}

void wxGISCheckList::UnCheckAll()
{
    for (size_t i = 0; i < GetItemCount(); ++i)
    {
        SetItemCheckState(i, 0);
    }
}

void wxGISCheckList::InvertCheck()
{
    for (size_t i = 0; i < GetItemCount(); ++i)
    {
        int nState = GetItemCheckState(i) == 1 ? 0 : 1;
        SetItemCheckState(i, nState);
    }
}

void wxGISCheckList::OnLeftDown(wxMouseEvent& event)
{
	event.Skip();

	wxPoint pt = event.GetPosition();
	unsigned long nFlags(0);
	long nItemId = HitTest(pt, (int &)nFlags);
	if(nItemId != wxNOT_FOUND && (nFlags & wxLIST_HITTEST_ONITEMICON))
	{
        LPITEM_DATA pdata = (LPITEM_DATA)wxListView::GetItemData(nItemId);
        if(pdata)
        {
            pdata->bChanged = !pdata->bChanged;
            pdata->nCheckState = !pdata->nCheckState;
            bool bCheck = pdata->nCheckState != 0;
            SetItemImage(nItemId, bCheck == true ? 1 : 0, bCheck == true ? 1 : 0);
       }
	}
}

bool wxGISCheckList::IsItemChanged(long item)
{
    LPITEM_DATA pdata = (LPITEM_DATA)wxListView::GetItemData(item);
    return pdata->bChanged;
}


int wxGISCheckList::GetItemCheckState(long item)
{
    LPITEM_DATA pdata = (LPITEM_DATA)wxListView::GetItemData(item);
    return pdata->nCheckState;
}

void wxGISCheckList::SetItemCheckState(long item, int nState)
{
    LPITEM_DATA pdata = (LPITEM_DATA)wxListView::GetItemData(item);
    pdata->nCheckState = nState;
    bool bCheck = pdata->nCheckState != 0;
    SetItemImage(item, bCheck == true ? 1 : 0, bCheck == true ? 1 : 0);
}


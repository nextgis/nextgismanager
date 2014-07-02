/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISTableView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011-2014 Dmitry Baryshnikov
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
#include "wxgis/catalogui/gxtableview.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/catalogui/gxapplication.h"

#include <wx/fontmap.h>

//------------------------------------------------------------
// wxGxTableView
//------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxTableView, wxGISTableView)

BEGIN_EVENT_TABLE(wxGxTableView, wxGISTableView)
	EVT_GXSELECTION_CHANGED(wxGxTableView::OnSelectionChanged)
END_EVENT_TABLE()


wxGxTableView::wxGxTableView(void) : wxGISTableView(), wxGxView()
{
    m_nParentGxObjectID = wxNOT_FOUND;
    m_ConnectionPointSelectionCookie = wxNOT_FOUND;
}

wxGxTableView::wxGxTableView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISTableView(parent, id, pos, size)
{
    m_nParentGxObjectID = wxNOT_FOUND;
    m_ConnectionPointSelectionCookie = wxNOT_FOUND;
	m_sViewName = wxString(_("Table View"));
	SetReadOnly(true);
}

wxGxTableView::~wxGxTableView(void)
{
}

bool wxGxTableView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
	m_nParentGxObjectID = wxNOT_FOUND;
	m_sViewName = wxString(_("Table View"));
    wxGISTableView::Create(parent, TABLECTRLID, pos, size, style, name);
	SetReadOnly(true);
    return true;
}

bool wxGxTableView::Activate(IApplication* const pApplication, wxXmlNode* const pConf)
{
	if(!wxGxView::Activate(pApplication, pConf))
		return false;
	//Serialize(m_pXmlConf, false);

    wxGxApplication* pApp = dynamic_cast<wxGxApplication*>(pApplication);
    if(!pApp)
        return false;

    m_pSelection = pApp->GetGxSelection();

        if(!GetGxCatalog())
		return false;
    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);

    m_ConnectionPointSelectionCookie = m_pSelection->Advise(this);

	return true;
}

void wxGxTableView::Deactivate(void)
{
	if(m_ConnectionPointSelectionCookie != wxNOT_FOUND)
        m_pSelection->Unadvise(m_ConnectionPointSelectionCookie);
	//Serialize(m_pXmlConf, true);
	wxGxView::Deactivate();
}

bool wxGxTableView::Applies(wxGxSelection* const Selection)
{
	if(Selection == NULL)
		return false;

	for(size_t i = 0; i < Selection->GetCount(); ++i)
	{
        wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(Selection->GetSelectedObjectId(i));
		wxGxDataset* pGxDataset = wxDynamicCast(pGxObject, wxGxDataset);
		if(pGxDataset != NULL)
		{
			wxGISEnumDatasetType type = pGxDataset->GetType();
			switch(type)
			{
			case enumGISFeatureDataset:
			case enumGISTableDataset:
				return true;
			case enumGISRasterDataset:
			case enumGISContainer:
				break;
			}
		}
	}
	return false;
}

void wxGxTableView::OnSelectionChanged(wxGxSelectionEvent& event)
{
	wxCHECK_RET(event.GetSelection(), wxT("the selection pointer is NULL"));

    if(!Applies(event.GetSelection()))
        return;

    if(!IsShown())
        return;

    long nLastSelID = event.GetSelection()->GetLastSelectedObjectId();
	if(m_nParentGxObjectID == nLastSelID)
    {
        wxGISGridTable* pTable = wxDynamicCast( GetTable(), wxGISGridTable);
        if(pTable)
        {
            wxGISTable* pDs = pTable->GetDataset();
            if(NULL != pDs && pDs->IsCaching())
            {
                pDs->StopCaching();
                wsDELETE(pDs)
            }
        }
        return;
    }

    LoadData(nLastSelID);

    m_pEncodingsCombo->SetStringSelection(wxFontMapper::GetEncodingDescription(wxFONTENCODING_DEFAULT));
}

void wxGxTableView::LoadData(long nGxObjectId)
{
    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(nGxObjectId);
	wxGxDataset* pGxDataset = wxDynamicCast(pGxObject, wxGxDataset);
	if(pGxDataset == NULL)
		return;

    wxBusyCursor wait;
	wxGISDataset* pwxGISDataset = pGxDataset->GetDataset(false);
    wxGISTable* pGISTable = wxDynamicCast(pwxGISDataset, wxGISTable);

	if(pGISTable == NULL)
		return;

	m_nParentGxObjectID = pGxObject->GetId();

    if (!pGISTable->IsOpened())
    {
        pGISTable->Open(0, TRUE, false);
    }
    if(pGISTable->IsCaching())
        pGISTable->StopCaching();
	wxGISGridTable* pTable = new wxGISGridTable(pwxGISDataset);
	wxGISTableView::SetTable(pTable, true);

	wxWindow::Refresh();
}

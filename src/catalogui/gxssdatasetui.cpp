/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Spreadsheet object ui classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
*   Copyright (C) 2014 NextGIS
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
#include "wxgis/catalogui/gxssdatasetui.h"

#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/datasource/featuredataset.h"

//propertypages
#include "wxgis/catalogui/spatrefpropertypage.h"
#include "wxgis/catalogui/vectorpropertypage.h"

#include "../../art/properties.xpm"

#include "wx/propdlg.h"
#include "wx/bookctrl.h"

//--------------------------------------------------------------
//class wxGxKMLDatasetUI
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxSpreadsheetDatasetUI, wxGxSpreadsheetDataset)

wxGxSpreadsheetDatasetUI::wxGxSpreadsheetDatasetUI(wxGISEnumTableType eType, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon, const wxIcon &SubLargeIcon, const wxIcon &SubSmallIcon) : wxGxSpreadsheetDataset(eType, oParent, soName, soPath), wxGISThreadHelper()
{
    m_nPendUId = wxNOT_FOUND;
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
    m_LargeSubIcon = SubLargeIcon;
    m_SmallSubIcon = SubSmallIcon;
}

wxGxSpreadsheetDatasetUI::~wxGxSpreadsheetDatasetUI(void)
{
}

wxIcon wxGxSpreadsheetDatasetUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxSpreadsheetDatasetUI::GetSmallImage(void)
{
	return m_SmallIcon;
}

void wxGxSpreadsheetDatasetUI::EditProperties(wxWindow *parent)
{
    wxPropertySheetDialog PropertySheetDialog;
    if (!PropertySheetDialog.Create(parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxSize( 480,640 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    PropertySheetDialog.SetIcon(properties_xpm);
    PropertySheetDialog.CreateButtons(wxOK);
    wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());

    wxGISVectorPropertyPage* VectorPropertyPage = new wxGISVectorPropertyPage(this, pParentWnd);
    PropertySheetDialog.GetBookCtrl()->AddPage(VectorPropertyPage, VectorPropertyPage->GetPageName());

    //PropertySheetDialog.LayoutDialog();
    PropertySheetDialog.SetSize(480,640);
    PropertySheetDialog.Center();

    PropertySheetDialog.ShowModal();
}

bool wxGxSpreadsheetDatasetUI::HasChildren(bool bWaitLoading)
{
	if(bWaitLoading)
	{
		LoadChildren();
	}
	else
	{
		CreateAndRunThread();
	}
	
    return wxGxObjectContainer::HasChildren(bWaitLoading);
}

void wxGxSpreadsheetDatasetUI::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

	wxGISDataset* pDSet = GetDataset(false);
    wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
    if(pDSet)
    {
        for(size_t i = 0; i < pDSet->GetSubsetsCount(); ++i)
        {
            wxGISDataset* pwxGISSuDataset = m_pwxGISDataset->GetSubset(i);
            wxString sSubsetName = pwxGISSuDataset->GetName();
            wxGxSpreadsheetSubDatasetUI* pGxSpreadsheetSubDatasetUI = new wxGxSpreadsheetSubDatasetUI((wxGISEnumTableType)GetSubType(), pwxGISSuDataset, wxStaticCast(this, wxGxObject), sSubsetName, wxGxObjectContainer::GetPath(), m_LargeSubIcon, m_SmallSubIcon);
            wxGIS_GXCATALOG_EVENT_ID(ObjectAdded, pGxSpreadsheetSubDatasetUI->GetId());
	    }

        wsDELETE(pDSet);
    }

	m_bIsChildrenLoaded = true;
}

wxThread::ExitCode wxGxSpreadsheetDatasetUI::Entry()
{
	wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
	if(pCat)
	{
		m_nPendUId = pCat->AddPending(GetId());
	}	
	
	LoadChildren();

    if(m_nPendUId != wxNOT_FOUND && pCat)
    {
        pCat->RemovePending(m_nPendUId);
        m_nPendUId = wxNOT_FOUND;
    }


    //wxGIS_GXCATALOG_EVENT(ObjectChanged);

    return (wxThread::ExitCode)wxTHREAD_NO_ERROR;
}

wxGISDataset* const wxGxSpreadsheetDatasetUI::GetDataset(bool bCache, ITrackCancel* const pTrackCancel)
{
    wxGISTable* pwxGISTable = wxDynamicCast(GetDatasetFast(), wxGISTable);

    if(pwxGISTable && !pwxGISTable->IsOpened())
    {
        if (!pwxGISTable->Open(0, true, true, bCache, pTrackCancel))
        {
			wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Open"));
			wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
			
            wsDELETE(pwxGISTable);
			return NULL;
        }
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
	}

	wsGET( m_pwxGISDataset );
}

//--------------------------------------------------------------
//class wxGxSpreadsheetSubDatasetUI
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxSpreadsheetSubDatasetUI, wxGxSpreadsheetSubDataset)

wxGxSpreadsheetSubDatasetUI::wxGxSpreadsheetSubDatasetUI(wxGISEnumTableType nType, wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon) : wxGxSpreadsheetSubDataset(nType, pwxGISDataset, oParent, soName, soPath)
{
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
}

wxGxSpreadsheetSubDatasetUI::~wxGxSpreadsheetSubDatasetUI(void)
{
}

wxIcon wxGxSpreadsheetSubDatasetUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxSpreadsheetSubDatasetUI::GetSmallImage(void)
{
	return m_SmallIcon;
}

void wxGxSpreadsheetSubDatasetUI::EditProperties(wxWindow *parent)
{
    wxPropertySheetDialog PropertySheetDialog;
    if (!PropertySheetDialog.Create(parent, wxID_ANY, _("Properties"), wxDefaultPosition, wxSize( 480,640 ), wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER))
        return;
    PropertySheetDialog.SetIcon(properties_xpm);
    PropertySheetDialog.CreateButtons(wxOK);
    wxWindow* pParentWnd = static_cast<wxWindow*>(PropertySheetDialog.GetBookCtrl());

    wxGISVectorPropertyPage* VectorPropertyPage = new wxGISVectorPropertyPage(this, pParentWnd);
    PropertySheetDialog.GetBookCtrl()->AddPage(VectorPropertyPage, VectorPropertyPage->GetPageName());

    //PropertySheetDialog.LayoutDialog();
    PropertySheetDialog.SetSize(480,640);
    PropertySheetDialog.Center();

    PropertySheetDialog.ShowModal();
}

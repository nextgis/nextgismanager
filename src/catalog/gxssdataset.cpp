/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Spreadsheet Dataset classes.
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
#include "wxgis/catalog/gxssdataset.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/core/app.h"
//--------------------------------------------------------------
// class wxGxSpreadsheetDataset
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxSpreadsheetDataset, wxGxDatasetContainer)

wxGxSpreadsheetDataset::wxGxSpreadsheetDataset(wxGISEnumTableType eType, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxDatasetContainer(oParent, soName, soPath)
{
    m_bIsChildrenLoaded = false;
    m_pwxGISDataset = NULL;
    m_eType = eType;
}

wxGxSpreadsheetDataset::~wxGxSpreadsheetDataset(void)
{
}

wxString wxGxSpreadsheetDataset::GetCategory(void) const
{
    switch(GetSubType())
    {
    case enumTableODS:
        return wxString(_("LibreOffice Spreadsheet"));
    case enumTableXLS:
    case enumTableXLSX:
        return wxString(_("Microsoft Office Excel"));
 	default:
		return wxEmptyString;
   }
}

wxGISEnumDatasetType wxGxSpreadsheetDataset::GetType(void) const
{
    switch(GetSubType())
    {
    case enumTableODS:
    case enumTableXLS:
    case enumTableXLSX:
        return enumGISContainer;
 	default:
		return enumGISFeatureDataset;
   }
}

wxGISDataset* const wxGxSpreadsheetDataset::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        switch (GetSubType())
        {
        case enumTableODS:
        case enumTableXLS:
        case enumTableXLSX:
        default:
            {
                wxGISTable* pDSet = new wxGISTable(m_sPath, m_eType);
                m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
                m_pwxGISDataset->Reference();
                break;
            }
        }
    }
    wsGET(m_pwxGISDataset);
}


wxGISDataset* const wxGxSpreadsheetDataset::GetDataset(bool bCache, ITrackCancel* const pTrackCancel)
{
    wxGISTable* pwxGISTable = wxDynamicCast(GetDatasetFast(), wxGISTable);

    if(NULL != pwxGISTable && !pwxGISTable->IsOpened())
    {
        if (!pwxGISTable->Open(0, true, true, bCache, pTrackCancel))
        {
			wxString sErr = wxString::Format(_("Operation '%s' failed!"), _("Open"));
			wxGISLogError(sErr, wxString::FromUTF8(CPLGetLastErrorMsg()), wxEmptyString, pTrackCancel);
            wsDELETE(pwxGISTable);			
			return NULL;
        }
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
        wsDELETE(pwxGISTable);
	}

    wsGET(m_pwxGISDataset);
}

bool wxGxSpreadsheetDataset::HasChildren(bool bWaitLoading)
{
    LoadChildren();
    return wxGxObjectContainer::HasChildren(bWaitLoading);
}

bool wxGxSpreadsheetDataset::DestroyChildren()
{
	m_bIsChildrenLoaded = false;
    return wxGxObjectContainer::DestroyChildren();
}

void wxGxSpreadsheetDataset::Refresh(void)
{
    DestroyChildren();
    LoadChildren();
    wxGxObject::Refresh();
}

void wxGxSpreadsheetDataset::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

	wxGISDataset* pDSet = GetDataset(false);
    if(pDSet)
    {
        for(size_t i = 0; i < pDSet->GetSubsetsCount(); ++i)
        {
            wxGISDataset* pwxGISSuDataset = m_pwxGISDataset->GetSubset(i);
            new wxGxSpreadsheetSubDataset((wxGISEnumTableType)GetSubType(), pwxGISSuDataset, wxStaticCast(this, wxGxObject), pwxGISSuDataset->GetName(), wxGxObjectContainer::GetPath());
	    }
        wsDELETE(pDSet);
    }
	m_bIsChildrenLoaded = true;
}

//--------------------------------------------------------------
//class wxGxSpreadsheetSubDataset
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxSpreadsheetSubDataset, wxGxTable)

wxGxSpreadsheetSubDataset::wxGxSpreadsheetSubDataset(wxGISEnumTableType nType, wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxTable(nType, oParent, soName, soPath)
{
    wsSET(m_pwxGISDataset, pwxGISDataset);

    m_sPath = CPLString(CPLFormFilename(soPath, soName.mb_str(wxConvUTF8), ""));
}

wxGxSpreadsheetSubDataset::~wxGxSpreadsheetSubDataset(void)
{
    wsDELETE(m_pwxGISDataset);
}

wxString wxGxSpreadsheetSubDataset::GetCategory(void) const
{
    switch (m_eType)
    {
    case enumTableODS:
    case enumTableXLS:
    case enumTableXLSX:
	return wxString(_("Spreadsheet page"));
    default:
        return wxEmptyString;
    }
}



/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  Markup language Dataset classes.
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
#include "wxgis/catalog/gxmldataset.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/sysop.h"
#include "wxgis/catalog/gxcatalog.h"

//--------------------------------------------------------------
// class wxGxMLDataset
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxMLDataset, wxGxDatasetContainer)

wxGxMLDataset::wxGxMLDataset(wxGISEnumVectorDatasetType eType, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxDatasetContainer(oParent, soName, soPath)
{
    m_bIsChildrenLoaded = false;
    m_pwxGISDataset = NULL;
    m_eType = eType;
}

wxGxMLDataset::~wxGxMLDataset(void)
{
}

wxString wxGxMLDataset::GetCategory(void) const
{
    switch(GetSubType())
    {
    case enumVecKML:
    case enumVecKMZ:
        return wxString(_("KML Dataset"));
    case enumVecSXF:
        return wxString(_("SXF Dataset"));
    case enumVecGML:
        return wxString(_("GML Dataset"));
 	default:
		return wxEmptyString;
   }
}

wxGISEnumDatasetType wxGxMLDataset::GetType(void) const
{
    switch(GetSubType())
    {
    case enumVecKML:
    case enumVecKMZ:
    case enumVecSXF:
    case enumVecGML:
        return enumGISContainer;
 	default:
		return enumGISFeatureDataset;
   }
}

wxGISDataset* const wxGxMLDataset::GetDatasetFast(void)
{
 	if(m_pwxGISDataset == NULL)
    {
        switch (GetSubType())
        {
        case enumVecKML:
        case enumVecKMZ:
        case enumVecGML:
        {
            wxGISFeatureDatasetCached* pDSet = new wxGISFeatureDatasetCached(m_sPath, m_eType);
            m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
            m_pwxGISDataset->Reference();
            break;
        }
        case enumVecSXF:
        default:
        {
            wxGISFeatureDataset* pDSet = new wxGISFeatureDataset(m_sPath, m_eType);
            m_pwxGISDataset = wxStaticCast(pDSet, wxGISDataset);
            m_pwxGISDataset->Reference();
            break;
        }
        }
    }
    wsGET(m_pwxGISDataset);
}

wxGISDataset* const wxGxMLDataset::GetDataset(bool bCache, ITrackCancel* const pTrackCancel)
{
    wxGISFeatureDataset* pwxGISFeatureDataset = wxDynamicCast(GetDatasetFast(), wxGISFeatureDataset);

    if(NULL != pwxGISFeatureDataset && !pwxGISFeatureDataset->IsOpened())
    {
        if (!pwxGISFeatureDataset->Open(0, TRUE, bCache, pTrackCancel))
        {
            wsDELETE(pwxGISFeatureDataset);
		    const char* err = CPLGetLastErrorMsg();
			wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s"), _("Open"), wxString(err, wxConvUTF8).c_str());
            wxLogError(sErr);
			if(pTrackCancel)
				pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
			return NULL;
        }
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
        wsDELETE(pwxGISFeatureDataset);
	}

    wsGET(m_pwxGISDataset);
}

bool wxGxMLDataset::HasChildren()
{
    LoadChildren(); 
    return wxGxObjectContainer::HasChildren();
}

bool wxGxMLDataset::DestroyChildren()
{
	m_bIsChildrenLoaded = false;
    return wxGxObjectContainer::DestroyChildren();
}

void wxGxMLDataset::Refresh(void)
{
    DestroyChildren();
    LoadChildren();
    wxGxObject::Refresh();
}

void wxGxMLDataset::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

	wxGISDataset* pDSet = GetDataset(false);
    if(pDSet)
    {
        for(size_t i = 0; i < pDSet->GetSubsetsCount(); ++i)
        {
            wxGISDataset* pwxGISFeatureSuDataset = m_pwxGISDataset->GetSubset(i);
            new wxGxMLSubDataset((wxGISEnumVectorDatasetType)GetSubType(), pwxGISFeatureSuDataset, wxStaticCast(this, wxGxObject), pwxGISFeatureSuDataset->GetName(), wxGxObjectContainer::GetPath());
	    }
        wsDELETE(pDSet);
    }
	m_bIsChildrenLoaded = true;
}

//--------------------------------------------------------------
//class wxGxMLSubDataset
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxMLSubDataset, wxGxFeatureDataset)

wxGxMLSubDataset::wxGxMLSubDataset(wxGISEnumVectorDatasetType nType, wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxFeatureDataset(nType, oParent, soName, soPath)
{
    wsSET(m_pwxGISDataset, pwxGISDataset);

    m_sPath = CPLString(CPLFormFilename(soPath, soName.mb_str(wxConvUTF8), ""));
}

wxGxMLSubDataset::~wxGxMLSubDataset(void)
{
    wsDELETE(m_pwxGISDataset);
}

wxString wxGxMLSubDataset::GetCategory(void) const
{
    switch (m_eType)
    {
    case enumVecSXF:
        return wxString(_("SXF Feature class"));
    case enumVecGML:
        return wxString(_("GML Feature class"));
    case enumVecKML:
    case enumVecKMZ:
	return wxString(_("KML Feature class"));
    default:
        return wxEmptyString;
    }
}



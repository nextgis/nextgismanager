/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxKMLDatasetUI classes.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2014 Dmitry Baryshnikov
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
#include "wxgis/catalogui/gxgnmui.h"

#ifdef wxGIS_USE_GDAL_GNM

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
IMPLEMENT_CLASS(wxGxGNMConnectivityUI, wxGxGNMConnectivity)

wxGxGNMConnectivityUI::wxGxGNMConnectivityUI(wxGISEnumVectorDatasetType eType, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon, const wxIcon &SubLargeIcon, const wxIcon &SubSmallIcon) : wxGxGNMConnectivity(eType, oParent, soName, soPath), wxGISThreadHelper()
{
    m_nPendUId = wxNOT_FOUND;
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
    m_LargeSubIcon = SubLargeIcon;
    m_SmallSubIcon = SubSmallIcon;
}

wxGxGNMConnectivityUI::~wxGxGNMConnectivityUI(void)
{
}

wxIcon wxGxGNMConnectivityUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxGNMConnectivityUI::GetSmallImage(void)
{
	return m_SmallIcon;
}


void wxGxGNMConnectivityUI::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    CreateAndRunThread();

	m_bIsChildrenLoaded = true;
}

wxThread::ExitCode wxGxGNMConnectivityUI::Entry()
{
    /*
    wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
    if(pCat)
    {
        m_nPendUId = pCat->AddPending(GetId());
    }    //ITrackCancel trackcancel;
	wxGISDataset* pDSet = GetDataset(false);
    wxGxCatalogUI* pCat = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);
    if(pDSet)
    {
        for(size_t i = 0; i < pDSet->GetSubsetsCount(); ++i)
        {
            wxGISDataset* pwxGISFeatureSuDataset = m_pwxGISDataset->GetSubset(i);
            wxString sSubsetName = pwxGISFeatureSuDataset->GetName();
            wxGxMLSubDatasetUI* pGxMLSubDatasetUI = new wxGxMLSubDatasetUI((wxGISEnumVectorDatasetType)GetSubType(), pwxGISFeatureSuDataset, wxStaticCast(this, wxGxObject), sSubsetName, wxGxObjectContainer::GetPath(), m_LargeSubIcon, m_SmallSubIcon);
            wxGIS_GXCATALOG_EVENT_ID(ObjectAdded, pGxMLSubDatasetUI->GetId());
	    }

        wsDELETE(pDSet);
    }
    if(m_nPendUId != wxNOT_FOUND && pCat)
    {
        pCat->RemovePending(m_nPendUId);
        m_nPendUId = wxNOT_FOUND;
    }


    //wxGIS_GXCATALOG_EVENT(ObjectChanged);
    */
    return (wxThread::ExitCode)wxTHREAD_NO_ERROR;
}


/*
wxGISDataset* const wxGxGNMConnectivityUI::GetDataset(bool bCache, ITrackCancel* const pTrackCancel)
{
    wxGISFeatureDataset* pwxGISFeatureDataset = wxDynamicCast(GetDatasetFast(), wxGISFeatureDataset);

    if(pwxGISFeatureDataset && !pwxGISFeatureDataset->IsOpened())
    {
        if (!pwxGISFeatureDataset->Open(0, TRUE, bCache, pTrackCancel))
        {
		    const char* err = CPLGetLastErrorMsg();
			wxString sErr = wxString::Format(_("Operation '%s' failed! GDAL error: %s"), _("Open"), wxString(err, wxConvUTF8).c_str());
            wxLogError(sErr);
            if (pTrackCancel)
            {
				pTrackCancel->PutMessage(sErr, wxNOT_FOUND, enumGISMessageErr);
            }
            wsDELETE(pwxGISFeatureDataset);
			return NULL;
        }
        wxGIS_GXCATALOG_EVENT(ObjectChanged);
	}

	wsGET( m_pwxGISDataset );
}
*/

/*
//--------------------------------------------------------------
//class wxGxKMLSubDatasetUI
//--------------------------------------------------------------
IMPLEMENT_CLASS(wxGxMLSubDatasetUI, wxGxMLSubDataset)

wxGxMLSubDatasetUI::wxGxMLSubDatasetUI(wxGISEnumVectorDatasetType nType, wxGISDataset* pwxGISDataset, wxGxObject *oParent, const wxString &soName, const CPLString &soPath, const wxIcon &LargeIcon, const wxIcon &SmallIcon) : wxGxMLSubDataset(nType, pwxGISDataset, oParent, soName, soPath)
{
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
}

wxGxMLSubDatasetUI::~wxGxMLSubDatasetUI(void)
{
}

wxIcon wxGxMLSubDatasetUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxMLSubDatasetUI::GetSmallImage(void)
{
	return m_SmallIcon;
}

*/
#endif

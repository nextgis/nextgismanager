/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMapView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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
#include "wxgis/catalogui/gxmapview.h"
#include "wxgis/carto/featurelayer.h"
#include "wxgis/carto/rasterlayer.h"
#include "wxgis/framework/framework.h"
#include "wxgis/framework/application.h"
#include "wxgis/datasource/rasterop.h"

#include <wx/msgdlg.h>

//-------------------------------------------------------------------------
// wxGxMapView
//-------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxMapView, wxGISDrawingMapView)

BEGIN_EVENT_TABLE(wxGxMapView, wxGISDrawingMapView)
	EVT_LEFT_DOWN(wxGxMapView::OnMouseDown)
	EVT_MIDDLE_DOWN(wxGxMapView::OnMouseDown)
	EVT_RIGHT_DOWN(wxGxMapView::OnMouseDown)
	EVT_LEFT_UP(wxGxMapView::OnMouseUp)
	EVT_MIDDLE_UP(wxGxMapView::OnMouseUp)
	EVT_RIGHT_UP(wxGxMapView::OnMouseUp)
	EVT_LEFT_DCLICK(wxGxMapView::OnMouseDoubleClick)
	EVT_MIDDLE_DCLICK(wxGxMapView::OnMouseDoubleClick)
	EVT_RIGHT_DCLICK(wxGxMapView::OnMouseDoubleClick)
	EVT_MOTION(wxGxMapView::OnMouseMove)
	EVT_GXSELECTION_CHANGED(wxGxMapView::OnSelectionChanged)
END_EVENT_TABLE()

wxGxMapView::wxGxMapView(void) : wxGISDrawingMapView(), wxGxView()
{
    m_nPanCmdId = wxNOT_FOUND;
    m_nParentGxObjectID = wxNOT_FOUND;
    m_ConnectionPointSelectionCookie = m_ConnectionPointCatalogCookie = wxNOT_FOUND;
}

wxGxMapView::wxGxMapView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISDrawingMapView(parent, id, pos, size), m_pStatusBar(NULL)
{
    m_nPanCmdId = wxNOT_FOUND;
    m_nParentGxObjectID = wxNOT_FOUND;
	m_sViewName = wxString(_("Geography View"));
    m_ConnectionPointSelectionCookie = m_ConnectionPointCatalogCookie = wxNOT_FOUND;
}

wxGxMapView::~wxGxMapView(void)
{
}
    
bool wxGxMapView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_pStatusBar = NULL;
	m_sViewName = wxString(_("Geography View"));
    return wxGISMapView::Create(parent, MAPCTRLID, pos, size, style, name);
}

bool wxGxMapView::Activate(IApplication* const pApplication, wxXmlNode* const pConf)
{
	if(!wxGxView::Activate(pApplication, pConf))
		return false;
	//Serialize(m_pXmlConf, false);

    m_pApp = dynamic_cast<wxGxApplication*>(pApplication);
    if(!m_pApp)
        return false;

    //get/store from/in config, set from property page
    wxGISAppConfig oConfig = GetConfig();
    if (oConfig.IsOk())
    {
        m_CFormat.Create(oConfig.Read(enumGISHKCU, m_pApp->GetAppName() + wxString(wxT("/statusbar/coord/format_mask")), wxT("X: dd.dddd[ ]Y: dd.dddd")));
    }
    else
    {
	    m_CFormat.Create(wxString(wxT("X: dd.dddd[ ]Y: dd.dddd")));
    }

    m_pSelection = m_pApp->GetGxSelection();

    if(!GetGxCatalog())
		return false;
    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalogUI);

    if(!m_pCatalog || !m_pSelection)
		return false;
    m_ConnectionPointSelectionCookie = m_pSelection->Advise(this);
    m_ConnectionPointCatalogCookie = m_pCatalog->Advise(this);

	m_pStatusBar = m_pApp->GetStatusBar();

	SetTrackCancel(new wxGxTrackCancel(m_pStatusBar));

    //find pan cmd
    wxGISCommand *pPanCmd = m_pApp->GetCommand(wxT("wxGISCartoMainTool"), 2);
    m_nPanCmdId = pPanCmd->GetID();
	return true;
}

void wxGxMapView::Deactivate(void)
{
	if(m_ConnectionPointCatalogCookie != wxNOT_FOUND)
        m_pCatalog->Unadvise(m_ConnectionPointCatalogCookie);
	if(m_ConnectionPointSelectionCookie != wxNOT_FOUND)
        m_pSelection->Unadvise(m_ConnectionPointSelectionCookie);

    //Serialize(m_pXmlConf, true);
    DestroyDrawThread();
	wxDELETE(m_pTrackCancel);
	wxGxView::Deactivate();
}

bool wxGxMapView::Applies(wxGxSelection* const Selection)
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
			case enumGISRasterDataset:
			case enumGISFeatureDataset:
			case enumGISContainer:
				return true;
			case enumGISTableDataset:
				break;
			}
		}
 		wxGxDatasetContainer* pGxDatasetContainer = wxDynamicCast(pGxObject, wxGxDatasetContainer);
		if(pGxDatasetContainer != NULL)
		{
			wxGISEnumDatasetType type = pGxDatasetContainer->GetType();
			switch(type)
			{
			case enumGISRasterDataset:
			case enumGISFeatureDataset:
			case enumGISContainer:
				return true;
			case enumGISTableDataset:
				break;
			}
		}
       
	}
	return false;
}

void wxGxMapView::OnSelectionChanged(wxGxSelectionEvent& event)
{
	wxCHECK_RET(event.GetSelection(), wxT("the selection pointer is NULL"));
    
    if(!Applies(event.GetSelection()))
        return;

    if(!IsShown())
        return;

    long nLastSelID = event.GetSelection()->GetLastSelectedObjectId();
	if(m_nParentGxObjectID == nLastSelID)
    {
        //check if data is cached
        bool bIsCached = true;
        for(size_t i = 0; i < GetLayerCount(); ++i)
        {
            wxGISLayer* const pLayer = GetLayerByIndex(i);
            if(pLayer)
            {
                wxGISDataset* pDSet = pLayer->GetDataset();
                if(NULL != pDSet && !pDSet->IsCached() && !pDSet->IsCaching())
                {
                    wsDELETE(pDSet);
                    bIsCached = false;
                    break;
                }
            }
        }
        
        if(bIsCached)
            return;
    }
    else
    {
        for(size_t i = 0; i < GetLayerCount(); ++i)
        {
            wxGISLayer* const pLayer = GetLayerByIndex(i);
            if(pLayer)
            {
                wxGISDataset* pDSet = pLayer->GetDataset();
                if(NULL != pDSet && pDSet->IsCaching())
                {
                    pDSet->StopCaching();
                    wsDELETE(pDSet);
                }
            }
        }
    }
    	
    LoadData(nLastSelID);
}

wxGISLayer* wxGxMapView::GetLayerFromDataset(wxGxDataset* const pGxDataset)
{
	wxGISDataset* pwxGISDataset = pGxDataset->GetDataset(true, m_pTrackCancel);
	if(pwxGISDataset == NULL)
		return NULL;

	wxGISEnumDatasetType type = pwxGISDataset->GetType();
    wxGISLayer* pLayer(NULL);

	switch(type)
	{
	case enumGISFeatureDataset:
		{
			wxGISFeatureDataset* pGISFeatureDataset = wxDynamicCast(pwxGISDataset, wxGISFeatureDataset);
			if(!pGISFeatureDataset->IsOpened())
				pGISFeatureDataset->Open(0, 0, true, m_pTrackCancel);
			if(!pGISFeatureDataset->IsCached())
				pGISFeatureDataset->Cache(m_pTrackCancel);
			wxGISFeatureLayer* pGISFeatureLayer = new wxGISFeatureLayer(pwxGISDataset->GetName(), pwxGISDataset);
			pLayer = wxStaticCast(pGISFeatureLayer, wxGISLayer);
		}
		break;
	case enumGISRasterDataset:
		{
        //CheckOverviews(pwxGISDataset, pGxObject->GetName());
		//pwxGISLayers.push_back(new wxGISRasterLayer(pwxGISDataset));
        //pwxGISLayers[pwxGISLayers.size() - 1]->SetName(pwxGISDataset->GetName());
			wxGISRasterDataset* pGISRasterDataset = wxDynamicCast(pwxGISDataset, wxGISRasterDataset);
			if(!pGISRasterDataset->IsOpened())
				pGISRasterDataset->Open(true);
			//if(!pGISRasterDataset->IsCached())
			//	pGISRasterDataset->Cache(m_pTrackCancel);
            wxGISRasterLayer* pGISRasterLayer = new wxGISRasterLayer(pwxGISDataset->GetName(), pwxGISDataset);
			pLayer = wxStaticCast(pGISRasterLayer, wxGISLayer);
		}
		break;
	default:
		break;
	}
    wsDELETE(pwxGISDataset);
    return pLayer;
}

void wxGxMapView::LoadLayer(wxGxDataset* const pGxDataset)
{
    wxCHECK_RET(pGxDataset, wxT("Input wxGxDataset pointer is NULL"));

    wxGISLayer* pLayer = GetLayerFromDataset(pGxDataset);

    Clear();

    if(pLayer && pLayer->IsValid())
    {
        AddLayer(pLayer);
    }
    else
    {
        wxDELETE(pLayer);
    }
}

void wxGxMapView::LoadLayers(wxGxDatasetContainer* const pGxDataset)
{
    wxCHECK_RET(pGxDataset, wxT("Input wxGxDataset pointer is NULL"));
    
    wxVector<wxGISLayer*> paLayers;
    wxBusyCursor wait;
    if(pGxDataset->HasChildren())
    {
	    wxGxObjectList ObjectList = pGxDataset->GetChildren();
        wxGxObjectList::iterator iter;
        for (iter = ObjectList.begin(); iter != ObjectList.end(); ++iter)
        {
            wxGxObject *current = *iter;
            if(current && current->IsKindOf(wxCLASSINFO(wxGxDataset)))
            {
                wxGISLayer* pLayer = GetLayerFromDataset(wxDynamicCast(current, wxGxDataset));
                if(pLayer)
                {
                    if(pLayer->IsValid())
                    {
                        paLayers.push_back(pLayer);
                    }
                    else
                    {
                        wxDELETE(pLayer);
                    }
                }
            }
        }
    }

    Clear();

    for(size_t i = 0; i < paLayers.size(); ++i)
    {
        if(paLayers[i]->IsValid())
        {
            AddLayer(paLayers[i]);
        }
        else
        {
            wxDELETE(paLayers[i]);
        }
    }
}

void wxGxMapView::LoadData(long nGxObjectId)
{
    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(nGxObjectId);
    if (NULL == pGxObject)
        return;

	if(m_pStatusBar)
    {
        IProgressor* pProgressor = m_pStatusBar->GetProgressor();
        //pProgressor->SetYield(true);
		m_pTrackCancel->SetProgressor(pProgressor);
    }
	m_pTrackCancel->Reset();


    if(pGxObject->IsKindOf(wxCLASSINFO(wxGxDataset)))
    {
        LoadLayer(wxDynamicCast(pGxObject, wxGxDataset));
    }
    else if(pGxObject->IsKindOf(wxCLASSINFO(wxGxDatasetContainer)))
    {
        LoadLayers(wxDynamicCast(pGxObject, wxGxDatasetContainer));
    }
    else
        return;

    m_nParentGxObjectID = pGxObject->GetId();    

    SetFullExtent();

	if(m_pStatusBar)
		m_pTrackCancel->SetProgressor(m_pStatusBar->GetAnimation());
}

void wxGxMapView::OnMouseMove(wxMouseEvent& event)
{
	if(m_pGISDisplay)
	{
		double dX(event.m_x), dY(event.m_y);
		m_pGISDisplay->DC2World(&dX, &dY);
        int nPanelPos = m_pStatusBar->GetPanelPos(enumGISStatusPosition);
		//m_pStatusBar->SetMessage(wxString::Format(_("X: %.4f  Y: %.4f"), dX, dY), nPanePos);
		m_pStatusBar->SetMessage(m_CFormat.Format(dX, dY), nPanelPos);
	}

	if(m_pApp)
		m_pApp->OnMouseMove(event);
	event.Skip();
}

void wxGxMapView::OnMouseDown(wxMouseEvent& event)
{
	if(m_pApp)
		m_pApp->OnMouseDown(event);
	event.Skip();
}

void wxGxMapView::OnMouseUp(wxMouseEvent& event)
{
	if(m_pApp)
		m_pApp->OnMouseUp(event);
	event.Skip();
}

void wxGxMapView::OnMouseDoubleClick(wxMouseEvent& event)
{
	if(m_pApp)
		m_pApp->OnMouseDoubleClick(event);
	event.Skip();
}


void wxGxMapView::OnShow(bool bShow)
{
    if (bShow && wxNOT_FOUND != m_nPanCmdId)
    {
        wxCommandEvent event(wxEVT_MENU, m_nPanCmdId);
        m_pApp->ProcessWindowEvent(event);
    }
}

//typedef struct OvrProgressData
//{
//    IStatusBar* pStatusBar;
//    IProgressor* pProgressor;
//    wxString sMessage;
//} *LPOVRPROGRESSDATA;
//
//int CPL_STDCALL OvrProgress( double dfComplete, const char *pszMessage, void *pData)
//{
//    LPOVRPROGRESSDATA pOvrData = (LPOVRPROGRESSDATA)pData;
//    if( pszMessage != NULL )
//        pOvrData->pStatusBar->SetMessage(wgMB2WX(pszMessage));
//    else if( pOvrData != NULL && !pOvrData->sMessage.IsEmpty() )
//        pOvrData->pStatusBar->SetMessage(pOvrData->sMessage);
//    else
//        pOvrData->pStatusBar->SetMessage(_("Building overviews"));
//
//    if(pOvrData->pProgressor)
//        pOvrData->pProgressor->SetValue((int) (dfComplete*100));
//
//    if(wxGetKeyState(WXK_SHIFT) || wxGetKeyState(WXK_ALT) || wxGetKeyState(WXK_CONTROL))
//        return 1;
//
//    bool bKeyState = wxGetKeyState(WXK_ESCAPE);
//    return bKeyState == true ? 0 : 1;
//}
/*
void wxGxMapView::CheckOverviews(wxGISDatasetSPtr pwxGISDataset, wxString soFileName)
{
    wxGISRasterDatasetSPtr pwxGISRasterDataset = boost::dynamic_pointer_cast<wxGISRasterDataset>(pwxGISDataset);
    if(!pwxGISRasterDataset)
        return;
   //pyramids
    if(!pwxGISRasterDataset->HasOverviews())
    {
    	bool bAskCreateOvr = true;
		wxGISAppConfigSPtr pConfig = GetConfig();
		if(!pConfig)
			return false;

        bool bCreateOverviews = true;
        wxString sResampleMethod(wxT("GAUSS"));
        if(pConfig)
        {
            wxString sCompress;
            wxXmlNode* pNode = pConfig->GetConfigNode(enumGISHKCU, wxString(wxT("catalog/raster")));
            if(pNode)
            {
                bAskCreateOvr = wxAtoi(pNode->GetAttribute(wxT("ask_create_ovr"), wxT("1")));
                sCompress = pNode->GetAttribute(wxT("ovr_compress"), wxT("NONE"));
                sResampleMethod = pNode->GetAttribute(wxT("ovr_resample"), wxT("GAUSS"));
                //"NEAREST", "GAUSS", "CUBIC", "AVERAGE", "MODE", "AVERAGE_MAGPHASE" or "NONE"
                bCreateOverviews = wxAtoi(pNode->GetAttribute(wxT("create_ovr"), wxT("1")));
            }
            else
            {
                pNode = pConfig->CreateConfigNode(enumGISHKCU, wxString(wxT("catalog/raster")), true);
                pNode->AddAttribute(wxT("create_ovr"), wxT("1"));
                pNode->AddAttribute(wxT("ask_create_ovr"), wxT("1"));
                pNode->AddAttribute(wxT("ovr_compress"), wxT("NONE"));
                pNode->AddAttribute(wxT("ovr_resample"), wxT("NONE"));//wxT("GAUSS"));
            }
            CPLSetConfigOption( "COMPRESS_OVERVIEW", wgWX2MB(sCompress) );//LZW "DEFLATE" NONE
            if(bAskCreateOvr)
            {
                //show ask dialog
                wxGISMessageDlg dlg(this->GetParent()->GetParent(), wxID_ANY, wxString::Format(_("Create pyramids for %s (%d x %d)"), soFileName.c_str(), pwxGISRasterDataset->GetWidth(), pwxGISRasterDataset->GetHeight()), wxString(_("This raster datasource does not have pyramids. Pyramids allow rapid display at different resolutions.")), wxString(_("Pyramids building may take few moments.\nWould you like to create pyramids?")), wxDefaultPosition, wxSize( 400,160 ));

                if(dlg.ShowModal() == wxID_NO)
                    bCreateOverviews = false;
                else
                    bCreateOverviews = true;

                if(!dlg.GetShowInFuture())
                {
                    pNode->DeleteAttribute(wxT("ask_create_ovr"));
                    pNode->AddAttribute(wxT("ask_create_ovr"), wxT("0"));
                    pNode->DeleteAttribute(wxT("create_ovr"));
                    pNode->AddAttribute(wxT("create_ovr"), wxString::Format(wxT("%d"), bCreateOverviews));
                }
            }
        }

        if(bCreateOverviews)
        {
            int anOverviewList[25] = {0};
            int nLevelCount = GetOverviewLevels(pwxGISRasterDataset, anOverviewList);

            wxString sProgressMsg = wxString::Format(_("Creating pyramids for : %s (%d bands)"), soFileName.c_str(), pwxGISRasterDataset->GetRaster()->GetRasterCount());
            IStatusBar* pStatusBar = m_pApp->GetStatusBar();
            IProgressor* pProgressor = pStatusBar->GetProgressor();
            if(pProgressor)
                pProgressor->Show(true);

            OvrProgressData Data = {pStatusBar, pProgressor, sProgressMsg};
            GDALDataset* pDSet = pwxGISRasterDataset->GetRaster();
            if(!pDSet)
                return;

            if(pwxGISRasterDataset->GetSubType() == enumRasterImg)
            {
                CPLSetConfigOption( "USE_RRD", "YES" );
                CPLSetConfigOption( "HFA_USE_RRD", "YES" );
            }
            else
                CPLSetConfigOption( "USE_RRD", "NO" );

	        CPLErr eErr = pDSet->BuildOverviews( wgWX2MB(sResampleMethod), nLevelCount, anOverviewList, 0, NULL, OvrProgress, (void*)&Data );

            if(pProgressor)
                pProgressor->Show(false);
            pStatusBar->SetMessage(_("Done"));

		    if(eErr != CE_None)
		    {
                const char* pszErr = CPLGetLastErrorMsg();
                wxLogError(_("BuildOverviews failed! GDAL error: %s"), wgMB2WX(pszErr));
                wxMessageBox(_("Build Overviews failed!"), _("Error"), wxICON_ERROR | wxOK );
		    }
            else
                pwxGISRasterDataset->SetHasOverviews(true);
        }
    }
	
}
*/
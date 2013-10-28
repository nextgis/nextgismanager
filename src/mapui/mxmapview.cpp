/******************************************************************************
 * Project:  wxGIS (GIS Map)
 * Purpose:  wxMxMapView class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/mapui/mxmapview.h"
#include "wxgis/carto/featurelayer.h"
#include "wxgis/carto/rasterlayer.h"

BEGIN_EVENT_TABLE(wxMxMapView, wxGISMapView)
	EVT_LEFT_DOWN(wxMxMapView::OnMouseDown)
	EVT_MIDDLE_DOWN(wxMxMapView::OnMouseDown)
	EVT_RIGHT_DOWN(wxMxMapView::OnMouseDown)
	EVT_LEFT_UP(wxMxMapView::OnMouseUp)
	EVT_MIDDLE_UP(wxMxMapView::OnMouseUp)
	EVT_RIGHT_UP(wxMxMapView::OnMouseUp)
	EVT_LEFT_DCLICK(wxMxMapView::OnMouseDoubleClick)
	EVT_MIDDLE_DCLICK(wxMxMapView::OnMouseDoubleClick)
	EVT_RIGHT_DCLICK(wxMxMapView::OnMouseDoubleClick)
	EVT_MOTION(wxMxMapView::OnMouseMove)
END_EVENT_TABLE()

IMPLEMENT_DYNAMIC_CLASS(wxMxMapView, wxGISMapView)

wxMxMapView::wxMxMapView(void)
{
	m_pTrackCancel = NULL;
}

wxMxMapView::wxMxMapView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size) : wxGISMapView(parent, id, pos, size), m_pStatusBar(NULL)
{
	m_pTrackCancel = NULL;
}

wxMxMapView::~wxMxMapView(void)
{
}
    
bool wxMxMapView::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    m_pStatusBar = NULL;
    return wxGISMapView::Create(parent, id, pos, size, style, name);
}

bool wxMxMapView::Activate(IFrameApplication* application, wxXmlNode* pConf)
{
	//Serialize(m_pXmlConf, false);

    m_pApp = application;
    if(!m_pApp)
        return false;
	m_pStatusBar = m_pApp->GetStatusBar();

	m_pTrackCancel = new ITrackCancel();
	if(m_pStatusBar)
		m_pTrackCancel->SetProgressor(m_pStatusBar->GetAnimation());
	SetTrackCancel(m_pTrackCancel);

////tetst
//    Clear();
//
//	wxGISRasterDatasetSPtr pGISRasterDataset =  boost::make_shared<wxGISRasterDataset>("D:\\work\\bes.bmp", enumRasterBmp);
//	if(!pGISRasterDataset->IsOpened())
//		pGISRasterDataset->Open(false);
//	if(!pGISRasterDataset->IsCached())
//		pGISRasterDataset->Cache();
//	wxGISRasterLayerSPtr pGISRasterLayer = boost::make_shared<wxGISRasterLayer>(pGISRasterDataset);
//	AddLayer(boost::static_pointer_cast<wxGISLayer>(pGISRasterLayer));
//
//	//wxGISFeatureDatasetSPtr pGISFeatureDataset1 = boost::make_shared<wxGISFeatureDataset>("D:\\work\\Projects\\wxGIS\\gisdata\\continent.shp", enumVecESRIShapefile);
//	//if(!pGISFeatureDataset1->IsOpened())
//	//	pGISFeatureDataset1->Open(0, 0, true, m_pTrackCancel);
//	//if(!pGISFeatureDataset1->IsCached())
//	//	pGISFeatureDataset1->Cache();
//	//wxGISFeatureLayerSPtr pGISFeatureLayer1 = boost::make_shared<wxGISFeatureLayer>(pGISFeatureDataset1);
//	//AddLayer(boost::static_pointer_cast<wxGISLayer>(pGISFeatureLayer1));
//
//	//wxGISFeatureDatasetSPtr pGISFeatureDataset2 = boost::make_shared<wxGISFeatureDataset>("D:\\work\\Projects\\wxGIS\\gisdata\\continent.shp", enumVecESRIShapefile);
//	//if(!pGISFeatureDataset2->IsOpened())
//	//	pGISFeatureDataset2->Open(0, 0, true, m_pTrackCancel);
//	//if(!pGISFeatureDataset2->IsCached())
//	//	pGISFeatureDataset2->Cache();
//	//wxGISFeatureLayerSPtr pGISFeatureLayer2 = boost::make_shared<wxGISFeatureLayer>(pGISFeatureDataset2);
//	//AddLayer(boost::static_pointer_cast<wxGISLayer>(pGISFeatureLayer2));
//
//    SetFullExtent();
//
////end test
	return true;
}

void wxMxMapView::Deactivate(void)
{
	//Serialize(m_pXmlConf, true);
	//wxGxView::Deactivate();
	wxDELETE(m_pTrackCancel);
}

void wxMxMapView::OnMouseMove(wxMouseEvent& event)
{
	if(m_pGISDisplay)
	{
		double dX(event.m_x), dY(event.m_y);
		m_pGISDisplay->DC2World(&dX, &dY);
        int nPanelPos = m_pStatusBar->GetPanelPos(enumGISStatusPosition);
		m_pStatusBar->SetMessage(wxString::Format(_("X: %.4f  Y: %.4f"), dX, dY), nPanelPos);
	}

	if(m_pApp)
		m_pApp->OnMouseMove(event);
	event.Skip();
}

void wxMxMapView::OnMouseDown(wxMouseEvent& event)
{
	if(m_pApp)
		m_pApp->OnMouseDown(event);
	event.Skip();
}

void wxMxMapView::OnMouseUp(wxMouseEvent& event)
{
	if(m_pApp)
		m_pApp->OnMouseUp(event);
	event.Skip();
}

void wxMxMapView::OnMouseDoubleClick(wxMouseEvent& event)
{
	if(m_pApp)
		m_pApp->OnMouseDoubleClick(event);
	event.Skip();
}


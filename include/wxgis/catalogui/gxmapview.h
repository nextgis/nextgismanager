/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMapView class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2013 Bishop
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
#include "wxgis/catalogui/gxview.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/catalogui/gxcatalogui.h"
#include "wxgis/catalogui/gxeventui.h"
#include "wxgis/cartoui/drawingmap.h"
#include "wxgis/core/format.h"
#include "wxgis/framework/statusbar.h"
#include "wxgis/catalogui/gxapplication.h"

/** \class wxGxMapView gxmapview.h
 *  \brief The view for map showing.
 */

class wxGxTrackCancel :
    public ITrackCancel
{
public:
    wxGxTrackCancel(wxGISStatusBar* pStatusBar)
    {
        m_pStatusBar = pStatusBar;
    	if(m_pStatusBar)
        {
            IProgressor* pAni = m_pStatusBar->GetAnimation();
    		SetProgressor(pAni);
        }
    }
    virtual ~wxGxTrackCancel(void)
    {
    }
	virtual void PutMessage(const wxString &sMessage, size_t nIndex, wxGISEnumMessageType eType)
    {
    	if(m_pStatusBar && !sMessage.IsEmpty())
        {
            m_pStatusBar->SetMessage(sMessage);
        }
    }
protected:
    wxGISStatusBar* m_pStatusBar;
};


/** \class wxGxMapView gxmapview.h
 *  \brief The view for map showing.
 */

class WXDLLIMPEXP_GIS_CLU wxGxMapView :
    public wxGISDrawingMapView,
	public wxGxView
{
    DECLARE_DYNAMIC_CLASS(wxGxMapView)
public:
	wxGxMapView(void);
	wxGxMapView(wxWindow* parent, wxWindowID id = MAPCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize);
	virtual ~wxGxMapView(void);
    //virtual void CheckOverviews(wxGISDatasetSPtr pwxGISDataset, wxString soFileName);
    //events
	void OnMouseMove(wxMouseEvent& event);
	void OnMouseDown(wxMouseEvent& event);
	void OnMouseUp(wxMouseEvent& event);
	void OnMouseDoubleClick(wxMouseEvent& event);
//IGxView
    virtual bool Create(wxWindow* parent, wxWindowID id = MAPCTRLID, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL, const wxString& name = wxT("MapView"));
	virtual bool Activate(IApplication* const pApplication, wxXmlNode* const pConf);
	virtual void Deactivate(void);
	virtual bool Applies(wxGxSelection* const Selection);
    //wxGxView
    virtual void OnShow(bool bShow);
    //events
	virtual void OnSelectionChanged(wxGxSelectionEvent& event);
protected:
    virtual void LoadData(long nGxObjectId);
    virtual void LoadLayer(wxGxDataset* const pGxDataset);
    virtual void LoadLayers(wxGxDatasetContainer* const pGxDataset);
    virtual wxGISLayer* GetLayerFromDataset(wxGxDataset* const pGxDataset);
protected:
    long m_ConnectionPointCatalogCookie, m_ConnectionPointSelectionCookie;
	wxGxSelection* m_pSelection;
    wxGxCatalogUI* m_pCatalog;

    long m_nParentGxObjectID;

	wxGISStatusBar* m_pStatusBar;
	wxGxApplication* m_pApp;

    long m_nPanCmdId;

	wxGISCoordinatesFormat m_CFormat;
private:
	DECLARE_EVENT_TABLE()
};

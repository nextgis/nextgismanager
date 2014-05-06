/******************************************************************************
 * Project:  wxGIS
 * Purpose:  FeatureLayer header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013,2014 Bishop
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

#include "wxgis/carto/layer.h"
#include "wxgis/datasource/featuredataset.h"
#include "wxgis/datasource/vectorop.h"
#include "wxgis/carto/featurerenderer.h"

/** @class wxGISFeatureLayer
    
    The class represent vector datasource in map.

    @library{carto}
*/

class WXDLLIMPEXP_GIS_CRT wxGISFeatureLayer :
	public wxGISLayer
{
    DECLARE_CLASS(wxGISFeatureLayer)
public:
	wxGISFeatureLayer(const wxString &sName = _("new feature layer"), wxGISDataset* pwxGISDataset = NULL);
	virtual ~wxGISFeatureLayer(void);
//wxGISLayer
	virtual bool Draw(wxGISEnumDrawPhase DrawPhase, ITrackCancel* const pTrackCancel = NULL);
	virtual bool IsValid(void) const;
	virtual bool IsCacheNeeded(void) const;
	virtual wxGISEnumDatasetType GetType(void) const {return enumGISFeatureDataset;};
    virtual void SetSpatialReference(const wxGISSpatialReference &SpatialReference);
    virtual void SetRenderer(wxGISRenderer* pRenderer);
    virtual bool IsLoading() const;
//wxGISFeatureLayer
	virtual wxGISSpatialTreeCursor Idetify(const wxGISGeometry &Geom); 
    virtual wxGISSpatialTreeCursor SearchGeometry(const OGREnvelope &Env = OGREnvelope());
    virtual OGRwkbGeometryType GetGeometryType(void) const;
    virtual wxGISFeature GetFeatureByID (long nFID);
    //events
    void OnDSClosed(wxFeatureDSEvent& event);
    void OnDSFeaturesAdded(wxFeatureDSEvent& event);
    void OnDSFeatureAdded(wxFeatureDSEvent& event);
    void OnDSFeatureDeleted(wxFeatureDSEvent& event);
    void OnDSFeatureChanged(wxFeatureDSEvent& event);
protected:
    //virtual void LoadGeometry(void);
	virtual long GetPointsInGeometry(const wxGISGeometry& Geom) const;
protected:
    wxGISFeatureDataset* m_pwxGISFeatureDataset;
    wxGISFeatureRenderer* m_pFeatureRenderer;
    long m_nConnectionPointDSCookie;
	wxGISSpatialTree* m_pSpatialTree;
private:
	DECLARE_EVENT_TABLE()
};

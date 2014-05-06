/******************************************************************************
 * Project:  wxGIS
 * Purpose:  RasterLayer header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013 Bishop
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

#include "wxgis/carto/renderer.h"

#include <wx/event.h>

/** \class wxGISLayer layer.h
    \brief The base class for map layers.
*/

class WXDLLIMPEXP_GIS_CRT wxGISLayer :
	public wxEvtHandler,
    public wxGISConnectionPointContainer
{
    DECLARE_ABSTRACT_CLASS(wxGISLayer)
public:
    wxGISLayer(const wxString &sName = _("new layer"), wxGISDataset* pwxGISDataset = NULL);
	virtual ~wxGISLayer(void);
	virtual const wxGISSpatialReference GetSpatialReference(void);
	virtual OGREnvelope GetEnvelope(void) const;
	virtual void SetMaximumScale(double dMaxScale){m_dMaxScale = dMaxScale;};
	virtual double GetMaximumScale(void) const {return m_dMaxScale;};
	virtual void SetMinimumScale(double dMinScale){m_dMinScale = dMinScale;};
	virtual double GetMinimumScale(void) const {return m_dMinScale;};
	virtual bool GetVisible(void) const {return m_bVisible;};
	virtual void SetVisible(bool bVisible){m_bVisible = bVisible;};
	virtual void SetName(const wxString &sName){m_sName = sName;};
	virtual wxString GetName(void) const {return m_sName;};
	virtual bool IsCacheNeeded(void) const;
	virtual wxGISEnumDatasetType GetType(void) const {return enumGISAny;};
	virtual bool IsValid(void) const;
	virtual bool Draw(wxGISEnumDrawPhase DrawPhase, ITrackCancel* const pTrackCancel = NULL) = 0;
    virtual void SetRenderer(wxGISRenderer* pRenderer);
    virtual wxGISRenderer* GetRenderer(void);
	virtual size_t GetCacheId(void) const {return m_nCacheId;};
	virtual void SetCacheId(size_t nCacheId) {m_nCacheId = nCacheId;};
	virtual void SetSpatialReference(const wxGISSpatialReference &SpatialReference);
    virtual void SetDisplay(wxGISDisplay *pDisplay) { m_pDisplay = pDisplay; };
    virtual wxGISDataset* GetDataset() {wsGET(m_pwxGISDataset);};
    virtual bool IsLoading() const;
    virtual short GetId() const;
    virtual void SetId(short nNewId);
protected:
    wxGISDataset* m_pwxGISDataset;
    wxGISSpatialReference m_SpatialReference;
    wxGISDisplay *m_pDisplay;
    OGREnvelope m_FullEnvelope;
    double m_dMaxScale, m_dMinScale;
    bool m_bVisible;
    wxString m_sName;
    size_t m_nCacheId;
    short m_nId;
    //renderer
    wxGISRenderer* m_pRenderer;
};

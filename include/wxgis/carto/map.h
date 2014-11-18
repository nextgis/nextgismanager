/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISMap class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013 Dmitry Baryshnikov
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

#include "wxgis/carto/carto.h"
#include "wxgis/carto/layer.h"

/** @class wxGISMap

    The Map class - array of layers. This class stores array of layers.

    @library{carto}
 */
class WXDLLIMPEXP_GIS_CRT wxGISMap
{
public:
	wxGISMap(void);
	virtual ~wxGISMap(void);
	virtual void SetName(const wxString &sName) {m_sMapName = sName;};
	virtual wxString GetName(void) const {return m_sMapName;};
	virtual bool AddLayer(wxGISLayer* pLayer);
    virtual void ChangeLayerOrder(size_t nOldIndex, size_t nNewIndex);
	virtual void Clear(void);
	virtual size_t GetLayerCount(void) const {return m_paLayers.size();};
	virtual wxGISLayer* const GetLayerByIndex(size_t nIndex);
    virtual wxGISLayer* const GetLayerById(short nId);
    virtual wxString GetDescription(void) const { return m_sDescription; };
	virtual void SetDescription(const wxString &sDescription){m_sDescription = sDescription;};
	virtual OGREnvelope GetFullExtent(void) const;
	virtual void SetSpatialReference(const wxGISSpatialReference &SpatialReference);
	virtual wxGISSpatialReference GetSpatialReference(void) const;
    virtual bool HasLayerType(wxGISEnumDatasetType eType) const;
	virtual wxGISLayer* GetLayerFromDataset(wxGISDataset* const pDataset, ITrackCancel* const pTrackCancel = NULL);
protected:
	wxString m_sMapName, m_sDescription;
	wxVector<wxGISLayer*> m_paLayers;
	wxGISSpatialReference m_SpatialReference;
    wxGISDisplay *m_pGISDisplay; 
	OGREnvelope m_FullExtent;
	bool m_bFullExtIsInit;
    short m_nIdCounter;
	wxCriticalSection m_CritSect;
};

/** @class wxGISExtentStack

    The class keep history of map bounds changes.

    @library{carto}
 */
class WXDLLIMPEXP_GIS_CRT wxGISExtentStack : 
    public wxGISMap
{
public:
	wxGISExtentStack(void);
	virtual ~wxGISExtentStack(void);
	virtual bool CanRedo(void);
	virtual bool CanUndo(void);
	virtual void Redo(void);
	virtual void Undo(void);
	virtual void Clear(void);
	virtual size_t GetSize(void) const;
	virtual void Do(const OGREnvelope &Env);
	virtual OGREnvelope GetCurrentExtent(void) const;
	virtual void SetExtent(const OGREnvelope &Env);
protected:
	wxVector<OGREnvelope> m_staEnvelope;
	int m_nPos;
	OGREnvelope m_CurrentExtent;
};

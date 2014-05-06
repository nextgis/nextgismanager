/******************************************************************************
 * Project:  wxGIS
 * Purpose:  RasterLayer header.
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
#include "wxgis/datasource/rasterdataset.h"

/** @class wxGISRasterLayer

    The class represent raster datasource in map.

    @library{cartoui}
*/

class WXDLLIMPEXP_GIS_CRT wxGISRasterLayer :
	public wxGISLayer
{
    DECLARE_CLASS(wxGISRasterLayer)
public:
	wxGISRasterLayer(const wxString &sName = _("new raster layer"), wxGISDataset* pwxGISDataset = NULL);
	virtual ~wxGISRasterLayer(void);
//wxGISLayer
	virtual bool Draw(wxGISEnumDrawPhase DrawPhase, ITrackCancel * const pTrackCancel = NULL);
	virtual bool IsValid(void) const;
//wxGISRasterLayer
	//virtual IRasterRendererSPtr GetRenderer(void){return m_pRasterRenderer;};
	//virtual void SetRenderer(IRasterRendererSPtr pRasterRenderer){m_pRasterRenderer = pRasterRenderer;};
//protected:
//	virtual bool GetPixelData(RAWPIXELDATA &stPixelData, wxGISDisplay *pDisplay, ITrackCancel* const pTrackCancel = NULL);
//protected:
	//wxGISRasterDatasetSPtr m_pwxGISRasterDataset;

	//OGREnvelope m_FullEnvelope;
	//OGREnvelope m_PreviousEnvelope;
};

/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISMapBitmap class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013 Dmitry Baryshnikov
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
#include "wxgis/display/symbol.h"
#include "wxgis/carto/map.h"

/** @class wxGISMapBitmap
    
    The MapBitmap class draw layers to bitmap. It may used in export to bitmap, svg, pdf etc.

    @librarycartoui
*/

class WXDLLIMPEXP_GIS_CRT wxGISMapBitmap :
	public wxGISExtentStack
{
public:
    wxGISMapBitmap(int nWidth, int nHeight);
    virtual ~wxGISMapBitmap(void);
	virtual void SetTrackCancel(ITrackCancel* pTrackCancel);
	virtual wxGISDisplay* GetDisplay(void){return m_pGISDisplay;};
	//wxGISExtentStack
	virtual bool AddLayer(wxGISLayer* pLayer);
	virtual void Clear(void);
	virtual void SetSpatialReference(const wxGISSpatialReference &SpatialReference);
	virtual void SetExtent(const OGREnvelope& Env);
	virtual void SetFullExtent(void);
	virtual OGREnvelope GetFullExtent(void);
    virtual void SetRotate(double dfAngleRad);
    virtual double GetRotate(void) const;
    virtual void DrawGeometry(const wxGISGeometry &Geometry, wxGISSymbol* const pSymbol);
    //
    virtual bool SaveAsBitmap(const CPLString &szPath, wxGISEnumRasterDatasetType eType, char **papszOptions, bool bAddMetadata = true);
protected:
	ITrackCancel *m_pTrackCancel;
	double m_nFactor;
    double m_dfCurrentAngleRad;
    int m_nWidth, m_nHeight;
};

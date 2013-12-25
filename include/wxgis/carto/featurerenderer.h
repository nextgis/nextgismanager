/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISSimpleRenderer class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2013 Bishop
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
#pragma once

#include "wxgis/carto/renderer.h"
#include "wxgis/display/symbol.h"

class wxGISFeatureLayer;

/** \class wxGISSimpleRenderer featurerenderer.h
    \brief The vector layer renderer
*/

class WXDLLIMPEXP_GIS_CRT wxGISFeatureRenderer :
	public wxGISRenderer
{
    DECLARE_CLASS(wxGISFeatureRenderer)
public:
	wxGISFeatureRenderer(wxGISLayer* pwxGISLayer = NULL);
	virtual ~wxGISFeatureRenderer(void);
    virtual bool CanRender(wxGISLayer* const pwxGISLayer) const;
    virtual bool Draw(wxGISEnumDrawPhase DrawPhase, wxGISDisplay* const pDisplay, ITrackCancel* const pTrackCancel = NULL);

    virtual void SetSymbol(wxGISSymbol *pSymbol);
	virtual void Draw(const wxGISSpatialTreeCursor& Cursor, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel = NULL);
    virtual bool Apply(ITrackCancel* const pTrackCancel = NULL);
protected:
    wxGISFeatureLayer* m_pwxGISFeatureLayer;
	wxGISSymbol* m_pSymbol;
    wxCriticalSection m_CritSect;
};


//WX_DECLARE_STRING_HASH_MAP( wxGISSymbol*, wxGISUniqueValuesSymbolMap );
WX_DECLARE_HASH_MAP(long, wxGISSymbol*, wxIntegerHash, wxIntegerEqual, wxGISUniqueValuesSymbolMap);


/** \class wxGISSimpleRenderer featurerenderer.h
    \brief The vector layer renderer
*/

class WXDLLIMPEXP_GIS_CRT wxGISUniqueValueRenderer  :
	public wxGISFeatureRenderer
{
    DECLARE_CLASS(wxGISFeatureRenderer)
public:
	wxGISUniqueValueRenderer(wxGISLayer* pwxGISLayer = NULL);
	virtual ~wxGISUniqueValueRenderer(void);
    virtual void AddValue(int nField, const wxString &sValue, wxGISSymbol* const Symbol);
    virtual void SetSymbol(wxGISSymbol *pSymbol);
	virtual void Draw(const wxGISSpatialTreeCursor& Cursor, wxGISEnumDrawPhase DrawPhase, wxGISDisplay *pDisplay, ITrackCancel *pTrackCancel = NULL);
    virtual bool Apply(ITrackCancel* const pTrackCancel = NULL);

    typedef struct _uniq_value{
        wxString sField;
        wxString sValue;
        wxGISSymbol* Symbol;
    }UNIQ_VALUE;
protected:
    wxVector<UNIQ_VALUE> m_astUniqueValues;
    //wxArrayInt m_naCheckFields;
    wxGISUniqueValuesSymbolMap m_omSymbols;
    wxGISFeatureDataset *m_pwxGISFeatureDataset;
};

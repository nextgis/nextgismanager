/******************************************************************************
 * Project:  wxGIS
 * Purpose:  wxGISRenderer class.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013 Bishop
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

#include "wxgis/carto/carto.h"

class wxGISLayer;

/** \class wxGISRenderer renderer.h
    \brief The base class for renderers
*/

class WXDLLIMPEXP_GIS_CRT wxGISRenderer :
	public wxObject
{
    DECLARE_ABSTRACT_CLASS(wxGISRenderer)
public:
	wxGISRenderer(wxGISLayer* pwxGISLayer = NULL);
	virtual ~wxGISRenderer(void);
	virtual bool CanRender(wxGISLayer* const pwxGISLayer) const;
    virtual wxString GetName(void) const;
    virtual wxGISEnumRendererType GetType(void) const;
	virtual bool Draw(wxGISEnumDrawPhase DrawPhase, wxGISDisplay* const pDisplay, ITrackCancel* const pTrackCancel = NULL) = 0;
protected:
	wxGISLayer* const m_pwxGISLayer;
};

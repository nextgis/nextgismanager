/******************************************************************************
 * Project:  wxGIS
 * Purpose:  event UI classes special for MapView events.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011,2013 Bishop
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
#include "wxgis/carto/mxevent.h"

IMPLEMENT_DYNAMIC_CLASS(wxMxMapViewEvent, wxEvent)

wxDEFINE_EVENT( wxMXMAP_LAYER_CHANGED, wxMxMapViewEvent );
wxDEFINE_EVENT( wxMXMAP_LAYER_LOADING, wxMxMapViewEvent );
wxDEFINE_EVENT( wxMXMAP_LAYER_DS_CLOSED, wxMxMapViewEvent );


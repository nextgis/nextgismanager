/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  event classes special for GxCatalog & Selection events.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Dmitry Baryshnikov
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
#include "wxgis/catalog/gxevent.h"

IMPLEMENT_DYNAMIC_CLASS(wxGxCatalogEvent, wxEvent)

wxDEFINE_EVENT( wxGXOBJECT_ADDED, wxGxCatalogEvent );
wxDEFINE_EVENT( wxGXOBJECT_CHANGED, wxGxCatalogEvent );
wxDEFINE_EVENT( wxGXOBJECT_DELETED, wxGxCatalogEvent );
wxDEFINE_EVENT( wxGXOBJECT_REFRESHED, wxGxCatalogEvent );


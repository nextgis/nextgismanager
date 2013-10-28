/******************************************************************************
 * Project:  wxGIS
 * Purpose:  event classes special for Process events.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#include "wxgis/core/event.h"

IMPLEMENT_DYNAMIC_CLASS(wxGISProcessEvent, wxEvent)

wxDEFINE_EVENT( wxPROCESS_START, wxGISProcessEvent );
wxDEFINE_EVENT( wxPROCESS_FINISH, wxGISProcessEvent );
wxDEFINE_EVENT( wxPROCESS_CANCELED, wxGISProcessEvent );
wxDEFINE_EVENT( wxPROCESS_STATE_CHANGED, wxGISProcessEvent );

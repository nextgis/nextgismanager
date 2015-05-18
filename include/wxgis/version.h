/******************************************************************************
 * Project:  wxGIS
 * Purpose:  version functions
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "wx/version.h"

/*  NB: this file is parsed by automatic tools so don't change its format! */
#define wxGIS_MAJOR_VERSION      0
#define wxGIS_MINOR_VERSION      6
#define wxGIS_RELEASE_NUMBER     7
#define wxGIS_SUBRELEASE_NUMBER  0

/*  these are used by version.rc and should always be ASCII, not Unicode */
#define wxGIS_VERSION_NUM_STRING \
  wxMAKE_VERSION_STRING(wxGIS_MAJOR_VERSION, wxGIS_MINOR_VERSION, wxGIS_RELEASE_NUMBER)
#define wxGIS_VERSION_NUM_DOT_STRING \
  wxMAKE_VERSION_DOT_STRING(wxGIS_MAJOR_VERSION, wxGIS_MINOR_VERSION, wxGIS_RELEASE_NUMBER)

/* those are Unicode-friendly */
#define wxGIS_VERSION_NUM_STRING_T \
  wxMAKE_VERSION_STRING_T(wxGIS_MAJOR_VERSION, wxGIS_MINOR_VERSION, wxGIS_RELEASE_NUMBER)
#define wxGIS_VERSION_NUM_DOT_STRING_T \
  wxMAKE_VERSION_DOT_STRING_T(wxGIS_MAJOR_VERSION, wxGIS_MINOR_VERSION, wxGIS_RELEASE_NUMBER)

/*  check if the current version is at least major.minor.release */
#define wxGIS_CHECK_VERSION(major,minor,release) \
    (wxGIS_MAJOR_VERSION > (major) || \
    (wxGIS_MAJOR_VERSION == (major) && wxGIS_MINOR_VERSION > (minor)) || \
    (wxGIS_MAJOR_VERSION == (major) && wxGIS_MINOR_VERSION == (minor) && wxGIS_RELEASE_NUMBER >= (release)))

/* the same but check the subrelease also */
#define wxGIS_CHECK_VERSION_FULL(major,minor,release,subrel) \
    (wxCHECK_VERSION(major, minor, release) && \
        ((major) != wxGIS_MAJOR_VERSION || \
            (minor) != wxGIS_MINOR_VERSION || \
                (release) != wxGIS_RELEASE_NUMBER || \
                    (subrel) <= wxGIS_SUBRELEASE_NUMBER))



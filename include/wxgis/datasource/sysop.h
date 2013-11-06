/******************************************************************************
 * Project:  wxGIS
 * Purpose:  system operations.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011,2013 Bishop
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

#include "wxgis/datasource/datasource.h"
#include "wxgis/datasource/gdalinh.h"

WXDLLIMPEXP_GIS_DS bool DeleteDir(const CPLString &sPath, ITrackCancel* const pTrackCancel = NULL);
WXDLLIMPEXP_GIS_DS bool MoveDir(const CPLString &sPathFrom, const CPLString &sPathTo, long mode, ITrackCancel* const pTrackCancel = NULL);
WXDLLIMPEXP_GIS_DS bool CopyDir(const CPLString &sPathFrom, const CPLString &sPathTo, long mode, ITrackCancel* const pTrackCancel = NULL);
WXDLLIMPEXP_GIS_DS bool CreateDir(const CPLString &sPath, long mode = 0777, ITrackCancel* const pTrackCancel = NULL); 
WXDLLIMPEXP_GIS_DS bool DeleteFile(const CPLString &sPath, ITrackCancel* const pTrackCancel = NULL);
WXDLLIMPEXP_GIS_DS bool RenameFile(const CPLString &sOldPath, const CPLString &sNewPath, ITrackCancel* const pTrackCancel = NULL);
WXDLLIMPEXP_GIS_DS bool CopyFile(const CPLString &sDestPath, const CPLString &sSrcPath, ITrackCancel* const pTrackCance = NULL);
WXDLLIMPEXP_GIS_DS bool MoveFile(const CPLString &sDestPath, const CPLString &sSrcPath, ITrackCancel* const pTrackCancel = NULL);
WXDLLIMPEXP_GIS_DS wxFontEncoding GetEncodingFromCpg(const CPLString &sPath);
WXDLLIMPEXP_GIS_DS wxFontEncoding ToFontEncoding(const CPLString &soCodePage);
//WXDLLIMPEXP_GIS_DS CPLString GetEncodingName(wxFontEncoding eEncoding);
WXDLLIMPEXP_GIS_DS wxString ClearExt(const wxString &sPath);
WXDLLIMPEXP_GIS_DS bool IsFileHidden(const CPLString &sPath);
WXDLLIMPEXP_GIS_DS wxString CheckUniqName(const CPLString &sPath, const wxString &sName, const wxString &sExt, int nCounter = 0);
WXDLLIMPEXP_GIS_DS CPLString CheckUniqPath(const CPLString &sPath, const CPLString &sName, bool bIsFolder = false, const CPLString &sAdd = "_copy", int nCounter = 0);
WXDLLIMPEXP_GIS_DS CPLString GetUniqPath(const CPLString &szOriginalFullPath, const CPLString &szNewPath, const CPLString &szNewName);
WXDLLIMPEXP_GIS_DS CPLString Transliterate(const char* str);
WXDLLIMPEXP_GIS_DS CPLString GetExtension(const CPLString &sPath, const CPLString &sName = "");
WXDLLIMPEXP_GIS_DS wxString GetConvName(const CPLString &szPath, bool bIsPath = true);

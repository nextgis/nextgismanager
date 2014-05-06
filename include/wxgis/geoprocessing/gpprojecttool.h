/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  project geoprocessing tools.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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

#include "wxgis/geoprocessing/gptool.h"

/** \class wxGISGPExportTool gpexporttool.h
    \brief The geoprocessing tool export vector data to various formats.
*/
class WXDLLIMPEXP_GIS_GP wxGISGPProjectVectorTool : 
    public wxGISGPTool
{
   DECLARE_DYNAMIC_CLASS(wxGISGPProjectVectorTool)

public:
    wxGISGPProjectVectorTool(void);
    virtual ~wxGISGPProjectVectorTool(void);
    //IGPTool
    virtual const wxString GetDisplayName(void);
    virtual const wxString GetName(void);
    virtual const wxString GetCategory(void);
    virtual bool Execute(ITrackCancel* pTrackCancel);
    virtual bool Validate(void);
    virtual GPParameters GetParameterInfo(void);
};
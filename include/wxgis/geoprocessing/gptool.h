/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  base geoprocessing tool.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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

#include "wxgis/geoprocessing/geoprocessing.h"

/** @def GPTOOLSEPARATOR 
    The tool parameters separartor for serialize
*/
#define GPTOOLSEPARATOR wxT("|") 

int CPL_STDCALL ExecToolProgress( double dfComplete, const char *pszMessage, void *pData);

/** @class wxGISGPTool gptool.h
    
    The base class for geoprocessing tools. This class implements common functions.
    
    @library{wxgisgp}
*/

class WXDLLIMPEXP_GIS_GP wxGISGPTool : 
    public IGPTool,
    public wxObject
{
public:
    /** Constructor */
    wxGISGPTool(void);
    /** Destructor */
    virtual ~wxGISGPTool(void);
    //IGPTool
    /** Returns tool name for display in UI */
    virtual const wxString GetDisplayName(void) = 0;
    /** Returns uniq tool name for internal use */
    virtual const wxString GetName(void) = 0;
    /** Returns tool category name. This is used in add tool to the toolbox dialog */
    virtual const wxString GetCategory(void) = 0;
    /** 
        This function call from task manager than task is started.
        @param pTrackCancel
            The pointer to the ITrackCancel which get status meesages, done percent and 
            indicating, that tool should stop execution than user cancel it

        @return @true on success and @false if failed
    */
    virtual bool Execute(ITrackCancel* pTrackCancel) = 0;
    /** Returns @true if tool parameters are valid */
    virtual bool Validate(void) = 0;
    /** Returns tool parameters */
    virtual GPParameters GetParameterInfo(void) = 0;
    /** This function called by task manager before tool execution started 
        @param pCatalog
            The pointer to catalog
    */
    virtual void SetCatalog(IGxCatalog* pCatalog);
    /** Returns pointer to catalog */
    virtual IGxCatalog* const GetCatalog(void);
    /** Returns tool parameters as string */
	virtual const wxString GetAsString(void);
    /** Initialize the class from tool parameters, represented as string */
	virtual bool SetFromString(const wxString& sParams);
    /** Copy parameters from input tool to current
        @param pTool
            The tool pointer there parameters should be captured 
    */
    virtual void Copy(IGPTool* const pTool);
    //TODO: export/import tool to XML for server execution
protected:
    GPParameters m_paParam;
    IGxCatalog* m_pCatalog;
};
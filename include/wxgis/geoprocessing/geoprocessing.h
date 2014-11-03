/******************************************************************************
 * Project:  wxGIS (GIS Toolbox)
 * Purpose:  geoprocessing header.
 * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Dmitry Baryshnikov
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

#include "wxgis/catalog/catalog.h"

#define CTSTR _("Conversion Tools")
#define VECSTR _("Vector")
#define DMTSTR _("Data Management Tools")
#define RASSTR _("Raster")
#define STATSTR _("Statistics Tools")

#define TOOLBX_NAME wxT("wxGISToolbox")

#define SEGSTEP 3.0 /**<The segmentation step for vector geometry */

//GPFunctionTool 1
//GPModelTool 2
//GPScriptTool 3
//GPCustomTool

/** @enum wxGISEnumGPParameterType

    A geoprocessing tool parameter type.
*/

enum wxGISEnumGPParameterType
{
    enumGISGPParameterTypeRequired = 1, /**< The required parameter */
    enumGISGPParameterTypeOptional      /**< The optional parameter */
};

/** @enum wxGISEnumGPParameterDirection

    A geoprocessing tool parameter direction.
*/
enum wxGISEnumGPParameterDirection
{
    enumGISGPParameterDirectionInput = 1, /**< The input parameter */
    enumGISGPParameterDirectionOutput     /**< The output parameter */
};

/** @enum wxGISEnumGPParameterDataType

    A geoprocessing tool parameter data type. The input or output parmeters can be different types. So this enum helps to determnate which of type are parameters
*/
enum wxGISEnumGPParameterDataType
{
    enumGISGPParamDTUnknown = 0,        /**< The parameter type is undefined */
	enumGISGPParamDTBool,               /**< The parameter type is bool */
	enumGISGPParamDTInteger,            /**< The parameter type is integer (long or int) */
	enumGISGPParamDTDouble,             /**< The parameter type is double (double or float) */
	enumGISGPParamDTText,               /**< The parameter type is text */

	enumGISGPParamDTStringChoice,       /**< The parameter type is string list (single selection) */
	enumGISGPParamDTIntegerChoice,      /**< The parameter type is int list (single selection) */
	enumGISGPParamDTDoubleChoice,       /**< The parameter type is double list (single selection) */
	enumGISGPParamDTStringChoiceEditable,/**< The parameter type is string list with edit dialog (single selection)*/

    enumGISGPParamDTFieldAnyChoice,     /**< The parameter type is field of feature class with any type */
    enumGISGPParamDTFieldStringChoice,  /**< The parameter type is field of feature class with string type */
    enumGISGPParamDTFieldIntegerChoice, /**< The parameter type is field of feature class with int type */
    enumGISGPParamDTFieldRealChoice,    /**< The parameter type is field of feature class with real type */
    enumGISGPParamDTFieldDateChoice,    /**< The parameter type is field of feature class with date type */
    enumGISGPParamDTFieldTimeChoice,    /**< The parameter type is field of feature class with time type */
    enumGISGPParamDTFieldDateTimeChoice,/**< The parameter type is field of feature class with date/time type */
    enumGISGPParamDTFieldBinaryChoice,  /**< The parameter type is field of feature class with bimary (BLOB) type */

    enumGISGPParamDTStringList,         /**< The parameter type is comma separated string list */
	enumGISGPParamDTIntegerList,        /**< The parameter type is comma separated int list */
	enumGISGPParamDTDoubleList,         /**< The parameter type is comma separated double list */

	enumGISGPParamDTSpatRef,            /**< The parameter type is Spatial Reference */
	enumGISGPParamDTQuery,              /**< The parameter type is SQL Query */

	enumGISGPParamDTPath,               /**< The parameter type is path in catalog */
	enumGISGPParamDTFolderPath,         /**< The parameter type is folder path in catalog */
	enumGISGPParamDTPathArray,          /**< The parameter type is array of paths in catalog */

	enumGISGPParamDTParamArray,         /**< The array of IGPParameter's*/

	enumGISGPParamMax                   /**< The max type value fo iterators */
};

/** @class IGxTask

    An abstract class - base for all geoprocessing task classes

    @library{gp}
*/

class IGxTask
{
public:
    /** Destructor */
    virtual ~IGxTask(void){};
    /** Returns the task state flag*/
    virtual wxGISEnumTaskStateType GetState(void) const = 0;
    /** Returns the task priority*/
    virtual long GetPriority(void) const = 0;
    /** Returns the task start time*/
    virtual wxDateTime GetStart() const = 0;
    /** Returns the task finish time*/
    virtual wxDateTime GetFinish() const = 0;
    /** Returns the task done percent*/
    virtual double GetDone() const = 0;
    /** Start task execution. If failed returns @false */
    virtual bool StartTask() = 0;
    /** Stop task execution. If failed returns @false */
    virtual bool StopTask() = 0;
    /**
        Sets task priority. The priority influence on next task in queue which should be started after executing task (tasks) finished.

        @param nNewPriority
            The task priority
    */
    virtual void SetPriority(long nNewPriority) = 0;
};

/*
class IGPTool
{
public:
    virtual ~IGPTool(void){};
    virtual const wxString GetDisplayName(void) = 0;
    virtual const wxString GetName(void) = 0;
    virtual const wxString GetCategory(void) = 0;
    virtual bool Execute(ITrackCancel* pTrackCancel) = 0;
    virtual bool Validate(void) = 0;
    virtual GPParameters GetParameterInfo(void) = 0;
    /** \fn wxString GetAsString(void)
     *  \brief Serialize tool parameters to string.
     *  \return The string representation of tool parameters
     */	/*
    virtual const wxString GetAsString(void) = 0;
    /** \fn void SetFromString(wxString sParams)
     *  \brief Serialize tool parameters to string.
     *  \param sParams The string representation of tool parameters
     */	/*
    virtual bool SetFromString(const wxString& sParams) = 0;
    virtual void SetCatalog(IGxCatalog* pCatalog) = 0;
    virtual IGxCatalog* const GetCatalog(void) = 0;
    //virtual GetToolType(void) = 0;
    virtual void Copy(IGPTool* const pTool) = 0;
};
*/

/** @class wxGISConfigOptionReset
 * 
 *  Helper class to set and unset on desctruction the gdal config option
 * 
 *  @library{gp}
 */

class wxGISConfigOptionReset
{
public:
    wxGISConfigOptionReset(CPLString sName, CPLString sSetValue, CPLString sResetValue)
    {
        m_sName = sName;
        m_sSetValue = sSetValue;
        m_sResetValue = sResetValue;
        CPLSetConfigOption(m_sName, m_sSetValue);
    }
    ~wxGISConfigOptionReset()
    {
        CPLSetConfigOption(m_sName, m_sResetValue);
    }
protected:
    CPLString m_sName, m_sSetValue, m_sResetValue;
};

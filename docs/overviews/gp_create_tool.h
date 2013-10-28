/******************************************************************************
 * Project:  wxGIS
 * Purpose:  Geoprocessing tool creation overview
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

/**

@page overview_gp_create_tool Create your own geoprocessing tool

@tableofcontents

A guide to creating own geoprocessing tool and adding it to the wxGISToolbox.
The tool is a class inherited from the base class wxGISGPTool. The compiled 
tool is a dynamic link library (DLL). The library can consist one or several 
tools.

@see wxGISGPTool, IGPTool


@section overview_gp_create_tool_prepare The preparation

We will demonstrate creating the new tool on simple tool which compute raster file 
statistics. 

To create your own tool you need to create the new project in your favourite IDE.
The CmakeLists.txt file for such project can be looks like:

@todo Add link to cmakes files in repository

@code
cmake_minimum_required (VERSION 2.8)
project (wxgisgpcompstattool)

set(LIB_NAME wxgisgpcompstattool)

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)
set(CMAKE_COLOR_MAKEFILE ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# set path to additional CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

# For windows, do not allow the compiler to use default target (Vista).
if(WIN32)
  add_definitions(-D_WIN32_WINNT=0x0501)
endif(WIN32)

add_definitions(-DSTRICT)

if(MSVC)
    if(CMAKE_CL_64)
        set_target_properties(${the_target} PROPERTIES STATIC_LIBRARY_FLAGS "/machine:x64")
        add_definitions(-D_WIN64)
    endif()
endif()

if(UNIX)
     if(CMAKE_COMPILER_IS_GNUCXX OR CV_ICC)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -fno-strict-aliasing")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -fno-strict-aliasing")
     endif()
endif()    

set(wxWidgets_EXCLUDE_COMMON_LIBRARIES ON)
find_package(wxWidgets COMPONENTS base xml REQUIRED)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

find_package(WXGIS COMPONENTS core catalog geoprocessing REQUIRED)
if(WXGIS_FOUND)
    include_directories(${WXGIS_INCLUDE_DIR})
endif(WXGIS_FOUND)

#find needed packages
find_package(GDAL REQUIRED)
if(GDAL_FOUND)
    include_directories(${GDAL_INCLUDE_DIR})
    include_directories(${GDAL_INCLUDE_DIR}/ogr)
    include_directories(${GDAL_INCLUDE_DIR}/ogr/ogrsf_frmts)
    include_directories(${GDAL_INCLUDE_DIR}/port)
    include_directories(${GDAL_INCLUDE_DIR}/gcore)
    include_directories(${GDAL_INCLUDE_DIR}/alg)
    include_directories(${GDAL_INCLUDE_DIR}/frmts/vrt)
    include_directories(${GDAL_INCLUDE_DIR}/build/port) #assume that out of source build was in build dir
endif(GDAL_FOUND)

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}
    ${LIB_HEADERS}/gpcompstattool.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/gpcompstattool.cpp
)

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

if(WXGIS_FOUND)
   target_link_libraries(${LIB_NAME} ${WXGIS_LIBRARIES})
endif(WXGIS_FOUND)

if(GDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${GDAL_LIBRARIES})
endif(GDAL_FOUND) 

@endcode

@section overview_gp_create_tool_skeleton The creation of a tool skeleton

To create your own tool you need to inherit the class from  wxGISGPTool and reimplement need methods.
The header file maybe look like this:

@code
class WXDLLIMPEXP_GIS_GP wxGISGPCompStatTool : 
    public wxGISGPTool
{
   DECLARE_DYNAMIC_CLASS(wxGISGPCompStatTool)

public:
    wxGISGPCompStatTool(void);
    virtual ~wxGISGPCompStatTool(void);
    //IGPTool
    virtual const wxString GetDisplayName(void);
    virtual const wxString GetName(void);
    virtual const wxString GetCategory(void);
    virtual bool Execute(ITrackCancel* pTrackCancel);
    virtual GPParameters GetParameterInfo(void);
};
@endcode

The only need to reimplement methods is present.   

@section overview_gp_create_tool_function Implementing tool functionality

The source file maybe look like this:

@code
IMPLEMENT_DYNAMIC_CLASS(wxGISGPCompStatTool, wxGISGPTool)

wxGISGPCompStatTool::wxGISGPCompStatTool(void) : wxGISGPTool()
{
}

wxGISGPCompStatTool::~wxGISGPCompStatTool(void)
{
}

const wxString wxGISGPCompStatTool::GetDisplayName(void)
{
    return wxString(_("Compute statistics"));
}

const wxString wxGISGPCompStatTool::GetName(void)
{
    return wxString(wxT("comp_stats"));
}

const wxString wxGISGPCompStatTool::GetCategory(void)
{
    return wxString(_("Data Management Tools/Raster"));
}

GPParameters wxGISGPCompStatTool::GetParameterInfo(void)
{
    if(m_paParam.IsEmpty())
    {
        //src path
        wxGISGPParameter* pParam1 = new wxGISGPParameter();
        pParam1->SetName(wxT("src_path"));
        pParam1->SetDisplayName(_("Source raster"));
        pParam1->SetParameterType(enumGISGPParameterTypeRequired);
        pParam1->SetDataType(enumGISGPParamDTPath);
        pParam1->SetDirection(enumGISGPParameterDirectionInput);

        wxGISGPGxObjectDomain* pDomain1 = new wxGISGPGxObjectDomain();
        pDomain1->AddFilter(new wxGxDatasetFilter(enumGISRasterDataset));
        pParam1->SetDomain(pDomain1);

        m_paParam.Add(static_cast<IGPParameter*>(pParam1));

        //build approx
        wxGISGPParameter* pParam2 = new wxGISGPParameter();
        pParam2->SetName(wxT("approx_ok"));
        pParam2->SetDisplayName(_("Compute statistics based on overviews or a subset of all tiles"));
        pParam2->SetParameterType(enumGISGPParameterTypeOptional);
        pParam2->SetDataType(enumGISGPParamDTBool);
        pParam2->SetDirection(enumGISGPParameterDirectionInput);
        pParam2->SetValue(false);

        m_paParam.Add(static_cast<IGPParameter*>(pParam2));
    }
    return m_paParam;
}

bool wxGISGPCompStatTool::Execute(ITrackCancel* pTrackCancel)
{
    if(!Validate())
    {
        //add messages to pTrackCancel
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Unexpected error occurred"), -1, enumGISMessageErr);
        return false;
    }

    IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pCatalog);
    if(!pGxObjectContainer)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting catalog object"), -1, enumGISMessageErr);
        return false;
    }

    wxString sSrcPath = m_paParam[0]->GetValue();
    IGxObject* pGxObject = pGxObjectContainer->SearchChild(sSrcPath);
    if(!pGxObject)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting source object"), -1, enumGISMessageErr);
        return false;
    }
    IGxDataset* pGxDataset = dynamic_cast<IGxDataset*>(pGxObject);
    if(!pGxDataset)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Source object is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }
    wxGISRasterDatasetSPtr pSrcDataSet = boost::dynamic_pointer_cast<wxGISRasterDataset>(pGxDataset->GetDataset());

    if(!pSrcDataSet)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Source dataset is of incompatible type"), -1, enumGISMessageErr);
        return false;
    }

	if(!pSrcDataSet->Open(false))
		return false;

    GDALDataset* poGDALDataset = pSrcDataSet->GetRaster();
    if(!poGDALDataset)
    {
        if(pTrackCancel)
            pTrackCancel->PutMessage(_("Error getting raster"), -1, enumGISMessageErr);
        return false;
    }

    bool bApproxOK = m_paParam[1]->GetValue();

    for(int nBand = 0; nBand < poGDALDataset->GetRasterCount(); nBand++ )
    {
        double      dfMin(0), dfMax(255), dfMean(127), dfStdDev(2);
        CPLErr      eErr;

        if(pTrackCancel)
            pTrackCancel->PutMessage(wxString::Format(_("Proceed band %d"), nBand + 1), -1, enumGISMessageInfo);
        GDALRasterBand* pBand = poGDALDataset->GetRasterBand(nBand + 1);
        eErr = pBand->ComputeStatistics(bApproxOK, &dfMin, &dfMax, &dfMean, &dfStdDev, ExecToolProgress, (void*)pTrackCancel);   
	    if(eErr != CE_None)
	    {
            if(pTrackCancel)
            {
                const char* pszErr = CPLGetLastErrorMsg();
				pTrackCancel->PutMessage(wxString::Format(_("ComputeStatistics failed! GDAL error: %s"), wxString(pszErr, wxConvUTF8).c_str()), -1, enumGISMessageErr);
            }
            return false;
        }
        pTrackCancel->PutMessage(wxString::Format(_("Band %d: min - %.2f, max - %.2f, mean - %.2f, StdDev - %.2f"), nBand + 1, dfMin, dfMax, dfMean, dfStdDev), -1, enumGISMessageNorm);

    pSrcDataSet->SetHasStatistics(true);

    return true;
}
@endcode

In function @b GetDisplayName we returns the tool name, which will be shown in user interface (toolbox and other controls). 
This name can be repeated (not recommended).

The @b GetName returns the internal name. This name should be unique.

The @b GetCategory returns the category name with subcategories. The subcategories divided by "/".

The @b GetParameterInfo return the array of parameters. In our case there are two parameters:
-# Source raster
-# Compute statistics based on overviews or a subset of all tiles

The first parameter is the type of @c choose @c path and second @c checkbox.

The @b Execute do all the work and returns @true on success and @false vice versa. 


@section overview_gp_create_tool_add2toolbox Add to the toolbox

After you successfully create and compile tool you need add it to the wxGIS toolbox. 

To do it, you need:
-# Add library path (absolute or relative) to the <libs> section of wxGISCommon.xml config file.
-# Add class name and tool internal name (the same name as you set in @c GetName function) to the <tools> section of wxGISToolbox.xml config file.
-# Put internal name to the <toolboxes> section in appropriate <toolbox> subsection of wxGISToolbox.xml config file.

The special console tool which will do it automatically is planned.

*/
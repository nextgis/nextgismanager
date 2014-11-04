# **************************************************************************** 
# * Project:  wxGIS
# * Purpose:  cmake script
# * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
# ****************************************************************************
# *   Copyright (C) 2013 Dmitry Baryshnikov
# *
# *    This program is free software: you can redistribute it and/or modify
# *    it under the terms of the GNU General Public License as published by
# *    the Free Software Foundation, either version 2 of the License, or
# *    (at your option) any later version.
# *
# *    This program is distributed in the hope that it will be useful,
# *    but WITHOUT ANY WARRANTY; without even the implied warranty of
# *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# *    GNU General Public License for more details.
# *
# *    You should have received a copy of the GNU General Public License
# *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
# ****************************************************************************
cmake_minimum_required (VERSION 2.8)
set(PROJECT_NAME geoprocessing)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/geoprocessing)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/geoprocessing)

if(WIN32)
    set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
endif(WIN32)

find_package(wxWidgets 2.9 REQUIRED base net)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

option(wxGIS_USE_POSTGRES "Set OFF to build without PostGIS support" OFF)
if(wxGIS_USE_POSTGRES)
    find_package(POSTGRES REQUIRED)
    if(POSTGRES_FOUND)
        include_directories(${POSTGRES_INCLUDE_DIR})
        add_definitions(-DHAVE_PG)
    endif(POSTGRES_FOUND)
endif(wxGIS_USE_POSTGRES)


if(GDAL_FOUND)
    include_directories(${GDAL_INCLUDE_DIR}/ogr/ogrsf_frmts)
endif(GDAL_FOUND)

set(PROJECT_HHEADERS ${PROJECT_HHEADERS} 
    ${LIB_HEADERS}/geoprocessing.h
    ${LIB_HEADERS}/tskmngr.h
    ${LIB_HEADERS}/task.h
    ${LIB_HEADERS}/gpdomain.h
    ${LIB_HEADERS}/gpparam.h
    ${LIB_HEADERS}/gpraster.h
   # ${LIB_HEADERS}/gptable.h
   # ${LIB_HEADERS}/gptool.h
   # ${LIB_HEADERS}/gptoolmngr.h
   ${LIB_HEADERS}/gpvector.h    
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}  
    ${LIB_SOURCES}/tskmngr.cpp  
    ${LIB_SOURCES}/task.cpp  
     ${LIB_SOURCES}/gpdomain.cpp
     ${LIB_SOURCES}/gpparam.cpp
    ${LIB_SOURCES}/gpraster.cpp
    # ${LIB_SOURCES}/gptable.cpp
    # ${LIB_SOURCES}/gptool.cpp
    # ${LIB_SOURCES}/gptoolmngr.cpp
    ${LIB_SOURCES}/gpvector.cpp    
)

## tools

set(PROJECT_HHEADERS ${PROJECT_HHEADERS} 
   # ${LIB_HEADERS}/gpcompstattool.h
   # ${LIB_HEADERS}/gpcreateovrtool.h
   # ${LIB_HEADERS}/gpexporttool.h
   # ${LIB_HEADERS}/gpgeomverttoptstool.h
   # ${LIB_HEADERS}/gporthocorrecttool.h
   # ${LIB_HEADERS}/gpprojecttool.h
   # ${LIB_HEADERS}/gpshapetotexttool.h
   # ${LIB_HEADERS}/gpsubrasterbyvectortool.h
   # ${LIB_HEADERS}/gpsumtabletool.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}  
    # ${LIB_SOURCES}/gpcompstattool.cpp
    # ${LIB_SOURCES}/gpcreateovrtool.cpp
    # ${LIB_SOURCES}/gpexporttool.cpp
    # ${LIB_SOURCES}/gpgeomverttoptstool.cpp
    # ${LIB_SOURCES}/gporthocorrecttool.cpp
    # ${LIB_SOURCES}/gpprojecttool.cpp
    # ${LIB_SOURCES}/gpshapetotexttool.cpp
    # ${LIB_SOURCES}/gpsubrasterbyvectortool.cpp
    # ${LIB_SOURCES}/gpsumtabletool.cpp
)

add_definitions(-DwxUSE_GUI=0 -DWXMAKINGDLL_GIS_GP)
set( WXGISGEOPROCESSING_LIB_NAME ${LIB_NAME} PARENT_SCOPE )

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

if(POSTGRES_FOUND)
    target_link_libraries(${LIB_NAME} ${POSTGRES_LIBRARY})
    if(WIN32)
        target_link_libraries(${LIB_NAME} Ws2_32.lib Secur32.lib)
    endif(WIN32)
endif(POSTGRES_FOUND)

if(GDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${GDAL_LIBRARIES})
endif(GDAL_FOUND) 

target_link_libraries(${LIB_NAME} ${WXGISCORE_LIB_NAME} ${WXGISNET_LIB_NAME} ${WXGISCATALOG_LIB_NAME})#${WXGISDATASOURCE_LIB_NAME}  

include(installlib)


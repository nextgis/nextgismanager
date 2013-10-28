# **************************************************************************** 
# * Project:  wxGIS
# * Purpose:  cmake script
# * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
# ****************************************************************************
# *   Copyright (C) 2013 Bishop
# *
# *    This program is free software: you can redistribute it and/or modify
# *    it under the terms of the GNU General Public License as published by
# *    the Free Software Foundation, either version 3 of the License, or
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
set(PROJECT_NAME carto)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/carto)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/carto)

if(WIN32)
    set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
endif(WIN32)

find_package(wxWidgets 2.9 REQUIRED core base)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

if(UNIX)
    find_package(GTK2 REQUIRED)
    if(GTK2_FOUND)
        include_directories(${GTK2_INCLUDE_DIRS})
        add_definitions(-DHAVE_GTK2)
    endif(GTK2_FOUND)
    message(STATUS "GTK2 libs=[${GTK2_LIBRARIES}] headers=[${GTK2_INCLUDE_DIRS}]")
endif()

find_package(CAIRO REQUIRED)
if(CAIRO_FOUND)
    include_directories(${CAIRO_INCLUDE_DIR})
    add_definitions(-DHAVE_CAIRO)
    set(wxGIS_USE_CAIRO ON)
endif(CAIRO_FOUND)

include_directories(${LIB_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}  
    ${LIB_HEADERS}/carto.h
    ${LIB_HEADERS}/layer.h
    ${LIB_HEADERS}/renderer.h
    ${LIB_HEADERS}/featurelayer.h
    ${LIB_HEADERS}/map.h
    ${LIB_HEADERS}/rasterlayer.h
    ${LIB_HEADERS}/rasterrenderer.h
    ${LIB_HEADERS}/featurerenderer.h
    ${LIB_HEADERS}/stretch.h
    ${LIB_HEADERS}/transformthreads.h    
    ${LIB_HEADERS}/mxevent.h   
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/featurelayer.cpp
    ${LIB_SOURCES}/layer.cpp
    ${LIB_SOURCES}/renderer.cpp
    ${LIB_SOURCES}/map.cpp
    ${LIB_SOURCES}/rasterlayer.cpp
    ${LIB_SOURCES}/rasterrenderer.cpp
    ${LIB_SOURCES}/featurerenderer.cpp
    ${LIB_SOURCES}/stretch.cpp
    ${LIB_SOURCES}/transformthreads.cpp    
    ${LIB_SOURCES}/mxevent.cpp
)

add_definitions(-DWXMAKINGDLL_GIS_CRT)
set( WXGISCARTO_LIB_NAME ${LIB_NAME} PARENT_SCOPE )

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

if(WXGISGDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${WXGISGDAL_LIBRARIES})
endif(WXGISGDAL_FOUND) 

if(GDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${GDAL_LIBRARIES})
endif(GDAL_FOUND) 

if(CAIRO_FOUND)
    target_link_libraries(${LIB_NAME} ${CAIRO_LIBRARIES})
endif(CAIRO_FOUND)

if(UNIX)
    if(GTK2_FOUND)
        target_link_libraries(${LIB_NAME} ${GTK2_LIBRARIES})
    endif(GTK2_FOUND)
endif(UNIX)

target_link_libraries(${LIB_NAME} ${WXGISCORE_LIB_NAME} ${WXGISDATASOURCE_LIB_NAME} ${WXGISFRAMEWORK_LIB_NAME} ${WXGISDISPLAY_LIB_NAME})

include(installlib)


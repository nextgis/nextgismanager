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
set(PROJECT_NAME datasource)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/datasource)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/datasource)

if(WIN32)
    set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
endif(WIN32)

find_package(wxWidgets 2.9 REQUIRED core base xml)
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

option(wxGIS_USE_OPENSSL "Set OFF to build without OpenSSL support" OFF)
if(wxGIS_USE_OPENSSL)
    find_package( OpenSSL )
    if(OPENSSL_FOUND)
        include_directories(${OPENSSL_INCLUDE_DIR})
        add_definitions(-DHAVE_OPENSSL)
    endif(OPENSSL_FOUND) 
endif(wxGIS_USE_OPENSSL)

set(PROJECT_HHEADERS ${PROJECT_HHEADERS} 
    ${LIB_HEADERS}/cursor.h
    ${LIB_HEADERS}/dataset.h
    ${LIB_HEADERS}/datacontainer.h
    ${LIB_HEADERS}/datasource.h
    ${LIB_HEADERS}/featuredataset.h
    ${LIB_HEADERS}/filter.h
    ${LIB_HEADERS}/postgisdataset.h
    ${LIB_HEADERS}/spatialtree.h
    ${LIB_HEADERS}/rasterdataset.h
    ${LIB_HEADERS}/rasterop.h
    #${LIB_HEADERS}/spvalidator.h
    ${LIB_HEADERS}/sysop.h
    ${LIB_HEADERS}/table.h
    ${LIB_HEADERS}/vectorop.h
    ${LIB_HEADERS}/gdalinh.h    
    ${LIB_HEADERS}/wmsdataset.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/cursor.cpp
    ${LIB_SOURCES}/dataset.cpp
    ${LIB_SOURCES}/datacontainer.cpp
    ${LIB_SOURCES}/featuredataset.cpp
    ${LIB_SOURCES}/filter.cpp
    ${LIB_SOURCES}/postgisdataset.cpp
    ${LIB_SOURCES}/spatialtree.cpp
    ${LIB_SOURCES}/rasterdataset.cpp
    ${LIB_SOURCES}/rasterop.cpp
    #${LIB_SOURCES}/spvalidator.cpp
    ${LIB_SOURCES}/sysop.cpp
    ${LIB_SOURCES}/table.cpp
    ${LIB_SOURCES}/vectorop.cpp
    ${LIB_SOURCES}/gdalinh.cpp
    ${LIB_SOURCES}/wmsdataset.cpp
)

add_definitions(-DWXMAKINGDLL_GIS_DS)
set( WXGISDATASOURCE_LIB_NAME ${LIB_NAME} PARENT_SCOPE)

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

if(GDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${GDAL_LIBRARIES})
endif(GDAL_FOUND) 

if(POSTGRES_FOUND)
    target_link_libraries(${LIB_NAME} ${POSTGRES_LIBRARY})
    if(WIN32)
        target_link_libraries(${LIB_NAME} Ws2_32.lib Secur32.lib)
    endif(WIN32)
endif(POSTGRES_FOUND)

if(OPENSSL_FOUND)
    target_link_libraries(${LIB_NAME} ${OPENSSL_LIBRARIES})
endif(OPENSSL_FOUND) 

target_link_libraries(${LIB_NAME} ${WXGISCORE_LIB_NAME})

include(installlib)

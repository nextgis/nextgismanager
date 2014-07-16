# **************************************************************************** 
# * Project:  wxGIS
# * Purpose:  cmake script
# * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
# ****************************************************************************
# *   Copyright (C) 2013,2014 Dmitry Baryshnikov
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
set(PROJECT_NAME core)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/core)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/core)

if(WIN32)
    set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
endif(WIN32)

find_package(wxWidgets 2.9 REQUIRED base xml)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

option(wxGIS_USE_OPENSSL "Set OFF to build without OpenSSL support" OFF)
if(wxGIS_USE_OPENSSL)
    find_package( OpenSSL )
    if(OPENSSL_FOUND)
        include_directories(${OPENSSL_INCLUDE_DIR})
        add_definitions(-DHAVE_OPENSSL)
    endif(OPENSSL_FOUND) 
endif(wxGIS_USE_OPENSSL)
    
# find_package(PROJ REQUIRED)
# if(PROJ_FOUND)
    # include_directories(${PROJ_INCLUDE_DIR})
    # add_definitions(-DHAVE_PROJ -DPROJ_STATIC)
# endif(PROJ_FOUND)

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}
    ${LIB_HEADERS}/config.h
    ${LIB_HEADERS}/core.h
    ${LIB_HEADERS}/crypt.h
    ${LIB_HEADERS}/event.h
    ${LIB_HEADERS}/format.h
    ${LIB_HEADERS}/pointer.h
    ${LIB_HEADERS}/app.h
    ${LIB_HEADERS}/init.h
    ${LIB_HEADERS}/process.h
    ${LIB_HEADERS}/json/json_defs.h
    ${LIB_HEADERS}/json/jsonreader.h
    ${LIB_HEADERS}/json/jsonval.h
    ${LIB_HEADERS}/json/jsonwriter.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/config.cpp
    ${LIB_SOURCES}/crypt.cpp
    ${LIB_SOURCES}/event.cpp
    ${LIB_SOURCES}/format.cpp
    ${LIB_SOURCES}/pointer.cpp
    ${LIB_SOURCES}/app.cpp
    ${LIB_SOURCES}/init.cpp
    ${LIB_SOURCES}/process.cpp
    ${LIB_SOURCES}/json/jsonreader.cpp
    ${LIB_SOURCES}/json/jsonval.cpp
    ${LIB_SOURCES}/json/jsonwriter.cpp
)

add_definitions(-DWXMAKINGDLL_GIS_CORE -DwxUSE_GUI=0 -DWXMAKINGDLL_JSON)
set(WXGISCORE_LIB_NAME ${LIB_NAME} PARENT_SCOPE)

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

if(OPENSSL_FOUND)
    target_link_libraries(${LIB_NAME} ${OPENSSL_LIBRARIES})
endif(OPENSSL_FOUND) 


if(GDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${GDAL_LIBRARIES})
endif(GDAL_FOUND) 

# if(PROJ_FOUND)
    # target_link_libraries(${LIB_NAME} ${PROJ_LIBRARY})
# endif(PROJ_FOUND)

# if(Boost_FOUND)
    # if(UNIX)
        # target_link_libraries(${LIB_NAME} boost_system )#boost_filesystem
    # endif(UNIX)
# endif(Boost_FOUND)

include(installlib)


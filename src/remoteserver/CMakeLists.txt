# **************************************************************************** 
# * Project:  wxGIS
# * Purpose:  cmake script
# * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
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
set(PROJECT_NAME remoteserver)

include(lib)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/remoteserver)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/remoteserver)

if(WIN32)
    set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
endif(WIN32)

find_package(wxWidgets 2.9 REQUIRED core base xml net)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

include_directories(${LIB_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}
    ${LIB_HEADERS}/gxremoteserver.h
    ${LIB_HEADERS}/gxremoteservers.h
    ${LIB_HEADERS}/remoteserver.h
    ${LIB_HEADERS}/rxobjectclient.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/gxremoteserver.cpp
    ${LIB_SOURCES}/gxremoteservers.cpp
    ${LIB_SOURCES}/rxobjectclient.cpp
)

add_definitions(-DWXMAKINGDLL_GIS_RS)
set(WXGISRS_LIB_NAME ${LIB_NAME} PARENT_SCOPE)

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

target_link_libraries(${LIB_NAME} ${WXGISCORE_LIB_NAME} ${WXGISCATALOG_LIB_NAME} ${WXGISNET_LIB_NAME}) 

include(installlib)


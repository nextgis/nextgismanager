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
set(PROJECT_NAME remoteserverui)

include(lib)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/remoteserverui)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/remoteserverui)

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
    ${LIB_HEADERS}/createnetworkconndlg.h
    ${LIB_HEADERS}/gxremoteserversui.h
    ${LIB_HEADERS}/gxremoteserverui.h
    ${LIB_HEADERS}/remoteservercmd.h
    ${LIB_HEADERS}/remoteserverui.h
    ${LIB_HEADERS}/rxdiscconnections.h
    ${LIB_HEADERS}/rxobjectclientui.h
    ${LIB_HEADERS}/serversearchdlg.h
    ${LIB_HEADERS}/netfactoryui.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/createnetworkconndlg.cpp
    ${LIB_SOURCES}/gxremoteserversui.cpp
    ${LIB_SOURCES}/gxremoteserverui.cpp
    ${LIB_SOURCES}/remoteservercmd.cpp
    ${LIB_SOURCES}/rxdiscconnections.cpp
    ${LIB_SOURCES}/rxobjectclientui.cpp
    ${LIB_SOURCES}/serversearchdlg.cpp
    ${LIB_SOURCES}/netfactoryui.cpp
)

add_definitions(-DWXMAKINGDLL_GIS_RSU)
set(WXGISRSU_LIB_NAME ${LIB_NAME} PARENT_SCOPE)

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

target_link_libraries(${LIB_NAME} ${WXGISRS_LIB_NAME} ${WXGISCATALOGUI_LIB_NAME} ${WXGISNET_LIB_NAME}) 

include(installlib)


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
set(PROJECT_NAME net)

include(lib)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/net)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/net)

if(WIN32)
    set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
endif(WIN32)

find_package(wxWidgets 2.9 REQUIRED base xml net)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

option(wxGIS_USE_CURL "Set ON to build with curl support" OFF)
if(wxGIS_USE_CURL)
    find_package(CURL REQUIRED)
    if(CURL_FOUND)
        include_directories(${CURL_INCLUDE_DIRS})
        add_definitions(-DHAVE_CURL)
    endif(CURL_FOUND)
endif(wxGIS_USE_CURL)

include_directories(${LIB_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}
    ${LIB_HEADERS}/message.h
    ${LIB_HEADERS}/net.h
    ${LIB_HEADERS}/network.h
    ${LIB_HEADERS}/netevent.h
    ${LIB_HEADERS}/netconn.h
    ${LIB_HEADERS}/servernet.h
    ${LIB_HEADERS}/netfactory.h
    ${LIB_HEADERS}/tcp.h
    ${LIB_HEADERS}/curl.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/message.cpp
    ${LIB_SOURCES}/network.cpp
    ${LIB_SOURCES}/netevent.cpp
    ${LIB_SOURCES}/netconn.cpp
    ${LIB_SOURCES}/servernet.cpp
    ${LIB_SOURCES}/netfactory.cpp
    ${LIB_SOURCES}/tcp.cpp
    ${LIB_SOURCES}/curl.cpp
)

option(wxGIS_USE_EMAIL "Set ON to build with e-mail support" OFF)
if(wxGIS_USE_EMAIL)
set(PROJECT_HHEADERS ${PROJECT_HHEADERS}
    ${LIB_HEADERS}/mail/email.h
    ${LIB_HEADERS}/mail/smapi.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/mail/email.cpp
    ${LIB_SOURCES}/mail/smapi.cpp
)
endif(wxGIS_USE_EMAIL)

add_definitions(-DWXMAKINGDLL_GIS_NET -DwxUSE_GUI=0)
set(WXGISNET_LIB_NAME ${LIB_NAME} PARENT_SCOPE)

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

if(CURL_FOUND)
   target_link_libraries(${LIB_NAME} ${CURL_LIBRARIES})
endif(CURL_FOUND)

target_link_libraries( ${LIB_NAME} ${WXGISCORE_LIB_NAME} ) 

include(installlib)


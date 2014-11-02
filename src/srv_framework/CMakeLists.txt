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
set(PROJECT_NAME srvframework)

include(lib)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgissrv/srv_framework)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/srv_framework)

set(wxWidgets_EXCLUDE_COMMON_LIBRARIES ON)
find_package(wxWidgets 2.9 REQUIRED base xml)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

# find_package( OpenSSL )
# if(OPENSSL_FOUND)
    # include_directories(${OPENSSL_INCLUDE_DIR})
# endif(OPENSSL_FOUND) 

include_directories(${LIB_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}
    ${LIB_HEADERS}/auth.h
    ${LIB_HEADERS}/catalog.h
    ${LIB_HEADERS}/framework.h
    ${LIB_HEADERS}/message.h
    ${LIB_HEADERS}/plugin.h
    ${LIB_HEADERS}/rxdiscconnections.h
    ${LIB_HEADERS}/rxobjectserver.h
    ${LIB_HEADERS}/server.h
    ${LIB_HEADERS}/xmlauth.h
    ${LIB_HEADERS}/service.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/auth.cpp
    ${LIB_SOURCES}/catalog.cpp
    ${LIB_SOURCES}/message.cpp
    ${LIB_SOURCES}/plugin.cpp
    ${LIB_SOURCES}/rxdiscconnections.cpp
    ${LIB_SOURCES}/rxobjectserver.cpp
    ${LIB_SOURCES}/server.cpp
    ${LIB_SOURCES}/xmlauth.cpp
    ${LIB_SOURCES}/service.cpp
)

add_definitions(-DWXMAKINGDLL_GIS_FRW)
set(WXGISSRVFRW_LIB_NAME ${LIB_NAME} PARENT_SCOPE)

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

# if(OPENSSL_FOUND)
    # target_link_libraries(${LIB_NAME} ${OPENSSL_LIBRARIES})
# endif(OPENSSL_FOUND) 

if(WXGISGDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${WXGISGDAL_LIBRARIES})
endif(WXGISGDAL_FOUND) 

if(GDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${GDAL_LIBRARIES})
endif(GDAL_FOUND) 

target_link_libraries(${LIB_NAME} ${WXGISCORE_LIB_NAME} ${WXGISCATALOG_LIB_NAME} ${WXGISNET_LIB_NAME}  ${WXGISGEOPROCESSING_LIB_NAME})

include(installlib)


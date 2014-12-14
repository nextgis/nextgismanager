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
set(PROJECT_NAME conf)
set(APP_NAME wxgisconf)
set(wxGIS_MAINFAMEICON "${WXGIS_CURRENT_SOURCE_DIR}/art/conf.ico")
set(wxGIS_MAINFAMEICON_X "${WXGIS_CURRENT_SOURCE_DIR}/art/conf.ico")

include(app)
include(common)

set(APP_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/conf)
set(APP_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/conf)

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

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}
    ${APP_HEADERS}/confmain.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${APP_SOURCES}/confmain.cpp
)

add_executable(${APP_NAME} ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${APP_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

if(OPENSSL_FOUND)
    target_link_libraries(${APP_NAME} ${OPENSSL_LIBRARIES})
endif(OPENSSL_FOUND) 

target_link_libraries(${APP_NAME} ${WXGISCORE_LIB_NAME})

include(installapp)


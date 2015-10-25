#*************************************************************************** 
# Project:  wxGIS
# Purpose:  cmake script
# Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
#***************************************************************************
#   Copyright (C) 2013-2014 Dmitry Baryshnikov
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#***************************************************************************
project (wxgis${PROJECT_NAME})

message(STATUS "${PROJECT_NAME} app name ${APP_NAME}")

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/version_app.h "//Copyright (C) 2009-2014 Baryshnikov Dmitry (aka Bishop), polimax@mail.ru\n#pragma once\n#define wxGIS_FILENAME \"${APP_NAME}\"\n#define wxGIS_MAINFAMEICON \"${wxGIS_MAINFAMEICON}\"\n\n" )

if(MSVC)
  set(CMAKE_DEBUG_POSTFIX "d")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${WXGIS_CURRENT_BINARY_DIR}/Debug/)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${WXGIS_CURRENT_BINARY_DIR}/Release/)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions(-D_UNICODE -DUNICODE -D_USRDLL)
  set(PROJECT_CSOURCES ${PROJECT_CSOURCES} ${WXGIS_CURRENT_SOURCE_DIR}/src/version_app.rc ${wxGIS_MAINFAMEICON})
  source_group("Resource Files" FILES ${WXGIS_CURRENT_SOURCE_DIR}/src/version_app.rc ${wxGIS_MAINFAMEICON}) 
endif(MSVC)

if(WIN32)
  add_definitions(-DWIN32 -D__WXMSW__)
endif(WIN32)



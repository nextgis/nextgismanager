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
set(PROJECT_NAME tskmngrapp)
set(APP_NAME wxgistaskmanager)

set(wxGIS_MAINFAMEICON "${WXGIS_CURRENT_SOURCE_DIR}/art/taskmanager.ico")
set(wxGIS_MAINFAMEICON_X "${WXGIS_CURRENT_SOURCE_DIR}/art/taskmanager.ico")

include(app)
include(common)

set(APP_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/tskmngr_app)
set(APP_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/tskmngr_app)

if(WIN32)
    set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
endif(WIN32)

find_package(wxWidgets 2.9 REQUIRED base net xml)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
    add_definitions("-DwxUSE_GUI=0")
#    if(UNIX)
#      add_definitions("-D${wxWidgets_DEFINITIONS}") #wxWidgets_DEFINITIONS_DEBUG
#      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")
#    endif(UNIX)
endif(wxWidgets_FOUND)



include_directories(${APP_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}  
    ${APP_HEADERS}/tskmngrapp.h
    ${APP_HEADERS}/tskmngr.h
    ${APP_HEADERS}/net.h
    ${APP_HEADERS}/task.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${APP_SOURCES}/tskmngrapp.cpp
    ${APP_SOURCES}/tskmngr.cpp
    ${APP_SOURCES}/net.cpp
    ${APP_SOURCES}/task.cpp
)

add_executable(${APP_NAME} ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})


if(wxWidgets_FOUND)
   target_link_libraries(${APP_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

target_link_libraries(${APP_NAME} ${WXGISCORE_LIB_NAME} ${WXGISNET_LIB_NAME})

include(installapp)


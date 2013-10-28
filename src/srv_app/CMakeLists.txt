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
set(PROJECT_NAME serverapp)
set(APP_NAME wxgisserver)
set(wxGIS_MAINFAMEICON "${WXGIS_CURRENT_SOURCE_DIR}/art/server.ico")
set(wxGIS_MAINFAMEICON_X "${WXGIS_CURRENT_SOURCE_DIR}/art/server.ico")

include(app)
include(common)

set(APP_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgissrv/srv_app)
set(APP_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/srv_app)

if(WIN32)
    set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
endif(WIN32)

find_package(wxWidgets 2.9 REQUIRED base net)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
#    if(UNIX)
#      add_definitions("-D${wxWidgets_DEFINITIONS}") #wxWidgets_DEFINITIONS_DEBUG
#      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${wxWidgets_CXX_FLAGS}")
#    endif(UNIX)
endif(wxWidgets_FOUND)

# find_package( OpenSSL )
# if(OPENSSL_FOUND)
    # include_directories(${OPENSSL_INCLUDE_DIR})
# endif(OPENSSL_FOUND)

# find_package(GEOS REQUIRED)
# if(GEOS_FOUND)
    # #get GEOS root directory from GEOS_INCLUDE_DIR
    # get_filename_component(GEOS_ROOT ${GEOS_INCLUDE_DIR} PATH)
    # include_directories(${GEOS_INCLUDE_DIR})
    # include_directories(${GEOS_ROOT}/capi)
    # add_definitions(-DHAVE_GEOS)
# endif(GEOS_FOUND)

# find_package(PROJ REQUIRED)
# if(PROJ_FOUND)
    # include_directories(${PROJ_INCLUDE_DIR})
    # add_definitions(-DHAVE_PROJ -DPROJ_STATIC)
# endif(PROJ_FOUND)

# if(WXGISGDAL_FOUND)
    # include_directories(${WXGISGDAL_INCLUDE_DIR}/ogr/ogrsf_frmts)
# endif(WXGISGDAL_FOUND) 

# if(GDAL_FOUND)
    # include_directories(${GDAL_INCLUDE_DIR}/ogr/ogrsf_frmts)
# endif(GDAL_FOUND)

include_directories(${APP_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}  
    ${APP_HEADERS}/serverapp.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${APP_SOURCES}/serverapp.cpp
)

add_executable(${APP_NAME} ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})


if(wxWidgets_FOUND)
   target_link_libraries(${APP_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

# if(OPENSSL_FOUND)
    # target_link_libraries(${APP_NAME} ${OPENSSL_LIBRARIES})
# endif(OPENSSL_FOUND) 

# if(WXGISGDAL_FOUND)
    # target_link_libraries(${APP_NAME} ${WXGISGDAL_LIBRARIES})
# endif(WXGISGDAL_FOUND) 

# if(GDAL_FOUND)
    # target_link_libraries(${APP_NAME} ${GDAL_LIBRARIES})
# endif(GDAL_FOUND) 

# if(PROJ_FOUND)
    # target_link_libraries(${APP_NAME} ${PROJ_LIBRARY})
# endif(PROJ_FOUND)

# if(GEOS_FOUND)
    # target_link_libraries(${APP_NAME} ${GEOS_LIBRARY})
# endif(GEOS_FOUND)

target_link_libraries(${APP_NAME} ${WXGISCORE_LIB_NAME} ${WXGISSRVNET_LIB_NAME} ${WXGISSRVFRW_LIB_NAME} )

include(installapp)


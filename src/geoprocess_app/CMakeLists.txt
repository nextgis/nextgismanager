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
set(PROJECT_NAME geoprocess)
set(APP_NAME wxgisgeoprocess)
set(wxGIS_MAINFAMEICON "${WXGIS_CURRENT_SOURCE_DIR}/art/geoprocess.ico")
set(wxGIS_MAINFAMEICON_X "${WXGIS_CURRENT_SOURCE_DIR}/art/geoprocess.ico")

include(app)
include(common)

set(APP_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/geoprocess)
set(APP_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/geoprocess)

if(WIN32)
    set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
endif(WIN32)

find_package(wxWidgets 2.9 REQUIRED core base)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

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
    ${APP_HEADERS}/geoprocess.h
    ${APP_HEADERS}/processmain.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${APP_SOURCES}/processmain.cpp
)

add_executable(${APP_NAME} ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${APP_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

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

# if(CAIRO_FOUND)
    # target_link_libraries(${APP_NAME} ${CAIRO_LIBRARIES})
# endif(CAIRO_FOUND)

target_link_libraries(${APP_NAME} ${WXGISCORE_LIB_NAME} ${WXGISGEOPROCESSING_LIB_NAME} )

include(installapp)


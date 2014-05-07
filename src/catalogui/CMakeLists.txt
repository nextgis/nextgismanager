# **************************************************************************** 
# * Project:  wxGIS
# * Purpose:  cmake script
# * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
# ****************************************************************************
# *   Copyright (C) 2013 Bishop
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
set(PROJECT_NAME catalogui)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/wxgis/catalogui)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/catalogui)

if(WIN32)
    set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
endif(WIN32)

find_package(wxWidgets 2.9 REQUIRED core base propgrid aui adv html xml)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

if(GDAL_FOUND)
    include_directories(${GDAL_INCLUDE_DIR}/ogr/ogrsf_frmts)
endif(GDAL_FOUND)

option(wxGIS_USE_POSTGRES "Set OFF to build without PostGIS support" OFF)
if(wxGIS_USE_POSTGRES)
    find_package(POSTGRES REQUIRED)
    if(POSTGRES_FOUND)
        include_directories(${POSTGRES_INCLUDE_DIR})
        add_definitions(-DHAVE_PG)
    endif(POSTGRES_FOUND)
endif(wxGIS_USE_POSTGRES)

option(wxGIS_USE_OPENSSL "Set OFF to build without OpenSSL support" OFF)
if(wxGIS_USE_OPENSSL)
    find_package( OpenSSL )
    if(OPENSSL_FOUND)
        include_directories(${OPENSSL_INCLUDE_DIR})
        add_definitions(-DHAVE_OPENSSL)
    endif(OPENSSL_FOUND) 
endif(wxGIS_USE_OPENSSL)

if(UNIX)
    find_package(GTK2 REQUIRED)
    if(GTK2_FOUND)
        include_directories(${GTK2_INCLUDE_DIRS})
        add_definitions(-DHAVE_GTK2)
    endif(GTK2_FOUND)
    message(STATUS "GTK2 libs=[${GTK2_LIBRARIES}] headers=[${GTK2_INCLUDE_DIRS}]")
endif()

find_package(CAIRO REQUIRED)
if(CAIRO_FOUND)
    include_directories(${CAIRO_INCLUDE_DIR})
    add_definitions(-DHAVE_CAIRO)
    set(wxGIS_USE_CAIRO ON)
endif(CAIRO_FOUND)

include_directories(${LIB_HEADERS})

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}  
    ${LIB_HEADERS}/catalogcmd.h
    ${LIB_HEADERS}/catalogui.h
    ${LIB_HEADERS}/createnewcmd.h
    ${LIB_HEADERS}/gxapplication.h
    ${LIB_HEADERS}/gxarchfolderui.h
    ${LIB_HEADERS}/gxarchivefactoryui.h
    ${LIB_HEADERS}/gxcatalogui.h
    ${LIB_HEADERS}/gxcontdialog.h
    ${LIB_HEADERS}/gxcontentview.h
    ${LIB_HEADERS}/gxcsvfactoryui.h
    ${LIB_HEADERS}/gxdatasetui.h
    ${LIB_HEADERS}/gxdiscconnectionsui.h
    ${LIB_HEADERS}/gxdiscconnectionui.h
    ${LIB_HEADERS}/gxeventui.h
    ${LIB_HEADERS}/gxfilefactoryui.h
    ${LIB_HEADERS}/gxfileui.h
    ${LIB_HEADERS}/gxfolderfactoryui.h
    ${LIB_HEADERS}/gxfolderui.h
    ${LIB_HEADERS}/gxmldatasetui.h
    ${LIB_HEADERS}/gxlocationcombobox.h
    ${LIB_HEADERS}/gxmapinfofactoryui.h
    ${LIB_HEADERS}/gxmapview.h
    ${LIB_HEADERS}/gxmlfactoryui.h
    ${LIB_HEADERS}/gxnoview.h
    ${LIB_HEADERS}/gxobjcmd.h
    ${LIB_HEADERS}/gxobjdialog.h
    ${LIB_HEADERS}/gxpending.h
    ${LIB_HEADERS}/gxprjfactoryui.h
    ${LIB_HEADERS}/gxpropertypages.h
    ${LIB_HEADERS}/gxrasterfactoryui.h
    ${LIB_HEADERS}/gxselection.h
    ${LIB_HEADERS}/gxshapefactoryui.h
    ${LIB_HEADERS}/gxspatreffolderui.h
    ${LIB_HEADERS}/gxtableview.h
    ${LIB_HEADERS}/gxtabview.h
    ${LIB_HEADERS}/gxtreeview.h
    ${LIB_HEADERS}/gxview.h
    ${LIB_HEADERS}/newmenu.h
    ${LIB_HEADERS}/rasterpropertypage.h
    ${LIB_HEADERS}/spatrefpropertypage.h
    ${LIB_HEADERS}/tablepropertypage.h
    ${LIB_HEADERS}/vectorpropertypage.h
    ${LIB_HEADERS}/viewscmd.h
    ${LIB_HEADERS}/remoteconndlgs.h
    ${LIB_HEADERS}/gxremoteconnui.h
    ${LIB_HEADERS}/gxpostgisdatasetui.h
    ${LIB_HEADERS}/gxdbconnectionsui.h
    ${LIB_HEADERS}/gxdbconnfactoryui.h
    ${LIB_HEADERS}/gxwebconnectionsui.h
    ${LIB_HEADERS}/gxwebconnfactoryui.h
    ${LIB_HEADERS}/droptarget.h #think about move to other lib
    ${LIB_HEADERS}/processing.h #think about move to other lib
    ${LIB_HEADERS}/gxshellconnui.h
    ${LIB_HEADERS}/gxshellconnectionsui.h
    ${LIB_HEADERS}/gxshellconnfactoryui.h
    ${LIB_HEADERS}/gxgisprojui.h
    ${LIB_HEADERS}/gxgisprojfactoryui.h
    ${LIB_HEADERS}/gxlocaldbfactoryui.h
    ${LIB_HEADERS}/gxlocaldbui.h)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/catalogcmd.cpp
    ${LIB_SOURCES}/createnewcmd.cpp
    ${LIB_SOURCES}/gxapplication.cpp
    ${LIB_SOURCES}/gxarchfolderui.cpp
    ${LIB_SOURCES}/gxarchivefactoryui.cpp
    ${LIB_SOURCES}/gxcatalogui.cpp
    ${LIB_SOURCES}/gxcontdialog.cpp
    ${LIB_SOURCES}/gxcontentview.cpp
    ${LIB_SOURCES}/gxcsvfactoryui.cpp
    ${LIB_SOURCES}/gxdatasetui.cpp
    ${LIB_SOURCES}/gxdiscconnectionsui.cpp
    ${LIB_SOURCES}/gxdiscconnectionui.cpp
    ${LIB_SOURCES}/gxeventui.cpp
    ${LIB_SOURCES}/gxfilefactoryui.cpp
    ${LIB_SOURCES}/gxfileui.cpp
    ${LIB_SOURCES}/gxfolderfactoryui.cpp
    ${LIB_SOURCES}/gxfolderui.cpp
    ${LIB_SOURCES}/gxmldatasetui.cpp
    ${LIB_SOURCES}/gxlocationcombobox.cpp
    ${LIB_SOURCES}/gxmapinfofactoryui.cpp
    ${LIB_SOURCES}/gxmapview.cpp
    ${LIB_SOURCES}/gxmlfactoryui.cpp
    ${LIB_SOURCES}/gxnoview.cpp
    ${LIB_SOURCES}/gxobgdialog.cpp
    ${LIB_SOURCES}/gxobjcmd.cpp
    ${LIB_SOURCES}/gxpending.cpp
    ${LIB_SOURCES}/gxprjfactoryui.cpp
    ${LIB_SOURCES}/gxpropertypages.cpp
    ${LIB_SOURCES}/gxrasterfactoryui.cpp
    ${LIB_SOURCES}/gxselection.cpp
    ${LIB_SOURCES}/gxshapefactoryui.cpp
    ${LIB_SOURCES}/gxspatreffolderui.cpp
    ${LIB_SOURCES}/gxtableview.cpp
    ${LIB_SOURCES}/gxtabview.cpp
    ${LIB_SOURCES}/gxtreeview.cpp
    ${LIB_SOURCES}/gxview.cpp
    ${LIB_SOURCES}/newmenu.cpp
    ${LIB_SOURCES}/rasterpropertypage.cpp
    ${LIB_SOURCES}/spatrefpropertypage.cpp
    ${LIB_SOURCES}/tablepropertypage.cpp
    ${LIB_SOURCES}/vectorpropertypage.cpp
    ${LIB_SOURCES}/viewscmd.cpp    
    ${LIB_SOURCES}/gxdbconnectionsui.cpp
    ${LIB_SOURCES}/gxdbconnfactoryui.cpp
    ${LIB_SOURCES}/gxwebconnectionsui.cpp
    ${LIB_SOURCES}/gxwebconnfactoryui.cpp
    ${LIB_SOURCES}/remoteconndlgs.cpp
    ${LIB_SOURCES}/gxremoteconnui.cpp
    ${LIB_SOURCES}/gxpostgisdatasetui.cpp
    ${LIB_SOURCES}/droptarget.cpp #think about move to other lib
    ${LIB_SOURCES}/processing.cpp #think about move to other lib
    ${LIB_SOURCES}/gxshellconnui.cpp
    ${LIB_SOURCES}/gxshellconnectionsui.cpp
    ${LIB_SOURCES}/gxshellconnfactoryui.cpp
    ${LIB_SOURCES}/gxgisprojui.cpp
    ${LIB_SOURCES}/gxgisprojfactoryui.cpp
    ${LIB_SOURCES}/gxlocaldbfactoryui.cpp
    ${LIB_SOURCES}/gxlocaldbui.cpp
)

add_definitions(-DWXMAKINGDLL_GIS_CLU)
set( WXGISCATALOGUI_LIB_NAME ${LIB_NAME} PARENT_SCOPE )

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

if(GDAL_FOUND)
    target_link_libraries(${LIB_NAME} ${GDAL_LIBRARIES})
endif(GDAL_FOUND) 

if(CAIRO_FOUND)
    target_link_libraries(${LIB_NAME} ${CAIRO_LIBRARIES})
endif(CAIRO_FOUND)

if(UNIX)
    if(GTK2_FOUND)
        target_link_libraries(${LIB_NAME} ${GTK2_LIBRARIES})
    endif(GTK2_FOUND)
endif(UNIX)

target_link_libraries(${LIB_NAME} ${WXGISCORE_LIB_NAME} ${WXGISFRAMEWORK_LIB_NAME} ${WXGISDATASOURCE_LIB_NAME} ${WXGISCATALOG_LIB_NAME} ${WXGISCARTO_LIB_NAME} ${WXGISNET_LIB_NAME} ${WXGISGEOPROCESSING_LIB_NAME} ${WXFREECHART_LIB_NAME} )

include(installlib)

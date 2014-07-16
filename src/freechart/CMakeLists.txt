# **************************************************************************** 
# * Project:  wxFreeChart
# * Purpose:  cmake script
# * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
# ****************************************************************************
# *   Copyright (C) 2013 Dmitry Baryshnikov
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
set(PROJECT_NAME freechart)

include(lib)
include(common)

set(LIB_HEADERS ${WXGIS_CURRENT_SOURCE_DIR}/include/freechart)
set(LIB_SOURCES ${WXGIS_CURRENT_SOURCE_DIR}/src/freechart)

if(WIN32)
    set(wxWidgets_EXCLUDE_COMMON_LIBRARIES TRUE)
endif(WIN32)

find_package(wxWidgets 2.9 REQUIRED core base aui adv)
# wxWidgets include (this will do all the magic to configure everything)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})
endif(wxWidgets_FOUND)

include_directories(${WXGIS_CURRENT_SOURCE_DIR}/include/freechart)

set(PROJECT_HHEADERS ${PROJECT_HHEADERS}    
    ${LIB_HEADERS}/wx/areadraw.h
    ${LIB_HEADERS}/wx/art.h
    ${LIB_HEADERS}/wx/axis/axis.h
    ${LIB_HEADERS}/wx/axisplot.h
    ${LIB_HEADERS}/wx/bars/barplot.h
    ${LIB_HEADERS}/wx/bars/barrenderer.h
    ${LIB_HEADERS}/wx/xyz/bubbleplot.h
    ${LIB_HEADERS}/wx/axis/categoryaxis.h
    ${LIB_HEADERS}/wx/category/categorydataset.h
    ${LIB_HEADERS}/wx/category/categoryrenderer.h
    ${LIB_HEADERS}/wx/category/categorysimpledataset.h
    ${LIB_HEADERS}/wx/chart.h
    ${LIB_HEADERS}/wx/chartpanel.h
    ${LIB_HEADERS}/wx/chartrenderer.h
    ${LIB_HEADERS}/wx/chartsplitpanel.h
    ${LIB_HEADERS}/wx/colorscheme.h
    ${LIB_HEADERS}/wx/axis/compdateaxis.h
    ${LIB_HEADERS}/wx/crosshair.h
    ${LIB_HEADERS}/wx/dataset.h
    ${LIB_HEADERS}/wx/axis/dateaxis.h
    ${LIB_HEADERS}/wx/drawobject.h
    ${LIB_HEADERS}/wx/drawutils.h
    ${LIB_HEADERS}/wx/gantt/ganttdataset.h
    ${LIB_HEADERS}/wx/gantt/ganttplot.h
    ${LIB_HEADERS}/wx/gantt/ganttrenderer.h
    ${LIB_HEADERS}/wx/gantt/ganttsimpledataset.h
    ${LIB_HEADERS}/wx/axis/juliandateaxis.h
    ${LIB_HEADERS}/wx/xy/juliantimeseriesdataset.h
    ${LIB_HEADERS}/wx/axis/labelaxis.h
    ${LIB_HEADERS}/wx/legend.h
    ${LIB_HEADERS}/wx/axis/logarithmicnumberaxis.h
    ${LIB_HEADERS}/wx/marker.h
    ${LIB_HEADERS}/wx/ohlc/movingaverage.h
    ${LIB_HEADERS}/wx/ohlc/movingavg.h
    ${LIB_HEADERS}/wx/multiplot.h
    ${LIB_HEADERS}/wx/axis/numberaxis.h
    ${LIB_HEADERS}/wx/observable.h
    ${LIB_HEADERS}/wx/ohlc/ohlcbarrenderer.h
    ${LIB_HEADERS}/wx/ohlc/ohlccandlestickrenderer.h
    ${LIB_HEADERS}/wx/ohlc/ohlcdataset.h
    ${LIB_HEADERS}/wx/ohlc/ohlcplot.h
    ${LIB_HEADERS}/wx/ohlc/ohlcrenderer.h
    ${LIB_HEADERS}/wx/ohlc/ohlcsimpledataset.h
    ${LIB_HEADERS}/wx/pie/pieplot.h
    ${LIB_HEADERS}/wx/plot.h
    ${LIB_HEADERS}/wx/xy/functions/polynom.h
    ${LIB_HEADERS}/wx/refobject.h
    ${LIB_HEADERS}/wx/xy/functions/sinefunction.h
    ${LIB_HEADERS}/wx/symbol.h
    ${LIB_HEADERS}/wx/xy/timeseriesdataset.h
    ${LIB_HEADERS}/wx/title.h
    ${LIB_HEADERS}/wx/tooltips.h
    ${LIB_HEADERS}/wx/xy/vectordataset.h
    ${LIB_HEADERS}/wx/wxfreechartdefs.h
    ${LIB_HEADERS}/wx/xy/xyarearenderer.h
    ${LIB_HEADERS}/wx/xy/xydataset.h
    ${LIB_HEADERS}/wx/xy/xydynamicdataset.h
    ${LIB_HEADERS}/wx/xy/xyhistorenderer.h
    ${LIB_HEADERS}/wx/xy/xylinerenderer.h
    ${LIB_HEADERS}/wx/xy/xyplot.h
    ${LIB_HEADERS}/wx/xy/xyrenderer.h
    ${LIB_HEADERS}/wx/xy/xysimpledataset.h
    ${LIB_HEADERS}/wx/xyz/xyzdataset.h
    ${LIB_HEADERS}/wx/xyz/xyzrenderer.h
    ${LIB_HEADERS}/wx/zoompan.h
)

set(PROJECT_CSOURCES ${PROJECT_CSOURCES}
    ${LIB_SOURCES}/areadraw.cpp
    ${LIB_SOURCES}/art.cpp
    ${LIB_SOURCES}/axis/axis.cpp
    ${LIB_SOURCES}/axisplot.cpp
    ${LIB_SOURCES}/bars/barplot.cpp
    ${LIB_SOURCES}/bars/barrenderer.cpp
    ${LIB_SOURCES}/xyz/bubbleplot.cpp
    ${LIB_SOURCES}/axis/categoryaxis.cpp
    ${LIB_SOURCES}/category/categorydataset.cpp
    ${LIB_SOURCES}/category/categoryrenderer.cpp
    ${LIB_SOURCES}/category/categorysimpledataset.cpp
    ${LIB_SOURCES}/chart.cpp
    ${LIB_SOURCES}/chartpanel.cpp
    ${LIB_SOURCES}/chartsplitpanel.cpp
    ${LIB_SOURCES}/colorscheme.cpp
    ${LIB_SOURCES}/axis/compdateaxis.cpp
    ${LIB_SOURCES}/crosshair.cpp
    ${LIB_SOURCES}/dataset.cpp
    ${LIB_SOURCES}/axis/dateaxis.cpp
    ${LIB_SOURCES}/gantt/ganttdataset.cpp
    ${LIB_SOURCES}/gantt/ganttplot.cpp
    ${LIB_SOURCES}/gantt/ganttrenderer.cpp
    ${LIB_SOURCES}/gantt/ganttsimpledataset.cpp
    ${LIB_SOURCES}/axis/juliandateaxis.cpp
    ${LIB_SOURCES}/xy/juliantimeseriesdataset.cpp
    ${LIB_SOURCES}/axis/labelaxis.cpp
    ${LIB_SOURCES}/legend.cpp
    ${LIB_SOURCES}/axis/logarithmicnumberaxis.cpp
    ${LIB_SOURCES}/marker.cpp
    ${LIB_SOURCES}/ohlc/movingaverage.cpp
    ${LIB_SOURCES}/multiplot.cpp
    ${LIB_SOURCES}/axis/numberaxis.cpp
    ${LIB_SOURCES}/ohlc/ohlcbarrenderer.cpp
    ${LIB_SOURCES}/ohlc/ohlccandlestickrenderer.cpp
    ${LIB_SOURCES}/ohlc/ohlcdataset.cpp
    ${LIB_SOURCES}/ohlc/ohlcplot.cpp
    ${LIB_SOURCES}/ohlc/ohlcrenderer.cpp
    ${LIB_SOURCES}/ohlc/ohlcsimpledataset.cpp
    ${LIB_SOURCES}/pie/pieplot.cpp
    ${LIB_SOURCES}/plot.cpp
    ${LIB_SOURCES}/xy/functions/polynom.cpp
    ${LIB_SOURCES}/renderer.cpp
    ${LIB_SOURCES}/xy/functions/sinefunction.cpp
    ${LIB_SOURCES}/symbol.cpp
    ${LIB_SOURCES}/xy/timeseriesdataset.cpp
    ${LIB_SOURCES}/title.cpp
    ${LIB_SOURCES}/tooltips.cpp
    ${LIB_SOURCES}/xy/vectordataset.cpp
    ${LIB_SOURCES}/xy/xyarearenderer.cpp
    ${LIB_SOURCES}/xy/xydataset.cpp
    ${LIB_SOURCES}/xy/xydynamicdataset.cpp
    ${LIB_SOURCES}/xy/xyhistorenderer.cpp
    ${LIB_SOURCES}/xy/xylinerenderer.cpp
    ${LIB_SOURCES}/xy/xyplot.cpp
    ${LIB_SOURCES}/xy/xyrenderer.cpp
    ${LIB_SOURCES}/xy/xysimpledataset.cpp
    ${LIB_SOURCES}/xyz/xyzdataset.cpp
    ${LIB_SOURCES}/xyz/xyzrenderer.cpp
    ${LIB_SOURCES}/zoompan.cpp
)

add_definitions(-DWXMAKINGDLL_FREECHART -DwxUSE_GUI=1)
set( WXFREECHART_LIB_NAME ${LIB_NAME} PARENT_SCOPE )

add_library(${LIB_NAME} SHARED ${PROJECT_HHEADERS} ${PROJECT_CSOURCES})

if(wxWidgets_FOUND)
   target_link_libraries(${LIB_NAME} ${wxWidgets_LIBRARIES})
endif(wxWidgets_FOUND)

include(installlib)


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
set_target_properties(${LIB_NAME}
	PROPERTIES PROJECT_LABEL ${PROJECT_NAME}
    VERSION ${WXGIS_VERSION}
    SOVERSION 1
	ARCHIVE_OUTPUT_DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}
    LIBRARY_OUTPUT_DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${WXGIS_CURRENT_BINARY_DIR} 
    )

if(WIN32)
    install(TARGETS ${LIB_NAME} RUNTIME DESTINATION bin)
    if(NOT wxGIS_PORTABLE)
        if(CMAKE_CL_64)
            install(TARGETS ${LIB_NAME} 
                ARCHIVE DESTINATION lib/x64
                LIBRARY DESTINATION lib/x64)
        else()
            install(TARGETS ${LIB_NAME} 
                ARCHIVE DESTINATION lib/x86
                LIBRARY DESTINATION lib/x86)
        endif()
    endif(NOT wxGIS_PORTABLE)
else()
    install(TARGETS ${LIB_NAME}
        RUNTIME DESTINATION bin
        ARCHIVE DESTINATION lib #/wxgis
        LIBRARY DESTINATION lib #/wxgis
    )
endif() 


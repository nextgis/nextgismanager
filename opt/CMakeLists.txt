# **************************************************************************** 
# * Project:  wxGIS
# * Purpose:  cmake script
# * Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
# ****************************************************************************
# *   Copyright (C) 2013-2014 Dmitry Baryshnikov
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

find_program(EXE_MSGFMT msgfmt)

set(MB_LANG "ru" "cs" "es" "gl_es" "pt" "pt_BR")

if(EXE_MSGFMT)
    set(MO_FILES)
    foreach(LANG ${MB_LANG}) 
        message(STATUS "process ${LANG} translation")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/loc/${LANG}")
        file(GLOB inFiles RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/loc/${LANG}/*.po")
        foreach(PO ${inFiles})
            GET_FILENAME_COMPONENT(MO_NAME ${PO} NAME_WE)
            set(MO ${CMAKE_CURRENT_BINARY_DIR}/loc/${LANG}/${MO_NAME}.mo)
            message(STATUS "process ${MO}")
            set(MO_FILES ${MO_FILES} ${MO})
            add_custom_command(
                OUTPUT ${MO}
                COMMAND ${EXE_MSGFMT} ${PO} -o ${MO}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                DEPENDS ${PO})
        endforeach()
    endforeach()
    add_custom_target(create-mo ALL DEPENDS ${MO_FILES})
else()
    message(STATUS "msgfmt not found, translations not generatied")
endif()

if(WIN32)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/opt/loc/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/locale CONFIGURATIONS Debug FILES_MATCHING PATTERN "*.mo" PATTERN "*.txt")
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/loc/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Release/locale CONFIGURATIONS Release FILES_MATCHING PATTERN "*.mo" PATTERN "*.txt")
else(WIN32)#UNIX
    file(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/wxgis)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/loc/ DESTINATION share/wxgis/locale FILES_MATCHING PATTERN "*.mo" PATTERN "*.txt" PATTERN ".svn" EXCLUDE)
endif(WIN32)


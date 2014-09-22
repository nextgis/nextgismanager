#*************************************************************************** 
# Project:  wxGIS
# Purpose:  cmake script
# Author:   Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
#***************************************************************************
#   Copyright (C) 2014 Dmitry Baryshnikov
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

set(CPACK_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake ${CPACK_MODULE_PATH})

if(WIN32)
#    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/loc/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/locale CONFIGURATIONS Debug FILES_MATCHING PATTERN "*.mo" PATTERN "*.txt")
#    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/loc/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Release/locale CONFIGURATIONS Release FILES_MATCHING PATTERN "*.mo" PATTERN "*.txt")
##    install(CODE "FILE(MAKE_DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}/Debug/log)"  CONFIGURATIONS Debug)
    # # install(CODE "FILE(MAKE_DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}/Release/log)"  CONFIGURATIONS Release)
    # # install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/sys/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/sys CONFIGURATIONS Debug)
    # # install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/sys/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Release/sys CONFIGURATIONS Release)
    # # install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/config CONFIGURATIONS Debug FILES_MATCHING PATTERN "*.xml")
    # # install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Release/config CONFIGURATIONS Release FILES_MATCHING PATTERN "*.xml")
    # # install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/wxGISCommon.xml.win DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Release/config/ RENAME wxGISCommon.xml CONFIGURATIONS Release)    
    # # install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/wxGISCommon.xml.win DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/config/ RENAME wxGISCommon.xml CONFIGURATIONS Debug)    
    # # install(DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN ".svn" EXCLUDE PATTERN "src" EXCLUDE)
###############################
    # DEBUG ##################
    install(CODE "FILE(MAKE_DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}/Debug/log)"  CONFIGURATIONS Debug)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/sys/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/sys CONFIGURATIONS Debug)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/config CONFIGURATIONS Debug FILES_MATCHING PATTERN "*.xml")
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/wxGISCommon.xml.win DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/config/ RENAME wxGISCommon.xml CONFIGURATIONS Debug)    
    install(DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}/ DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include CONFIGURATIONS Debug FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "src" EXCLUDE )
    # RELEASE ##################
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/sys/ DESTINATION sys CONFIGURATIONS Release)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/ DESTINATION config CONFIGURATIONS Release FILES_MATCHING PATTERN "*.xml")
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/wxGISCommon.xml.win DESTINATION config/ RENAME wxGISCommon.xml CONFIGURATIONS Release)    
    #install(DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}/ DESTINATION include CONFIGURATIONS Release FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "src" EXCLUDE )
else(WIN32)#UNIX
    FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/wxgis)
#    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/wxgis DESTINATION /var/log/ DIRECTORY_PERMISSIONS
#        OWNER_WRITE OWNER_READ OWNER_EXECUTE
#        GROUP_WRITE GROUP_READ GROUP_EXECUTE
#        WORLD_WRITE WORLD_READ WORLD_EXECUTE)
#    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/loc/ DESTINATION share/wxgis/locale FILES_MATCHING PATTERN "*.mo" PATTERN "*.txt" PATTERN ".svn" EXCLUDE)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/sys/ DESTINATION share/wxgis/sys FILES_MATCHING PATTERN "*")
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/ DESTINATION /etc/wxgis FILES_MATCHING PATTERN "*.xml")
    install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/wxGISCommon.xml.nix DESTINATION /etc/wxgis/ RENAME wxGISCommon.xml )    
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/wxgis DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
    install(DIRECTORY ${WXGIS_CURRENT_BINARY_DIR} DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "src" EXCLUDE)

#    install(FILES ${CMAKE_SOURCE_DIR}/inst/debian/wxgis.conf DESTINATION /etc/ld.so.conf.d)
#no need - use sudo lddconfig in install script
#    COMMAND "ldconfig"
endif(WIN32)

#INCLUDE(InstallRequiredSystemLibraries)
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "NextGIS Manager")
SET(CPACK_PACKAGE_VENDOR "NextGIS")
SET(CPACK_PACKAGE_VERSION_MAJOR "${wxGIS_MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${wxGIS_MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${wxGIS_RELEASE_NUMBER}")
SET(CPACK_PACKAGE_VERSION "${WXGIS_VERSION}")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "nextgis/manager")
SET(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "ngm")
SET(CPACK_PROJECT_CONFIG_FILE ${CMAKE_BINARY_DIR}/CPackOptions.cmake)
IF(WIN32 AND NOT UNIX)
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "bin/wxgiscat")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "ngm" "NextGIS Manager")

INCLUDE(CPack)

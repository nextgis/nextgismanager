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
    if(wxGIS_PORTABLE)
        message(STATUS "ngm [zip]")
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/sys/ DESTINATION bin/sys CONFIGURATIONS Release)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/ DESTINATION bin/config CONFIGURATIONS Release FILES_MATCHING PATTERN "*.xml")
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/wxGISCommon.xml.win DESTINATION bin/config/ RENAME wxGISCommon.xml CONFIGURATIONS Release)        
    else(wxGIS_PORTABLE)
        # DEBUG ##################
        install(CODE "FILE(MAKE_DIRECTORY ${WXGIS_CURRENT_BINARY_DIR}/Debug/log)"  CONFIGURATIONS Debug)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/sys/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/sys CONFIGURATIONS Debug)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/ DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/config CONFIGURATIONS Debug FILES_MATCHING PATTERN "*.xml")
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/wxGISCommon.xml.win DESTINATION ${WXGIS_CURRENT_BINARY_DIR}/Debug/config/ RENAME wxGISCommon.xml CONFIGURATIONS Debug)    
        install(FILES ${WXGIS_CURRENT_BINARY_DIR}/wxgisdefs.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include CONFIGURATIONS Debug )
        # RELEASE ##################
        message(STATUS "ngm [nsis]")
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/sys/ DESTINATION sys CONFIGURATIONS Release)
        install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/ DESTINATION config CONFIGURATIONS Release FILES_MATCHING PATTERN "*.xml")
        install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/wxGISCommon.xml.win DESTINATION config/ RENAME wxGISCommon.xml CONFIGURATIONS Release)  
        #install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION include CONFIGURATIONS Release FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" )
        #install(FILES ${WXGIS_CURRENT_BINARY_DIR}/wxgisdefs.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/include CONFIGURATIONS Release )   
    endif(wxGIS_PORTABLE)
    
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inst/doc/ DESTINATION doc CONFIGURATIONS Release)        
    #third party lib
    #need to more simple way to get paths to binary
    #wxWidgets 32 bit libs
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../wxWidgets/lib/vc_dll/wxbase30u_net_vc_custom.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../wxWidgets/lib/vc_dll/wxbase30u_vc_custom.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../wxWidgets/lib/vc_dll/wxbase30u_xml_vc_custom.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../wxWidgets/lib/vc_dll/wxmsw30u_adv_vc_custom.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../wxWidgets/lib/vc_dll/wxmsw30u_aui_vc_custom.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../wxWidgets/lib/vc_dll/wxmsw30u_core_vc_custom.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../wxWidgets/lib/vc_dll/wxmsw30u_propgrid_vc_custom.dll DESTINATION bin CONFIGURATIONS Release)
    #gdal libs
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/../gdal-nextgis-git/build32/Release/gdal20.dll DESTINATION bin CONFIGURATIONS Release)
    #proj lib
    install(FILES ${WXGIS_CURRENT_BINARY_DIR}/Debug/proj4.dll DESTINATION bin CONFIGURATIONS Release)
    #geos lib
    install(FILES ${WXGIS_CURRENT_BINARY_DIR}/Debug/geos_c.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${WXGIS_CURRENT_BINARY_DIR}/Debug/geos.dll DESTINATION bin CONFIGURATIONS Release)
    #curl lib
    install(FILES ${WXGIS_CURRENT_BINARY_DIR}/Debug/libcurl.dll DESTINATION bin CONFIGURATIONS Release)
    #
    install(FILES ${WXGIS_CURRENT_BINARY_DIR}/Debug/cairo.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${WXGIS_CURRENT_BINARY_DIR}/Debug/libeay32.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${WXGIS_CURRENT_BINARY_DIR}/Debug/libexpat.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${WXGIS_CURRENT_BINARY_DIR}/Debug/libiconv.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${WXGIS_CURRENT_BINARY_DIR}/Debug/libjpeg.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${WXGIS_CURRENT_BINARY_DIR}/Debug/libpng16.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${WXGIS_CURRENT_BINARY_DIR}/Debug/libtiff.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${WXGIS_CURRENT_BINARY_DIR}/Debug/ssleay32.dll DESTINATION bin CONFIGURATIONS Release)
    install(FILES ${WXGIS_CURRENT_BINARY_DIR}/Debug/zlib.dll DESTINATION bin CONFIGURATIONS Release)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/inst/dll32/ DESTINATION bin CONFIGURATIONS Release)
else(WIN32)#UNIX
    FILE(MAKE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/wxgis)
#    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/wxgis DESTINATION /var/log/ DIRECTORY_PERMISSIONS
#        OWNER_WRITE OWNER_READ OWNER_EXECUTE
#        GROUP_WRITE GROUP_READ GROUP_EXECUTE
#        WORLD_WRITE WORLD_READ WORLD_EXECUTE)
#    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/loc/ DESTINATION share/wxgis/locale FILES_MATCHING PATTERN "*.mo" PATTERN "*.txt" PATTERN ".svn" EXCLUDE)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/sys/ DESTINATION share/wxgis/sys FILES_MATCHING PATTERN "*" PATTERN "gdal" EXCLUDE)
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/ DESTINATION /etc/wxgis FILES_MATCHING PATTERN "*.xml")
    install( FILES ${CMAKE_CURRENT_SOURCE_DIR}/opt/config/wxGISCommon.xml.nix DESTINATION /etc/wxgis/ RENAME wxGISCommon.xml )    
    install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/wxgis DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
    install(FILES ${WXGIS_CURRENT_BINARY_DIR}/wxgisdefs.h DESTINATION include)

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
    IF(wxGIS_PORTABLE)
        SET(CPACK_BINARY_NSIS OFF)
        SET(CPACK_BINARY_ZIP ON)
        #todo: add libst to install
    ELSE(wxGIS_PORTABLE)
        SET(CPACK_BINARY_NSIS ON)
        SET(CPACK_NSIS_MODIFY_PATH ON)
        SET(CPACK_NSIS_ENABLE_UNINSTALL_BEFORE_INSTALL ON)
    ENDIF(wxGIS_PORTABLE)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "bin/wxgiscat")
  SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "ngm" "NextGIS Manager")

message(STATUS "pack info=[${CPACK_PACKAGE_VENDOR} ${CPACK_PACKAGE_VERSION}]")

INCLUDE(CPack)

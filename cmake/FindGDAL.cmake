# - Try to find the GDAL encryption library
# Once done this will define
#
#  GDAL_ROOT_DIR - Set this variable to the root installation of GDAL
#
# Read-Only variables:
#  GDAL_FOUND - system has the GDAL library
#  GDAL_INCLUDE_DIR - the GDAL include directory
#  GDAL_LIBRARIES - The libraries needed to use GDAL
#  GDAL_VERSION - This is set to $major.$minor.$revision (eg. 0.9.8)

#=============================================================================
# Copyright 2012 Dmitry Baryshnikov <polimax at mail dot ru>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)
MACRO(DBG_MSG _MSG)
  MESSAGE(STATUS
    "${CMAKE_CURRENT_LIST_FILE}(${CMAKE_CURRENT_LIST_LINE}): ${_MSG}")
ENDMACRO(DBG_MSG)


if (UNIX)
  find_package(PkgConfig)
  if (PKG_CONFIG_FOUND)
    pkg_check_modules(_GDAL gdal)
  endif (PKG_CONFIG_FOUND)
endif (UNIX)

set(_GDAL_ROOT_HINTS
	$ENV{GDAL}
	$ENV{GDAL_ROOT}
	$ENV{LIB}
	$ENV{LIB_HOME}/gdal
	${GDAL_ROOT_DIR}
	/usr/lib
	/usr/local/lib
)
  
set(_GDAL_ROOT_PATHS
	$ENV{GDAL}
	$ENV{GDAL_ROOT}
	$ENV{LIB}
	$ENV{LIB_HOME}/gdal
	/usr/lib
	/usr/local/lib
)

if(MSVC)
    set(P_SUFF "lib" "lib/Release" "lib/Debug")
    if(CMAKE_CL_64)
		set(P_SUFF ${P_SUFF} "lib/x64")
		set(_GDAL_ROOT_HINTS ${_GDAL_ROOT_HINTS}
			$ENV{GDAL}/lib/x64
			$ENV{GDAL_ROOT}/lib/x64
			${GDAL_ROOT_DIR}/lib/x64
		)
		
		set(_GDAL_ROOT_PATHS ${_GDAL_ROOT_PATHS}
			$ENV{GDAL}/lib/x64
			$ENV{GDAL_ROOT}/lib/x64
			${GDAL_ROOT_DIR}/lib/x64
		)
	else(CMAKE_CL_64)
        set(P_SUFF ${P_SUFF} "lib/x86")
		set(_GDAL_ROOT_HINTS ${_GDAL_ROOT_HINTS}
			$ENV{GDAL}/lib/x86
			$ENV{GDAL_ROOT}/lib/x86
			${GDAL_ROOT_DIR}/lib/x86
		)
		
		set(_GDAL_ROOT_PATHS ${_GDAL_ROOT_PATHS}
			$ENV{GDAL}/lib/x86
			$ENV{GDAL_ROOT}/lib/x86			
			${GDAL_ROOT_DIR}/lib/x86
		)    
	endif(CMAKE_CL_64)
endif()

find_path(GDAL_INCLUDE_DIR
  NAMES
    gdal.h
    gcore/gdal.h
  HINTS
    ${_GDAL_INCLUDE_DIR}
  ${_GDAL_ROOT_HINTS_AND_PATHS}
  PATH_SUFFIXES
    "include"
    "include/gdal"
    "local/include/gdal"
)

if (GDAL_INCLUDE_DIR) 
    file(READ "${GDAL_INCLUDE_DIR}/gcore/gdal_version.h" _gdal_VERSION_H_CONTENTS)
    string(REGEX MATCH "GDAL_VERSION_MAJOR[ \t]+([0-9]+)" GDAL_MAJOR_VERSION ${_gdal_VERSION_H_CONTENTS})
    string(REGEX MATCH "([0-9]+)" GDAL_MAJOR_VERSION ${GDAL_MAJOR_VERSION})
    string(REGEX MATCH "GDAL_VERSION_MINOR[ \t]+([0-9]+)" GDAL_MINOR_VERSION ${_gdal_VERSION_H_CONTENTS})
    string(REGEX MATCH "([0-9]+)" GDAL_MINOR_VERSION ${GDAL_MINOR_VERSION})
    string(REGEX MATCH "GDAL_VERSION_REV[ \t]+([0-9]+)" GDAL_RELEASE_NUMBER ${_gdal_VERSION_H_CONTENTS})  
    string(REGEX MATCH "([0-9]+)" GDAL_RELEASE_NUMBER ${GDAL_RELEASE_NUMBER})
        
    # Setup package meta-data
    set(GDAL_VERSION ${GDAL_MAJOR_VERSION}.${GDAL_MINOR_VERSION} CACHE INTERNAL "The version number for wxgis libraries")
	DBG_MSG("GDAL_VERSION : ${GDAL_VERSION}")  

endif (GDAL_INCLUDE_DIR)

find_library(GDAL_RELEASE
    NAMES
      gdal${GDAL_MAJOR_VERSION}${GDAL_MINOR_VERSION}.lib      
      gdal${GDAL_MAJOR_VERSION}${GDAL_MINOR_VERSION}.so          
      gdal${GDAL_MAJOR_VERSION}${GDAL_MINOR_VERSION}.a          
    PATHS 
		${_GDAL_ROOT_PATHS}
		${GDAL_INCLUDE_DIR}
	PATH_SUFFIXES
		${P_SUFF}
    NO_DEFAULT_PATH
)	
find_library(GDAL_DEBUG
    NAMES
      gdal${GDAL_MAJOR_VERSION}${GDAL_MINOR_VERSION}d.lib   
      gdal${GDAL_MAJOR_VERSION}${GDAL_MINOR_VERSION}d.so   
      gdal${GDAL_MAJOR_VERSION}${GDAL_MINOR_VERSION}d.a   
    PATHS
		${_GDAL_ROOT_PATHS}
		${GDAL_INCLUDE_DIR}
	PATH_SUFFIXES
		${P_SUFF}
	NO_DEFAULT_PATH
)
	
if(NOT GDAL_RELEASE AND GDAL_DEBUG)
	set(GDAL_RELEASE ${GDAL_DEBUG})
endif(NOT GDAL_RELEASE AND GDAL_DEBUG)
	
LIST(APPEND GDAL_LIBRARIES
        debug ${GDAL_DEBUG} optimized ${GDAL_RELEASE}
        )
		
DBG_MSG("GDAL_LIBRARIES : ${GDAL_LIBRARIES}")  
		
include(FindPackageHandleStandardArgs)

if (GDAL_VERSION)
  find_package_handle_standard_args(GDAL
    REQUIRED_VARS
      GDAL_LIBRARIES
      GDAL_INCLUDE_DIR
    VERSION_VAR
      GDAL_VERSION
    FAIL_MESSAGE
      "Could NOT find GDAL, try to set the path to GDAL root folder in the system variable GDAL_ROOT"
  )
else (GDAL_VERSION)
  find_package_handle_standard_args(GDAL "Could NOT find GDAL, try to set the path to GDAL root folder in the system variable GDAL_ROOT"
    GDAL_LIBRARIES
    GDAL_INCLUDE_DIR
  )
endif (GDAL_VERSION)

message(STATUS "gdal libs=[${GDAL_LIBRARIES}] headers=[${GDAL_INCLUDE_DIR}]")
MARK_AS_ADVANCED(GDAL_INCLUDE_DIR GDAL_LIBRARIES)

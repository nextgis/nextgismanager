# Copyright (c) 2014, Dmitry Baryshnikov (aka Bishop), polimax@mail.ru
# - Try to find the UNITY library
# Once done this will define
#
#  UNITY_ROOT_DIR - Set this variable to the root installation of UNITY
#
# Read-Only variables:
#  UNITY_FOUND - system has the UNITY library
#  UNITY_INCLUDE_DIR - the UNITY include directory
#  UNITY_LIBRARIES - The libraries needed to use UNITY
#  TODO: UNITY_VERSION - This is set to $major.$minor.$revision (eg. 0.9.8)

#=============================================================================
# Copyright 2014 Dmitry Baryshnikov <polimax at mail dot ru>
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

find_package(PkgConfig)
if (PKG_CONFIG_FOUND)
    pkg_check_modules(_UNITY unity)
endif (PKG_CONFIG_FOUND)

SET(_UNITY_ROOT_HINTS
  $ENV{UNITY}
  ${UNITY_ROOT_DIR}
  )
SET(_UNITY_ROOT_PATHS
  $ENV{UNITY}/src
  /usr
  /usr/local
  )
SET(_UNITY_ROOT_HINTS_AND_PATHS
  HINTS ${_UNITY_ROOT_HINTS}
  PATHS ${_UNITY_ROOT_PATHS}
  )

FIND_PATH(UNITY_INCLUDE_DIR
  NAMES
    unity.h
  HINTS
    ${_UNITY_INCLUDEDIR}
  ${_UNITY_ROOT_HINTS_AND_PATHS}
  PATH_SUFFIXES
    include
    "include/unity"
    "include/unity/unity"
)

FIND_PATH(DEE_INCLUDE_DIR
  NAMES
    dee.h
  PATH_SUFFIXES
    include
    "include/dee-0.7"
    "include/dee-0.8"
    "include/dee-0.9"
    "include/dee-1.0"
)

FIND_PATH(DBUSMENU_INCLUDE_DIR
  NAMES
    libdbusmenu-glib
  PATH_SUFFIXES
    include
    "include/libdbusmenu-glib-0.1"
    "include/libdbusmenu-glib-0.2"
    "include/libdbusmenu-glib-0.3"
    "include/libdbusmenu-glib-0.4"
)

FIND_LIBRARY(UNITY_LIBRARY
    NAMES
        unity
    HINTS
      ${_UNITY_LIBDIR}
    ${_UNITY_ROOT_HINTS_AND_PATHS}
    PATH_SUFFIXES
      "lib"
      "local/lib"
  ) 


  MARK_AS_ADVANCED(UNITY_LIBRARY)

  # compat defines
  SET(UNITY_LIBRARIES ${UNITY_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(UNITY "Could NOT find UNITY, try to set the path to UNITY root folder in the system variable UNITY"
    UNITY_LIBRARIES
    UNITY_INCLUDE_DIR
  )

SET(UNITY_INCLUDE_DIR ${UNITY_INCLUDE_DIR} ${DEE_INCLUDE_DIR} ${DBUSMENU_INCLUDE_DIR})

MARK_AS_ADVANCED(UNITY_INCLUDE_DIR UNITY_LIBRARIES)


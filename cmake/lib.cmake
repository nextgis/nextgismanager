project (wxgis${PROJECT_NAME})

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)

if(WIN32)
  set(LIB_NAME ${PROJECT_NAME}${wxGIS_MAJOR_VERSION}${wxGIS_MINOR_VERSION})
else(WIN32)
  set(LIB_NAME ${PROJECT_NAME})
endif(WIN32)
message(STATUS "${PROJECT_NAME} lib name ${LIB_NAME}")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/version_dll.h "//Copyright (C) 2013 Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru\n#pragma once\n#define wxGIS_FILENAME \"${PROJECT_NAME}\" \n\n" )

if(MSVC)
  set(CMAKE_DEBUG_POSTFIX "d")
#  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${WXGIS_CURRENT_BINARY_DIR}/Debug/)
#  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${WXGIS_CURRENT_BINARY_DIR}/Release/)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions(-D_UNICODE -DUNICODE -D_USRDLL)
  set(PROJECT_CSOURCES ${PROJECT_CSOURCES} ${WXGIS_CURRENT_SOURCE_DIR}/src/version_dll.rc)
  source_group("Resource Files" FILES ${WXGIS_CURRENT_SOURCE_DIR}/src/version_dll.rc)  
endif(MSVC)

if(WIN32)
  add_definitions(-DWIN32 -D__WXMSW__)
endif(WIN32)

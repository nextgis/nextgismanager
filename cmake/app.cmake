project (wxgis${PROJECT_NAME})

message(STATUS "${PROJECT_NAME} app name ${APP_NAME}")

set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "Configs" FORCE)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/version_app.h "//Copyright (C) 2009-2014 Baryshnikov Dmitry (aka Bishop), polimax@mail.ru\n#pragma once\n#define wxGIS_FILENAME \"${APP_NAME}\"\n#define wxGIS_MAINFAMEICON \"${wxGIS_MAINFAMEICON}\"\n#define wxGIS_MAINFAMEICON_X \"${wxGIS_MAINFAMEICON_X}\"\n\n" )

if(MSVC)
  set(CMAKE_DEBUG_POSTFIX "d")
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${WXGIS_CURRENT_BINARY_DIR}/Debug/)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${WXGIS_CURRENT_BINARY_DIR}/Release/)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE -D_CRT_NONSTDC_NO_DEPRECATE)
  add_definitions(-D_UNICODE -DUNICODE -D_USRDLL)
  set(PROJECT_CSOURCES ${PROJECT_CSOURCES} ${WXGIS_CURRENT_SOURCE_DIR}/src/version_app.rc ${wxGIS_MAINFAMEICON} ${wxGIS_MAINFAMEICON_X})
  source_group("Resource Files" FILES ${WXGIS_CURRENT_SOURCE_DIR}/src/version_app.rc ${wxGIS_MAINFAMEICON} ${wxGIS_MAINFAMEICON_X}) 
endif(MSVC)

if(WIN32)
  add_definitions(-DWIN32 -D__WXMSW__)
endif(WIN32)


